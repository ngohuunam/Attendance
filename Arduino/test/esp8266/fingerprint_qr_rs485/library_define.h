
#define SERIAL_FINGER_HEADER 0x7A
#define SERIAL_RS485_HEADER 0x7C
#define SERIAL_MCU_HEADER 0x7D
#define SERIAL_QR_HEADER 0x7E

#include <AsyncTimer.h>

AsyncTimer t;

#include <Adafruit_Fingerprint.h>

#define SERIAL_BUFFER_LENGTH 9

#define FINGER_RX 14
#define FINGER_TX 12

SoftwareSerial fingerSerial(FINGER_RX, FINGER_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

#define QR_RX 0
#define QR_TX 2

SoftwareSerial qr(QR_RX, QR_TX);

#define RS485_RX 15
#define RS485_TX 13

SoftwareSerial rs485(RS485_RX, RS485_TX);

void hwSend(SoftwareSerial *_serial, uint32_t _header, uint32_t _type, uint32_t _address, uint32_t _data, uint32_t _check1, uint32_t _check2) {
  uint8_t _buffer[SERIAL_BUFFER_LENGTH];

  _buffer[0] = _header;
  _buffer[1] = 0;
  _buffer[2] = _type;
  _buffer[3] = 1;
  _buffer[4] = 0;
  _buffer[5] = _address;
  _buffer[6] = _data;
  _buffer[7] = _check1;
  _buffer[8] = _check2;

  _serial->write(_buffer, SERIAL_BUFFER_LENGTH);
}

void qrSend(uint32_t _type, uint32_t _address, uint32_t _data, uint32_t _check1, uint32_t _check2) {
  uint32_t _header = SERIAL_QR_HEADER;
  hwSend(&qr, _header, _type, _address, _data, _check1, _check2);
}

void rs485Send(uint32_t _header, uint32_t _address, uint32_t _data) {
//  uint32_t _header = SERIAL_RS485_HEADER;
  uint32_t _type = 0;
  uint32_t _check1 = 0xAC;
  uint32_t _check2 = 0xBD;
  hwSend(&rs485, _header, _type, _address, _data, _check1, _check2);
}
