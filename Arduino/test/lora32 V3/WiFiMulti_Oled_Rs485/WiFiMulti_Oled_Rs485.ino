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

#define RS485_RX_PIN 41
#define RS485_TX_PIN 42

EspSoftwareSerial::UART rs485Serial(RS485_RX_PIN, RS485_TX_PIN);

#define RASP_RX 19
#define RASP_TX 20

EspSoftwareSerial::UART raspSerial(RASP_RX, RASP_TX);

String RS485_RECEIVED = "";
uint32_t RS485_HEARTBEAT_COUNTER = 0;

#define DEMO_DURATION 3000

uint32_t raspSendCount = 0;
uint32_t raspInputCount = 0;

String SEND_COUNT_STR = "";
String INPUT_COUNT_STR = "";
String READ_DATA_STR = "";

void raspSend(void) {
  raspSendCount++;
  SEND_COUNT_STR = "RASP SEND: " + String(raspSendCount);
  raspSerial.println(SEND_COUNT_STR);
}

void raspInput(void) {
  if (raspSerial.available() > 0) {
    raspInputCount++;
    delay(10);
    String READ_DATA = raspSerial.readString();
    READ_DATA_STR = "RASP INPUT: " + READ_DATA;
    INPUT_COUNT_STR = "RASP INPUT COUNT: " + String(raspInputCount);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(10);

  pinMode(RS485_RX_PIN, INPUT);
  pinMode(RS485_TX_PIN, OUTPUT);

  rs485Serial.begin(9600);

  pinMode(RASP_RX, INPUT);
  pinMode(RASP_TX, OUTPUT);

  raspSerial.begin(9600);

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
  t.setInterval(drawWifiInfo, 1000);
  t.setInterval(rs485Input, 10);
  t.setInterval(raspInput, 10);
  t.setInterval(raspSend, DEMO_DURATION);
}

void drawWifiInfo() {
  display.clear();
  display.drawString(0, 0, wifiStatus);
  //  display.drawString(0, 10, "IP address: ");
  display.drawString(0, 10, wifiIp);
  display.drawString(0, 20, RS485_RECEIVED);
  display.drawString(0, 30, SEND_COUNT_STR);
  display.drawString(0, 40, INPUT_COUNT_STR);
  display.drawString(0, 50, READ_DATA_STR);
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

void rs485Input() {
  if (rs485Serial.available() > 0) {
    delay(10);
    RS485_RECEIVED = rs485Serial.readString();
    Serial.print("RS485_RECEIVED: "); Serial.println(RS485_RECEIVED);
    if (RS485_RECEIVED == "heartbeat") {
      rs485Serial.print("heartbeat");
      RS485_HEARTBEAT_COUNTER++;
      RS485_RECEIVED += ": ";
      RS485_RECEIVED += String(RS485_HEARTBEAT_COUNTER);
      Serial.print("RS485_RECEIVED > 0: "); Serial.println(RS485_RECEIVED);
      drawWifiInfo();
    }
  }
}
