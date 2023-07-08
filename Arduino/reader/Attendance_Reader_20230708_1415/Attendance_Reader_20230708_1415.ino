#include "roboto.h"
#include "SH1106Wire.h"
SH1106Wire oled(0x3c, 9, 7); // ADDRESS, SDA, SCL

#define FONT_SIZE 9

#include <AsyncTimer.h>
AsyncTimer t;

struct
{
  unsigned short LED_FADE_IN = 0;
  unsigned short LED_FADE_OUT = 0;
  unsigned short FINGER_READING = 0;
  unsigned short FINGER_CHECK = 0;
  unsigned short QR_READING = 0;
  unsigned short QR_HEART_BEATING = 0;
  unsigned short QR_HEART_BEATING_HANDLE_NO_ANSWER = 0;
  unsigned short RS485_READING = 0;
  unsigned short RS485_PING_HOST = 0;
  unsigned short RS485_PING_HOST_NO_ANSWER = 0;
  unsigned short OLED_DRAW_INFO = 0;
} TimerID;

#include <SoftwareSerial.h>

#define RS485_RX 11
#define RS485_TX 12

EspSoftwareSerial::UART rs485Serial(RS485_RX, RS485_TX);

#define RS485_PING_HOST_INTERVAL 5000
#define RS485_PING_HOST_NO_ANSWER_COUNT 3

struct
{
  uint32_t rs485PingHostCount = 0;
  uint32_t re485PingHostAnswerCount = 0;
  uint32_t re485PingHostNoAnswerCount = 0;
  String status = "Start up";
  String read = "Nothing";
  String send = "Nothing";
} Rs485State;

const String QR_ID = "q";
const String FINGER_ID = "f";
const String PING = "p";

#define FINGER_RX 16
#define FINGER_TX 18

EspSoftwareSerial::UART fingerSerial(FINGER_RX, FINGER_TX);

#define FINGER_CAPTURE_INTERVAL 300

struct
{
  byte lastVerify = 99;
  byte currentVerify = 95;
  uint8_t enrollID = 0;
  uint8_t deleteID = 0;
  String work = "";
  String status = "Start up";
  String read = "Nothing";
} FingerState;

#define QR_RX 3
#define QR_TX 5

EspSoftwareSerial::UART qrSerial(QR_RX, QR_TX);

#define SERIAL_QR_HEADER 0x7E

#define QR_HEART_BEAT_INTERVAL 5000
#define QR_HEART_BEAT_NO_ANSWER_COUNT 3
#define QR_HEART_BEAT_NO_ANSWER_NOTIFY_COUNT 3

struct
{
  uint32_t heartBeatErrorCount = 0;
  String status = "Start up";
  String read = "Nothing";
  byte heartbeatFailNotifyCounter = 0;
} QrState;

#include <Adafruit_Fingerprint.h>
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

#define READER_ID 1

#include <RGBLed.h>

#define LED_GREEN_PIN 40
#define LED_RED_PIN 38
#define LED_BLUE_PIN 36

#define LED_FADE_MS 2000
#define LED_FADE_STEP 50
#define LED_FADE_INTERVAL 4000
#define LED_FLASH_ERROR_FINGER_INTERVAL 2000

RGBLed led(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN, RGBLed::COMMON_ANODE);

#define BUZZLE_PIN 37

uint32_t chipId = 0;

void getChipID() {
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
}

void oledDrawInfo(void)
{
  oled.clear();

  String _line_0 = "Fg status: " + FingerState.status;
  String _line_1 = "Fg read: " + FingerState.read;
  oled.drawString(0, 0 * FONT_SIZE, _line_0);
  oled.drawString(0, 1 * FONT_SIZE, _line_1);

  String _line_2 = "CPU Hz: " + QrState.status;
  oled.drawString(0, 2 * FONT_SIZE, _line_2);
  String _line_3 = "Str: " + QrState.read;
  oled.drawString(0, 3 * FONT_SIZE, _line_3);

  String _line_4 = "RS status: " + Rs485State.status;
  oled.drawString(0, 4 * FONT_SIZE, _line_4);
  String _line_5 = "RS read: " + Rs485State.read;
  oled.drawString(0, 5 * FONT_SIZE, _line_5);
  String _line_6 = "RS send: " + Rs485State.send;
  oled.drawString(0, 6 * FONT_SIZE, _line_6);

  oled.display();
}

