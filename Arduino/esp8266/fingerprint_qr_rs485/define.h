
String qrStr = "";

#define HW_STATUS_OK 1
#define HW_STATUS_NOT_OK 0

//enum qrCmd {
//  QR_READING
//};
//enum mcuCmd {
//  MCU_RESET
//};
//
//enum eHardwareCmd {
//  HW_CMD_ON,
//  HW_CMD_OFF,
//  HW_CMD_NORMAL_RUNNING
//};

//enum eFingerCmd {
//  FINGER_CMD_ENROLL,
//  FINGER_CMD_DELETE,
//  FINGER_CMD_EMPTY_DATABASE
//};

enum eSTICKING {
  NOTHING = 0,
  STARTUP,
  ALL,
  FINGER,
  QR,
  RS485
};

enum eSTICKING tick = NOTHING;

typedef struct {
  unsigned short CHECK = 0;
  unsigned short RUNNING = 0;
  unsigned short NORMAL_RUNNING = 0;
} tTimerID;

typedef struct {
  byte OK;
  String STATUS;
  String DATA;
} tHardwareStatus;
