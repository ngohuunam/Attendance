#include "library_define.h"
#include "define.h"

#define FINGER_CHECK_AVAILABLE_INTERVAL 5000

enum eFingerWork {
  CHECK_AVAILABLE = 0,
  PRINT_INFO,
  WAIT,
  START_READ,
  READING,
  MATCHED,
  START_ENROLL,
  ENROLLING,
  ENROLL_SUCCESS,
  EMPTY_DATABASE,
  ERR,
  NOT_AVAILABLE
};

enum eFingerResponse {
  NO_FINGER_DETECTED,
  IMAGE_TAKEN,
  COMMUNICATION_ERROR,
  IMAGING_ERROR,
  UNKNOWN_ERROR,
  IMAGE_CONVERTED,
  IMAGE_TOO_MESSY,
  COULD_NOT_FIND_FINGERPRINT_FEATURES,
  FOUND_A_PRINT_MATCH,
  DID_NOT_FIND_A_MATCH,
};

enum eFingerProcess {
  GET_IMAGE,
  IMAGE_2_TZ,
  FINGER_SEARCH,
  MACTH_FINGER,
};

typedef struct {
  //  tHardwareStatus *STATUS;
  byte OK = 254;
  String STATUS = "Start up";
  String DATA = "";
  uint16_t MATCHED_ID;
  uint16_t MATCHED_CONFIDENCE;
  uint8_t CODE;
  enum eFingerWork WORK;
  enum eFingerResponse RESPONSE;
  enum eFingerProcess PROCESS;
  void (*WORK_FUNC)();
} tFingerStatus;

tTimerID FingerTimer;
tFingerStatus FingerStatus;

#define FINGER_TIMER_MS 11
