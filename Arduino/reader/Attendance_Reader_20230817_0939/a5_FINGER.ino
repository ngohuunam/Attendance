bool finger_captureStarted = false;
String finger_state = "start";

void finger_setState(String state) {
  if (state == "start") {
    finger_startCapture();
  } else if (state == "stop") {
    finger_stopCapture();
  } else {
    return;
  }
  finger_state = state;
}

void finger_startCapture() {
  t.cancel(TimerID.FINGER_CAPTURE);
  TimerID.FINGER_CAPTURE = t.setInterval(finger_capture, FINGERPRINT_CAPTURE_INTERVAL);
  finger_captureStarted = true;
}

void finger_startCaptureWithInterval(unsigned long interval) {
  t.cancel(TimerID.FINGER_CAPTURE);
  TimerID.FINGER_CAPTURE = t.setInterval(finger_capture, interval);
  finger_captureStarted = true;
}

void finger_stopCapture() {
  t.cancel(TimerID.FINGER_CAPTURE);
  finger_captureStarted = false;
}

void finger_pauseCapture(unsigned long ms) {
  finger_stopCapture();
  TimerID.FINGER_PAUSE_CAPTURE = t.setTimeout(finger_startCapture, ms);
}

byte FINGERPRINT_PACKETRECIEVEERR_COUNTER = 0;
byte FINGERPRINT_PACKETRECIEVEERR_RETRY_TO_NOTIFY = 0;
byte FINGERPRINT_PACKETRECIEVEERR_RETRY_TO_STOP = 0;
const int FINGERPRINT_PACKETRECIEVEERR_COUNTER_MAX = 3 * 60 * (1000 / FINGERPRINT_CAPTURE_INTERVAL);

void finger_hanlde_FINGERPRINT_PACKETRECIEVEERR(void) {
  if (FINGERPRINT_PACKETRECIEVEERR_RETRY_TO_STOP > 3) {
    rs485_send("f", "err", "STOP", "");
    finger_stopCapture();
    return;
  }
  FINGERPRINT_PACKETRECIEVEERR_COUNTER++;
  if (FINGERPRINT_PACKETRECIEVEERR_COUNTER > FINGERPRINT_PACKETRECIEVEERR_COUNTER_MAX) {
    FINGERPRINT_PACKETRECIEVEERR_COUNTER = 0;
    ledError();
    finger_pauseCapture(2000);
    FINGERPRINT_PACKETRECIEVEERR_RETRY_TO_NOTIFY++;
  }
  if (FINGERPRINT_PACKETRECIEVEERR_RETRY_TO_NOTIFY > 3) {
    FINGERPRINT_PACKETRECIEVEERR_RETRY_TO_NOTIFY = 0;
    rs485_send("f", "err", "PACKETRECIEVEERR", "");
    FINGERPRINT_PACKETRECIEVEERR_RETRY_TO_STOP++;
  }
}

