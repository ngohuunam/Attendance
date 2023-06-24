#include <SoftwareSerial.h>

#define RX_PIN D2
#define TX_PIN D3
EspSoftwareSerial::UART swSerial(RX_PIN, TX_PIN);

#include <AsyncTimer.h>

AsyncTimer t;

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);

  swSerial.begin(9600);

  Serial.println("Start sending...");

  t.setInterval(swSerialSend, 5000);
}

void loop() {
  t.handle();
}

void swSerialSend() {
  swSerial.println(String(random(100, 200)));
}
