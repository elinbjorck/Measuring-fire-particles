#include <Arduino.h>
#include "SdsDustSensor.h"
#include "DHT.h"
#include "WiFi.h"
#include "mqtt_client.h"
#include "PubSubClient.h"
#include "FreeRTOSConfig.h"

// define SSID and PASSWORD in wifi_config.h (not git tracked)
// #ifndef SSID && PASSWORD
// #define SSID "<yourssid>"
// #define PASSWORD "<yourwifipassword>"
// #endif
#include "wifi_config.h"

SdsDustSensor sds(Serial2);
#define dhtpin 26
#define wifiFlag false
DHT dht(dhtpin, 11, 1);
#define mqttUsername "mqttparticles"
#define mqttPassword "particlespassword"
// Found here: https://forum.arduino.cc/t/serial-write-a-float-value/110198/8
// typedef union {
//  float floatingPoint;
//  byte binary[4];
// } binaryFloat;

// void send_float(float sensorValue) {
//   binaryFloat value;
//   value.floatingPoint = sensorValue;
//   Serial.write(value.binary, 4);
// }

WiFiClient espClient;
PubSubClient client(espClient);

float pm25;
float pm10;
float humidity;
float temperature;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.print("\nconnecting");

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.print("Wifi connected");
  Serial.print("\nLocal ESP32 IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP32 MAC: ");
  Serial.println(WiFi.macAddress());
}

void connectMQTT() {
  // esp_mqtt_client_config_t config = {
  //   .uri = "mqtt://homeassistant.local",
  //   .username = mqttUsername,
  //   .password = mqttPassword
  // };
  // esp_mqtt_client_handle_t handle = esp_mqtt_client_init(&config);
  // esp_mqtt_client_start(handle);
  // Serial.print("Connecting to mqtt brooker");
  // return handle;
  Serial.println("Connecting to mqtt brooker");
  client.setServer("homeassistant.local", 1883);
  if (client.connect("fire", mqttUsername, mqttPassword)) {
    Serial.println("sucsess!");
    client.publish("fire/status", "{ \"value\": true }");
  } else {
    Serial.println("No connect fo u :(");
  }
}

void connect() {
  connectWiFi();
  connectMQTT();
}

void sendData() {
  char jsonString[128];
  sprintf(jsonString, "{ \"pm25\": %f,  \"pm10\": %f, \"humidity\": %f, \"temperature\": %f}", pm25, pm10, humidity, temperature);

  client.publish("fire/data", jsonString);
}

float meassure(int measureTimeSek, boolean verbose)  {
  humidity = dht.readHumidity(false);

  if (humidity < 65) {
    sds.wakeup();
    sleep(measureTimeSek);

    temperature = dht.readTemperature(false, false);
    PmResult result =  sds.queryPm();
    sds.sleep();

    if (result.isOk()) {
      pm25 = result.pm25;
      pm10 = result.pm10;
      
      sendData();
      if (verbose) {
        Serial.printf("Temp: %f. Humidity: %f\n", temperature, humidity);
        Serial.printf("PM2.5: %f. PM10: %f\n", pm25, pm10);
      }
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
    // Serial.printf("\nSum: %f  steps: %d", sum, stepsThatCount);
    float pm25 = meassure(measureTimeSek, false);
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

  connect();
}

void loop() {
  meassure(2, true);
  sleep(3);
  if (WiFi.status() != WL_CONNECTED) {
    connect();
  }
}