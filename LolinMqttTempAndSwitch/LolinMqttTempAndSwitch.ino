#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//File for WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER, SERVER_FAN_TOPIC, SERVER_TEMPERATURE_TOPIC, MQTT_ID, MQTT_USER, MQTT_PASSWORD 
#include "Secrets.h"

const int oneWireBus = 4; //D2
const int relayInput = 5; //D1
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

const float onTempForFan = 33;
const float offTempForFan = 31;

WiFiClient espClient;
PubSubClient client(espClient);
//DHT dht(DHTPIN, DHTTYPE);
void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();
  pinMode(relayInput, OUTPUT);
  digitalWrite(relayInput, LOW);
}

void setup_wifi()
{
  delay(10);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
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

bool checkBound(float newValue, float prevValue, float maxDiff)
{
  return newValue < prevValue - maxDiff || newValue > prevValue + maxDiff;
}
long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;
bool fanStatus = false;
void loop()
{
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
    if (checkBound(newTemp, temp, diff))
    {
      temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      client.publish(SERVER_TEMPERATURE_TOPIC, String(temp).c_str(), true);
    }
    if (newTemp >= onTempForFan && fanStatus == false)
    {
      digitalWrite(relayInput, HIGH);
      Serial.print("Fan status:");
      Serial.println("ON");
      client.publish(SERVER_FAN_TOPIC, "true", true);
      fanStatus = true;
    }
    if (newTemp <= offTempForFan && fanStatus == true)
    {
      digitalWrite(relayInput, LOW);
      Serial.print("Fan status:");
      Serial.println("OFF");
      client.publish(SERVER_FAN_TOPIC, "false", true);
      fanStatus = false;
    }
  }
}
