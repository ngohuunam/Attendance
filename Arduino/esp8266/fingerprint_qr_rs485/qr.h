
#define QR_BUFFER_LENGTH 9
#define QR_HEARTBEAT_INTERVAL 5000
#define QR_TIMER_MS 10

tTimerID QrTimer;

enum eQrWork {
  QR_WORK_READING = 0,
  QR_WORK_TRIGGER,
  QR_WORK_RESTORE_USER_SETTING,
  QR_WORK_NORMAL_SETTING,
  QR_WORK_GET_CODE,
  QR_WORK_ERR,
  QR_WORK_NOT_AVAILABLE
};

typedef struct {
  char READ_DATA[QR_BUFFER_LENGTH];
  uint8_t HEARTBEAT = 0;
  enum eQrWork WORK = QR_WORK_READING;
  void (*WORK_FUNC)();
} tQrState;

tQrState QrState;

void qrSend(uint32_t type, uint32_t address, uint32_t data, uint32_t check1, uint32_t check2) {
  uint8_t qrBuffer[QR_BUFFER_LENGTH];

  qrBuffer[0] = 0x7E;
  qrBuffer[1] = 0;
  qrBuffer[2] = type;
  qrBuffer[3] = 1;
  qrBuffer[4] = 0;
  qrBuffer[5] = address;
  qrBuffer[6] = data;
  qrBuffer[7] = check1;
  qrBuffer[8] = check2;

  qr.write(qrBuffer, QR_BUFFER_LENGTH);
}

void qrHeartbeat(void) {
  // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
  // 7E 00 0A 01 00 00 00    30 1A   03 00 00 01 00    33 31   Send heartbeat every 10 seconds
  qrSend(10, 0, 0, 0x30, 0x1A);            // Send heartbeat
  QrState.HEARTBEAT++;
  // If no correct reply is received for three consecutive times, the main control should be handle it accordingly.
  if (QrState.HEARTBEAT > 3) {
    QrState.HEARTBEAT = 0;
    Serial.println("QR Heartbeat fail");
  }
}

void qrTrigger(void) {
  // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
  // 7E 00 08 01 00 02 01    AB CD   02 00 00 01 00    33 31   Command trigger Mode (Set bit0 of zone byte 0x0002)
  qrSend(8, 2, 1, 0xAB, 0xCD);
}

void qrRestoreUserSetting(void) {
  // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
  // 7E 00 08 01 00 D9 55    D1 76   02 00 00 01 00 33 31      Restore user - defined factory settings
  qrSend(8, 0xD9, 0x55, 0xD1, 0x76);
}

void qrNormalSetting(void) {
  // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
  // 7E 00 08 01 00 00 D6    AB CD                             LED on, Mute off, Normal lighting, Normal brightness, Continuous mode
  qrSend(8, 0, 0xD6, 0xAB, 0xCD);
}

void qrInput(void) {
  while (qr.available() > 0) {
    delay(10);

    uint32_t byte_counter = 0;
    // Use timeout to allow large serial reads within a window
    uint32_t timeout = millis();
    while (millis() - 10 < timeout) {
      if (qr.available()) {
        timeout = millis();
        QrState.READ_DATA[byte_counter++] = qr.read();
        if (byte_counter >= QR_BUFFER_LENGTH - 1) {
          break;
        }
      }
    }

    QrState.READ_DATA[byte_counter] = 0;            // Add string terminating zero

    if ((0 == QrState.READ_DATA[1]) && (3 == QrState.READ_DATA[0]) && (0 == QrState.READ_DATA[2]) && (1 == QrState.READ_DATA[3]) && (0 == QrState.READ_DATA[4])) {
      // 03 00 00 01 00 33 31                              - Heartbeat response
      QrState.HEARTBEAT = 0;
      Serial.println("QR Heartbeat ok");
    } else {                             // Bar code
      QrState.WORK = QR_WORK_GET_CODE;
      tick = QR;
    }
  }
}

void qrHandleCode(void) {
  Serial.print("QR get code: "); Serial.println(QrState.READ_DATA);
  QrState.WORK = QR_WORK_READING;
  tick = QR;
}

void (* qrWorkFunctions [7])() = {
  qrInput,
  qrTrigger,
  qrRestoreUserSetting,
  qrNormalSetting,
  qrHandleCode,
  nullptr,
  nullptr
};

void qrAssignWorkFunction(enum eQrWork _work) {
  t.cancel(QrTimer.RUNNING);
  QrState.WORK_FUNC = qrWorkFunctions[_work];
  QrTimer.RUNNING = t.setInterval(QrState.WORK_FUNC, QR_TIMER_MS);
}

void qrUpdateWorkFunction() {
  Serial.print("qrUpdateWorkFunction: "); Serial.println(QrState.WORK);
  qrAssignWorkFunction(QrState.WORK);
}
