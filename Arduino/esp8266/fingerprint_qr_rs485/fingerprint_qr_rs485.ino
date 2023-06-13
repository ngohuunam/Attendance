/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
//#include "define.h"
#include "define_finger.h"

#include <AsyncTimer.h>

AsyncTimer t;

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  pinMode(FINGER_RX, INPUT);
  pinMode(FINGER_TX, OUTPUT);
  pinMode(QR_RX, INPUT);
  pinMode(QR_TX, OUTPUT);
  pinMode(RS485_RX, INPUT);
  pinMode(RS485_TX, OUTPUT);


  // set the data rate for the sensor serial port
  finger.begin(57600);
  qr.begin(9600);
  rs485.begin(9600);

  delay(5);
  FingerStatus->STATUS->OK = finger.verifyPassword();
  if (FingerStatus->STATUS->OK) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
  }

  if (FingerStatus->STATUS->OK) {
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
    }
  }
  //  t.setInterval(getFingerprintID, 11);
  //  t.setInterval(qr_read, 10);
  //  t.setInterval(getFingerprintID, 11);
  //  t.setInterval(rs485Send, 12);
}

void rs485Send() {
  if (qrStr.length() > 0) {
    rs485.print(qrStr);
    qrStr = "";
  } else if (fingerID.length() > 0) {
    rs485.print(fingerID);
    fingerID = "";;
  }
}

void qr_read() {
  while (qr.available() > 0) {
    delay(10);
    qrStr = "QR: " + qr.readString();
  }
}

void loop()                     // run over and over again
{
  t.handle();       //don't ned to run this at full speed.
}

void ticking() {
  switch (stick) {
    case NOTHING:
      break;

    case FINGER:
      fingerHandleData();
      stick = NOTHING;
      break;
  }
}

void fingerCheckIsOk() {
  if (!!(FingerTimer->ID->RUNNING)) {
    t.cancel(FingerTimer->ID->RUNNING);
  }
  FingerStatus->STATUS->OK = finger.verifyPassword();
}

