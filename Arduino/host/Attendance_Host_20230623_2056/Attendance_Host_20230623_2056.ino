/*
    This sketch trys to Connect to the best AP based on a given list

*/

//#include "Arduino.h"
//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

String wifiIp = "";
String wifiStatus = "Start up...";

#include <AsyncTimer.h>

AsyncTimer t;

typedef struct {
  unsigned short RASP_PING = 0;
  unsigned short RASP_PING_CHECK_RESPONSE = 0;
} tTimerID;

tTimerID TimerID;

#include <SoftwareSerial.h>

#define RS485_RX 41
#define RS485_TX 42

EspSoftwareSerial::UART rs485Serial(RS485_RX, RS485_TX);

#define RASP_RX 19
#define RASP_TX 20

EspSoftwareSerial::UART raspSerial(RASP_RX, RASP_TX);

String RS485_RECEIVED = "";
uint32_t RS485_HEARTBEAT_COUNTER = 0;

#define PING_RASP_DURATION 2000
#define PING_RASP_CHECK_RESPONSE_DURATION 5000

uint32_t raspSendCount = 0;
uint32_t raspInputCount = 0;

String SEND_COUNT_STR = "";
String INPUT_COUNT_STR = "";
String READ_DATA_STR = "";

uint32_t raspPingCount = 0;


// string: string to parse
// c: delimiter
// returns number of items parsed
void split(String* _resStrArr, String _string, char _delim, int _len)
{
  int r = 0, t = 0;

  for (int i = 0; i < _len; i++)
  {
    if (_string.charAt(i) == _delim)
    {
      _resStrArr[t] = _string.substring(r, i);
      r = (i + 1);
      t++;
    }
  }
}

void pingRasp(void) {
  raspPingCount++;
  String _pingStr = "h,p," + String(raspPingCount);
  raspSerial.println(_pingStr);
  TimerID.RASP_PING_CHECK_RESPONSE = t.setTimeout(handdlePingRaspNotReponse, PING_RASP_CHECK_RESPONSE_DURATION);
  SEND_COUNT_STR = "PING RASP: " + String(raspPingCount);
}

byte pingRaspNotResponseCount = 0;

void handdlePingRaspNotReponse() {
  pingRaspNotResponseCount++;
}

//void raspSend(void) {
//  raspSendCount++;
//  SEND_COUNT_STR = "RASP SEND: " + String(raspSendCount);
//  raspSerial.println(SEND_COUNT_STR);
//}

void raspInput(void) {
  if (raspSerial.available() > 0) {
    raspInputCount++;
    delay(10);
    String READ_DATA = raspSerial.readString();
    READ_DATA_STR = READ_DATA;
    INPUT_COUNT_STR = "RASP INPUT COUNT: " + String(raspInputCount);
    handleRaspInputStr(READ_DATA);
  }
}

void handleRaspInputStr(String _str) {
  int _len = _str.length();
  String _strArr[_len];
  split(_strArr, _str, ',', _len);
  String _device = _strArr[0];
  String _cmd = _strArr[1];
  String _value = _strArr[2];
  if (_device == "h") {
    if (_cmd == "p") {
      if (_value == String(raspPingCount + 1)) {
        t.cancel(TimerID.RASP_PING_CHECK_RESPONSE);
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(10);

  pinMode(RS485_RX, INPUT);
  pinMode(RS485_TX, OUTPUT);

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
  TimerID.RASP_PING = t.setInterval(pingRasp, PING_RASP_DURATION);
  //  t.setInterval(raspSend, DEMO_DURATION);
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