String finger_capture() {
  //  while (rs485Serial.available() > 0 || qrSerial.available() > 0) {}
  String _status = "Capturing...";
  FINGER_IS_BUSY = true;
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      FINGERPRINT_PACKETRECIEVEERR_COUNTER = 0;
      _status = OLED_DEBUG("getImage(): FINGERPRINT_OK", "PLS WAIT...");
      ledNotify(RGBLed::YELLOW, 5000);
      buzzer();
      break;
    case FINGERPRINT_NOFINGER:
      FINGERPRINT_PACKETRECIEVEERR_COUNTER = 0;
      FINGER_IS_BUSY = false;
      return "";
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = OLED_DEBUG("getImage(): PACKETRECIEVEERR", "(1) ERR: PACKETRECIEVEERR");
      finger_hanlde_FINGERPRINT_PACKETRECIEVEERR();
      ledErrorNoBuzz();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_IMAGEFAIL:
      FINGERPRINT_PACKETRECIEVEERR_COUNTER = 0;
      _status = OLED_DEBUG("getImage(): IMAGEFAIL", "(1) ERR: IMAGEFAIL");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    default:
      FINGERPRINT_PACKETRECIEVEERR_COUNTER = 0;
      _status = OLED_DEBUG("getImage(): Unknown error: " + String(p, HEX), "(1) ERR: unknown: " + String(p, HEX));
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      _status = "Convert Image: converted";
      break;
    case FINGERPRINT_IMAGEMESS:
      _status = OLED_DEBUG("image2Tz(): IMAGEMESS", "(2) ERR: IMAGEMESS");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = OLED_DEBUG("image2Tz(): PACKETRECIEVEERR", "(2) ERR: PACKETRECIEVEERR");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_FEATUREFAIL:
      _status = OLED_DEBUG("image2Tz(): FEATUREFAIL", "(2) ERR: FEATURE FAIL");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_INVALIDIMAGE:
      _status = OLED_DEBUG("image2Tz(): INVALIDIMAGE", "(2) ERR: INVALIDIMAGE");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    default:
      _status = OLED_DEBUG("image2Tz(): Unknown error: " + String(p, HEX), "(2) ERR: unknown: " + String(p, HEX));
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
  }

  // OK converted!
  p = finger.fingerSearch();
  switch (p) {
    case FINGERPRINT_OK:
      _status = "ok";
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = OLED_DEBUG("fingerSearch(): PACKETRECIEVEERR", "(3) ERR: PACKETRECIEVEERR");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_NOTFOUND:
      _status = OLED_DEBUG("fingerSearch(): NOTFOUND", "(3) ERR: NOTFOUND");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    default:
      _status = OLED_DEBUG("fingerSearch(): Unknown error: " + String(p, HEX), "(3) ERR: unknown: " + String(p, HEX));
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
  }
  // found a match!
  OLED_DEBUG("Found matched: " + String(finger.fingerID), "WAIT FOR AUTH...!");
  finger_pauseCapture(3000);
  rs485_send("f", "captured", String(finger.fingerID), "");
  FINGER_IS_BUSY = false;
  return _status;
}