void setup()
{
  // put your setup code here, to run once:

  //  pinMode(BUZZLE_PIN, OUTPUT);

  setCpuFrequencyMhz(40);
  delay(100);

  setupOled();
  delay(10);
  //
  setupSerial();
  delay(10);

  led.off();
  led.brightness(50);
  ledBreathSimple();

  fingerOnSerialTimer();
  qrOnSerialTimer();

  TimerID.RS485_READING = t.setInterval(rs485Input, 11);
  TimerID.RS485_PING_HOST = t.setInterval(rs485PingHost, RS485_PING_HOST_INTERVAL);

  oledDrawText(0, "Setup done!");

  TimerID.OLED_DRAW_INFO = t.setInterval(oledDrawInfo, FINGER_CAPTURE_INTERVAL);
}

void buzzle (void) {
  digitalWrite(BUZZLE_PIN, LOW);
  delay(300);
  buzzleOff();
}

void buzzleOff (void) {
  digitalWrite(BUZZLE_PIN, HIGH);
}

void loop()
{
  // put your main code here, to run repeatedly:
  t.handle();
}

// LED

void ledOn(int _color[3])
{
  led.fadeIn(_color, LED_FADE_STEP, 100);
}

void ledOff(int _color[3])
{
  led.fadeOut(_color, LED_FADE_STEP, 100);
}

void ledBlueOn(void)
{
  ledOn(RGBLed::BLUE);
}

void ledBlueOff(void)
{
  ledOff(RGBLed::BLUE);
}

void ledBreathSimple()
{
  t.cancel(TimerID.LED_FADE_IN);
  t.cancel(TimerID.LED_FADE_OUT);
  ledBlueOff();
  TimerID.LED_FADE_IN = t.setTimeout(ledBlueOn, LED_FADE_INTERVAL / 2);
  TimerID.LED_FADE_OUT = t.setTimeout(ledBreathSimple, LED_FADE_INTERVAL);
}

void ledStopBreath()
{
  t.cancel(TimerID.LED_FADE_OUT);
  t.cancel(TimerID.LED_FADE_IN);
  led.off();
}

void setupSerial(void)
{

  oledDrawText(0, "Start serial...");

  pinMode(RS485_RX, INPUT);
  pinMode(RS485_TX, OUTPUT);

  pinMode(FINGER_RX, INPUT);
  pinMode(FINGER_TX, OUTPUT);

  pinMode(QR_RX, INPUT);
  pinMode(QR_TX, OUTPUT);

  rs485Serial.begin(9600);

  fingerSerial.begin(57600);
  finger.setBaudRate(FINGERPRINT_BAUDRATE_9600);
  fingerSerial.begin(9600);

  qrSerial.begin(9600);

  oledDrawText(0, "Setup serial done!");
}

// RS485

void split(String *_resStrArr, String _string, char _delim, int _len)
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

bool isRs485Busy = false;

void rs485SendStrToHost(String _sensor, String _action, String _value)
{
  isRs485Busy = true;
  QrState.status = getCpuFrequencyMhz();
  Rs485State.send = "r," + String(READER_ID) + "," + _sensor + "," + _action + "," + _value;
  rs485Serial.println(Rs485State.send);
  isRs485Busy = false;
}

void rs485SendStrToHostSafety (String _sensor, String _action, String _value) {
  unsigned long startedWaiting = millis();
  while (isRs485Busy && (millis() - startedWaiting < 20)) {};
  rs485SendStrToHost(_sensor, _action, _value);
}

void rs485PingHostNoAnswerHandle(void)
{
  t.cancel(TimerID.RS485_PING_HOST_NO_ANSWER);
  TimerID.RS485_PING_HOST_NO_ANSWER = 0;
  Rs485State.re485PingHostNoAnswerCount++;
  Rs485State.status = "RS485 Ping host err: " + String(Rs485State.re485PingHostNoAnswerCount);
}

void rs485PingHost(void)
{
  Rs485State.rs485PingHostCount++;
  rs485SendStrToHostSafety("r", PING, String(Rs485State.rs485PingHostCount));
  if (TimerID.RS485_PING_HOST_NO_ANSWER == 0)
  {
    TimerID.RS485_PING_HOST_NO_ANSWER = t.setTimeout(rs485PingHostNoAnswerHandle, RS485_PING_HOST_INTERVAL * RS485_PING_HOST_NO_ANSWER_COUNT);
  }
}

void qrOffSerialTimer() {
  t.cancel(TimerID.QR_READING);
  t.cancel(TimerID.QR_HEART_BEATING);
  rs485SendStrToHostSafety(QR_ID, "off", "done");
}

void qrOnSerialTimer() {
  TimerID.QR_READING = t.setInterval(qrInput, 13);
  TimerID.QR_HEART_BEATING = t.setInterval(qrHeartbeat, QR_HEART_BEAT_INTERVAL);
  rs485SendStrToHostSafety(QR_ID, "on", "done");
}

