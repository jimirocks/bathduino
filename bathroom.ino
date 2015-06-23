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

#define FAN_RELAY  5  

#define MAIN_DELAY 15000
#define FAN_RUNNING_CYCLES 8
#define FAN_WAITING_CYCLES 60
boolean showTemp = true;
const float MAX_HUMIDITY = 62;
const float HUMIDITY_THRESHOLD = 80;
int fanRunning = -1;
int fanWaiting = -1;


void setup() {
  Serial.begin(9600); 
  
  pinMode(FAN_RELAY, OUTPUT);
  stopFan();
 
  dht.begin();
  
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();
  delay(2000);
  display.clearDisplay();
  
  printFanStatus(false);
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
  
  printFanStatus(fanRunning > -1);
  
  display.display();  
}

void showError(String msg) {
  display.setTextSize(1);
  
  display.print("ERROR: ");
  display.println(msg);
}

void showMeasures(float temperature, float humidity) {  
  display.setTextSize(4);
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
  if (fanWaiting == -1 && fanRunning == -1) {
    if (humidity >= HUMIDITY_THRESHOLD) {
      Serial.println("start waiting");
      fanWaiting = FAN_WAITING_CYCLES;
    } 
  } else if (fanWaiting > 0 && fanRunning == -1) {
    Serial.println("waiting");
    fanWaiting--;
  } else if (fanWaiting == 0 && fanRunning == -1) {
    if (humidity >= HUMIDITY_THRESHOLD) {
      Serial.println("starting");
      fanRunning = FAN_RUNNING_CYCLES; 
      startFan();
    } 
    Serial.println("reset waiting");
    fanWaiting--;
  } else if (fanWaiting == -1 && fanRunning > 0) {
    Serial.println("running");
    fanRunning--;
  } else if (fanWaiting == -1 && fanRunning == 0) {
    if (humidity <= MAX_HUMIDITY) {
      Serial.println("stopping");
      fanRunning--;
      stopFan();
    } else {
      Serial.println("keep running");
      fanRunning = FAN_RUNNING_CYCLES;
    }
  }
}

void startFan() {
  digitalWrite(FAN_RELAY,0);
}

void stopFan() {
  digitalWrite(FAN_RELAY,1);
}

void printFanStatus(boolean on) {
  display.setTextSize(1);
  display.setCursor(0,0);
  String state = "OFF";
  if (on) {
    state = "ON";
  } 
  display.print("FAN " + state);
}
