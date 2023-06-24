#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

SoftwareSerial swSerial(D2, D3);  //D2 = RX -- D3 = TX

#include <AsyncTimer.h>

AsyncTimer t;

#define GM861_GUI_LENGTH 30  // Max number of code characters in GUI
// #define HEARTBEAT_RESPONSE 31

enum Gm861Cmd {
  GM861_SETTING_NORMAL,
  GM861_DEEP_SLEEP,
  GM861_WAKE_UP,
  GM861_TRIGGER,
  GM861_SAVE
};

// typedef struct {
//   char barcode[GM861_GUI_LENGTH];
//   uint8_t index;
//   uint8_t state;
//   uint8_t heartbeat;
//   bool read;
// } tGm861;
// tGm861 *Gm861 = nullptr;

// enum Gm861States {
//   GM861_STATE_DONE,
//   GM861_STATE_DUMP,
//   GM861_STATE_SERIAL_OUTPUT,
//   GM861_STATE_OUTPUT,
//   GM861_STATE_SETUP_CODE_ON,
//   GM861_STATE_INIT_OFFSET = 16,     // Init after (GM861_STATE_INIT_OFFSET - GM861_STATE_SETUP_CODE_ON) * 0.25 seconds restart
//   GM861_STATE_RESET
// };

void setup() {
  // Open serial communications and wait for port to open:
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  Serial.begin(115200);
  swSerial.begin(9600);
  t.setInterval(read_GM861, 10);
  t.setInterval(read_Serial_Cmd, 10);
  Gm861Heartbeat();
  t.setInterval(Gm861Heartbeat, 10000);
  // t.setInterval(send_GM861_Setting_Normal, 50);
}
// String str;

void loop() {  // run over and over
  // read_GM861();
  // read_Serial_Cmd();
  t.handle();
}

void read_Serial_Cmd() {
  String cmd = "";
  while (Serial.available() > 0) {
    delay(10);
    cmd = Serial.readStringUntil('\n');
  }
  if (cmd.length() > 0) {
    Serial.print("cmd: ");
    Serial.println(cmd);
    if (cmd == "s") {
      // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
      // 7E 00 08 01 00 D9 A5    3E 69   02 00 00 01 00    33 31   Deep sleep, can be awakened by serial port interrupt, this serial port command is invalid
      Gm861Send(8, 0xD9, 0xA5, 0x3E, 0x69);
      Serial.print("Send: ");
      Serial.println(cmd);
    } else if (cmd == "w") {
      // 7E 00 08 01 00 D9 00    DB 26   02 00 00 01 00    33 31   Wake up from sleep
      Gm861Send(8, 0xD9, 0, 0xDB, 0x26);
      Serial.print("Send: ");
      Serial.println(cmd);
    } else if (cmd == "t") {
      // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
      // 7E 00 08 01 00 02 01    AB CD   02 00 00 01 00    33 31   Command trigger Mode (Set bit0 of zone byte 0x0002)
      Gm861Send(0x08, 0x02, 0x01, 0xAB, 0xCD);
      Serial.print("Send: ");
      Serial.println(cmd);
    } else if (cmd == "tm") {
      // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
      // 7E 00 08 01 00 02 01    AB CD                             Trigger mode
      Gm861Send(8, 2, 1, 0xAB, 0xCD);
      Serial.print("Send: ");
      Serial.println(cmd);
    } else if (cmd == "f") {
      // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
      // 7E 00 08 01 00 00 D6    AB CD                             LED on, Mute off, Normal lighting, Normal brightness, Continuous mode
      Gm861Send(8, 0, 0xD6, 0xAB, 0xCD);
      Serial.print("Send: ");
      Serial.println(cmd);
    } else if (cmd == "u") {
      // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
      // 7E 00 08 01 00 D9 55    D1 76   02 00 00 01 00 33 31      Restore user - defined factory settings
      Gm861Send(8, 0xD9, 0x55, 0xD1, 0x76);
      Serial.print("Send: ");
      Serial.println(cmd);
    } else if (cmd == "on") {
      GM861_SET_CODE_ON();
    } else if (cmd == "off") {
      GM861_SET_CODE_OFF();
    } else if (cmd == "save") {
      send_GM861_CMD(GM861_SAVE);
    }
  }
}

