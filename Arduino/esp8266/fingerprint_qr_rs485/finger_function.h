#include "finger_define.h"

//uint8_t fingerEnroll() {
//
//  int p = -1;
//  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
//  while (p != FINGERPRINT_OK) {
//    p = finger.getImage();
//    switch (p) {
//    case FINGERPRINT_OK:
//      Serial.println("Image taken");
//      break;
//    case FINGERPRINT_NOFINGER:
//      Serial.println(".");
//      break;
//    case FINGERPRINT_PACKETRECIEVEERR:
//      Serial.println("Communication error");
//      break;
//    case FINGERPRINT_IMAGEFAIL:
//      Serial.println("Imaging error");
//      break;
//    default:
//      Serial.println("Unknown error");
//      break;
//    }
//  }
//
//  // OK success!
//
//  p = finger.image2Tz(1);
//  switch (p) {
//    case FINGERPRINT_OK:
//      Serial.println("Image converted");
//      break;
//    case FINGERPRINT_IMAGEMESS:
//      Serial.println("Image too messy");
//      return p;
//    case FINGERPRINT_PACKETRECIEVEERR:
//      Serial.println("Communication error");
//      return p;
//    case FINGERPRINT_FEATUREFAIL:
//      Serial.println("Could not find fingerprint features");
//      return p;
//    case FINGERPRINT_INVALIDIMAGE:
//      Serial.println("Could not find fingerprint features");
//      return p;
//    default:
//      Serial.println("Unknown error");
//      return p;
//  }
//
//  Serial.println("Remove finger");
//  delay(2000);
//  p = 0;
//  while (p != FINGERPRINT_NOFINGER) {
//    p = finger.getImage();
//  }
//  Serial.print("ID "); Serial.println(id);
//  p = -1;
//  Serial.println("Place same finger again");
//  while (p != FINGERPRINT_OK) {
//    p = finger.getImage();
//    switch (p) {
//    case FINGERPRINT_OK:
//      Serial.println("Image taken");
//      break;
//    case FINGERPRINT_NOFINGER:
//      Serial.print(".");
//      break;
//    case FINGERPRINT_PACKETRECIEVEERR:
//      Serial.println("Communication error");
//      break;
//    case FINGERPRINT_IMAGEFAIL:
//      Serial.println("Imaging error");
//      break;
//    default:
//      Serial.println("Unknown error");
//      break;
//    }
//  }
//
//  // OK success!
//
//  p = finger.image2Tz(2);
//  switch (p) {
//    case FINGERPRINT_OK:
//      Serial.println("Image converted");
//      break;
//    case FINGERPRINT_IMAGEMESS:
//      Serial.println("Image too messy");
//      return p;
//    case FINGERPRINT_PACKETRECIEVEERR:
//      Serial.println("Communication error");
//      return p;
//    case FINGERPRINT_FEATUREFAIL:
//      Serial.println("Could not find fingerprint features");
//      return p;
//    case FINGERPRINT_INVALIDIMAGE:
//      Serial.println("Could not find fingerprint features");
//      return p;
//    default:
//      Serial.println("Unknown error");
//      return p;
//  }
//
//  // OK converted!
//  Serial.print("Creating model for #");  Serial.println(id);
//
//  p = finger.createModel();
//  if (p == FINGERPRINT_OK) {
//    Serial.println("Prints matched!");
//  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//    Serial.println("Communication error");
//    return p;
//  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
//    Serial.println("Fingerprints did not match");
//    return p;
//  } else {
//    Serial.println("Unknown error");
//    return p;
//  }
//
//  Serial.print("ID "); Serial.println(id);
//  p = finger.storeModel(id);
//  if (p == FINGERPRINT_OK) {
//    Serial.println("Stored!");
//  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//    Serial.println("Communication error");
//    return p;
//  } else if (p == FINGERPRINT_BADLOCATION) {
//    Serial.println("Could not store in that location");
//    return p;
//  } else if (p == FINGERPRINT_FLASHERR) {
//    Serial.println("Error writing to flash");
//    return p;
//  } else {
//    Serial.println("Unknown error");
//    return p;
//  }
//
//  return true;
//}

