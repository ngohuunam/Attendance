
#define HW_STATUS_OK 1
#define HW_STATUS_NOT_OK 0

enum eTICKING {
  NOTHING = 0,
  STARTUP,
  ALL,
  FINGER,
  QR,
  RS485,
  RS485_QR,
  RS485_FINGER
} tick = NOTHING;

//eTICKING tick = NOTHING;

typedef struct {
  unsigned short CHECK = 0;
  unsigned short RUNNING = 0;
  unsigned short HEARTBEAT = 0;
  unsigned short RETRY = 0;
} tTimerID;

typedef struct {
  byte OK;
  String STATUS;
  String DATA;
} tHardwareStatus;
