#include <SoftwareSerial.h>

#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;
const uint32_t connectTimeoutMs = 5000;

#include <AsyncTimer.h>

AsyncTimer t;

typedef struct {
  unsigned short CHECK = 0;
  unsigned short RUNNING = 0;
  unsigned short HEARTBEAT = 0;
  unsigned short RETRY = 0;
} tTimerID;


//#include <WiFiMulti.h>
//
//WiFiMulti wifiMulti;
//
//String wifiIp = "";
//String wifiStatus = "";

//#include "SSD1306Wire.h"
//
//SSD1306Wire oled(0x3c, SDA, SCL);

#include <Adafruit_Fingerprint.h>

#define FINGER_RX 15
#define FINGER_TX 13

EspSoftwareSerial::UART fingerSerial(FINGER_RX, FINGER_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

#define RS485_RX 14
#define RS485_TX 12

EspSoftwareSerial::UART rs485Serial(RS485_RX, RS485_TX);

#define HW_STATUS_OK 1
#define HW_STATUS_NOT_OK 0

#define FINGER_CHECK_AVAILABLE_INTERVAL 5000

enum eFingerWork {
  CHECK_AVAILABLE = 0,
  PRINT_INFO,
  WAIT,
  START_READ,
  READING,
  MATCHED,
  START_ENROLL,
  ENROLLING,
  ENROLL_SUCCESS,
  EMPTY_DATABASE,
  ERR,
  NOT_AVAILABLE
};

enum eFingerResponse {
  NO_FINGER_DETECTED,
  IMAGE_TAKEN,
  COMMUNICATION_ERROR,
  IMAGING_ERROR,
  UNKNOWN_ERROR,
  IMAGE_CONVERTED,
  IMAGE_TOO_MESSY,
  COULD_NOT_FIND_FINGERPRINT_FEATURES,
  FOUND_A_PRINT_MATCH,
  DID_NOT_FIND_A_MATCH,
};

enum eFingerProcess {
  GET_IMAGE,
  IMAGE_2_TZ,
  FINGER_SEARCH,
  MACTH_FINGER,
};

typedef struct {
  //  tHardwareStatus *STATUS;
  byte OK = 254;
  String STATUS = "Start up";
  String DATA = "";
  uint16_t MATCHED_ID;
  uint16_t MATCHED_CONFIDENCE;
  uint8_t CODE;
  enum eFingerWork WORK;
  enum eFingerResponse RESPONSE;
  enum eFingerProcess PROCESS;
  void (*WORK_FUNC)();
} tFingerStatus;

tTimerID FingerTimer;
tFingerStatus FingerStatus;

#define FINGER_TIMER_MS 11

typedef struct {
  String READ_DATA;
  String SEND_DATA;
  uint8_t HEARTBEAT = 0;
} tRs485State;

tRs485State Rs485State;

uint32_t RS485_HEARTBEAT_COUNTER = 0;

enum eTICKING {
  NOTHING = 0,
  STARTUP,
  ALL,
  FINGER,
  QR,
  RS485,
  RS485_QR,
  RS485_FINGER
} tick = NOTHING;


void setup() {
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);

  //  oled.clear();
  //
  //  oled.setFont(ArialMT_Plain_10);
  //  oled.setTextAlignment(TEXT_ALIGN_RIGHT);
  //  oled.drawString(0, 0, "Startup...");
  //  oled.display();


  pinMode(FINGER_RX, INPUT);
  pinMode(FINGER_TX, OUTPUT);
  pinMode(RS485_RX, INPUT);
  pinMode(RS485_TX, OUTPUT);
  pinMode(2, OUTPUT);

  rs485Serial.begin(9600);

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(100);
  finger.setBaudRate(FINGERPRINT_BAUDRATE_9600);
  delay(100);
  finger.begin(9600);
  delay(100);

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Register multi WiFi networks
  wifiMulti.addAP("Ti Teo", "12345678");
  wifiMulti.addAP("MDFKG", "hoianhtuan");

  wifiConnect();

  t.setInterval(fingerPrintInfo, 5000);
  //  t.setInterval(rs485Input, 10);
  t.setInterval(rs485HeartBeat, 3000);
  t.setInterval(wifiConnect, 5*60000);

  //  oled.drawString(0, 0, "Setup done...");
}

void loop() {
  // put your main code here, to run repeatedly:
  t.handle();
}

void rs485HeartBeat(void) {
  rs485Serial.print("heartbeat");
}

void rs485Input(void) {
  if (rs485Serial.available() > 0) {
    delay(10);
    Rs485State.READ_DATA = rs485Serial.readString();
    Serial.print("Rs485State.READ_DATA: "); Serial.println(Rs485State.READ_DATA);
    if (Rs485State.READ_DATA == "heartbeat") {
      Serial.println("RS485 Heartbeat ok");
      RS485_HEARTBEAT_COUNTER++;
      //      String _draw = "Heartbeat ok: " + RS485_HEARTBEAT_COUNTER;
      //      oled.drawString(0, 0, _draw);
      rs485Serial.print("heartbeat");
    }
  }
}

