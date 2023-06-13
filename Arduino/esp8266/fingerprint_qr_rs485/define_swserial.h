#include <Adafruit_Fingerprint.h>

#define FINGER_RX 0
#define FINGER_TX 2

SoftwareSerial fingerSerial(FINGER_RX, FINGER_TX);

#define QR_RX 14
#define QR_TX 12

SoftwareSerial qr(QR_RX, QR_TX);

#define RS485_RX 13
#define RS485_TX 15

SoftwareSerial rs485(RS485_RX, RS485_TX);
