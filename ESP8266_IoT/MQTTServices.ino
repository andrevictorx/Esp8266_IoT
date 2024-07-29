
void OnMessageCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void onSTAGotIP(WiFiEventStationModeGotIP ipInfo) {
    Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
    //do connect upon WiFi connected
    connectToBroker();
}

void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
    Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
    Serial.printf("Reason: %d\n", event_info.reason);
}

void connectToBroker() {
  while (!client.connected()) {
      Serial.print("Connecting to MQTT Server ... ");
      if (client.connect(CLIENT_ID)) {
        Serial.println("connected.");
      } else {
        Serial.print("failed, status code =");
        Serial.print(client.state());
        Serial.println(". Try again in 5 seconds.");
        /* Wait 5 seconds before retrying */
        delay(5000);
      }
    }
}

void initMQTT(){
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(OnMessageCallback);
}
