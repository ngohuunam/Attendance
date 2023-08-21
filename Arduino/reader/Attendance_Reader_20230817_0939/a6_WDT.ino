#include <esp_task_wdt.h>

#define WDT_TIMEOUT 10 * 60
const unsigned long WDT_RESET_INTERVAL = (WDT_TIMEOUT - (1 * 60)) * 1000;

void wdt_reset(void) {
  esp_task_wdt_reset();
}

void wdt_setup(void) {
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  TimerID.WDT_RESET_INTERVAL = t.setInterval(wdt_reset, WDT_RESET_INTERVAL);
}