void send_GM861_Setting_Normal() {
  send_GM861_CMD(GM861_SETTING_NORMAL);
}

void send_GM861_CMD(int act) {
  switch (act) {
    case GM861_SETTING_NORMAL:
      // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
      // 7E 00 08 01 00 00 D6    AB CD                             LED on, Mute off, Normal lighting, Normal brightness, Continuous mode
      Gm861Send(8, 0, 0xD6, 0xAB, 0xCD);
      Serial.println("Send cmd: GM861_SETTING_NORMAL 7E 00 08 01 00 00 D6 AB CD");
      break;

    case GM861_SAVE:
      // Headr Ty Ln Addrs Data  Check   Headr Ty Ln Data  Check
      // 7E 00 09 01 00 00 00    AB CD                             command to save:
      Gm861Send(9, 0, 0, 0xAB, 0xCD);
      Serial.println("Send cmd: GM861_SAVE 7E 00 09 01 00 00 00 AB CD");
      break;
  }
}

// bool is_GM861_CMD = false;

void read_GM861() {
  // char buffer[7] = { 0, 0, 0, 0, 0, 0, 0 };
  char buffer[7];
  uint32_t idx = 0;
  bool getdata = false;
  while (swSerial.available() > 0) {
    delay(10);
    buffer[idx++] = swSerial.read();
    getdata = true;
    if (idx >= sizeof(buffer) - 1) { break; }
  }

  if (getdata) {
    // Serial.println();
    String inString = "";
    if (buffer[0] == 2 || buffer[0] == 3) {
      for (int i = 0; i < 7; i++) {
        inString += String(buffer[i], HEX);
        inString += " ";
      }
      inString.toUpperCase();
    } else {
      inString = String(buffer);
    }

    if (inString.length() > 0) {
      Serial.print("Received: ");
      Serial.println(inString);
      inString = "";
    }
    getdata = false;
  }
}

void GM861_SET_CODE_ON() {
  String cmd = "_SET_CODE_ON";
  swSerial.print(cmd);
  // uint8_t buffer[12] = { 95, 83, 69, 84, 95, 67, 79, 68, 69, 95, 79, 78 };
  // swSerial.write(buffer, 12);
  // for (int i = 0; i < cmd.length(); i++) {
  //   swSerial.write(cmd[i]);
  // }
  Serial.println("GM861_SET_CODE_ON");
  send_GM861_CMD(GM861_SAVE);
}

void GM861_SET_CODE_OFF() {
  String cmd = "_SET_CODE_OFF";

  for (int i = 0; i < cmd.length(); i++) {
    swSerial.write(cmd[i]);
  }
  send_GM861_CMD(GM861_SAVE);
  Serial.println("GM861_SET_CODE_OFF");
}

void Gm861Heartbeat(void) {
  Serial.println("Send heartbeat");
  Gm861Send(10, 0, 0, 0x30, 0x1A);
}

void Gm861Send(uint32_t type, uint32_t address, uint32_t data, uint32_t check1, uint32_t check2) {
  uint8_t buffer[9];

  buffer[0] = 0x7E;
  buffer[1] = 0;
  buffer[2] = type;
  buffer[3] = 1;
  buffer[4] = 0;
  buffer[5] = address;
  buffer[6] = data;
  buffer[7] = check1;
  buffer[8] = check2;

  String cmd = "";

  for (int i = 0; i < 9; i++) {
    cmd += String(buffer[i], HEX);
    cmd += " ";
  }
  cmd.toUpperCase();
  Serial.print("Send cmd: ");
  Serial.println(cmd);

  swSerial.write(buffer, 9);
}
