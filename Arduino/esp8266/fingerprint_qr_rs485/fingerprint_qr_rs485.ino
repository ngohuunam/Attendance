#include "finger.h"
#include "Qr.h"

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);

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

  FingerStatus.OK = finger.verifyPassword();

  fingerCheckIsAvailable();

  t.setInterval(fingerCheckIsAvailable, FINGER_CHECK_AVAILABLE_INTERVAL);
  t.setInterval(qrHeartbeat, QR_HEARTBEAT_INTERVAL);
  tick = STARTUP;
}

void rs485Send(String _str) {
  if (qrStr.length() > 0) {
    rs485.print(_str);
    qrStr = "";
  } else if (_str.length() > 0) {
    rs485.print(_str);
    //    fingerID = "";;
  }
}

void loop()                     // run over and over again
{
  ticking();
  t.handle();       //don't ned to run this at full speed.
}

void ticking() {
  switch (tick) {
    case NOTHING:
      break;

    case STARTUP:
      fingerAssignWorkFunction(PRINT_INFO);
      qrAssignWorkFunction(QR_WORK_READING);
      break;

    case FINGER:
      fingerUpdateWorkFunction();
      break;

    case QR:
      qrUpdateWorkFunction();
      break;
  }
  tick = NOTHING;
}


void sendStruct (const tFingerStatus* sta)
{
  int size__status = sizeof(sta);
  Serial.write((const char*)sta, size__status);  // 2 bytes.
}

void print__status(tFingerStatus &_status) {
  Serial.print(F("Status OK = \""));
  Serial.print(_status.OK);
  Serial.print(F("\", STATUS "));
  Serial.print(_status.STATUS);
}
