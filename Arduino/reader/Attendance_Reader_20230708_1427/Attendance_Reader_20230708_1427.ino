// OLED DECLARE
#include "roboto.h"
#include "SH1106Wire.h"

#define SDA_PIN 9
#define SCL_PIN 7
#define OLED_ADDRESS 0x3c
#define FONT_SIZE 9

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
  unsigned short QR_INPUT = 0;
  unsigned short RS485_INPUT = 0;
  unsigned short OLED_DISPLAY = 0;
} TimerID;


// SOFTWARE SERIAL DECLARE
#include <SoftwareSerial.h>

#define BAUDRATE 57600

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
#define LED_FADE_INTERVAL 4000

#define LED_FLASH_INTERVAL_SLOWEST 800
#define LED_FLASH_INTERVAL_SLOW 700
#define LED_FLASH_INTERVAL_NORMAL 500
#define LED_FLASH_INTERVAL_FAST 300
#define LED_FLASH_INTERVAL_FASTEST 200

RGBLed led(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN, RGBLed::COMMON_ANODE);

// FINGERPRINT READER DECLARE
#include <Adafruit_Fingerprint.h>
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

#define FINGERPRINT_CAPTURE_INTERVAL 300
#define FINGERPRINT_ENROLL_TIMEOUT 1 * 3 * 60 * 1000

// QR READER DECLARE
#define QR_INPUT_INTERVAL 13

//  RS485 DECLARE
#define RS485_INPUT_INTERVAL 11

/////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485
// RS485 FUNCTIONS
const String DEVICE = "r1";
const int RS485_DATA_LEN = 5;
bool DEBUG = 1;
String cmdID = "";

struct {
  String device = "";
  String fn = "";
  String cmd = "";
  String cmdid = "";
  String value = "";
  String err = "";
} Rs485_data;

int rs485_parseData(String str) {
  str.trim();
  int r = 0, t = 0, i = 0, len = str.length();
  String arr[RS485_DATA_LEN];
  for (i = 0; i < len; i++)
  {
    if (t == RS485_DATA_LEN)
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

  Rs485_data.device = arr[0];
  Rs485_data.fn = arr[1];
  Rs485_data.cmd = arr[2];
  Rs485_data.value = arr[3];
  if (arr[4]) {
    Rs485_data.err = arr[4];
  }

  return 0;
}

int rs485_println(String fn, String cmd, String cmdid, String value, String err) {
  while (rs485Serial.availableForWrite() != 1) {};
  String str = String(DEVICE) + "," + fn + "," + cmd + "," + cmdid + "," + value;
  if (err) {
    str = str + "," + err;
  }
  rs485Serial.println(str);
  rs485Serial.flush();
  return 1;
}

int rs485_send(String fn, String cmd, String value, String err) {
  return rs485_println(fn, cmd, cmdID, value, err);
}

int rs485_handleInputStr(String _str) {
  rs485_parseData(_str);
  if (Rs485_data.device != DEVICE) {
    return 0;
  }
  if (Rs485_data.fn == "f") {
    finger_handleCmd();
  } else if (Rs485_data.fn == "q") {
    qr_handleCmd();
  } else if (Rs485_data.fn == "r") {
    reader_handleCmd();
  } else if (Rs485_data.fn == "o") {
    oled_handleCmd();
  } else if (Rs485_data.fn == "l") {
    led_handleCmd();
  }
  return 1;
}

void rs485_input(void)
{
  if (rs485Serial.available() > 0)
  {
    delay(10);
    String _str = rs485Serial.readStringUntil('\n');
    rs485_handleInputStr(_str);
  }
}
/////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485  /////  RS485

/////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED
int* led_color;
unsigned long led_ms;

void ledOn(void)
{
  led.fadeIn(led_color, LED_FADE_STEP, LED_FADE_MS);
}

void ledSetColor(int _color[3])
{
  led_color = _color;
}

void ledSetMs(unsigned long _ms)
{
  led_ms = LED_FADE_INTERVAL * 3;
  if (_ms > led_ms) {
    led_ms = _ms;
  }
}

void ledOnWithColor(int _color[3])
{
  ledSetColor(_color);
  ledOn();
}

void ledOff()
{
  t.cancel(TimerID.LED_FLASH_ON);
  t.cancel(TimerID.LED_FLASH_OFF);
  t.cancel(TimerID.LED_BLUE_BREATH_TIMEOUT);
  led.fadeOut(led_color, LED_FADE_STEP, LED_FADE_MS);
}

void ledBlueOn(void)
{
  ledOnWithColor(RGBLed::BLUE);
}

void ledFlash(void)
{
  ledOff();
  TimerID.LED_FLASH_ON = t.setTimeout(ledOn, led_ms / 2);
  TimerID.LED_FLASH_OFF = t.setTimeout(ledFlash, led_ms);
}

void ledFlashNotify(unsigned long ms)
{
  ledOff();
  TimerID.LED_FLASH_ON = t.setTimeout(ledOn, ms / 2);
  TimerID.LED_FLASH_OFF = t.setTimeout(ledFlash, ms);
}

void ledOnNotify(int _color[3])
{
  led.fadeIn(_color, LED_FADE_STEP, LED_FADE_MS);
}

void ledNotify(int _color[3], unsigned long ms)
{
  ledOff();
  ledOnNotify(_color);
  TimerID.LED_BLUE_BREATH_TIMEOUT = t.setTimeout(ledBlueBreath, ms);
}

void ledSuccess()
{
  ledNotify(RGBLed::GREEN, 3000);
}

void ledError()
{
  ledNotify(RGBLed::RED, 2000);
}

void ledWaiting(unsigned long ms)
{
  ledNotify(RGBLed::YELLOW, ms);
}

void ledFlashErrorSlowest()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_SLOWEST);
}