String finger_enroll(String id) {
  //  while (rs485Serial.available() > 0 || qrSerial.available() > 0) {}
  FINGER_IS_BUSY = true;
  String _status = OLED_DEBUG("Waiting for enroll...", "PLACE FINGER...!");
  ledNotify(RGBLed::YELLOW, FINGERPRINT_ENROLL_TIMEOUT + 1000);
  unsigned long startedWaiting = millis();
  int _timeout = (int) FINGERPRINT_ENROLL_TIMEOUT;
  int p = -1;
  while (p != FINGERPRINT_OK && p != _timeout) {
    if (millis() - startedWaiting > FINGERPRINT_ENROLL_TIMEOUT) {
      p = _timeout;
    }
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        _status = OLED_DEBUG("Enroll.getImage: OK", "READ, WAIT!");
        buzzer();
        break;
      case FINGERPRINT_NOFINGER:
        FINGER_IS_BUSY = false;
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        _status = OLED_DEBUG("Enroll.getImage: PACKETRECIEVEERR", "(1) ERR: PACKETRECIEVEERR");
        ledError();
        FINGER_IS_BUSY = false;
        break;
      case FINGERPRINT_IMAGEFAIL:
        _status = OLED_DEBUG("Enroll.getImage: IMAGEFAIL", "(1) ERR: IMAGEFAIL");
        ledError();
        FINGER_IS_BUSY = false;
        break;
      case FINGERPRINT_ENROLL_TIMEOUT:
        _status = OLED_DEBUG("Enroll timeout", "TIMEOUT");
        ledError();
        FINGER_IS_BUSY = false;
        break;
      default:
        _status = OLED_DEBUG("Enroll.getImage: Unknown error: " + String(p, HEX), "(1) ERR: Unknown: " + String(p, HEX));
        ledError();
        FINGER_IS_BUSY = false;
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      _status = OLED_DEBUG("Enroll.image2Tz(1): IMAGEMESS", "(2) ERR: IMAGEMESS");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = OLED_DEBUG("Enroll.image2Tz(1): PACKETRECIEVEERR", "(2) ERR: PACKETRECIEVEERR");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_FEATUREFAIL:
      _status = OLED_DEBUG("Enroll.image2Tz(1): FEATUREFAIL", "(2) ERR: FEATUREFAIL");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_INVALIDIMAGE:
      _status = OLED_DEBUG("Enroll.image2Tz(1): INVALIDIMAGE", "(2) ERR: INVALIDIMAGE");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_ENROLL_TIMEOUT:
      _status = OLED_DEBUG("Enroll timeout", "HET GIO");
      ledError();
      FINGER_IS_BUSY = false;
      break;
    default:
      _status = OLED_DEBUG("Enroll.image2Tz(1): Unknown error: " + String(p, HEX), "(2) ERR: Unknown: " + String(p, HEX));
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
  }

  _status = OLED_DEBUG("Remove finger", "Nhấc tay ra...!");
  ledNotify(RGBLed::BLUE, 3000);
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  _status = OLED_DEBUG("Place same finger again", "Dat lai ngon tay vua lay de doc lan 2");
  ledNotify(RGBLed::YELLOW, FINGERPRINT_ENROLL_TIMEOUT + 1000);
  startedWaiting = millis();
  while (p != FINGERPRINT_OK && p != _timeout) {
    if (millis() - startedWaiting > FINGERPRINT_ENROLL_TIMEOUT) {
      p = _timeout;
    }
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        _status = OLED_DEBUG("Enroll.getImage: OK", "Đã đọc, xin chờ!");
        buzzer();
        break;
      case FINGERPRINT_NOFINGER:
        FINGER_IS_BUSY = false;
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        _status = OLED_DEBUG("Enroll.getImage: PACKETRECIEVEERR", "(3) ERR: PACKETRECIEVEERR");
        ledError();
        FINGER_IS_BUSY = false;
        break;
      case FINGERPRINT_IMAGEFAIL:
        _status = OLED_DEBUG("Enroll.getImage: IMAGEFAIL", "(3) ERR: IMAGEFAIL");
        ledError();
        FINGER_IS_BUSY = false;
        break;
      case FINGERPRINT_ENROLL_TIMEOUT:
        _status = OLED_DEBUG("Enroll timeout", "HET GIO");
        ledError();
        FINGER_IS_BUSY = false;
        break;
      default:
        _status = OLED_DEBUG("Enroll.getImage: Unknown error: " + String(p, HEX), "(3) ERR: Unknown: " + String(p, HEX));
        ledError();
        FINGER_IS_BUSY = false;
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      _status = OLED_DEBUG("Enroll.image2Tz(2): IMAGEMESS", "(4) ERR: IMAGEMESS");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = OLED_DEBUG("Enroll.image2Tz(2): PACKETRECIEVEERR", "(4) ERR: PACKETRECIEVEERR");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_FEATUREFAIL:
      _status = OLED_DEBUG("Enroll.image2Tz(2): FEATUREFAIL", "(4) ERR: FEATUREFAIL");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_INVALIDIMAGE:
      _status = OLED_DEBUG("Enroll.image2Tz(2): INVALIDIMAGE", "(4) ERR: INVALIDIMAGE");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    default:
      _status = OLED_DEBUG("Enroll.image2Tz(2): Unknown error: " + String(p, HEX), "(4) ERR: Unknown: " + String(p, HEX));
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
  }

  // OK converted!

  p = finger.createModel();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = OLED_DEBUG("Enroll.createModel: PACKETRECIEVEERR", "(5) ERR: PACKETRECIEVEERR");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_ENROLLMISMATCH:
      _status = OLED_DEBUG("Enroll.createModel: ENROLLMISMATCH", "(5) FINGER 2 TIMES NOT MATCH");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    default:
      _status = OLED_DEBUG("Enroll.createModel: Unknown error: " + String(p, HEX), "(5) ERR: Unknown: " + String(p, HEX));
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
  }

  p = finger.storeModel(id.toInt());
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      _status = OLED_DEBUG("Enroll.storeModel: PACKETRECIEVEERR", "(6) ERR: PACKETRECIEVEERR");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_BADLOCATION:
      _status = OLED_DEBUG("Enroll.storeModel: BADLOCATION", "(6) ERR: BADLOCATION!");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    case FINGERPRINT_FLASHERR:
      _status = OLED_DEBUG("Enroll.storeModel: FLASHERR", "(6) ERR: FLASHERR");
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
    default:
      _status = OLED_DEBUG("Enroll.storeModel: Unknown error: " + String(p, HEX), "(6) ERR: Unknown: " + String(p, HEX));
      ledError();
      FINGER_IS_BUSY = false;
      return _status;
  }
  OLED_DEBUG("Stored: " + String(finger.fingerID), "WAIT FOR SAVE...!");
  finger_pauseCapture(3000);
  rs485_send("f", "enrolled", String(finger.fingerID), "");
  FINGER_IS_BUSY = false;
  return id;
}

