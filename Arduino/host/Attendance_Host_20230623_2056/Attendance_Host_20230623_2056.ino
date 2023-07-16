
#include <WiFi.h>
#include "driver/adc.h"

#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);

#include <AsyncTimer.h>

AsyncTimer t;

typedef struct {
  unsigned short RASP_PING = 0;
  unsigned short RASP_PING_CHECK_RESPONSE = 0;
  unsigned short RASP_INPUT = 0;
  unsigned short RS485_1_INPUT = 0;
  unsigned short RS485_2_INPUT = 0;
  unsigned short RS485_3_INPUT = 0;
  unsigned short RS485_4_INPUT = 0;
  unsigned short OLED_DRAW = 0;

} tTimerID;

tTimerID TimerID;

#define INPUT_TIMER_INTERVAL 10

#include <SoftwareSerial.h>

#define BAUDRATE 57600

#define RS485_RX_1 7  //18
#define RS485_TX_1 6  //17

EspSoftwareSerial::UART rs485_1_Serial(RS485_RX_1, RS485_TX_1);

#define RS485_RX_2 5  //16
#define RS485_TX_2 4  //15

EspSoftwareSerial::UART rs485_2_Serial(RS485_RX_2, RS485_TX_2);

#define RS485_RX_3 39  //10
#define RS485_TX_3 40  //9

EspSoftwareSerial::UART rs485_3_Serial(RS485_RX_3, RS485_TX_3);

#define RS485_RX_4 41  //8
#define RS485_TX_4 42  //7

EspSoftwareSerial::UART rs485_4_Serial(RS485_RX_4, RS485_TX_4);

#define RASP_RX 19
#define RASP_TX 20

EspSoftwareSerial::UART raspSerial(RASP_RX, RASP_TX);

typedef struct {
  String _1 = "";
  String _2 = "";
  String _3 = "";
  String _4 = "";
} tRs485InputStr;

tRs485InputStr RS485_INPUT_STR;

#define PING_RASP_DURATION 5000
#define PING_RASP_CHECK_RESPONSE_DURATION 20000

String SEND_COUNT_STR = "";
String RASP_PING_STATUS = "";
String READ_DATA_STR = "";

uint32_t raspPingCount = 0;

void disableWiFi();

void setup()
{
  Serial.begin(9600);
  disableWiFi();
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, LOW);
  delay(20);
  digitalWrite(RST_OLED, HIGH);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.init();
  //  display.flipScreenVertically();
  display.clear();
  display.drawString(0, 0, "START UP...");
  display.display();

  //  Serial.begin(115200);
  delay(10);

  pinMode(RS485_RX_1, INPUT);
  pinMode(RS485_TX_1, OUTPUT);

  pinMode(RS485_RX_2, INPUT);
  pinMode(RS485_TX_2, OUTPUT);

  pinMode(RS485_RX_3, INPUT);
  pinMode(RS485_TX_3, OUTPUT);

  pinMode(RS485_RX_4, INPUT);
  pinMode(RS485_TX_4, OUTPUT);

  rs485_1_Serial.begin(BAUDRATE);
  rs485_2_Serial.begin(BAUDRATE);
  rs485_3_Serial.begin(BAUDRATE);
  rs485_4_Serial.begin(BAUDRATE);

  pinMode(RASP_RX, INPUT);
  pinMode(RASP_TX, OUTPUT);

  raspSerial.begin(9600);

  TimerID.RS485_1_INPUT = t.setInterval(rs485_1_Input, INPUT_TIMER_INTERVAL);
  TimerID.RS485_2_INPUT = t.setInterval(rs485_2_Input, INPUT_TIMER_INTERVAL);
  TimerID.RS485_3_INPUT = t.setInterval(rs485_3_Input, INPUT_TIMER_INTERVAL);
  TimerID.RS485_4_INPUT = t.setInterval(rs485_4_Input, INPUT_TIMER_INTERVAL);
  TimerID.RASP_INPUT = t.setInterval(raspInput, INPUT_TIMER_INTERVAL);
  TimerID.RASP_PING = t.setInterval(pingRasp, PING_RASP_DURATION);
  TimerID.OLED_DRAW = t.setInterval(oledDraw, 1000);
  //  t.setInterval(raspSend, DEMO_DURATION);
  display.clear();
  display.drawString(0, 0, "DONE SETUP...");
  display.display();
}

