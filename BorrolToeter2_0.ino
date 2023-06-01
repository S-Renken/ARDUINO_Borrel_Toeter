#include <ESP8266WiFi.h>             // Wifi library
#include <WiFiClientSecure.h>
#include <PubSubClient.h>            // MQTT library
#include <DNSServer.h>               // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>        // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>             // https://github.com/tzapu/WiFiManager WiFi Configuration Magic 

#include <ESP8266httpUpdate.h>       // OTA update

WiFiClientSecure wifiClientSecure;   // WiFi
WiFiClient wifiClient;
PubSubClient client(wifiClient);     // MQTT

// Function prototypes
void subscribeReceive(char* topic, byte* payload, unsigned int length);

// MQTT settings
const char* mqtt_server = "mqtt.orcaroeien.nl";
const int mqtt_port = 1883;
const char* mqtt_username = "webmaster";
const char* mqtt_password = "Pho3iesa";
const char* mqtt_client_name = "BorrolToeter";

bool isConnected = false;

int toeterpin = 4;

void setup()
{
  Serial.begin(115200);
  Serial.print("Start");
  pinMode(toeterpin, OUTPUT);
  digitalWrite(toeterpin, LOW);
  // Wifi Manager
  WiFiManager wifiManager;
  WiFiManagerParameter custom_text("<p>(c) 2020 by <a href=\"maito:sander.renken@gmail.com\">TEST123!@#</a></p>");
  wifiManager.addParameter(&custom_text);

  if (wifiManager.autoConnect("Name_of_the_network_from_the_arduino", "password for_connecting_to_the_arduino")){
    isConnected = true;
    Serial.println("Connected to wifi");
  }

  // MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(subscribeReceive);

  // configure updater
  ESPhttpUpdate.closeConnectionsOnUpdate(false);
  ESPhttpUpdate.setAuthorization("orca","haaldoor");

  client.subscribe("connection/toeter");
}

void loop()
{
  client.loop();  
  if (!client.connected() && isConnected){
    reconnect();
  }
  delay(500);
}

void toeter()
{
  Serial.println("");
  Serial.println("Toeter");
  Serial.println("");
  digitalWrite(toeterpin, HIGH);
  delay(1500);     
  digitalWrite(toeterpin, LOW);
  delay(1800000);
}

void subscribeReceive(char* topic, byte* payload, unsigned int length)
{
  Serial.println();
  // Print the topic
  Serial.print("Topic: ");
  Serial.println(topic);
 
  // Print the message
  Serial.print("Message: ");
  String msg = ""; // payload
  for (int i = 0; i < length; i++) {
    msg += ((char)payload[i]);
  }
  Serial.print(msg);

  // Print a newline
  Serial.println("");
  Serial.println(topic);
  
  if (strcmp(topic, "connection/toeter") == 0 && msg  == "COMMANDO_OM_TEGAAN_TOETEREN")
  {
    toeter();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect(mqtt_client_name, mqtt_username, mqtt_password, "connection/toeter", 0, 1, "0")) {

      client.subscribe("connection/toeter");
      Serial.println("connected to MQTT");
    }
  }
}