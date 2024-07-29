#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <TimeLib.h>
#include <NtpClientLib.h>
#include "ESP8266Helpers.h"

#define NTP_TIMEOUT 1500

//NTP Settings
int8_t timeZone = -3;
int8_t minutesTimeZone = 0;
const PROGMEM char *ntpServer = "pool.ntp.org";
boolean syncEventTriggered = true; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

//WiFi Credentials
const char* WIFI_USER = "Vivo-Internet-BF17";
const char* WIFI_PWD = "78814222";

//WiFi Connection Settings
bool autoConnect = false;
bool autoReconnect = true;

//MQTT Conection
const char* CLIENT_ID = "esp8266";
const char* MQTT_BROKER = "192.168.1.144";
const int   MQTT_PORT = 1883;
const char* MQTT_PUB_TOPIC = "treinamento/sensors/esp8266";

WiFiClient wifiClient;
PubSubClient client(MQTT_BROKER,MQTT_PORT,wifiClient);

void setup() {
 
  //Initilaize Built In LED
  initGPIO();

  //Initilaize Serial Port
  initSerialPort();
  
  //Initialize WiFi
  initWiFi(WIFI_USER, WIFI_PWD);

  //Initialize NTP
  NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
      ntpEvent = event;
      syncEventTriggered = true;
  });

  NTP.setInterval (63);
  NTP.setNTPTimeout (NTP_TIMEOUT);
  NTP.begin (ntpServer, timeZone, true, minutesTimeZone);

  //Initialize MQTT Settings
  initMQTT();

  //Connect to Azure IoT Hub
  connectToBroker();
}

void loop() {
    
  if(WiFi.isConnected()) {
    if (syncEventTriggered) {
        processSyncEvent (ntpEvent);
        syncEventTriggered = false;
    }
    
    // Display connection state via LED
    turnOnDigitalPin(DIGITAL_PIN2);
    
    if (!client.connected()) {
     connectToBroker();  
    }

    client.loop();
    
    if (client.connected()){
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = random(60,70); //getHumidity();
      
      // Read temperature as Celsius (the default)
      float t = random(20,30); //getTemperature();

      // Read luminosity in percentage
      float l = random(90,100); //readLDRSensor();
      
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }

      // Check if any reads failed and exit early (to try again).
      if (isnan(l)) {
        Serial.println(F("Failed to read from LDR sensor!"));
        return;
      }

//      const char* humidityPayload = getSensorValuesJSON(CLIENT_ID,"humidity",h,"percentage");
//      Serial.println(humidityPayload);
//      client.publish(MQTT_PUB_TOPIC,humidityPayload);
//
//      const char* tempPayload = getSensorValuesJSON(CLIENT_ID,"temperature",t,"celcius");    
//      Serial.println(tempPayload);  
//      client.publish(MQTT_PUB_TOPIC,tempPayload);
//
//      const char* ldrPayload = getSensorValuesJSON(CLIENT_ID,"luminosity",l,"percentage");  
//      Serial.println(ldrPayload);  
//      client.publish(MQTT_PUB_TOPIC,ldrPayload);

        const char* payload = getJSON(t,h,l);
        Serial.println(payload);
        client.publish(MQTT_PUB_TOPIC,payload);
    }
  } else {
    turnOffDigitalPin(DIGITAL_PIN2);
    reConnectWiFi();
  }
  delay(10000);
}
