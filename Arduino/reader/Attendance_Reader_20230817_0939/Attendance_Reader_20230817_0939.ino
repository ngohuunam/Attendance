const String VERSION = "210813.01";
const String DEVICE = "r1";

// OLED DECLARE
#include "SH1106Wire.h"

#define SDA_PIN 9
#define SCL_PIN 7
#define OLED_ADDRESS 0x3c
#define FONT_SIZE 10
#define WIDTH 128
#define HEIGHT 64

SH1106Wire oled(OLED_ADDRESS, SDA_PIN, SCL_PIN); // ADDRESS, SDA, SCL

// TIMER DECLARE
#include <AsyncTimer.h>
AsyncTimer t;

struct
{
  unsigned short LED_FLASH_ON = 0;
  unsigned short LED_FLASH_OFF = 0;
  unsigned short LED_BLUE_BREATH_TIMEOUT = 0;
  unsigned short FINGER_CAPTURE = 0;
  unsigned short FINGER_ENROLL = 0;
  unsigned short FINGER_PAUSE_CAPTURE = 0;
  unsigned short FINGER_PACKETRECIEVEERR = 0;
  unsigned short QR_INPUT = 0;
  unsigned short RS485_INPUT = 0;
  unsigned short OLED_DISPLAY_OFF_TIMEOUT = 0;
  unsigned short CONTROL_NOT_COMMUNICATE = 0;
  unsigned short PING_CONTROL = 0;
  unsigned short BUZZER_TOGGLE_TIMEOUT = 0;
  unsigned short WDT_RESET_INTERVAL = 0;
} TimerID;

const unsigned long CONTROL_NOT_COMMUNICATE_TIMEOUT = 15 * 60 * 1000;
const unsigned long PING_CONTROL_INTERVAL = 0.5 * 60 * 1000;

// SOFTWARE SERIAL DECLARE
#include <SoftwareSerial.h>

#define BAUDRATE 9600

// RS485
#define RS485_RX_PIN 11
#define RS485_TX_PIN 12
EspSoftwareSerial::UART rs485Serial(RS485_RX_PIN, RS485_TX_PIN);

// FINGERPRINT READER
#define FINGER_RX_PIN 16
#define FINGER_TX_PIN 18
EspSoftwareSerial::UART fingerSerial(FINGER_RX_PIN, FINGER_TX_PIN);

// QR READER
#define QR_RX_PIN 3
#define QR_TX_PIN 5
EspSoftwareSerial::UART qrSerial(QR_RX_PIN, QR_TX_PIN);

// LED RGB
#include <RGBLed.h>

#define LED_GREEN_PIN 40
#define LED_RED_PIN 38
#define LED_BLUE_PIN 36

#define LED_FADE_MS 50
#define LED_FADE_STEP 50
#define LED_FADE_INTERVAL 3900

#define LED_FLASH_INTERVAL_SLOWEST 500
#define LED_FLASH_INTERVAL_SLOW 400
#define LED_FLASH_INTERVAL_NORMAL 300
#define LED_FLASH_INTERVAL_FAST 200
#define LED_FLASH_INTERVAL_FASTEST 100

RGBLed led(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN, RGBLed::COMMON_ANODE);

// FINGERPRINT READER DECLARE
#include <Adafruit_Fingerprint.h>
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

#define FINGERPRINT_CAPTURE_INTERVAL 300
#define FINGERPRINT_CAPTURE_SLOW_INTERVAL 500
#define FINGERPRINT_CAPTURE_SLOWER_INTERVAL 600
#define FINGERPRINT_ENROLL_TIMEOUT 1 * 1 * 30 * 1000

const String FINGER_STARTUP_STATE = "start";
bool FINGER_IS_BUSY = false;

// QR READER DECLARE
#define QR_INPUT_INTERVAL 13
#define QR_TOGGLE_PIN 35
const String QR_STARTUP_STATE = "off";

//  RS485 DECLARE
#define RS485_INPUT_INTERVAL 7

// READER
#define BUZZER_PIN 33
bool BUZZER_STATE = LOW;
int BUZZER_REMAIN = 0;
const unsigned long BUZZER_TOGGLE_MS = 100;
bool CONTROL_IS_NOT_COMMUNICATE = false;