String finger_delete(String id) {
  FINGER_IS_BUSY = true;
  uint8_t p = -1;
  String resp = Rs485_data.value;

  p = finger.deleteModel(id.toInt());

  if (p == FINGERPRINT_OK) {
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    resp = OLED_DEBUG("deleteModel: PACKETRECIEVEERR", "(1) ERR: PACKETRECIEVEERR");
  } else if (p == FINGERPRINT_BADLOCATION) {
    resp = OLED_DEBUG("deleteModel: BADLOCATION", "(1) ERR: BADLOCATION");
  } else if (p == FINGERPRINT_FLASHERR) {
    resp = OLED_DEBUG("deleteModel: FLASHERR", "(1) ERR: FLASHERR");
  } else {
    resp = OLED_DEBUG("deleteModel: Unknown: " + String(p, HEX), "(1) ERR: Unknown: " + String(p, HEX));
  }
  FINGER_IS_BUSY = false;
  return resp;
}

String finger_emptyDatabase() {
  FINGER_IS_BUSY = true;
  finger.emptyDatabase();
  FINGER_IS_BUSY = false;
  return Rs485_data.value;
}

int finger_handleCmd(void) {
  while (FINGER_IS_BUSY) {}
  String resp = Rs485_data.value;
  if (Rs485_data.cmd == "ping") {
    if (!finger.verifyPassword()) {
      resp = "Fingerprint verify failed";
    }
  } else if (Rs485_data.cmd == "enroll") {
    resp = finger_enroll(Rs485_data.value);
  } else if (Rs485_data.cmd == "delete") {
    resp = finger_delete(Rs485_data.value);
  } else if (Rs485_data.cmd == "empty") {
    resp = finger_emptyDatabase();
  } else if (Rs485_data.cmd == "getid") {
    finger.getParameters();
    rs485_println("f", Rs485_data.cmd, Rs485_data.cmdid, String(finger.system_id, HEX), "");
    return 1;
  } else if (Rs485_data.cmd == "slow") {
    finger_startCaptureWithInterval(FINGERPRINT_CAPTURE_SLOW_INTERVAL);
  } else if (Rs485_data.cmd == "normal" || Rs485_data.cmd == "start") {
    finger_setState("start");
  } else if (Rs485_data.cmd == "stop") {
    finger_setState("stop");
  } else if (Rs485_data.cmd == "restart") {
    finger_pauseCapture(3000);
  } else {
    resp = "NO CMD";
  }
  return rs485_response("f", resp);
}
