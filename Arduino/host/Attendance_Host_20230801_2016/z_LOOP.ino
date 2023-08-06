

void setup() {
  // put your setup code here, to run once:
  setCpuFrequencyMhz(40);
  delay(100);

  setupOled();
  oledDrawText(1, "Startup");
  setupSerial();

  pinMode(LED_BUILTIN_PIN, OUTPUT);

  for (int i = 1; i < 7; i++) {
    pinMode(RL_PINs[i], OUTPUT);
    digitalWrite(RL_PINs[i], LOW);
  }

  oledDrawString(1, "DONE SETUP...");

  //  t.setInterval(pushRelay_1, 10000);
  //  triggerRelay_2();
  ledStartBreath();
}

void loop() {
  // put your main code here, to run repeatedly:
  rs485_input();
  t.handle();
}
