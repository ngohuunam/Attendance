#include "define_swserial.h"
#include "define.h"

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

String fingerID = "";

enum eFingerWork {
  START_READ = 0,
  READING,
  MATCHED,
  START_ENROLL,
  ENROLLING,
  ENROLL_SUCCESS,
  EMPTY_DATABASE,
  ERR,
  NOT_AVAILABLE
};

enum eFingerReadResponse {
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

enum eFingerReadProcess {
  GET_IMAGE,
  IMAGE_2_TZ,
  FINGER_SEARCH,
  MACTH_FINGER,
};

typedef struct {
  tHardwareStatus *STATUS;
  enum eFingerReadResponse READ_RESPONSE;
  enum eFingerReadProcess READ_PROCESS;
  enum eFingerWork WORK;
} tFingerStatus;

typedef struct {
  tTimerID *ID;
  uint8_t retry;
} tFingerTimer;

tFingerTimer *FingerTimer = nullptr;
tFingerStatus *FingerStatus = nullptr;

#define FINGER_TIMER_MS 11