void fingerCheckIsAvailable() {
  byte _verify = finger.verifyPassword();
  Serial.print("_verify: "); Serial.println(_verify);
  if (FingerStatus.OK != _verify) {
    FingerStatus.OK = finger.verifyPassword();
    switch (FingerStatus.OK) {
      case HW_STATUS_OK:
        t.cancel(FingerTimer.ID.RUNNING);
        t.cancel(FingerTimer.ID.CHECK);
        FingerTimer.ID.RUNNING = t.setInterval(FingerStatus.WORK_FUNC, FINGER_TIMER_MS);
        Serial.println("Finger Available");
        break;

      case HW_STATUS_NOT_OK:
        Serial.println("Finger not Available");
        FingerStatus.WORK = NOT_AVAILABLE;
        t.cancel(FingerTimer.ID.RUNNING);
        //        FingerTimer.ID.CHECK = t.setTimeout(fingerCheckIsAvailable, FINGER_CHECK_AVAILABLE_INTERVAL);
        break;
    }
    tick = FINGER;
  }
}

void fingerSetStartRead() {
  FingerStatus.MATCHED_ID = '\0';
  FingerStatus.MATCHED_CONFIDENCE = '\0';
  FingerStatus.WORK = START_READ;
  tick = FINGER;
}

void fingerHandleMatchedImage() {
  String str = "";
  str = "Found ID #" + String(FingerStatus.MATCHED_ID) + " with confidence of " + String(FingerStatus.MATCHED_CONFIDENCE);
  Serial.println(str);
  fingerSetStartRead();
}

void fingerHandleError() {
  String str = "";
  str = "Finger has Error - code: " + String(FingerStatus.CODE)
        + "  - work: " + String(FingerStatus.WORK)
        + " - process: " + String(FingerStatus.PROCESS)
        + " - reponse: " + String(FingerStatus.RESPONSE);
  Serial.println(str);
  fingerSetStartRead();
}

void fingerEnroll() {
  Serial.println("fingerEnroll()");
}

void fingerEmptyDatabase() {
  Serial.println("fingerEmptyDatabase()");
}

