void triggerRelay(int RL_NO) {
  const int RL_PIN = RL_PINs[RL_NO];
  digitalWrite(RL_PIN, HIGH);
//  digitalWrite(LED_BUILTIN_PIN, HIGH);
  delay(10);
}

void normalRelay(int RL_NO) {
  const int RL_PIN = RL_PINs[RL_NO];
  digitalWrite(RL_PIN, LOW);
//  digitalWrite(LED_BUILTIN_PIN, LOW);
  delay(10);
}

void pushRelay(int RL_NO) {
  triggerRelay(RL_NO);
  delay(300);
  normalRelay(RL_NO);
}

void pushRelay_1() {
  pushRelay(1);
}

void triggerRelay_2() {
  triggerRelay(2);
}
