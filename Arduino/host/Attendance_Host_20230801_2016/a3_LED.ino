const unsigned long LED_BREATH_INTERVAL = 4000;

void ledOn(){
  digitalWrite(LED_BUILTIN_PIN, HIGH);
}

void ledOff(){
  digitalWrite(LED_BUILTIN_PIN, LOW);
}

void ledBreath() {
  ledOn();
  TimerID.LED_OFF_TIMEOUT = t.setTimeout(ledOff, LED_BREATH_INTERVAL/3 * 2);
}

void ledStartBreath() {
  TimerID.LED_BREATH = t.setInterval(ledBreath, LED_BREATH_INTERVAL);
}

void ledStopBreath() {
  t.cancel(TimerID.LED_OFF_TIMEOUT);
  t.cancel(TimerID.LED_BREATH);
}
