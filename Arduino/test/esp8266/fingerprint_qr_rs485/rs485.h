//#include "define.h"

//#define RS485_BUFFER_LENGTH 9
#define RS485_HEARTBEAT_INTERVAL 2000
#define RS485_TIMER_MS 9

tTimerID Rs485Timer;

#define RS485_RES_OK 0xAA
#define RS485_RES_REJECT 0xBA
#define RS485_RES_NOT_AVAILABLE 0xBB
#define RS485_RES_ERROR 0xBC

enum eRS485Work {
  RS485_WORK_LISTENING = 0,
  RS485_WORK_SEND_CODE,
  RS485_WORK_GET_CODE,
  RS485_WORK_ERR,
  RS485_WORK_NOT_AVAILABLE
};

typedef struct {
  String READ_DATA;
  char* SEND_DATA;
  uint32_t SEND_HEADER = 0x7C;
  uint8_t HEARTBEAT = 0;
  void (*WORK_FUNC)();
  enum eRS485Work WORK = RS485_WORK_LISTENING;
} tRs485State;

tRs485State Rs485State;

void rs485Input(void) {
  if (rs485.available() > 0) {
    delay(10);
    Rs485State.READ_DATA = rs485.readString();
    Serial.print("Rs485State.READ_DATA: "); Serial.println(Rs485State.READ_DATA);
    if (Rs485State.READ_DATA == "heartbeat") {
      Rs485State.HEARTBEAT = 0;
      Serial.println("RS485 Heartbeat ok");
    } else {                             // Bar code
      Rs485State.WORK = RS485_WORK_GET_CODE;
      tick = RS485;
    }
  }
}

void rs485Heartbeat(void) {
  //  rs485Send(SERIAL_RS485_HEADER, 0, 0);            // Send heartbeat
  rs485.print("heartbeat");
  Rs485State.HEARTBEAT++;
  // If no correct reply is received for three consecutive times, the main control should be handle it accordingly.
  if (Rs485State.HEARTBEAT > 3) {
    Rs485State.HEARTBEAT = 0;
    Serial.println("RS485 Heartbeat fail");
  }
}

void rs485SendCode(void) {
  Serial.print("RS485 get code: -HEADER: "); Serial.print(Rs485State.SEND_HEADER, HEX); Serial.print(" -DATA: "); Serial.println(Rs485State.SEND_DATA);
  rs485Send(Rs485State.SEND_HEADER, 0, (uint32_t)atoi(Rs485State.SEND_DATA));
}

void (* rs485WorkFunctions [5])() = {
  rs485Input,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

void rs485AssignWorkFunction(enum eRS485Work _work) {
  t.cancel(Rs485Timer.RUNNING);
  Rs485State.WORK_FUNC = rs485WorkFunctions[_work];
  Rs485Timer.RUNNING = t.setInterval(Rs485State.WORK_FUNC, RS485_TIMER_MS);
}

void rs485UpdateWorkFunction() {
  Serial.print("rs485UpdateWorkFunction: "); Serial.println(Rs485State.WORK);
  rs485AssignWorkFunction(Rs485State.WORK);
}
