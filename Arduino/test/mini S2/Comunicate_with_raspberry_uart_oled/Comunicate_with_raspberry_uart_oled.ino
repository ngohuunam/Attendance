//#include "SH1106Wire.h"
//SH1106Wire display(0x3c, 12, 11);     // ADDRESS, SDA, SCL

#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);

#include <AsyncTimer.h>

AsyncTimer t;

#include <SoftwareSerial.h>

// s2 mini
//#define RS485_RX 39
//#define RS485_TX 37

// lora v3
#define RS485_RX 19
#define RS485_TX 20

EspSoftwareSerial::UART rs485Serial(RS485_RX, RS485_TX);

#define OLED_RESET -1

#define DEMO_DURATION 3000

uint32_t sendCount = 0;
uint32_t inputCount = 0;

void rs485Send(void) {
  sendCount++;
  String SEND_COUNT_STR = "SEND: " + String(sendCount);
  rs485Serial.print(String(sendCount));
  Serial.println(SEND_COUNT_STR);
  display.clear();
  display.drawString(0, 0, SEND_COUNT_STR);
  display.display();
}

void rs485Input(void) {
  if (rs485Serial.available() > 0) {
    inputCount++;
    delay(10);
    String READ_DATA = rs485Serial.readString();
    String READ_DATA_STR = "READ: " + READ_DATA;
    String INPUT_COUNT_STR = "COUNT: " + String(inputCount);
    Serial.println(READ_DATA_STR);
    Serial.println(INPUT_COUNT_STR);
    display.clear();
    display.drawString(0, 10, READ_DATA_STR);
    display.drawString(0, 20, INPUT_COUNT_STR);
    display.display();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, LOW);
  delay(20);
  digitalWrite(RST_OLED, HIGH);
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
//  display.clear();

  // Initialising the UI will init the display too.
  display.init();
  
  t.setInterval(rs485Send, DEMO_DURATION);
  t.setInterval(rs485Input, 10);
}

void loop() {
  // clear the display
  //  display.clear();
  t.handle();
}
