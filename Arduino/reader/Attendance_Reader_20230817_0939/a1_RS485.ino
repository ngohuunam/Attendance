const int RS485_DATA_LEN = 6;
bool DEBUG = 0;
String cmdID = "";

struct {
  String device = "";
  String fn = "";
  String cmd = "";
  String cmdid = "";
  String value = "";
  String err = "";
} Rs485_data;

int rs485_parseData(String str) {
  str.trim();
  int r = 0, t = 0, i = 0, len = str.length();
  String arr[RS485_DATA_LEN];
  for (i = 0; i < len; i++) {
    if (t == RS485_DATA_LEN) {
      break;
    } else if (str.charAt(i) == ',') {
      arr[t] = str.substring(r, i);
      r = (i + 1);
      t++;
    }
  }
  arr[t] = str.substring(r, i);

  Rs485_data.device = arr[0];
  Rs485_data.fn = arr[1];
  Rs485_data.cmd = arr[2];
  Rs485_data.cmdid = arr[3];
  Rs485_data.value = arr[4];
  if (arr[5].length() > 0) {
    Rs485_data.err = arr[5];
  }

  return 0;
}

int rs485_println(String fn, String cmd, String cmdid, String value, String err) {
  while (rs485Serial.availableForWrite() != 1) {};
  String str = DEVICE + "," + fn + "," + cmd + "," + cmdid;
  if (value.length() > 0) {
    str += ",";
    str += value;
  }
  if (err.length() > 0) {
    str += ",";
    str += err;
  }
  str += ".";
  if (DEBUG) {
    oledDrawText(2, "rs485_println: " + str);
  }
  rs485Serial.print(str);
  rs485Serial.flush();
  return 1;
}

int rs485_send(String fn, String cmd, String value, String err) {
  String cmdID = String(random(100, 300));
  return rs485_println(fn, cmd, cmdID, value, err);
}

int rs485_response(String fn, String resp) {
  if (resp == Rs485_data.value) {
    return rs485_println(fn, Rs485_data.cmd, Rs485_data.cmdid, resp, "");
  } else {
    return rs485_println(fn, Rs485_data.cmd, Rs485_data.cmdid, "err", resp);
  }
  return 0;
}

int rs485_handleInputStr(String _str) {
  if (_str.length() < 4) return 0;
  rs485_parseData(_str);
  if (DEBUG) {
    oledDrawText(3, _str + "-" + Rs485_data.device + "," + Rs485_data.fn + "," + Rs485_data.cmd + "," + Rs485_data.cmdid + "," + Rs485_data.value);
    oledDrawString(1, _str);
  }
  if (Rs485_data.device != DEVICE & Rs485_data.device != "all") {
    return 0;
  }
  resetNotifyServerCommunicate();
  if (Rs485_data.fn == "f") {
    finger_handleCmd();
  } else if (Rs485_data.fn == "q") {
    qr_handleCmd();
  } else if (Rs485_data.fn == "r") {
    reader_handleCmd();
  } else if (Rs485_data.fn == "o") {
    oled_handleCmd();
  } else if (Rs485_data.fn == "l") {
    led_handleCmd();
  }
  return 1;
}

void rs485_input(void) {
  while (rs485Serial.available() > 0) {
    delay(10);
    String _str = rs485Serial.readStringUntil('.');
    rs485_handleInputStr(_str);
  }
}

void rs485_start() {
  TimerID.RS485_INPUT =  t.setInterval(rs485_input, RS485_INPUT_INTERVAL);
  rs485Serial.println('.');
  rs485Serial.flush();
  delay(20);
}

void rs485_stop() {
  rs485Serial.flush();
  t.cancel(TimerID.RS485_INPUT);
}