void fingerOffSerialTimer() {
  t.cancel(TimerID.FINGER_READING);
  rs485SendStrToHostSafety(FINGER_ID, "off", "done");
}

void fingerOnSerialTimer() {
  TimerID.FINGER_READING = t.setInterval(fingerCapturing, FINGER_CAPTURE_INTERVAL);
  rs485SendStrToHostSafety(FINGER_ID, "on", "done");
}

void handleRs485InputStr(String _str)
{
  t.cancel(TimerID.RS485_PING_HOST_NO_ANSWER);
  _str.trim();
  int _len = _str.length();
  String _strArr[_len];
  split(_strArr, _str, ',', _len);
  String _device = _strArr[0];
  String _id = _strArr[1];
  String _fn = _strArr[2];
  String _cmd = _strArr[3];
  String _value = _strArr[4];
  String _pingStr = "_device: " + _device + ", _id: " + _id + ", _fn: " + _fn + ", _cmd: " + _cmd + ", _value: " + _value;
  //  Serial.println(_pingStr);
  QrState.read = String(_pingStr);
  if (_device == "r" && _id == String(READER_ID))
  {
    if (_cmd == "pan")
    {
      Rs485State.re485PingHostAnswerCount++;
    }
    else if (_fn == "l")
    {
      if (_cmd == "red") {
        ledStopBreath();
        ledOn(RGBLed::RED);
      }
      else if (_cmd == "breath")
      {
        ledBreathSimple();
      }
    }
    else if (_fn == "f")
    {
      if (_cmd == "fe")
      {
        ledStopBreath();
        if (_value == "err") {
          String _err = _strArr[5];
          FingerState.status = _err;
          oledDrawInfo();
          ledOn(RGBLed::RED);
          t.setTimeout(ledBreathSimple, 5000);
        } else if (_value == "done") {
          FingerState.status = "Enroll success";
          oledDrawInfo();
          ledOn(RGBLed::GREEN);
          t.setTimeout(ledBreathSimple, 5000);
        } else {
          ledOn(RGBLed::YELLOW);
          uint8_t id = _value.toInt();
          getFingerprintEnroll(id);
        }
      }
      else if (_cmd == "del")
      {
        uint8_t id = _value.toInt();
        deleteFingerprint(id);
      }
      else if (_cmd == "empty")
      {
        fingerEmptyDatabase();
      }
      else if (_cmd == "off")
      {
        fingerOffSerialTimer();
      }
      else if (_cmd == "on")
      {
        fingerOnSerialTimer();
      }
    }
    else if (_fn == "r")
    {
      if (_cmd == "rst") {
        ESP.restart();
      }
    }
    else if (_fn == "q")
    {
      if (_cmd == "off") {
        qrOffSerialTimer();
      } else if (_cmd == "on") {
        qrOnSerialTimer();
      }
    }
    else if (_fn == "o")
    {
      if (_cmd == "off") {
        oled.displayOff();
      } else if (_cmd == "on") {
        oled.displayOn();
      } else if (_cmd == "flip") {
        oled.flipScreenVertically();
      }
    }
  }
}

void rs485Input(void)
{
  if (rs485Serial.available() > 0)
  {
    delay(10);
    String _readStr = rs485Serial.readStringUntil('\n');
    Serial.print("Rs485 input: ");
    Serial.println(_readStr);
    handleRs485InputStr(_readStr);
    Rs485State.read = _readStr;
  }
}

// QR

void qrInput(void)
{
  char _read[9];
  uint32_t byte_counter = 0;
  while (qrSerial.available() > 0)
  {
    delay(10);
    _read[byte_counter] = qrSerial.read();
    if (byte_counter >= 8)
    {
      break;
    }
    byte_counter++;
  }
  _read[byte_counter] = 0; // Add string terminating zero
  if (byte_counter > 0) {
    if ((0 == _read[1]) && (3 == _read[0]) && (0 == _read[2]) && (1 == _read[3]) && (0 == _read[4]))
    {
      // 03 00 00 01 00 33 31                              - Heartbeat response
      Serial.println("QR Heartbeat ok");
      QrState.status = "Working";
      t.cancel(TimerID.QR_HEART_BEATING_HANDLE_NO_ANSWER);
      TimerID.QR_HEART_BEATING_HANDLE_NO_ANSWER = 0;
      QrState.heartbeatFailNotifyCounter = 0;
    }
    else
    {
      QrState.read = String(_read);
      Serial.print("QR read str: ");
      Serial.println(QrState.read);
      rs485SendStrToHostSafety(QR_ID, QrState.read, "");
    }
  }
}

