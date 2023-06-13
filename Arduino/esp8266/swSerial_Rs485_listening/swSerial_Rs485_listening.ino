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
   
  Serial.println(" ");
  Serial.println("Listening...");

  t.setInterval(swSerialListen, 10);
}

void loop() {
  t.handle();
}

void swSerialListen() {
  String rec;
  while (swSerial.available() > 0) {
    delay(10);
    rec = swSerial.readString();
  }

  if (rec.length() > 0) {
    Serial.print("Received: ");
    Serial.println(rec);
    rec = "";
  }
}