void ledFlashErrorSlow()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_SLOW);
}

void ledFlashErrorNormal()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_NORMAL);
}

void ledFlashErrorFast()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_FAST);
}

void ledFlashErrorFastest()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_FASTEST);
}

void ledFlashWithInterval(unsigned long ms)
{
  ledSetMs(ms);
  ledFlash();
}

void ledFlashWithColor(int _color[3])
{
  ledSetColor(_color);
  ledFlash();
}

void ledFlashWithColorAndInterval(int _color[3], unsigned long ms)
{
  ledSetMs(ms);
  ledSetColor(_color);
  ledFlash();
}

void ledBreath()
{
  ledFlashWithInterval(LED_FADE_INTERVAL);
}

void ledBreathWithColor(int _color[3])
{
  ledSetColor(_color);
  ledBreath();
}

void ledBlueBreath()
{
  ledBreathWithColor(RGBLed::BLUE);
}

void led_handleCmd(void) {

  String resp = "", err = "";
  if (Rs485_data.cmd == "off") {
    ledOff();
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "on") {
    ledOn();
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "breath") {
    if (Rs485_data.value = "red") {
      ledBreathWithColor(RGBLed::RED);
    } else if (Rs485_data.value = "blue") {
      ledBreathWithColor(RGBLed::BLUE);
    } else if (Rs485_data.value = "green") {
      ledBreathWithColor(RGBLed::GREEN);
    } else if (Rs485_data.value = "yellow") {
      ledBreathWithColor(RGBLed::YELLOW);
    } else {
      ledBreath();
    }
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "ms") {
    ledSetMs( ( unsigned long )Rs485_data.value.toInt());
  } else if (Rs485_data.cmd == "flash") {
    if (Rs485_data.value = "red") {
      ledFlashWithColor(RGBLed::RED);
    } else if (Rs485_data.value = "blue") {
      ledFlashWithColor(RGBLed::BLUE);
    } else if (Rs485_data.value = "green") {
      ledFlashWithColor(RGBLed::GREEN);
    } else if (Rs485_data.value = "yellow") {
      ledFlashWithColor(RGBLed::YELLOW);
    } else {
      ledFlash();
    }
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "red") {
    ledOnWithColor(RGBLed::RED);
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "yellow") {
    ledOnWithColor(RGBLed::YELLOW);
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "green") {
    ledOnWithColor(RGBLed::GREEN);
    resp = Rs485_data.value;
  }

  if (resp) {
    rs485_println("l", Rs485_data.cmd, Rs485_data.cmdid, resp, err);
  }
}

/////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED  /////  LED

/////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED
const unsigned long OLED_DISPLAY_OFF_TIMEOUT = 5000;

void oledDrawText(const String &text)
{
  t.cancel(TimerID.OLED_DISPLAY);
  oled.displayOn();
  oled.clear();
  //  oled.setFont(ArialMT_Plain_10);
  //  oled.setTextAlignment(TEXT_ALIGN_LEFT);
  oled.drawStringMaxWidth(0, 0, 128, text);
  oled.display();
  t.setTimeout(oledClearText, OLED_DISPLAY_OFF_TIMEOUT);
}

