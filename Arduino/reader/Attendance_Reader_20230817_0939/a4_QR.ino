String qr_state = "on";

void qr_setState(String state) {
  if (state == "on") {
    qr_onReading();
  } else if (state == "off") {
    qr_offReading();
  } else {
    return;
  }
  qr_state = state;
}

void qr_onReading(void) {
  digitalWrite(QR_TOGGLE_PIN, HIGH);
  TimerID.QR_INPUT = t.setInterval(qr_input, QR_INPUT_INTERVAL);
}

void qr_offReading(void) {
  digitalWrite(QR_TOGGLE_PIN, LOW);
  t.cancel(TimerID.QR_INPUT);
}

void qr_write(uint32_t _type, uint32_t _address, uint32_t _data, uint32_t _check1, uint32_t _check2) {
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

void qr_input(void) {
  while (rs485Serial.available() > 0 || FINGER_IS_BUSY) {}
  char _read[255];
  uint32_t byte_counter = 0;
  while (qrSerial.available() > 0) {
    delay(10);
    _read[byte_counter] = qrSerial.read();
    byte_counter++;
  }
  _read[byte_counter] = 0;  // Add string terminating zero
  if (byte_counter > 0) {
    if ((3 == _read[0]) && (0 == _read[1]) && (0 == _read[2]) && (1 == _read[3]) && (0 == _read[4])) {
      rs485_println("q", "ping", Rs485_data.cmdid, Rs485_data.value, "");
    } else if ((2 == _read[0]) && (0 == _read[1]) && (0 == _read[2]) && (1 == _read[3]) && (0 == _read[4])) {
      rs485_println("q", "user", Rs485_data.cmdid, Rs485_data.value, "");
    } else {
      String cmdid = String(random(100, 200));
      rs485_println("q", "read", cmdid, String(_read), "");
      ledSuccess();
    }
  }
}

void hardReboot(String dev) {
  while (rs485Serial.availableForWrite() != 1) {};
  String str = "h0,rl,reboot,000," + dev + ".";
  rs485Serial.print(str);
  rs485Serial.flush();
}

int qr_handleCmd(void) {
  String resp = Rs485_data.value;
  if (Rs485_data.cmd == "ping") {
    qr_write(10, 0, 0, 0x30, 0x1A);
    return 0;
  } else if (Rs485_data.cmd == "user") {
    qr_write(8, 0xD9, 0x55, 0xD1, 0x76);
    return 0;
  } else if (Rs485_data.cmd == "normal") {
    qr_write(8, 0, 0xD6, 0xAB, 0xCD);
  } else if (Rs485_data.cmd == "off") {
    qr_setState("off");
  } else if (Rs485_data.cmd == "on") {
    qr_setState("on");
  } else if (Rs485_data.cmd == "reboot") {
    if (CONTROL_IS_NOT_COMMUNICATE) {
      if (Rs485_data.value == "soft") {
        ESP.restart();
      } else if (Rs485_data.value == "hard") {
        hardReboot(DEVICE);
      } else if (Rs485_data.value == "control") {
        hardReboot("ct");
      }
    }
  } else {
    resp = "NO CMD";
  }
  return rs485_response("q", resp);
}
