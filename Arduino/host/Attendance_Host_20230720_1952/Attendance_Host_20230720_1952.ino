const String VERSION = "2007";

#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);
#define FONT_SIZE 10
#define WIDTH 128
#define HEIGHT 64

#include <AsyncTimer.h>
AsyncTimer t;

typedef struct {
  unsigned short RASP_INPUT = 0;
} tTimerID;

tTimerID TimerID;

#include <SoftwareSerial.h>
#define BAUDRATE 9600

#define RASP_RX 19
#define RASP_TX 20
EspSoftwareSerial::UART raspSerial(RASP_RX, RASP_TX);

#define INPUT_DATA_LEN 6
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
  String str = String(DEVICE) + "," + fn + "," + cmd + "," + cmdid;
  if (value.length() > 2) {
    str = str + "," + value;
  }
  if (err.length() > 2) {
    str = str + "," + err;
  }
  raspSerial.print(str + ".");
  raspSerial.flush();
}

int input_parseData(String str) {
  str.trim();
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
  Input_data.cmdid = arr[3];
  Input_data.value = arr[4];
  if (arr[5]) {
    Input_data.err = arr[5];
  }

  return 0;
}

void rasp_response(String fn, String resp) {
  if (resp == Input_data.value) {
    raspSerialPrintln(fn, Input_data.cmd, Input_data.cmdid, resp, "");
  } else {
    raspSerialPrintln(fn, Input_data.cmd, Input_data.cmdid, "err", resp);
  }
}

void host_handleCmd(void) {
  String resp = Input_data.value;
  if (Input_data.cmd == "ping") {
  }
  else if (Input_data.fn == "rl") {
  }
  else {
    resp = "NO CMD";
  }
  return rasp_response(Input_data.fn, resp);
}

int rasp_handleInputStr(String str) {
  if (str.length() < 4 || str.startsWith(DEVICE) == false) return 0;
  input_parseData(str);
  host_handleCmd();
  return 1;
}

void rasp_input(void)
{
  while (raspSerial.available() > 0)
  {
    delay(10);
    String str = raspSerial.readStringUntil('.');
    rasp_handleInputStr(str);
  }
}

void devSendcmd() {
  String str = "r1,f,captured," + String(random(122, 333)) + "," + String(random(1, 5));
}

void setup() {
  // put your setup code here, to run once:
  //  setCpuFrequencyMhz(160);
  //  delay(100);

  setupOled();
  displayDrawText(1, "Startup");
  setupSerial();

  displayDrawString(1, "DONE SETUP...");
}

void loop() {
  // put your main code here, to run repeatedly:
  rasp_input();
  t.handle();
}

int displayClearLine(int lineNo, int height) {
  int line = FONT_SIZE * (lineNo - 1);
  display.setColor(BLACK);
  display.fillRect(0, line, WIDTH, height + 2);
  display.setColor(WHITE);
  return line;
}

void displayDrawString(int lineNo, const String &text)
{
  int line = displayClearLine(lineNo, FONT_SIZE);
  display.drawString(0, line, text);
  display.display();
}

void displayDrawText(int lineNo, const String &text)
{
  int line = displayClearLine(lineNo, HEIGHT);
  display.drawStringMaxWidth(0, line, WIDTH, text);
  display.display();
  //  t.setTimeout(oledClearText, OLED_DISPLAY_OFF_TIMEOUT);
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
  Serial.begin(BAUDRATE);
  pinMode(RASP_RX, INPUT);
  pinMode(RASP_TX, OUTPUT);
  raspSerial.begin(BAUDRATE);
  return 1;
}