void oledClearText(void)
{
  oled.clear();
  oled.drawStringMaxWidth(0, 0, 128, "                                                                                                                                        ");
  oled.display();
  oled.displayOff();
}

void setupOled(void)
{
  //  pinMode(RST_OLED, OUTPUT);
  //  digitalWrite(RST_OLED, LOW);
  //  delay(20);
  //  digitalWrite(RST_OLED, HIGH);
  oled.setFont(ArialMT_Plain_10);
  oled.setTextAlignment(TEXT_ALIGN_LEFT);

  oled.init();
  oled.flipScreenVertically();
  oledDrawText("START UP...");
}

void oled_handleCmd(void) {

  String resp = "", err = "";
  if (Rs485_data.cmd == "off") {
    oled.displayOff();
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "on") {
    oled.displayOn();
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "flip") {
    oled.flipScreenVertically();
    resp = Rs485_data.value;
  }

  if (resp) {
    rs485_println("o", Rs485_data.cmd, Rs485_data.cmdid, resp, err);
  }
}
/////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED  /////  OLED

/////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER
String finger_capture() {
  String _status = "Capturing...";
  uint8_t p = finger.getImage();
  switch (p)
  {
    case FINGERPRINT_OK:
      _status = "Image taken";
      oledDrawText(_status);
      ledOnNotify(RGBLed::YELLOW);
      break;
    case FINGERPRINT_NOFINGER:
      return "";
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = "Get Image: Communication error";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_IMAGEFAIL:
      _status = "Get Image: Imaging error";
      oledDrawText(_status);
      ledError();
      return _status;
    default:
      _status = "Get Image: Unknown error";
      oledDrawText(_status);
      ledError();
      return _status;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p)
  {
    case FINGERPRINT_OK:
      _status = "Convert Image: converted";
      break;
    case FINGERPRINT_IMAGEMESS:
      _status = "Convert Image: Image too messy";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = "Convert Image: Communication error";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_FEATUREFAIL:
      _status = "Convert Image: Could not find fingerprint features";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_INVALIDIMAGE:
      _status = "Convert Image: Could not find fingerprint features";
      oledDrawText(_status);
      ledError();
      return _status;
    default:
      _status = "Convert Image: Unknown error";
      oledDrawText(_status);
      ledError();
      return _status;
  }

  // OK converted!
  p = finger.fingerSearch();
  switch (p)
  {
    case FINGERPRINT_OK:
      _status = "ok";
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = "Finger search: Communication error";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_NOTFOUND:
      _status = "Finger search: Did not find a match";
      oledDrawText(_status);
      ledError();
      return _status;
    default:
      _status = "Finger search: Unknown error";
      oledDrawText(_status);
      ledError();
      return _status;
  }
  // found a match!
  oledDrawText("Wait for authorize...");
  ledSuccess();
  rs485_send("f", "captured", String(finger.fingerID), "");
  return _status;
}

