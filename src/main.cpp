#include <Arduino.h>
#include "SdsDustSensor.h"
#include "DHT.h"

SdsDustSensor sds(Serial2);
#define dhtpin 26
DHT dht(dhtpin, 11, 1);

float meassure(int measureTimeSek, boolean verbose)  {
  float humidity = dht.readHumidity(false);

  if (humidity < 65) {
    sds.wakeup();
    sleep(measureTimeSek);

    float temp = dht.readTemperature(false, false);
    PmResult ressult =  sds.queryPm();
    sds.sleep();

    if (ressult.isOk()) {
      if (verbose) {
        Serial.printf("Temp: %f. Humidity: %f\n", temp, humidity);
        Serial.printf("PM2.5: %f. PM10: %f\n", ressult.pm25, ressult.pm10);
      }
      return ressult.pm25;
    }
  } else {
    Serial.printf("Humidity of: %f is to high for dust sensor\n", humidity);
  }
  return -1;
}

float meanTest(int timeMin) {
  int sleepTimeSek = 2;
  int measureTimeSek = 5;
  int steps = (int)roundf(timeMin * 60 / (sleepTimeSek + measureTimeSek));

  float sum = 0;
  int stepsThatCount = 0;


  Serial.printf("\nStartring %d minutes meassure.", timeMin);
  for (int i = 0; i < steps; i++) {
    int pm25 = meassure(measureTimeSek, false);
    if(pm25 != -1) {
      sum += pm25;
      stepsThatCount++;
    }
    sleep(sleepTimeSek);
  }
  return sum / stepsThatCount;
}

void repeatedMeanTest() {
  int timeMin = 10;
  float mean = meanTest(timeMin);
  Serial.printf("mean value of PM2.5 during %d minutes: %f\n", timeMin, mean);
  Serial.println("New test will start in one minute. Set up your new enviroment");
  sleep(55);
  for (int i = 0; i < 5; i++)
  {
    Serial.printf("%d... ", 5 - i);
    sleep(1);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  

  delay(500);
  
  Serial.println("Hello from ESP32!");

  sds.begin();
  dht.begin();
  sds.wakeup();
  Serial.println(sds.queryFirmwareVersion().toString()); 
  sds.setQueryReportingMode();

}

void loop() {
  meassure(2, true);
  sleep(5);
}