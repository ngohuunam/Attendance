typedef struct {
  char barcode[GM861_GUI_LENGTH];
  uint8_t index;
  uint8_t state;
  uint8_t heartbeat;
  bool read;
} tGm861;

uint32_t Gm861Crc(uint8_t* ptr, uint32_t len) {
  // When no need for checking CRC, CRC byte can be filled in 0xAB 0xCD
  uint32_t crc = 0;
  while (len-- != 0) {
    for (uint8_t i = 0x80; i != 0; i /= 2) {
      crc *= 2;
      if ((crc & 0x10000) !=0) {       // After multiplying the last bit of CRC by 2, if the first bit is 1, then divide by 0x11021
        crc ^= 0x11021;
      }
      if ((*ptr & i) != 0) {           // If this bit is 1, then CRC = previous CRC + this bit/CRC_CCITT
        crc ^= 0x1021;
      }
    }
    ptr++;
  }
  return crc;
}

void Gm861Send(uint32_t type, uint32_t len, uint32_t address, uint32_t data) {
  uint8_t buffer[10];

  buffer[0] = 0x7E;
  buffer[1] = 0;
  buffer[2] = type;
  buffer[3] = len;
  buffer[4] = 0;
  buffer[5] = address;
  buffer[6] = data;
  uint8_t index = 7;
  if (len > 1) {
    buffer[7] = data >> 8;
    index++;
  }
  uint32_t crc = Gm861Crc(buffer+2, len + 4);
  buffer[index] = crc >> 8;
  index++;
  buffer[index] = crc;
  index++;

  Gm861Serial->write(buffer, index);
}