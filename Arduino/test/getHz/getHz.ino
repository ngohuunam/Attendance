/*
  LAB: 7
  Name: ESP32 Read Default Clocks
  Author: Khaled Magdy
  For More Info Visit: www.DeepBlueMbedded.com
*/

#define GPIO_pin 5

uint32_t Freq = 0;

void setup()
{
  pinMode(GPIO_pin, OUTPUT);
  Serial.begin(115200);
  Serial.print("CPU Freq = ");
//  Freq = getCpuFrequencyMhz();
  Serial.print(Freq);
  Serial.println(" MHz");
//  Freq = getXtalFrequencyMhz();
  Serial.print("XTAL Freq = ");
  Serial.print(Freq);
  Serial.println(" MHz");
//  Freq = getApbFrequency();
  Serial.print("APB Freq = ");
  Serial.print(Freq);
  Serial.println(" Hz");
}

void loop()
{
//  digitalWrite(GPIO_pin, 1);
//  digitalWrite(GPIO_pin, 0);
}
