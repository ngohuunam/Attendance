const String VERSION = "0108";

#define LED_BUILTIN_PIN 15

#define RL1_PIN 40
#define RL2_PIN 38
#define RL3_PIN 36
#define RL4_PIN 34
#define RL5_PIN 21
#define RL6_PIN 17

const int RL_PINs[7] = {0, RL1_PIN, RL2_PIN, RL3_PIN, RL4_PIN, RL5_PIN, RL6_PIN};

enum RL_NAMEs {
  MAIN = RL1_PIN,
  PARK = RL2_PIN,
  CONTROL = RL3_PIN,
  READER_1 = RL4_PIN,
  READER_2 = RL5_PIN,
  READER_3 = RL6_PIN
};

#include "SH1106Wire.h"

#define SDA_PIN 8
#define SCL_PIN 10
#define OLED_ADDRESS 0x3c
#define FONT_SIZE 10
#define WIDTH 128
#define HEIGHT 64

SH1106Wire oled(OLED_ADDRESS, SDA_PIN, SCL_PIN); // ADDRESS, SDA, SCL

#include <AsyncTimer.h>
AsyncTimer t;

typedef struct {
  unsigned short RS485_INPUT = 0;
  unsigned short OLED_DISPLAY_OFF_TIMEOUT = 0;
  unsigned short LED_OFF_TIMEOUT = 0;
  unsigned short LED_BREATH = 0;
} tTimerID;

tTimerID TimerID;

#include <SoftwareSerial.h>
#define BAUDRATE 9600

#define RS485_RX 14
#define RS485_TX 13
EspSoftwareSerial::UART rs485Serial(RS485_RX, RS485_TX);

#define RS485_DATA_LEN 6
const String DEVICE = "h0";
bool DEBUG = 1;

struct {
  String device = "";
  String fn = "";
  String cmd = "";
  String cmdid = "";
  String value = "";
  String err = "";
} Rs485_data;
