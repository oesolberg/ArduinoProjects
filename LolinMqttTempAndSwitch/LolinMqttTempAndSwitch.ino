

//nodeMCU 1.0 ESP-12E
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>



//File for WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER, SERVER_FAN_TOPIC, SERVER_TEMPERATURE_TOPIC, MQTT_ID, MQTT_USER, MQTT_PASSWORD
#include "Secrets.h"

const int ESP_BUILTIN_LED = 2;

#ifdef NODEMCU_SENSOR
  #if (NODEMCU_SENSOR==MOBEL_MEDIA)
    //MediaMobel
    const int ONE_WIRE_BUS = 5; //D1
    const int RELAY_INPUT = 13; //D7
    const int RELAY_ON=0;
    const int RELAY_OFF=1;
  #elif (NODEMCU_SENSOR==CLOSET_SERVER)
    //ServerCloset
    const int ONE_WIRE_BUS = 4; //D2
    const int RELAY_INPUT = 13; //D7
    const int RELAY_ON=1;
    const int RELAY_OFF=0;
  #endif
#endif

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

const float ON_TEMP_FOR_FAN = 31;
const float OFF_TEMP_FOR_FAN = 27;
const int MORSE_TIME_PERIOD = 150;
const long MAX_BETWEEN_PUBLISH_TEMPERATURE=300000;//5 min

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
  pinMode(RELAY_INPUT, OUTPUT);
  pinMode(ONE_WIRE_BUS, INPUT);
  //Set
  digitalWrite(RELAY_INPUT, RELAY_OFF);
  digitalWrite(ESP_BUILTIN_LED, LOW);

  SendOK();
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);

  // Start the DS18B20 sensor
  sensors.begin();
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

//Morse rules
//The length of a dot is 1 time unit.
//A dash is 3 time units.
//The space between symbols (dots and dashes) of the same letter is 1 time unit.
//The space between letters is 3 time units.
//The space between words is 7 time units

void SendOK() {
  Serial.println("Sending OK");

  delay(MORSE_TIME_PERIOD * 3);
  SendO();
  delay(MORSE_TIME_PERIOD * 2);
  SendK();
  delay(MORSE_TIME_PERIOD * 2);
  digitalWrite(ESP_BUILTIN_LED, HIGH);
  Serial.println("Ok sendt");

}

void SendO() {
  SendLong();
  SendLong();
  SendLong();
  delay(MORSE_TIME_PERIOD * 2);

}

void SendK() {
  SendLong();
  SendShort();
  SendLong();
  delay(MORSE_TIME_PERIOD * 2);
}
void SendLong() {
  digitalWrite(ESP_BUILTIN_LED, LOW);
  delay(3 * MORSE_TIME_PERIOD);
  digitalWrite(ESP_BUILTIN_LED, HIGH);
  delay(MORSE_TIME_PERIOD);
}

void SendShort() {

  digitalWrite(ESP_BUILTIN_LED, LOW);
  delay(MORSE_TIME_PERIOD);
  digitalWrite(ESP_BUILTIN_LED, HIGH);
  delay(MORSE_TIME_PERIOD);
}

long lastMsg = 0;
long lastPublishedMsg = 0;
float temp = 0.0;
float diff = 0.5;
bool fanRunning = false;

bool checkBound(float newValue, float prevValue, float maxDiff)
{
  return newValue < prevValue - maxDiff || newValue > prevValue + maxDiff;
}

bool timeDiffHigher(long lastPublished,long now,long maxBetweenMessages){
  return now>=(lastPublished+maxBetweenMessages);
}

void Blink() {
  digitalWrite(ESP_BUILTIN_LED, LOW);
  delay(MORSE_TIME_PERIOD);
  digitalWrite(ESP_BUILTIN_LED, HIGH);
  delay(MORSE_TIME_PERIOD);
  digitalWrite(ESP_BUILTIN_LED, LOW);
  delay(MORSE_TIME_PERIOD);
  digitalWrite(ESP_BUILTIN_LED, HIGH);
  delay(10);
}

void SetFanRelayToOn() {
  SetFanRelayAndSendMessage(true);
}

void SetFanRelayToOff() {
  SetFanRelayAndSendMessage(false);
}

void SetFanRelayAndSendMessage(bool runFan) {
  Serial.print("Fan status:");
  String fanSerialStatus = "OFF";
  char mqttMessage[] = "Off";
  if (runFan)
  {
    fanSerialStatus = "ON";
    strcpy(mqttMessage, "On");
    digitalWrite(RELAY_INPUT, RELAY_ON);
  }
  else
  {
    digitalWrite(RELAY_INPUT, RELAY_OFF);
  }
  Serial.println(fanSerialStatus);
  client.publish(SERVER_FAN_TOPIC, mqttMessage, true);
  Blink();
  fanRunning = runFan;
}

void SendInitialDataToMqtt(float temperature , bool relayState) {
  Serial.println("Sending initial state");
  //Info
  char data[100];  
  sprintf(data,"Device with mqtt user '%s' is online at %s",MQTT_USER,WiFi.localIP().toString().c_str());
  client.publish(SERVER_INFO_TOPIC,data,true);
  //Temperature
  client.publish(SERVER_TEMPERATURE_TOPIC, String(temperature).c_str(), true);
  //Fan status
  char mqttMessage[] = "Off";
  if (relayState)
  {
    strcpy(mqttMessage, "On");
  }
  client.publish(SERVER_FAN_TOPIC, mqttMessage , true);
  
  Blink();
}

bool isFirstRunInLoop = true;

void loop()
{
  ArduinoOTA.handle();

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 3000)
  {
    // Wait a few seconds between measurements
    lastMsg = now;
    sensors.requestTemperatures();
    float newTemp = sensors.getTempCByIndex(0);
    Serial.print("Got new reading:");
    Serial.println(String(newTemp).c_str());

    bool relayState = (digitalRead(RELAY_INPUT) && RELAY_ON);
    
    if (isFirstRunInLoop) {
      SendInitialDataToMqtt(newTemp, relayState);
      isFirstRunInLoop=false;
    }

    if (newTemp >= ON_TEMP_FOR_FAN && !fanRunning) {
      SetFanRelayToOn();
    }
    if (newTemp <= OFF_TEMP_FOR_FAN && fanRunning) {
      SetFanRelayToOff();
    }
    if (checkBound(newTemp, temp, diff) || timeDiffHigher(lastPublishedMsg,now,MAX_BETWEEN_PUBLISH_TEMPERATURE))
    {      
      temp = newTemp;
      Serial.print("*** New temperature:");
      Serial.println(String(temp).c_str());
      client.publish(SERVER_TEMPERATURE_TOPIC, String(temp).c_str(), true);
      lastPublishedMsg = now;
      Blink();
    }
  }
  //Delay 5 secs between each reading
  delay(5000);
  MDNS.update();
}