void (* arr [9])() = {fingerReading, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

void fingerHandleWork() {
  String str = "";
  switch (FingerStatus->WORK) {
    case START_READ:
      arr[START_READ]();
      break;

    default:
      Serial.println(FingerStatus->STATUS->STATUS);
      break;
  }
}

void fingerHandleData() {
  String str = "";
  switch (FingerStatus->READ_PROCESS) {
    case MACTH_FINGER:
      // found a match!
      str = "Found ID #" + String(finger.fingerID) + " with confidence of " + String(finger.confidence);
      Serial.println(str);
      fingerID = "ID: " + String(finger.fingerID);
      break;

    default:
      Serial.println(FingerStatus->STATUS->STATUS);
      break;
  }
}

void fingerIsAvailable() {
  FingerStatus->STATUS->OK = finger.verifyPassword();
  switch (FingerStatus->STATUS->OK) {
    case HW_STATUS_OK:
      t.cancel(FingerTimer->ID->CHECK);
      FingerTimer->ID->RUNNING = t.setInterval(fingerRunning, FINGER_TIMER_MS);
      break;

    case HW_STATUS_NOT_OK:
      t.cancel(FingerTimer->ID->RUNNING);
      FingerTimer->ID->CHECK = t.setInterval(fingerCheckIsOk, FINGER_TIMER_MS);
      break;
  }
}

void fingerRunning() {
  switch (FingerStatus->STATUS->CMD) {
    case HW_CMD_NORMAL_RUNNING:
      FingerTimer->ID->RUNNING = t.setInterval(fingerReading, FINGER_TIMER_MS);
      break;

    case FINGER_CMD_ENROLL:
      Serial.println("FINGER_CMD_ENROLL");
      //      FingerTimer->ID->RUNNING = t.setInterval(fingerReading, FINGER_TIMER_MS);
      break;
  }
}

void DEBUG(String str, tHardwareStatus *hwStatus) {
  Serial.println(str);
  hwStatus->STATUS = str;
}

void fingerEnroll() {
  Serial.println("fingerEnroll()");
}

void fingerEmptyDatabase() {
  Serial.println("fingerEmptyDatabase()");
}

void fingerReading() {
  uint8_t p = getFingerprintID();
  FingerStatus->STATUS->DATA = String(p);
}

uint8_t getFingerprintID() {
  //  t.cancel(FingerTimer->ID->CHECK);
  uint8_t p = finger.getImage();
  FingerStatus->READ_PROCESS = GET_IMAGE;
  switch (p) {
    case FINGERPRINT_OK:
      //      DEBUG("Image taken", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Image taken";
      FingerStatus->READ_RESPONSE = IMAGE_TAKEN;
      stick = FINGER;
      break;
    case FINGERPRINT_NOFINGER:
      //      DEBUG("No finger detected", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "No finger detected";
      FingerStatus->READ_RESPONSE = NO_FINGER_DETECTED;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      DEBUG("Image taken Communication error", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Image taken";
      FingerStatus->READ_RESPONSE = COMMUNICATION_ERROR;
      stick = FINGER;
      return p;
    case FINGERPRINT_IMAGEFAIL:
      //      DEBUG("Imaging error", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Imaging error";
      FingerStatus->READ_RESPONSE = IMAGING_ERROR;
      stick = FINGER;
      return p;
    default:
      //      DEBUG("Image taken Unknown error", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Image taken Unknown error";
      FingerStatus->READ_RESPONSE = UNKNOWN_ERROR;
      stick = FINGER;
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  FingerStatus->READ_PROCESS = IMAGE_2_TZ;
  switch (p) {
    case FINGERPRINT_OK:
      //      DEBUG("Image converted", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Image converted";
      FingerStatus->READ_RESPONSE = IMAGE_CONVERTED;
      break;
    case FINGERPRINT_IMAGEMESS:
      //      DEBUG("Image too messy", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Image too messy";
      FingerStatus->READ_RESPONSE = IMAGE_TOO_MESSY;
      stick = FINGER;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //      DEBUG("Image convert Communication error", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Image convert Communication error";
      FingerStatus->READ_RESPONSE = COMMUNICATION_ERROR;
      stick = FINGER;
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //      DEBUG("Could not find fingerprint features", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Could not find fingerprint features";
      FingerStatus->READ_RESPONSE = COULD_NOT_FIND_FINGERPRINT_FEATURES;
      stick = FINGER;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //      DEBUG("Could not find fingerprint features", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Could not find fingerprint features";
      FingerStatus->READ_RESPONSE = COULD_NOT_FIND_FINGERPRINT_FEATURES;
      stick = FINGER;
      return p;
    default:
      //      DEBUG("Image convert Unknown error", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Image convert Unknown error";
      FingerStatus->READ_RESPONSE = UNKNOWN_ERROR;
      stick = FINGER;
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  FingerStatus->READ_PROCESS = FINGER_SEARCH;
  switch (p) {
    case FINGERPRINT_OK:
      //      DEBUG("Found a print match", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Found a print match";
      FingerStatus->READ_RESPONSE = IMAGE_CONVERTED;
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      //      DEBUG("Finger Search Communication error", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Finger Search Communication error";
      FingerStatus->READ_RESPONSE = COMMUNICATION_ERROR;
      return p;
    case FINGERPRINT_NOTFOUND:
      //      DEBUG("Did not find a match", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Did not find a match";
      FingerStatus->READ_RESPONSE = DID_NOT_FIND_A_MATCH;
      return p;
      //      DEBUG("Finger Search Unknown error", FingerStatus->STATUS);
      FingerStatus->STATUS->STATUS = "Finger Search Unknown error";
      FingerStatus->READ_RESPONSE = UNKNOWN_ERROR;
      return p;
  }
  FingerStatus->READ_PROCESS = MACTH_FINGER;
  stick = FINGER;
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