void hwSend(SoftwareSerial *_serial, uint32_t _header, uint32_t _type, uint32_t _address, uint32_t _data, uint32_t _check1, uint32_t _check2)
{
  uint8_t _buffer[9];

  _buffer[0] = _header;
  _buffer[1] = 0;
  _buffer[2] = _type;
  _buffer[3] = 1;
  _buffer[4] = 0;
  _buffer[5] = _address;
  _buffer[6] = _data;
  _buffer[7] = _check1;
  _buffer[8] = _check2;

  _serial->write(_buffer, 9);
}

void qrSend(uint32_t _type, uint32_t _address, uint32_t _data, uint32_t _check1, uint32_t _check2)
{
  uint32_t _header = SERIAL_QR_HEADER;
  hwSend(&qrSerial, _header, _type, _address, _data, _check1, _check2);
}

void qrHeartbeat(void)
{
  // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
  // 7E 00 0A 01 00 00 00    30 1A   03 00 00 01 00    33 31   Send heartbeat every 10 seconds
  qrSend(10, 0, 0, 0x30, 0x1A); // Send heartbeat
  if (TimerID.QR_HEART_BEATING_HANDLE_NO_ANSWER == 0)
  {
    TimerID.QR_HEART_BEATING_HANDLE_NO_ANSWER = t.setTimeout(qrHeartbeatNoAnswerHandle, QR_HEART_BEAT_INTERVAL * QR_HEART_BEAT_NO_ANSWER_COUNT);
  }
}

void qrHeartbeatNoAnswerHandle(void)
{
  t.cancel(TimerID.QR_HEART_BEATING_HANDLE_NO_ANSWER);
  TimerID.QR_HEART_BEATING_HANDLE_NO_ANSWER = 0;
  QrState.heartBeatErrorCount++;
  //  QrState.status = "QR Hearbeat err: " + String(QrState.heartBeatErrorCount);
  //  Serial.println(QrState.status);
  if (QrState.heartbeatFailNotifyCounter < QR_HEART_BEAT_NO_ANSWER_NOTIFY_COUNT) {
    rs485SendStrToHostSafety(QR_ID, "hbf", String(QrState.heartBeatErrorCount));
    QrState.heartbeatFailNotifyCounter++;
  }
}

void qrTrigger(void)
{
  // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
  // 7E 00 08 01 00 02 01    AB CD   02 00 00 01 00    33 31   Command trigger Mode (Set bit0 of zone byte 0x0002)
  qrSend(8, 2, 1, 0xAB, 0xCD);
}

void qrRestoreUserSetting(void)
{
  // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
  // 7E 00 08 01 00 D9 55    D1 76   02 00 00 01 00 33 31      Restore user - defined factory settings
  qrSend(8, 0xD9, 0x55, 0xD1, 0x76);
}

void qrNormalSetting(void)
{
  // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
  // 7E 00 08 01 00 00 D6    AB CD                             LED on, Mute off, Normal lighting, Normal brightness, Continuous mode
  qrSend(8, 0, 0xD6, 0xAB, 0xCD);
}

// OLED

void oledDrawText(byte _lineNo, const String &text)
{
  int16_t _y = 10 * _lineNo;
  oled.clear();
  oled.drawString(0, _lineNo, text);
  oled.display();
}

void oledClearText(byte _lineNo)
{
  int16_t _y = 10 * _lineNo;
  oled.clear();
  oled.drawString(0, _lineNo, " ");
  oled.display();
}

void setupOled(void)
{
  //  pinMode(RST_OLED, OUTPUT);
  //  digitalWrite(RST_OLED, LOW);
  //  delay(20);
  //  digitalWrite(RST_OLED, HIGH);
  oled.setFont(Roboto);
  oled.setTextAlignment(TEXT_ALIGN_LEFT);

  oled.init();
  oled.flipScreenVertically();
  oledDrawText(0, "START UP...");
}

// FINGER