void fingerPrintInfo() {

  if (finger.verifyPassword() != HW_STATUS_OK) {
    Serial.println("Fingerprint sensor not available!");
    digitalWrite(2, 0);
  } else {
    digitalWrite(2, 1);

    Serial.println("Found fingerprint sensor!");

    Serial.println("Reading sensor parameters");
    finger.getParameters();
    Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
    Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
    Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
    Serial.print(F("Security level: ")); Serial.println(finger.security_level);
    Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
    Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
    Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

    finger.getTemplateCount();

    if (finger.templateCount == 0) {
      Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    }
    else {
      Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
      //      String _draw = finger.templateCount+ " templates";
      //      oled.drawString(0, 10, _draw);
    }
  }
}

void fingerReading() {
  FingerStatus.STATUS = "Reading";
  FingerStatus.CODE = finger.getImage();
  FingerStatus.PROCESS = GET_IMAGE;
  switch (FingerStatus.CODE) {
    case FINGERPRINT_OK:
      FingerStatus.STATUS = "Image taken";
      FingerStatus.RESPONSE = IMAGE_TAKEN;
      break;
    case FINGERPRINT_NOFINGER:
      FingerStatus.STATUS = "No finger detected";
      FingerStatus.RESPONSE = NO_FINGER_DETECTED;
      return;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerStatus.STATUS = "Image taken";
      FingerStatus.RESPONSE = COMMUNICATION_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_IMAGEFAIL:
      FingerStatus.STATUS = "Imaging error";
      FingerStatus.RESPONSE = IMAGING_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    default:
      FingerStatus.STATUS = "Image taken Unknown error";
      FingerStatus.RESPONSE = UNKNOWN_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
  }

  // OK success!

  FingerStatus.CODE = finger.image2Tz();
  FingerStatus.PROCESS = IMAGE_2_TZ;
  switch (FingerStatus.CODE) {
    case FINGERPRINT_OK:
      FingerStatus.STATUS = "Image converted";
      FingerStatus.RESPONSE = IMAGE_CONVERTED;
      break;
    case FINGERPRINT_IMAGEMESS:
      FingerStatus.STATUS = "Image too messy";
      FingerStatus.RESPONSE = IMAGE_TOO_MESSY;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerStatus.STATUS = "Image convert Communication error";
      FingerStatus.RESPONSE = COMMUNICATION_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_FEATUREFAIL:
      FingerStatus.STATUS = "Could not find fingerprint features";
      FingerStatus.RESPONSE = COULD_NOT_FIND_FINGERPRINT_FEATURES;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_INVALIDIMAGE:
      FingerStatus.STATUS = "Could not find fingerprint features";
      FingerStatus.RESPONSE = COULD_NOT_FIND_FINGERPRINT_FEATURES;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    default:
      FingerStatus.STATUS = "Image convert Unknown error";
      FingerStatus.RESPONSE = UNKNOWN_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
  }

  // OK converted!
  FingerStatus.CODE = finger.fingerSearch();
  FingerStatus.PROCESS = FINGER_SEARCH;
  switch (FingerStatus.CODE) {
    case FINGERPRINT_OK:
      FingerStatus.STATUS = "Found a print match";
      FingerStatus.RESPONSE = FOUND_A_PRINT_MATCH;
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerStatus.STATUS = "Finger Search Communication error";
      FingerStatus.RESPONSE = COMMUNICATION_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_NOTFOUND:
      FingerStatus.STATUS = "Did not find a match";
      FingerStatus.RESPONSE = DID_NOT_FIND_A_MATCH;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
      FingerStatus.STATUS = "Finger Search Unknown error";
      FingerStatus.RESPONSE = UNKNOWN_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
  }

  FingerStatus.PROCESS = MACTH_FINGER;
  FingerStatus.WORK = MATCHED;
  FingerStatus.MATCHED_ID = finger.fingerID;
  FingerStatus.MATCHED_CONFIDENCE = finger.confidence;
  tick = FINGER;
}

void wifiConnect() {
  // Maintain WiFi connection
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.localIP());
    rs485Serial.print("WiFi connected: ");
    rs485Serial.print(WiFi.SSID());
    rs485Serial.print(" ");
    rs485Serial.print("WiFi connected: ");
  } else {
    Serial.println(WiFi.localIP());
  }
}

//void drawWifiInfo() {
//  display.clear();
//  display.drawString(0, 0, wifiStatus);
//  //  display.drawString(0, 10, "IP address: ");
//  display.drawString(0, 12, wifiIp);
//  display.drawString(0, 22, RS485_RECEIVED);
//  display.display();
//}
