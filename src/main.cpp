#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <PubSubClient.h>
#include <Wire.h>
#include <ssid_info.c>

// Network details 
const char* s = get_ssid();
const char* p = get_pass();
const char* mqtt_serv ="192.168.4.15"; // AccessPoint

int sensor = 15; // Pin til Sensor input
int sensor_value = 0; // Sensor value

// sensor ID nr.
const int sensorNumber = 4;

WiFiClient espClient; // Netværks client
PubSubClient client(espClient); //delvist initialiseret MQTT client -> tager MQTT server IP og port på MQTT Mosquitto Docker containeren.

void reconnect();

void setup() {
  delay(100);
  pinMode(sensor, INPUT);
  
  Serial.begin(115200);
  while(!Serial);

  WiFi.begin(s,p);

  while(WiFi.status() != WL_CONNECTED){ // Connection progress
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wifi Connected, IP: ");
  Serial.print(WiFi.localIP());

  client.setServer(mqtt_serv, 1883); // Sætter serveren på MQTT clienten.

}

void loop() {

  if (!client.connected()){
    reconnect();
  }

  StaticJsonDocument<80> doc; // nyt object af typen statiskjson 
  char output[80]; // Char array på 80 chars

  sensor_value = digitalRead(sensor); // Henter sensor reading 
  if (sensor_value == HIGH){

    float value = sensor_value;
    doc["a"] = value;
    doc["s"] = sensorNumber;

    serializeJson(doc, output); 
    Serial.println(output);
    client.publish("/home/sensors", output);
    delay(500);
  }

}

void reconnect() {
  while(!client.connected()){
    Serial.println("MQTT not connected");

    //Client id
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // Attempts connect
    if(client.connect(clientId.c_str())){
      Serial.println("Connected!");
    }
    else{
      Serial.print("failed, code: ");
      Serial.println(client.state());
      delay(5000);
      }
    }
}