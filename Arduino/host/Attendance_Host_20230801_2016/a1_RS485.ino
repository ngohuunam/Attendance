
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
  oledDrawText(2, "output: " + str);
  rs485Serial.print(str);
  rs485Serial.flush();
  return 1;
}

int input_parseData(String str) {
  str.trim();
  int r = 0, t = 0, i = 0, len = str.length();
  String arr[RS485_DATA_LEN];
  for (i = 0; i < len; i++)
  {
    if (t == RS485_DATA_LEN)
    {
      break;
    }
    else if (str.charAt(i) == ',')
    {
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

int host_handleCmd(void) {
  String resp = Rs485_data.value;
  if (Rs485_data.cmd == "ping") {
  }
  else if (Rs485_data.fn == "rl") {
    int RL_NO = 0;
    if (Rs485_data.value == "main"){
      RL_NO = MAIN;
    } else if (Rs485_data.value == "park"){
      RL_NO = PARK;
    } else if (Rs485_data.value == "control"){
      RL_NO = CONTROL;
    } else if (Rs485_data.value == "r1"){
      RL_NO = READER_1;
    } else if (Rs485_data.value == "r2"){
      RL_NO = READER_2;
    } else if (Rs485_data.value == "r3"){
      RL_NO = READER_3;
    } else {
      RL_NO = Rs485_data.value.toInt();
    }
    if (Rs485_data.cmd == "trigger") {
      triggerRelay(RL_NO);
    } else if (Rs485_data.cmd == "normal") {
      normalRelay(RL_NO);
    } else if (Rs485_data.cmd == "push") {
      pushRelay(RL_NO);
    } else {
      resp = "NO CMD";
    }
  }
  else {
    resp = "NO CMD";
  }
  return rs485_response(Rs485_data.fn, resp);
}

int rs485_handleInputStr(String str) {
  if (str.length() < 4 || str.startsWith(DEVICE) == false) return 0;
  input_parseData(str);
  if (DEBUG) {
    oledDrawString(1, str);
  }
  host_handleCmd();
  return 1;
}

void rs485_input(void)
{
  while (rs485Serial.available() > 0)
  {
    delay(10);
    String str = rs485Serial.readStringUntil('.');
    rs485_handleInputStr(str);
  }
}

int setupSerial(void) {
  Serial.begin(BAUDRATE);
  pinMode(RS485_RX, INPUT);
  pinMode(RS485_TX, OUTPUT);
  rs485Serial.begin(BAUDRATE);
  return 1;
}
