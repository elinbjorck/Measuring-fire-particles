#include <Arduino.h>
#include "SdsDustSensor.h"

SdsDustSensor sds(Serial2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(500);
  
  Serial.println("Hello from ESP32!");

  sds.begin();

  Serial.println(sds.queryFirmwareVersion().toString()); 
  sds.setQueryReportingMode();
}

void loop() {
  sds.wakeup();
  sleep(2);
  PmResult ressult =  sds.queryPm();
  Serial.println(ressult.pm25);
  sds.sleep();
  sleep(2);
  // put your main code here, to run repeatedly:
}