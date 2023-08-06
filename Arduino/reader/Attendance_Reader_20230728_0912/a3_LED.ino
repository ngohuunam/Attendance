int* led_color;
unsigned long led_ms = 4000;
String led_status = "";

void ledOn(void)
{
  led.fadeIn(led_color, LED_FADE_STEP, LED_FADE_MS);
}

void ledSetColor(int _color[3])
{
  led_color = _color;
}

void ledSetMs(unsigned long _ms)
{
  led_ms = LED_FADE_INTERVAL * 3;
  if (_ms > led_ms) {
    led_ms = _ms;
  }
}

void ledOnWithColor(int _color[3])
{
  ledSetColor(_color);
  ledOn();
}

void ledOff()
{
  t.cancel(TimerID.LED_FLASH_ON);
  t.cancel(TimerID.LED_FLASH_OFF);
  t.cancel(TimerID.LED_BLUE_BREATH_TIMEOUT);
  led.fadeOut(led_color, LED_FADE_STEP, LED_FADE_MS);
  led_status = "off";
}

void ledBlueOn(void)
{
  ledOnWithColor(RGBLed::BLUE);
  led_status = "blueon";
}

void ledFlash(void)
{
  ledOff();
  TimerID.LED_FLASH_ON = t.setTimeout(ledOn, led_ms / 2);
  TimerID.LED_FLASH_OFF = t.setTimeout(ledFlash, led_ms);
}

void ledFlashNotify(unsigned long ms)
{
  ledOff();
  TimerID.LED_FLASH_ON = t.setTimeout(ledOn, ms / 2);
  TimerID.LED_FLASH_OFF = t.setTimeout(ledFlash, ms);
}

void ledOnNotify(int _color[3])
{
  led.fadeIn(_color, LED_FADE_STEP, LED_FADE_MS);
}

void ledNotify(int _color[3], unsigned long ms)
{
  ledOff();
  ledOnNotify(_color);
  TimerID.LED_BLUE_BREATH_TIMEOUT = t.setTimeout(ledBlueBreath, ms);
  led_status = "notify";
}

void ledSuccess()
{
  ledNotify(RGBLed::GREEN, 2000);
  led_status = "success";
  buzzer();
}

void ledSuccessWithTime(unsigned long ms)
{
  ledNotify(RGBLed::GREEN, ms);
}

void ledError()
{
  ledNotify(RGBLed::RED, 2000);
  led_status = "error";
  buzzers(2);
}

void ledErrorNoBuzz()
{
  ledNotify(RGBLed::RED, 2000);
  led_status = "error";
}

void ledErrorWithTime(unsigned long ms)
{
  ledNotify(RGBLed::RED, ms);
}

void ledWaiting(unsigned long ms)
{
  ledNotify(RGBLed::YELLOW, ms);
  led_status = "waiting";
}

void ledFlashErrorSlowest()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_SLOWEST);
  led_status = "error";
}

void ledFlashErrorSlow()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_SLOW);
  led_status = "error";
}

void ledFlashErrorNormal()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_NORMAL);
  led_status = "error";
}

void ledFlashErrorFast()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_FAST);
  led_status = "error";
}

void ledFlashErrorFastest()
{
  ledSetColor(RGBLed::RED);
  ledFlashNotify(LED_FLASH_INTERVAL_FASTEST);
  led_status = "error";
}

void ledFlashWithInterval(unsigned long ms)
{
  ledSetMs(ms);
  ledFlash();
}

void ledFlashWithColor(int _color[3])
{
  ledSetColor(_color);
  ledFlash();
}

void ledFlashWithColorAndInterval(int _color[3], unsigned long ms)
{
  ledSetMs(ms);
  ledSetColor(_color);
  ledFlash();
}

void ledBreath(void)
{
  ledOff();
  TimerID.LED_FLASH_ON = t.setTimeout(ledOn, LED_FADE_INTERVAL / 3);
  TimerID.LED_FLASH_OFF = t.setTimeout(ledBreath, LED_FADE_INTERVAL);
}

void ledBreathWithColor(int _color[3])
{
  ledSetColor(_color);
  ledBreath();
}

void ledBlueBreath()
{
  ledBreathWithColor(RGBLed::BLUE);
  led_status = "bluebreath";
}

int led_handleCmd(void) {
  String resp = Rs485_data.value;
  if (Rs485_data.cmd == "off") {
    ledOff();
  } else if (Rs485_data.cmd == "on") {
    ledOn();
  } else if (Rs485_data.cmd == "breath") {
    if (Rs485_data.value == "red") {
      ledBreathWithColor(RGBLed::RED);
    } else if (Rs485_data.value == "blue") {
      ledBreathWithColor(RGBLed::BLUE);
    } else if (Rs485_data.value == "green") {
      ledBreathWithColor(RGBLed::GREEN);
    } else if (Rs485_data.value == "yellow") {
      ledBreathWithColor(RGBLed::YELLOW);
    } else {
      ledBreath();
    }
  } else if (Rs485_data.cmd == "notify") {
    if (Rs485_data.value == "ok") {
      ledSuccess();
    } else if (Rs485_data.value == "error") {
      ledError();
    }
  } else if (Rs485_data.cmd == "wait") {
    ledWaiting(( unsigned long )Rs485_data.value.toInt());
  } else if (Rs485_data.cmd == "ms") {
    ledSetMs( ( unsigned long )Rs485_data.value.toInt());
  } else if (Rs485_data.cmd == "flash") {
    if (Rs485_data.value == "red") {
      ledFlashWithColor(RGBLed::RED);
    } else if (Rs485_data.value == "blue") {
      ledFlashWithColor(RGBLed::BLUE);
    } else if (Rs485_data.value == "green") {
      ledFlashWithColor(RGBLed::GREEN);
    } else if (Rs485_data.value == "yellow") {
      ledFlashWithColor(RGBLed::YELLOW);
    } else {
      ledFlash();
    }
  } else if (Rs485_data.cmd == "red") {
    ledOnWithColor(RGBLed::RED);
  } else if (Rs485_data.cmd == "blue") {
    ledOnWithColor(RGBLed::BLUE);
  } else if (Rs485_data.cmd == "green") {
    ledOnWithColor(RGBLed::GREEN);
  } else if (Rs485_data.cmd == "yellow") {
    ledOnWithColor(RGBLed::YELLOW);
  } else {
    resp = "NO CMD";
  }
  oledDrawString(4, "Led resp: " + resp);
  return rs485_response("l", resp);
}
