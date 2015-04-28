#include "DHT.h"

#define DHTPIN 2  
#define DHTTYPE DHT22 

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// for OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// If using software SPI (the default case):
#define OLED_MOSI   9 // DI
#define OLED_CLK    8 // DO
#define OLED_DC    11 // DC
#define OLED_CS    12 // CS
#define OLED_RESET 10 // RES
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define MAIN_DELAY 15000
#define FAN_RUNNING_CYCLES 8
boolean showTemp = true;
const float MAX_HUMIDITY = 50;
const float HUMIDITY_THRESHOLD = 60;
int fanCycles = -1;


void setup() {
  Serial.begin(9600); 
 
  dht.begin();
  
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(MAIN_DELAY);
  
  display.clearDisplay();
  display.setTextColor(WHITE);

  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    showError("Failed to read from DHT sensor!");
    return;
  }
  
  
  controlFan(h);
  
  showMeasures(t, h);
  
  
  display.display();  
}

void showError(String msg) {
  display.setTextSize(1);
  
  display.print("ERROR: ");
  display.println(msg);
}

void showMeasures(float temperature, float humidity) {  
  display.setTextSize(5);
  display.setCursor(0,30);

  if (showTemp) {
    display.print(temperature, 1);
    display.setTextSize(2);
    display.print(" C");
  } else {
    display.print(humidity, 1);
    display.setTextSize(2);
    display.println(" %");
  }
  display.display();
  showTemp = ! showTemp;  
}

void controlFan(float humidity) {
  if (fanCycles > 0) {
    // fan running
    Serial.println("running");
    fanCycles--;
  } else if (fanCycles == 0) {
    if (humidity <= MAX_HUMIDITY) {
      // fan stop
      Serial.println("stopping");
      fanCycles--;
    } else {
      Serial.println("keep running");
      fanCycles = FAN_RUNNING_CYCLES;
    }
  } else {
    Serial.println("is off");
    //fanCycles == -1
    if (humidity >= HUMIDITY_THRESHOLD) {
      // start fan
      Serial.println("starting");
      fanCycles = FAN_RUNNING_CYCLES;  
    } 
  }
  
  display.setTextSize(1);
  display.setCursor(0,0);
  if (fanCycles > -1) {
    display.print("FAN ON");
  } else {
    display.print("FAN OFF");
  }
}
