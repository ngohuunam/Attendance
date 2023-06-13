

String qrStr = "";

#define HW_STATUS_OK 1
#define HW_STATUS_NOT_OK 0

enum qrCmd {
  QR_READING
};
enum mcuCmd {
  MCU_RESET
};

enum eHardwareCmd {
  HW_CMD_ON,
  HW_CMD_OFF,
  HW_CMD_NORMAL_RUNNING
};

enum eFingerCmd {
  FINGER_CMD_ENROLL,
  FINGER_CMD_DELETE,
  FINGER_CMD_EMPTY_DATABASE
};

enum eSTICKING {
  NOTHING,
  FINGER,
  QR,
  RS485
};

enum eSTICKING stick = NOTHING;

typedef struct {
  unsigned short CHECK = 0;
  unsigned short RUNNING = 0;
  unsigned short NORMAL_RUNNING = 0;
} tTimerID;

typedef struct {
  byte OK;
  byte NEXT_STICK;
  enum eHardwareCmd CMD;
  String STATUS;
  String DATA;
  bool IS_BUSY;
  void *RUNNING_FUNC;
} tHardwareStatus;
