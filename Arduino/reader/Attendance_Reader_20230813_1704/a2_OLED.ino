const unsigned long OLED_DISPLAY_OFF_TIMEOUT = 1 * 5 * 60 * 1000;

int oledClearLine(int lineNo, int height) {
  int line = FONT_SIZE * (lineNo - 1);
  oled.setColor(BLACK);
  oled.fillRect(0, line, WIDTH, height + 2);
  oled.setColor(WHITE);
  return line;
}

void oledDrawString(int lineNo, const String &text)
{
  t.cancel(TimerID.OLED_DISPLAY_OFF_TIMEOUT);
  oled.displayOn();
  int line = oledClearLine(lineNo, FONT_SIZE);
  oled.drawString(0, line, text);
  oled.display();
  TimerID.OLED_DISPLAY_OFF_TIMEOUT = t.setTimeout(oledClearText, OLED_DISPLAY_OFF_TIMEOUT);
}

void oledDrawText(int lineNo, const String &text)
{
  t.cancel(TimerID.OLED_DISPLAY_OFF_TIMEOUT);
  oled.displayOn();
  int line = oledClearLine(lineNo, HEIGHT);
  oled.drawStringMaxWidth(0, line, WIDTH, text);
  oled.display();
  TimerID.OLED_DISPLAY_OFF_TIMEOUT = t.setTimeout(oledClearText, OLED_DISPLAY_OFF_TIMEOUT);
}

void oledClearText(void)
{
  oled.clear();
  oled.setColor(BLACK);
  oled.fillRect(0, 0, WIDTH, HEIGHT);
  oled.display();
  oled.displayOff();
}

void setupOled(void)
{
  //  pinMode(RST_OLED, OUTPUT);
  //  digitalWrite(RST_OLED, LOW);
  //  delay(20);
  //  digitalWrite(RST_OLED, HIGH);
  oled.setFont(ArialMT_Plain_10);
  oled.setTextAlignment(TEXT_ALIGN_LEFT);

  oled.init();
  //  oled.flipScreenVertically();
  oledDrawString(1, "START UP...");
}

int oled_handleCmd(void) {

  String resp = Rs485_data.value;
  if (Rs485_data.cmd == "off") {
    oled.displayOff();
  } else if (Rs485_data.cmd == "on") {
    oled.displayOn();
  } else if (Rs485_data.cmd == "flip") {
    oled.flipScreenVertically();
  } else if (Rs485_data.cmd == "draw") {
    oledDrawText(2, Rs485_data.value);
  } else if (Rs485_data.cmd == "clear") {
    oledClearText();
  } else {
    resp = "NO CMD";
  }
  return rs485_response("o", resp);
}

String OLED_DEBUG(String debugText, String defaultText) {
  if (DEBUG) {
    oledDrawText(2, debugText);
  } else {
    oledDrawText(2, defaultText);
  }
  return debugText;
}