uint8_t fingerCapturing()
{
  FingerState.work = "finger.getImage()";
  uint8_t p = finger.getImage();
  switch (p)
  {
    case FINGERPRINT_OK:
      FingerState.status = "Image taken";
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //      FingerState.status = "Working...";
      //      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerState.status = "getImage Communication error";
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      FingerState.status = "getImage Imaging error";
      Serial.println("Imaging error");
      return p;
    default:
      FingerState.status = "getImage Unknown error";
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  FingerState.work = "finger.image2Tz()";
  p = finger.image2Tz();
  switch (p)
  {
    case FINGERPRINT_OK:
      FingerState.status = "Image converted";
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      FingerState.status = "Image too messy";
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerState.status = "image2Tz Communication error";
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      FingerState.status = "image2Tz Could not find fingerprint features";
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      FingerState.status = "image2Tz Could not find fingerprint features";
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      FingerState.status = "image2Tz Unknown error";
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  FingerState.work = "finger.fingerSearch()";
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK)
  {
    FingerState.status = "Found a print match!";
    Serial.println("Found a print match!");
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    FingerState.status = "fingerSearch Communication error";
    Serial.println("Communication error");
    return p;
  }
  else if (p == FINGERPRINT_NOTFOUND)
  {
    FingerState.status = "fingerSearch Did not find a match";
    Serial.println("Did not find a match");
    return p;
  }
  else
  {
    FingerState.status = "fingerSearch Unknown error";
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  FingerState.read = String(finger.fingerID);
  rs485SendStrToHostSafety(FINGER_ID, "fsc", FingerState.read);
  return finger.fingerID;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  FingerState.work = "finger.deleteModel()";
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    FingerState.status = "Finger " + String(id) + " deleted!";
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    FingerState.status = "Delete Finger Communication error";
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    FingerState.status = "Could not delete in that location";
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    FingerState.status = "Delete Finger Error writing to flash";
    Serial.println("Error writing to flash");
  } else {
    FingerState.status = "Delete Finger Unknown error";
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }

  rs485SendStrToHostSafety(FINGER_ID, "del", String(id));

  return p;
}

uint8_t getFingerprintEnroll(uint8_t id) {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  FingerState.status = "Waiting...";
  FingerState.work = "finger.getImage()";
  oledDrawInfo();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        FingerState.status = "Image taken";
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        FingerState.status += ".";
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        FingerState.status = "Finger enroll Communication error";
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        FingerState.status = "Finger enroll Imaging error";
        Serial.println("Imaging error");
        break;
      default:
        FingerState.status = "Finger enroll Unknown error";
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  oledDrawInfo();
  FingerState.work = "finger.image2Tz(1)";
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      FingerState.status = "Image converted";
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      FingerState.status = "Image2Tz Communication error";
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerState.status = "Image2Tz Communication error";
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      FingerState.status = "Image2Tz Could not find fingerprint features";
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      FingerState.status = "Image2Tz Could not find fingerprint features";
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      FingerState.status = "Image2Tz Unknown error";
      Serial.println("Unknown error");
      return p;
  }

  FingerState.status = "Remove finger";
  Serial.println("Remove finger");
  oledDrawInfo();
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  FingerState.status = "Place same finger again";
  oledDrawInfo();
  FingerState.work = "finger.getImage()";
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        FingerState.status = "Image taken";
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        FingerState.status += ".";
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        FingerState.status = "Finger enroll Communication error";
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        FingerState.status = "Finger enroll Imaging error";
        Serial.println("Imaging error");
        break;
      default:
        FingerState.status = "Finger enroll Unknown error";
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  oledDrawInfo();
  FingerState.work = "finger.image2Tz(2)";
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      FingerState.status = "Image2Tz converted";
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      FingerState.status = "Image2Tz Communication error";
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerState.status = "Image2Tz Communication error";
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      FingerState.status = "Image2Tz Could not find fingerprint features";
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      FingerState.status = "Image2Tz Could not find fingerprint features";
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      FingerState.status = "Image2Tz Unknown error";
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  oledDrawInfo();
  FingerState.work = "finger.createModel()";
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    FingerState.status = "Prints matched!";
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    FingerState.status = "Create model Communication error";
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    FingerState.status = "Create model Fingerprints did not match";
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    FingerState.status = "Create model Unknown error";
    Serial.println("Unknown error");
    return p;
  }

  oledDrawInfo();
  Serial.print("ID "); Serial.println(id);
  FingerState.work = "finger.storeModel(id)";
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    FingerState.status = "Model Stored!";
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    FingerState.status = "Store model Communication error";
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    FingerState.status = "Store model Could not store in that location";
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    FingerState.status = "Store model Error writing to flash";
    Serial.println("Error writing to flash");
    return p;
  } else {
    FingerState.status = "Store model Unknown error";
    Serial.println("Unknown error");
    return p;
  }

  rs485SendStrToHostSafety(FINGER_ID, "fe", String(id));
  oledDrawInfo();

  return true;
}

void fingerEmptyDatabase() {
  finger.emptyDatabase();
  FingerState.status = "Finger database is empty";
  rs485SendStrToHostSafety(FINGER_ID, "empty", "ok");
}