void oledDraw() {
  display.clear();
  display.drawString(0, 0, "RS485 1: " + RS485_INPUT_STR._1);
  display.drawString(0, 10, "RS485 2: " + RS485_INPUT_STR._2);
  display.drawString(0, 20, "RS485 3: " + RS485_INPUT_STR._3);
  display.drawString(0, 30, SEND_COUNT_STR);
  display.drawString(0, 40, RASP_PING_STATUS);
  display.drawString(0, 50, READ_DATA_STR);
  display.display();
}

void loop()
{
  t.handle();
}

void rs485Forwarder() {
  rs485_1_Input();
  rs485_2_Input();
  rs485_3_Input();
  rs485_4_Input();
}

void rs485_1_Input() {
  if (rs485_1_Serial.available() > 0) {
    delay(10);
    RS485_INPUT_STR._1 = rs485_1_Serial.readStringUntil('\n');
    raspSerial.print(RS485_INPUT_STR._1);
  }
}

void rs485_2_Input() {
  if (rs485_2_Serial.available() > 0) {
    delay(10);
    RS485_INPUT_STR._2 = rs485_2_Serial.readStringUntil('\n');
    raspSerial.print(RS485_INPUT_STR._2);
  }
}

void rs485_3_Input() {
  if (rs485_3_Serial.available() > 0) {
    delay(10);
    RS485_INPUT_STR._3 = rs485_3_Serial.readStringUntil('\n');
    raspSerial.print(RS485_INPUT_STR._3);
  }
}

void rs485_4_Input() {
  if (rs485_4_Serial.available() > 0) {
    delay(10);
    RS485_INPUT_STR._4 = rs485_4_Serial.readStringUntil('\n');
    raspSerial.print(RS485_INPUT_STR._4);
  }
}


// string: string to parse
// c: delimiter
// returns number of items parsed
void split(String* _resStrArr, String _string, char _delim, int _len)
{
  int r = 0, t = 0, i = 0;

  for (i = 0; i < _len; i++)
  {
    if (_string.charAt(i) == _delim)
    {
      _resStrArr[t] = _string.substring(r, i);
      r = (i + 1);
      t++;
    }
  }
  _resStrArr[t] = _string.substring(r, i);
}

void pingRasp(void) {
  raspPingCount++;
  String _pingStr = "h,0,h,p," + String(raspPingCount);
  raspSerial.println(_pingStr);
  TimerID.RASP_PING_CHECK_RESPONSE = t.setTimeout(handdlePingRaspNotReponse, PING_RASP_CHECK_RESPONSE_DURATION);
  SEND_COUNT_STR = "PING RASP: " + String(raspPingCount);
}

byte pingRaspNotResponseCount = 0;

void handdlePingRaspNotReponse() {
  pingRaspNotResponseCount++;
  RASP_PING_STATUS = "Ping Rasp FAILED";
}

void raspInput(void) {
  if (raspSerial.available() > 0) {
    delay(10);
    String READ_DATA = raspSerial.readString();
    READ_DATA_STR = READ_DATA;
    handleRaspInputStr(READ_DATA);
  }
}

void handleRaspInputStr(String _str) {
  int _len = _str.length();
  String _strArr[_len];
  split(_strArr, _str, ',', _len);
  String _device = _strArr[0];
  String _id = _strArr[1];
  String _fn = _strArr[2];
  String _cmd = _strArr[3];
  String _value = _strArr[4];
  if (_device == "h") {
    if (_cmd == "p") {
      String _pingStr = "h,0,h,pan," + String(raspPingCount);
      raspSerial.println(_pingStr);
    } else if (_cmd == "pan") {
      t.cancel(TimerID.RASP_PING_CHECK_RESPONSE);
      RASP_PING_STATUS = "Ping Rasp OK";
    }
  } else if (_device == "r") {
    if (_id == "1") {
      rs485_1_Serial.println(_str);
    } else if (_id == "2") {
      rs485_2_Serial.println(_str);
    } else if (_id == "3") {
      rs485_3_Serial.println(_str);
    } else if (_id == "4") {
      rs485_4_Serial.println(_str);
    }
  }
}

void disableWiFi() {
  adc_power_off();
  WiFi.disconnect(true);  // Disconnect from the network
  WiFi.mode(WIFI_OFF);    // Switch WiFi off
}
