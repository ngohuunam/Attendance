/*
    This sketch trys to Connect to the best AP based on a given list

*/

#include "Arduino.h"
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"
#include <SoftwareSerial.h>

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

String wifiIp = "";
String wifiStatus = "Start up...";

#include <AsyncTimer.h>

AsyncTimer t;

#define RX_PIN 45
#define TX_PIN 46
EspSoftwareSerial::UART swSerial(RX_PIN, TX_PIN);

String STR_FROM_GM861 = "";

void setup()
{
  Serial.begin(115200);
  delay(10);

  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);

  swSerial.begin(9600);

  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, LOW);
  delay(20);
  digitalWrite(RST_OLED, HIGH);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.init();
//  display.flipScreenVertically();
  drawWifiInfo();

  wifiMulti.addAP("Ti Teo", "12345678");
  wifiMulti.addAP("MDFKG", "hoianhtuan");

  Serial.println("Connecting Wifi...");
  wifiStatus = "Connecting Wifi...";
  if (wifiMulti.run() == WL_CONNECTED) {
    wifiIp = String(WiFi.localIP().toString());
    wifiStatus = "Wifi connected";
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(wifiIp);
  }
  t.setInterval(drawWifiInfo, 5000);
  t.setInterval(read_GM861, 10);
}

void drawWifiInfo() {
  display.clear();
  display.drawString(0, 0, wifiStatus);
  //  display.drawString(0, 10, "IP address: ");
  display.drawString(0, 12, wifiIp);
  display.drawString(0, 22, STR_FROM_GM861);
  display.display();
}

void loop()
{
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    wifiStatus = "WiFi not connected!";
    delay(1000);
  }
  t.handle();
}

void read_GM861() {
  char buffer[7] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
  uint32_t idx = 0;
  bool getdata = false;
  while (swSerial.available() > 0) {
    delay(10);
    buffer[idx++] = swSerial.read();
    getdata = true;
    if (idx >= 6) {
      break;
    }
  }

  if (getdata) {
    if (buffer[0] == 2 || buffer[0] == 3) {
      for (int i = 0; i < 7; i++) {
        STR_FROM_GM861 += String(buffer[i], HEX);
        STR_FROM_GM861 += " ";
      }
      STR_FROM_GM861.toUpperCase();
    } else {
      STR_FROM_GM861 = String(buffer);
    }

    if (STR_FROM_GM861.length() > 0) {
      Serial.print("Received: ");
      Serial.println(STR_FROM_GM861);
      display.drawString(0, 22, " ");
      delay(10);
      display.drawString(0, 22, STR_FROM_GM861);
      display.display();
    }
    getdata = false;
  }
}
