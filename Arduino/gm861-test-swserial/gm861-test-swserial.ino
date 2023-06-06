#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
SoftwareSerial serial_ESP(D2, D3);  //D2 = RX -- D3 = TX

#include <AsyncTimer.h>

AsyncTimer t;

#define GM861_GUI_LENGTH 30  // Max number of code characters in GUI

// typedef struct {
//   char barcode[GM861_GUI_LENGTH];
//   uint8_t index;
//   uint8_t state;
//   uint8_t heartbeat;
//   bool read;
// } tGm861;
// tGm861 *Gm861 = nullptr;

// enum Gm861States {
//   GM861_STATE_DONE,
//   GM861_STATE_DUMP,
//   GM861_STATE_SERIAL_OUTPUT,
//   GM861_STATE_OUTPUT,
//   GM861_STATE_SETUP_CODE_ON,
//   GM861_STATE_INIT_OFFSET = 16,     // Init after (GM861_STATE_INIT_OFFSET - GM861_STATE_SETUP_CODE_ON) * 0.25 seconds restart
//   GM861_STATE_RESET
// };

void setup() {
  // Open serial communications and wait for port to open:
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  Serial.begin(115200);
  serial_ESP.begin(9600);
  t.setInterval(Gm861Heartbeat, 5000);
}

void loop() {  // run over and over
  if (serial_ESP.available() > 0) {
    char b = Serial.read();
    Serial.print(b);
    Serial.print(' ');
  }
  // if (readIndx > 0) {
  //   for (int i = 0; i < 9; i++) {
  //     Serial.print(variable[i], HEX);
  //     Serial.print(' ');
  //   }
  //   readIndx = 0;
  // }
  t.handle();
}

void Gm861Heartbeat(void) {
  Gm861Send(10, 0, 0);  // Send heartbeat
  // Gm861->heartbeat++;
  // // If no correct reply is received for three consecutive times, the main control should be handle it accordingly.
  // if (Gm861->heartbeat > 3) {
  //   Gm861->heartbeat = 0;
  //   Gm861->state = GM861_STATE_RESET;
  // }
}

uint32_t Gm861Crc(uint8_t* ptr, uint32_t len) {
  // When no need for checking CRC, CRC byte can be filled in 0xAB 0xCD
  uint32_t crc = 0;
  while (len-- != 0) {
    for (uint8_t i = 0x80; i != 0; i /= 2) {
      crc *= 2;
      if ((crc & 0x10000) != 0) {  // After multiplying the last bit of CRC by 2, if the first bit is 1, then divide by 0x11021
        crc ^= 0x11021;
      }
      if ((*ptr & i) != 0) {  // If this bit is 1, then CRC = previous CRC + this bit/CRC_CCITT
        crc ^= 0x1021;
      }
    }
    ptr++;
  }
  return crc;
}

void Gm861Send(uint32_t type, uint32_t address, uint32_t data) {
  uint8_t buffer[9];

  buffer[0] = 0x7E;
  buffer[1] = 0;
  buffer[2] = type;
  buffer[3] = 1;
  buffer[4] = 0;
  buffer[5] = address;
  buffer[6] = data;
  buffer[7] = 0x30;
  buffer[8] = 0x1A;

  serial_ESP.write(buffer, 9);
}