#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
//File for WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER, SERVER_FAN_TOPIC, SERVER_TEMPERATURE_TOPIC, MQTT_ID, MQTT_USER, MQTT_PASSWORD
#include "Secrets.h"

const int RELAY_OUTPUT=26;
const long MAX_BETWEEN_PUBLISH_INFO=600000;//10 min
const int ESP_BUILTIN_LED = 2;

long lastPublishedIpAddressMsg = 0;

WiFiClient espClient;
PubSubClient client(espClient);
void setup()
{
  // Start the Serial Monitor
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Booting");
  //Set pinmodes
  pinMode(ESP_BUILTIN_LED, OUTPUT);
  pinMode(RELAY_OUTPUT, OUTPUT);

  //Set
  digitalWrite(RELAY_OUTPUT, LOW);
  digitalWrite(ESP_BUILTIN_LED, LOW);

  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
  
}

void setup_wifi()
{
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int loopTime = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    loopTime++;
    //Only wait for 10 seconds. If not connected reboot
    if (loopTime >= 20) {
      Serial.println("Connection Failed! Rebooting in 5 seconds...");
      delay(5000);
      ESP.restart();
    }
  }
  //Do OTA stuff
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("Unit ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_ID, MQTT_USER, MQTT_PASSWORD))
    {
      Serial.println("connected");
      client.subscribe(SERVER_BELL_TOPIC);
      Serial.print("subscribed to");
      Serial.println(SERVER_BELL_TOPIC);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == SERVER_BELL_TOPIC) {
    Serial.println("sounding the bell");
    soundBell();
//    if(messageTemp == "on"){
//      Serial.println("on");
//      digitalWrite(ESP_BUILTIN_LED, HIGH);
//    }
//    else if(messageTemp == "off"){
//      Serial.println("off");
//      digitalWrite(ESP_BUILTIN_LED, LOW);
//    }
  }
}

void PublishIpAddressInfo(){
  Serial.println("Sending ip address");
  //Info
  char data[100];  
  sprintf(data,"Device with mqtt user '%s' is online at %s",MQTT_USER,WiFi.localIP().toString().c_str());
  client.publish(SERVER_INFO_TOPIC,data,true);  
}

void SendInitialDataToMqtt(float temperature , bool relayState) {
  Serial.println("Sending initial state");
  //Info
  char data[100];  
  sprintf(data,"Device with mqtt user '%s' is online at %s",MQTT_USER,WiFi.localIP().toString().c_str());
  client.publish(SERVER_INFO_TOPIC,data,true);
 
 }

 void SendInitialDataToMqtt() {
  Serial.println("Sending initial state");
  //Info
  char data[100];  
  sprintf(data,"Device with mqtt user '%s' is online at %s",MQTT_USER,WiFi.localIP().toString().c_str());
  client.publish(SERVER_INFO_TOPIC,data,true);
  
}

bool timeDiffHigher(long lastPublished,long now,long maxBetweenMessages){
  return now>=(lastPublished+maxBetweenMessages);
}

bool isFirstRunInLoop = true;

void loop(){
  ArduinoOTA.handle();

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  
  long now = millis();
   if (isFirstRunInLoop) {
      SendInitialDataToMqtt();
      lastPublishedIpAddressMsg=now;
      isFirstRunInLoop=false;
    }
  if (timeDiffHigher(lastPublishedIpAddressMsg,now,MAX_BETWEEN_PUBLISH_INFO))
    {       
      PublishIpAddressInfo();     
      lastPublishedIpAddressMsg=now;
    }
  delay(400);
}



 
void soundBell() {
  
  //2x ding dong
  digitalWrite(RELAY_OUTPUT,HIGH);
  Serial.println("Ding dong 2x");
  delay(500);
  digitalWrite(RELAY_OUTPUT,LOW);  
  delay(200);
  digitalWrite(RELAY_OUTPUT,HIGH);
  delay(500);
  digitalWrite(RELAY_OUTPUT,LOW);    
  Serial.println("Done");
  
}