void fingerReading() {
  FingerStatus.STATUS = "Reading";
  FingerStatus.CODE = finger.getImage();
  FingerStatus.PROCESS = GET_IMAGE;
  switch (FingerStatus.CODE) {
    case FINGERPRINT_OK:
      FingerStatus.STATUS = "Image taken";
      FingerStatus.RESPONSE = IMAGE_TAKEN;
      break;
    case FINGERPRINT_NOFINGER:
      FingerStatus.STATUS = "No finger detected";
      FingerStatus.RESPONSE = NO_FINGER_DETECTED;
      return;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerStatus.STATUS = "Image taken";
      FingerStatus.RESPONSE = COMMUNICATION_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_IMAGEFAIL:
      FingerStatus.STATUS = "Imaging error";
      FingerStatus.RESPONSE = IMAGING_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    default:
      FingerStatus.STATUS = "Image taken Unknown error";
      FingerStatus.RESPONSE = UNKNOWN_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
  }

  // OK success!

  FingerStatus.CODE = finger.image2Tz();
  FingerStatus.PROCESS = IMAGE_2_TZ;
  switch (FingerStatus.CODE) {
    case FINGERPRINT_OK:
      FingerStatus.STATUS = "Image converted";
      FingerStatus.RESPONSE = IMAGE_CONVERTED;
      break;
    case FINGERPRINT_IMAGEMESS:
      FingerStatus.STATUS = "Image too messy";
      FingerStatus.RESPONSE = IMAGE_TOO_MESSY;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerStatus.STATUS = "Image convert Communication error";
      FingerStatus.RESPONSE = COMMUNICATION_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_FEATUREFAIL:
      FingerStatus.STATUS = "Could not find fingerprint features";
      FingerStatus.RESPONSE = COULD_NOT_FIND_FINGERPRINT_FEATURES;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_INVALIDIMAGE:
      FingerStatus.STATUS = "Could not find fingerprint features";
      FingerStatus.RESPONSE = COULD_NOT_FIND_FINGERPRINT_FEATURES;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    default:
      FingerStatus.STATUS = "Image convert Unknown error";
      FingerStatus.RESPONSE = UNKNOWN_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
  }

  // OK converted!
  FingerStatus.CODE = finger.fingerSearch();
  FingerStatus.PROCESS = FINGER_SEARCH;
  switch (FingerStatus.CODE) {
    case FINGERPRINT_OK:
      FingerStatus.STATUS = "Found a print match";
      FingerStatus.RESPONSE = FOUND_A_PRINT_MATCH;
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      FingerStatus.STATUS = "Finger Search Communication error";
      FingerStatus.RESPONSE = COMMUNICATION_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
    case FINGERPRINT_NOTFOUND:
      FingerStatus.STATUS = "Did not find a match";
      FingerStatus.RESPONSE = DID_NOT_FIND_A_MATCH;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
      FingerStatus.STATUS = "Finger Search Unknown error";
      FingerStatus.RESPONSE = UNKNOWN_ERROR;
      FingerStatus.WORK = ERR;
      tick = FINGER;
      return;
  }

  FingerStatus.PROCESS = MACTH_FINGER;
  FingerStatus.WORK = MATCHED;
  FingerStatus.MATCHED_ID = finger.fingerID;
  FingerStatus.MATCHED_CONFIDENCE = finger.confidence;
  tick = FINGER;
}

// returns -1 if failed, otherwise returns ID #
//int getFingerprintIDez() {
//  uint8_t p = finger.getImage();
//  if (p != FINGERPRINT_OK)  return -1;
//
//  p = finger.image2Tz();
//  if (p != FINGERPRINT_OK)  return -1;
//
//  p = finger.fingerFastSearch();
//  if (p != FINGERPRINT_OK)  return -1;
//
//  // found a match!
//  Serial.print("Found ID #"); Serial.print(finger.fingerID);
//  Serial.print(" with confidence of "); Serial.println(finger.confidence);
//  return finger.fingerID;
//}

void fingerWait() {
  Serial.println("Finger waiting...");
}

void fingerPrintInfo() {

  if (FingerStatus.OK) {
    Serial.println("Found fingerprint sensor!");

    Serial.println("Reading sensor parameters");
    finger.getParameters();
    Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
    Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
    Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
    Serial.print(F("Security level: ")); Serial.println(finger.security_level);
    Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
    Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
    Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

    finger.getTemplateCount();

    if (finger.templateCount == 0) {
      Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    }
    else {
      Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
    }

    FingerStatus.WORK = START_READ;
    tick = FINGER;
  } else {
    Serial.println("Did not find fingerprint sensor :(");
  }
}

void (* fingerWorkFunctions [12])() = {
  fingerCheckIsAvailable,
  fingerPrintInfo,
  fingerWait,
  fingerReading,
  nullptr,
  fingerHandleMatchedImage,
  nullptr, nullptr, nullptr, nullptr,
  fingerHandleError,
  nullptr
};

void fingerAssignWorkFunction(enum eFingerWork _work) {
  t.cancel(FingerTimer.ID.RUNNING);
  FingerStatus.WORK_FUNC = fingerWorkFunctions[_work];
  FingerTimer.ID.RUNNING = t.setInterval(FingerStatus.WORK_FUNC, FINGER_TIMER_MS);
}

void fingerUpdateWorkFunction() {
  Serial.print("fingerUpdateWorkFunction: "); Serial.println(FingerStatus.WORK);
  fingerAssignWorkFunction(FingerStatus.WORK);
}
