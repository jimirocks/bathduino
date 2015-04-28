#include "DHT.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHTPIN 2  
#define DHTTYPE DHT22 

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// If using software SPI (the default case):
#define OLED_MOSI   9 // DI
#define OLED_CLK    8 // DO
#define OLED_DC    11 // DC
#define OLED_CS    12 // CS
#define OLED_RESET 10 // RES
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

boolean showTemp = true;

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
  delay(10000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();;
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  display.clearDisplay();
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(0,30);

  if (showTemp) {
    display.print(t, 1);
    display.setTextSize(2);
    display.print(" C");
  } else {
    display.print(h, 1);
    display.setTextSize(2);
    display.println(" %");
  }
  display.display();
  showTemp = ! showTemp;
}
