#include "finger.h"
#include "qr.h"
//#include "rs485.h"

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
  uint8_t setBaudrate = finger.setBaudRate(FINGERPRINT_BAUDRATE_9600);
  Serial.print("setBaudrate: "); Serial.println(setBaudrate, HEX);
  finger.begin(9600);
  
  qr.begin(9600);
  rs485.begin(9600);

  delay(5);

  FingerStatus.OK = finger.verifyPassword();

  fingerCheckIsAvailable();

  FingerTimer.CHECK = t.setInterval(fingerCheckIsAvailable, FINGER_CHECK_AVAILABLE_INTERVAL);
  QrTimer.HEARTBEAT = t.setInterval(qrHeartbeat, QR_HEARTBEAT_INTERVAL);
  Rs485Timer.HEARTBEAT = t.setInterval(rs485Heartbeat, RS485_HEARTBEAT_INTERVAL);
  tick = STARTUP;
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
      rs485AssignWorkFunction(RS485_WORK_LISTENING);
      break;

    case FINGER:
      fingerUpdateWorkFunction();
      break;

    case QR:
      qrUpdateWorkFunction();
      break;

    case RS485_QR:
      qrUpdateWorkFunction();
      rs485UpdateWorkFunction();
      break;

  }
  tick = NOTHING;
}

void rs485HandleCode(void) {
  Serial.print("RS485 get code: "); Serial.println(Rs485State.READ_DATA);
  //  rs485.write(QrState.READ_DATA);
  char _hwCode = Rs485State.READ_DATA[0];
  char _data = Rs485State.READ_DATA[1];
  switch (_hwCode) {
    case SERIAL_QR_HEADER:
      switch (_data) {
        case RS485_RES_OK:
          QrState.WORK = QR_WORK_READING;
          tick = QR;
          break;
      }
      break;
  }
  Rs485State.WORK = RS485_WORK_GET_CODE;
  tick = RS485_QR;
}
