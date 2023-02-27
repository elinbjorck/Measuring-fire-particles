#include <Arduino.h>
#include "SdsDustSensor.h"
#include "DHT.h"

SdsDustSensor sds(Serial2);
#define dhtpin 12
DHT dht(dhtpin, 11, 1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  

  delay(500);
  
  Serial.println("Hello from ESP32!");

  sds.begin();
  dht.begin();

  Serial.println(sds.queryFirmwareVersion().toString()); 
  sds.setQueryReportingMode();

}

void loop() {
  sds.wakeup();
  sleep(2);
  PmResult ressult =  sds.queryPm();
  float humidity = dht.readHumidity(false);
  float temp = dht.readTemperature(false, false);

  if (ressult.isOk()) {
  Serial.printf("Temp: %f. Humidity: %f\n", temp, humidity);
  Serial.printf("PM2.5: %f. PM10: %f\n", ressult.pm25, ressult.pm10);
  }
  sds.sleep();
  sleep(5);
  // put your main code here, to run repeatedly:
}