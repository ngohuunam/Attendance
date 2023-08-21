void buzzer_setup(void) {
  pinMode(BUZZER_PIN, OUTPUT);
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
  //  TimerID.BUZZER_TOGGLE_TIMEOUT = t.setTimeout(buzzer_Off, BUZZER_TOGGLE_MS);
  delay(100);
  buzzer_Off();
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