String finger_enroll(String id) {
  String _status = "Waiting finger on...";
  oledDrawText(_status);
  ledOnNotify(RGBLed::YELLOW);
  unsigned long startedWaiting = millis();
  int p = -1;
  while (p != FINGERPRINT_OK) {
    if (millis() - startedWaiting > FINGERPRINT_ENROLL_TIMEOUT) {
      _status = "Enroll finger timeout";
      oledDrawText(_status);
      ledError();
      return _status;
    }
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        _status = "Image taken";
        break;
      case FINGERPRINT_NOFINGER:
        _status += ".";
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        _status = "Get image Communication error";
        oledDrawText(_status);
        ledError();
        break;
      case FINGERPRINT_IMAGEFAIL:
        _status = "Get image Imaging error";
        oledDrawText(_status);
        ledError();
        break;
      default:
        _status = "Get image Unknown error";
        oledDrawText(_status);
        ledError();
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      _status = "Image converted";
      break;
    case FINGERPRINT_IMAGEMESS:
      _status = "Image convert 1: Image too messy";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = "Image convert 1: Communication error";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_FEATUREFAIL:
      _status = "Image convert 1: Could not find fingerprint features";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_INVALIDIMAGE:
      _status = "Image convert 1: Could not find fingerprint features";
      oledDrawText(_status);
      ledError();
      return _status;
    default:
      _status = "Image convert 1: Unknown error";
      oledDrawText(_status);
      ledError();
      return _status;
  }

  _status = "Remove finger";
  oledDrawText(_status);
  ledOnNotify(RGBLed::GREEN);
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  _status = "Place same finger again";
  oledDrawText(_status);
  ledOnNotify(RGBLed::YELLOW);
  startedWaiting = millis();
  while (p != FINGERPRINT_OK) {
    if (millis() - startedWaiting > FINGERPRINT_ENROLL_TIMEOUT) {
      _status = "Capture finger again timeout";
      oledDrawText(_status);
      ledError();
      return _status;
    }
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        _status = "Image taken";
        break;
      case FINGERPRINT_NOFINGER:
        _status += ".";
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        _status = "Get image Communication error";
        oledDrawText(_status);
        ledError();
        break;
      case FINGERPRINT_IMAGEFAIL:
        _status = "Get image Imaging error";
        oledDrawText(_status);
        ledError();
        break;
      default:
        _status = "Get image Unknown error";
        oledDrawText(_status);
        ledError();
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      _status = "Image converted";
      break;
    case FINGERPRINT_IMAGEMESS:
      _status = "Image convert 2: Image too messy";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = "Image convert 2: Communication error";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_FEATUREFAIL:
      _status = "Image convert 2: Could not find fingerprint features";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_INVALIDIMAGE:
      _status = "Image convert 2: Could not find fingerprint features";
      oledDrawText(_status);
      ledError();
      return _status;
    default:
      _status = "Image convert 2: Unknown error";
      oledDrawText(_status);
      ledError();
      return _status;
  }

  // OK converted!

  p = finger.createModel();
  switch (p) {
    case FINGERPRINT_OK:
      _status = "Image converted";
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = "Create model: Communication error";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_ENROLLMISMATCH:
      _status = "Create model: Fingerprints did not match";
      oledDrawText(_status);
      ledError();
      return _status;
    default:
      _status = "Create model: Unknown error";
      oledDrawText(_status);
      ledError();
      return _status;
  }

  p = finger.storeModel(id.toInt());
  switch (p) {
    case FINGERPRINT_OK:
      _status = "Finger Stored!";
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = "Store model Communication error";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_BADLOCATION:
      _status = "Store model Could not store in that location";
      oledDrawText(_status);
      ledError();
      return _status;
    case FINGERPRINT_FLASHERR:
      _status = "Store model Error writing to flash";
      oledDrawText(_status);
      ledError();
      return _status;
    default:
      _status = "Store model Unknown error";
      oledDrawText(_status);
      ledError();
      return _status;
  }
  oledDrawText("Finger created, waiting for store on server...");
  ledSuccess();
  return _status;
}

String finger_delete(String id) {
  uint8_t p = -1;
  String resp = Rs485_data.value;

  p = finger.deleteModel(id.toInt());

  if (p == FINGERPRINT_OK) {
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    resp = "Communication error";
  } else if (p == FINGERPRINT_BADLOCATION) {
    resp = "Could not delete in that location";
  } else if (p == FINGERPRINT_FLASHERR) {
    resp = "Error writing to flash";
  } else {
    resp = "Unknown error: ";
    resp += String(p, HEX);
  }
  return resp;
}

String finger_emptyDatabase() {
  finger.emptyDatabase();
  return Rs485_data.value;
}

int finger_handleCmd(void) {

  unsigned long startedWaiting = millis();
  while (fingerSerial.available() > 0 && (millis() - startedWaiting < 20)) {};

  String resp = Rs485_data.value;
  String err = "";
  if (Rs485_data.cmd == "ping") {
    if (!finger.verifyPassword()) {
      resp = "";
    }
  } else if (Rs485_data.cmd == "enroll") {
    if (Rs485_data.value == "err") {
      String _status = "Fingerprint enroll store on server err!";
      oledDrawText(_status);
      ledError();
    } else if (Rs485_data.value == "done") {
      String _status = "FIngerprint enroll success!";
      oledDrawText(_status);
      ledSuccess();
    } else {
      resp = finger_enroll(Rs485_data.value);
    }
  } else if (Rs485_data.cmd == "delete") {
    resp = finger_delete(Rs485_data.value);
  } else if (Rs485_data.cmd == "empty") {
    resp = finger_emptyDatabase();
  } else if (Rs485_data.cmd == "getid") {
    finger.getParameters();
    resp = String(finger.system_id, HEX);
  } else if (Rs485_data.cmd == "stop") {
    t.cancel(TimerID.FINGER_CAPTURE);
  } else if (Rs485_data.cmd == "start") {
    TimerID.FINGER_CAPTURE = t.setInterval(finger_capture, FINGERPRINT_CAPTURE_INTERVAL);
  }

  if (resp) {
    rs485_println("f", Rs485_data.cmd, Rs485_data.cmdid, resp, err);
  }
  return 1;
}
/////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER  /////  FINGER


