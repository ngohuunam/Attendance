#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);
#define FONT_SIZE 10

#include <AsyncTimer.h>
AsyncTimer t;

typedef struct {
  unsigned short RS485_1_INPUT = 0;
  unsigned short RS485_2_INPUT = 0;
  unsigned short RS485_3_INPUT = 0;
  unsigned short RS485_4_INPUT = 0;
} tTimerID;

tTimerID TimerID;

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



#define INPUT_DATA_LEN 5
const String DEVICE = "h0";
bool DEBUG = 1;

struct {
  String device = "";
  String fn = "";
  String cmd = "";
  String cmdid = "";
  String value = "";
  String err = "";
} Input_data;

void raspSerialPrintln(String fn, String cmd, String cmdid, String value, String err) {
  while (raspSerial.availableForWrite() != 1) {};
  String str = String(DEVICE) + "," + fn + "," + cmd + "," + cmdid + "," + value;
  if (err) {
    str = str + "," + err;
  }
  raspSerial.println(str);
  raspSerial.flush();
}

void raspSerialForward(String str) {
  str.trim();
  if (!str) return;
  while (raspSerial.availableForWrite() != 1) {};
  raspSerial.println(str);
  raspSerial.flush();
}

void rs485_1_Input() {
  String input = "";
  while (rs485_1_Serial.available() > 0) {
    delay(10);
    input = rs485_1_Serial.readStringUntil('\n');
  }
  raspSerialForward(input);
}

void rs485_2_Input() {
  String input = "";
  while (rs485_2_Serial.available() > 0) {
    delay(10);
    input = rs485_2_Serial.readStringUntil('\n');
  }
  raspSerialForward(input);
}

void rs485_3_Input() {
  String input = "";
  while (rs485_3_Serial.available() > 0) {
    delay(10);
    input = rs485_3_Serial.readStringUntil('\n');
  }
  raspSerialForward(input);
}

void rs485_4_Input() {
  String input = "";
  while (rs485_4_Serial.available() > 0) {
    delay(10);
    input = rs485_4_Serial.readStringUntil('\n');
  }
  raspSerialForward(input);
}

int input_parseData(String str) {
  int r = 0, t = 0, i = 0, len = str.length();
  String arr[INPUT_DATA_LEN];
  for (i = 0; i < len; i++)
  {
    if (t == INPUT_DATA_LEN)
    {
      break;
    }
    else if (str.charAt(i) == ',')
    {
      arr[t] = str.substring(r, i);
      r = (i + 1);
      t++;
    }
  }
  arr[t] = str.substring(r, i);

  Input_data.device = arr[0];
  Input_data.fn = arr[1];
  Input_data.cmd = arr[2];
  Input_data.value = arr[3];
  if (arr[4]) {
    Input_data.err = arr[4];
  }

  return 0;
}

void host_handleCmd(void) {
  String resp = "", err = "";
  if (Input_data.cmd == "ping") {
    resp = Input_data.value;
  }

  if (resp) {
    raspSerialPrintln(Input_data.fn, Input_data.cmd, Input_data.cmdid, resp, err);
  }
}

int rasp_handleInputStr(String str) {
  str.trim();
  if (!str) return 0;
  input_parseData(str);
  if (Input_data.device == DEVICE) {
    host_handleCmd();
  }
  else if (Input_data.device == "r1") {
    while (rs485_1_Serial.availableForWrite() != 1) {};
    rs485_1_Serial.println(str);
    rs485_1_Serial.flush();
  }
  else if (Input_data.device == "r2") {
    while (rs485_2_Serial.availableForWrite() != 1) {};
    rs485_2_Serial.println(str);
    rs485_2_Serial.flush();
  }
  else if (Input_data.device == "r3") {
    while (rs485_3_Serial.availableForWrite() != 1) {};
    rs485_3_Serial.println(str);
    rs485_3_Serial.flush();
  }
  else if (Input_data.device == "r4") {
    while (rs485_4_Serial.availableForWrite() != 1) {};
    rs485_4_Serial.println(str);
    rs485_4_Serial.flush();
  }
  return 1;
}

void rasp_input(void)
{
  String input = "";
  while (raspSerial.available() > 0)
  {
    delay(10);
    input = raspSerial.readStringUntil('\n');
  }
  rasp_handleInputStr(input);
}

void setup() {
  // put your setup code here, to run once:
  setCpuFrequencyMhz(40);
  delay(100);
  
  setupOled();
  displayDrawText("Startup");
  setupSerial();

  TimerID.RS485_1_INPUT = t.setInterval(rs485_1_Input, 9);
  TimerID.RS485_2_INPUT = t.setInterval(rs485_2_Input, 11);
  TimerID.RS485_3_INPUT = t.setInterval(rs485_3_Input, 13);
  TimerID.RS485_4_INPUT = t.setInterval(rs485_4_Input, 14);
  
  displayDrawText("Setup finish");
}

void loop() {
  // put your main code here, to run repeatedly:

  t.handle();
}

void displayDrawText(const String &text)
{
  display.displayOn();
  display.clear();
  display.drawStringMaxWidth(0, 0, 128, text);
  display.display();
}

int setupOled(void) {
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
  return 1;
}

int setupSerial(void) {
  pinMode(RS485_RX_1, INPUT);
  pinMode(RS485_TX_1, OUTPUT);
  pinMode(RS485_RX_2, INPUT);
  pinMode(RS485_TX_2, OUTPUT);
  pinMode(RS485_RX_3, INPUT);
  pinMode(RS485_TX_3, OUTPUT);
  pinMode(RS485_RX_4, INPUT);
  pinMode(RS485_TX_4, OUTPUT);
  pinMode(RASP_RX, INPUT);
  pinMode(RASP_TX, OUTPUT);
  rs485_1_Serial.begin(BAUDRATE);
  rs485_2_Serial.begin(BAUDRATE);
  rs485_3_Serial.begin(BAUDRATE);
  rs485_4_Serial.begin(BAUDRATE);
  raspSerial.begin(BAUDRATE);
  return 1;
}
