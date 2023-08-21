
int reader_handleCmd(void) {
  String resp = Rs485_data.value;
  if (Rs485_data.cmd == "ping") {
  } else if (Rs485_data.cmd == "version") {
    rs485_println("r", Rs485_data.cmd, Rs485_data.cmdid, VERSION, "");
    return 1;
  } else if (Rs485_data.cmd == "restart") {
    ESP.restart();
  } else if (Rs485_data.cmd == "debug") {
    if (Rs485_data.value == "on") {
      DEBUG = true;
    } else if (Rs485_data.value == "off") {
      DEBUG = false;
    }
  } else {
    resp = "NO CMD";
  }
  return rs485_response("r", resp);
}

void setupSerial(void)
{

  oledDrawText(2, "Start serial...");

  pinMode(RS485_RX_PIN, INPUT);
  pinMode(RS485_TX_PIN, OUTPUT);

  pinMode(FINGER_RX_PIN, INPUT);
  pinMode(FINGER_TX_PIN, OUTPUT);

  pinMode(QR_RX_PIN, INPUT);
  pinMode(QR_TX_PIN, OUTPUT);

  rs485Serial.begin(BAUDRATE);
  fingerSerial.begin(BAUDRATE);
  qrSerial.begin(BAUDRATE);

  oledDrawString(1, "Setup serial done!");
}

void setup() {
  // put your setup code here, to run once:
  setCpuFrequencyMhz(80);
  delay(100);

  setupOled();
  delay(10);

  setupSerial();
  delay(10);

  pinMode(QR_TOGGLE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  led.off();
  led.brightness(50);
  ledBlueBreath();

//  finger_setState(FINGER_STARTUP_STATE);
  qr_setState(QR_STARTUP_STATE);
  rs485_start();
//  TimerID.CONTROL_NOT_COMMUNICATE = t.setTimeout(controlNotCommunicate, CONTROL_NOT_COMMUNICATE_TIMEOUT);
  rs485_send("r", "startup", VERSION, "");
  buzzer();
}

void buzzer_On(void) {
  if (BUZZER_STATE == LOW) {
    BUZZER_STATE = HIGH;
    digitalWrite(BUZZER_PIN, BUZZER_STATE);
  }
}

void buzzer_Off(void) {
  if (BUZZER_STATE == HIGH) {
    BUZZER_STATE = LOW;
    digitalWrite(BUZZER_PIN, BUZZER_STATE);
  }
}

void buzzer(void) {
  if (BUZZER_REMAIN > 0) {
    BUZZER_REMAIN--;
  }
  buzzer_On();
  TimerID.BUZZER_TOGGLE_TIMEOUT = t.setTimeout(buzzer_Off, BUZZER_TOGGLE_MS);
}

void buzzers(byte times) {
  if (BUZZER_REMAIN == 0) {
    BUZZER_REMAIN = times - 1;
    buzzer();
    for (int i = 0; i <= BUZZER_REMAIN; i++) {
      t.setTimeout(buzzer, i * 2 * BUZZER_TOGGLE_MS);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //  rs485_input();
  //  qr_input();
  t.handle();
}

void pingControl() {
  rs485_send("r", "ping", "", "");
}

void resetNotifyServerCommunicate() {
  if (CONTROL_IS_NOT_COMMUNICATE) {
    t.cancel(TimerID.PING_CONTROL);
    if (led_status != "bluebreath") {
      ledBlueBreath();
    }
    finger_setState(finger_state);
    qr_setState(qr_state);
  }
  CONTROL_IS_NOT_COMMUNICATE = false;
//  t.reset(TimerID.CONTROL_NOT_COMMUNICATE);
}

void controlNotCommunicate() {
  if (CONTROL_IS_NOT_COMMUNICATE == false) {
    OLED_DEBUG("serverNotCommunicate", "KHONG CO KET NOI");
    finger_stopCapture();
    qr_offReading();
    ledFlashErrorFast();
    TimerID.PING_CONTROL = t.setInterval(pingControl, PING_CONTROL_INTERVAL);
    CONTROL_IS_NOT_COMMUNICATE = true;
  }
}