/////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR  /////  QR
void qr_write(uint32_t _type, uint32_t _address, uint32_t _data, uint32_t _check1, uint32_t _check2)
{
  uint8_t _buffer[9];

  _buffer[0] = 0x7E;
  _buffer[1] = 0;
  _buffer[2] = _type;
  _buffer[3] = 1;
  _buffer[4] = 0;
  _buffer[5] = _address;
  _buffer[6] = _data;
  _buffer[7] = _check1;
  _buffer[8] = _check2;

  qrSerial.write(_buffer, 9);
}

void qr_input(void)
{
  char _read[255];
  uint32_t byte_counter = 0;
  while (qrSerial.available() > 0)
  {
    delay(10);
    _read[byte_counter] = qrSerial.read();
    byte_counter++;
  }
  _read[byte_counter] = 0; // Add string terminating zero
  if (byte_counter > 0) {
    if ((3 == _read[0]) && (0 == _read[1]) && (0 == _read[2]) && (1 == _read[3]) && (0 == _read[4]))
    {
      rs485_println("q", "ping", Rs485_data.cmdid, Rs485_data.value, "");
    }
    else
    {
      String cmdid = String(random(100, 200));
      rs485_println("q", "read", cmdid, String(_read), "");
    }
  }
}

void qr_handleCmd(void) {

  unsigned long startedWaiting = millis();
  while (qrSerial.available() > 0 && (millis() - startedWaiting < 20)) {};

  String resp = "", err = "";
  if (Rs485_data.cmd == "ping") {
    qr_write(10, 0, 0, 0x30, 0x1A);
  } else if (Rs485_data.cmd == "stop") {
    t.cancel(TimerID.QR_INPUT);
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "start") {
    TimerID.QR_INPUT = t.setInterval(qr_input, QR_INPUT_INTERVAL);
    resp = Rs485_data.value;
  }

  if (resp) {
    rs485_println("q", Rs485_data.cmd, Rs485_data.cmdid, resp, err);
  }
}

void reader_handleCmd(void) {
  String resp = "", err = "";
  if (Rs485_data.cmd == "ping") {
    resp = Rs485_data.value;
  } else if (Rs485_data.cmd == "reset") {
    ESP.restart();
  }

  if (resp) {
    rs485_println("r", Rs485_data.cmd, Rs485_data.cmdid, resp, err);
  }
}

void setupSerial(void)
{

  oledDrawText("Start serial...");

  pinMode(RS485_RX_PIN, INPUT);
  pinMode(RS485_TX_PIN, OUTPUT);

  pinMode(FINGER_RX_PIN, INPUT);
  pinMode(FINGER_TX_PIN, OUTPUT);

  pinMode(QR_RX_PIN, INPUT);
  pinMode(QR_TX_PIN, OUTPUT);

  rs485Serial.begin(BAUDRATE);

  fingerSerial.begin(9600);
  finger.setBaudRate(FINGERPRINT_BAUDRATE_57600);
  fingerSerial.begin(BAUDRATE);

  qrSerial.begin(BAUDRATE);

  oledDrawText("Setup serial done!");
}

void setup() {
  // put your setup code here, to run once:
  setCpuFrequencyMhz(40);
  delay(100);

  setupOled();
  delay(10);

  setupSerial();
  delay(10);

  led.off();
  led.brightness(50);
  ledBlueBreath();

  TimerID.FINGER_CAPTURE = t.setInterval(finger_capture, FINGERPRINT_CAPTURE_INTERVAL);
  TimerID.QR_INPUT = t.setInterval(qr_input, QR_INPUT_INTERVAL);
  TimerID.RS485_INPUT =  t.setInterval(rs485_input, RS485_INPUT_INTERVAL);
}

void loop() {
  // put your main code here, to run repeatedly:
  t.handle();
}
