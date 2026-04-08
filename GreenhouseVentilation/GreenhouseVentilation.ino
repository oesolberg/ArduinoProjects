//Lolin D32

#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "esp_mac.h"
//File for WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER, SERVER_FAN_TOPIC, SERVER_TEMPERATURE_TOPIC, MQTT_ID, MQTT_USER, MQTT_PASSWORD
#include "Secrets.h"
#include <ArduinoJson.h>

#define USE_SERIAL Serial

#define CommonCurrentRelay 12
#define SmallMotorRelay 13
#define L1MotorRelay 14
#define L2MotorRelay 27


//UDP stuff
char packetBuffer[255];               //buffer to hold incoming packet
char ReplyBuffer[] = "acknowledged";  // a string to send back
WiFiUDP Udp;


#define ONE_WIRE_BUS 18
//Temperature stuff
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;
int numberOfDevices;
float previousTemperatureC = -99;
bool TemperatureRising = 0;

//SSID of your network
char* ssid = WIFI_SSID;
//password of your WPA Network
char* pass = WIFI_PASSWORD;



//Relay pins
bool commonCurrentRelayActive = false;
bool L1MotorRelayActive = false;
bool L2MotorRelayActive = false;
bool smallMotorRelayActive = false;



void setup() {
  USE_SERIAL.begin(115200);
  //set up pins for relay
  SetUpPins();

  delay(100);
  GetMacAddress();
  delay(100);
  ScanNetworks();
  delay(100);

  for (uint8_t t = 5; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  sensors.begin();
  SetUpWifi();

  delay(100);
  SetUpTemperatureDevices();

  Udp.begin(UDP_PORT);
}

void SetUpPins() {
  pinMode(CommonCurrentRelay, OUTPUT);
  pinMode(SmallMotorRelay, OUTPUT);
  pinMode(L1MotorRelay, OUTPUT);
  pinMode(L2MotorRelay, OUTPUT);
}

void SetUpTemperatureDevices() {
  numberOfDevices = sensors.getDeviceCount();
  delay(1000);
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++) {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      PrintAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}

void PrintAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void GetMacAddress() {
  uint8_t mac[6] = { 0 };
  esp_err_t ret = ESP_OK;

  ret = esp_read_mac(mac, ESP_MAC_EFUSE_FACTORY);
  if (ret != ESP_OK) {
    USE_SERIAL.printf("Failed to get base MAC address from EFUSE BLK0. (%s)", esp_err_to_name(ret));
  } else {
    USE_SERIAL.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  }
}

void ScanNetworks() {
  int n = WiFi.scanNetworks();
  USE_SERIAL.println("scan done");
  if (n == 0) {
    USE_SERIAL.println("no networks found");
  } else {
    USE_SERIAL.print(n);
    USE_SERIAL.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      USE_SERIAL.print(i + 1);
      USE_SERIAL.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
}

void SetUpWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  WiFi.setHostname("GreenhouseFanController");
}

void loop() {
  TryToConnectToWifi();
  float temperatureC = GetTemperatureFromDevices();

  SetFansBasedOnTemperature(temperatureC);
  previousTemperatureC = temperatureC;

  SendDataWithUdp();

  delay(10000);  //Delay for 10 seconds before doing a new check
}

void TryToConnectToWifi() {
  int numberOfTries = 4;
  Serial.print("Trying to connect to WiFi ..");
  while (WiFi.status() != WL_CONNECTED && numberOfTries > 0) {
    Serial.print('.');
    delay(6000);
    numberOfTries--;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Connected with IP address: ");
    Serial.println(WiFi.localIP());
  }
}

float GetTemperatureFromDevices() {
  sensors.requestTemperatures();  // Send the command to get temperatures
  delay(250);
  float highestTemperatureC = 0;

  // Loop through each device, print out temperature data
  for (int i = 0; i < numberOfDevices; i++) {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i)) {
      // Output the device ID
      Serial.print("Temperature for device: ");
      Serial.println(i, DEC);
      // Print the data
      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print("Temp C: ");
      Serial.print(tempC);
      Serial.print(" Temp F: ");
      Serial.println(DallasTemperature::toFahrenheit(tempC));  // Converts tempC to Fahrenheit
      if (tempC > highestTemperatureC)
        highestTemperatureC = tempC;
    }
  }
  Serial.print("Returning highest temp C: ");
  Serial.println(highestTemperatureC);
  return highestTemperatureC;
}

void SetFansBasedOnTemperature(float temperatureC) {
  bool temperatureRising = IsTemperatureIsRising(temperatureC);
  TemperatureRising = temperatureRising;
  Serial.print("temperature rising: ");
  Serial.println(temperatureRising);
  //turing on motors when temperature is going up
  SetCommonCurrentDependingOnTemperature(temperatureC);
  if (temperatureRising && temperatureC >= 26) {
    Serial.println("");
    Serial.println("*** Temperature rising and above 26 degrees C ***");

    if (temperatureC >= 34 && (!L2MotorRelayActive || !smallMotorRelayActive)) {
      StartSmallMotor();
      StopL1Motor();
      StartL2Motor();
    }
    if (temperatureC < 34 && temperatureC >= 32 && (!L2MotorRelayActive || smallMotorRelayActive)) {
      StopL1Motor();
      StartL2Motor();
      StopSmallMotor();
    }
    if (temperatureC < 32 && temperatureC >= 30 && (!L1MotorRelayActive || !smallMotorRelayActive)) {
      StartSmallMotor();
      StopL2Motor();
      StartL1Motor();
    }
    if (temperatureC < 30 && temperatureC >= 28 && (!L1MotorRelayActive || smallMotorRelayActive)) {
      StopL2Motor();
      StopSmallMotor();
      StartL1Motor();
    }
    if (temperatureC < 28 && temperatureC >= 26 && (!smallMotorRelayActive)) {
      StartSmallMotor();
      StopL1Motor();
      StopL2Motor();
    }
  }
  //Turning off motors when temperature is going down
  if (!temperatureRising && temperatureC < 34 && temperatureC > 23) {
    Serial.println("");
    Serial.println("*** Temperature falling and between 23 and 34 degrees C ***");
    if (temperatureC < 25) {
      StopEverything();
    }
    if (temperatureC > 25 && temperatureC <= 27) {
      StartSmallMotor();
      StopL1Motor();
      StopL2Motor();
    }
    if (temperatureC > 27 && temperatureC <= 29) {
      StopL2Motor();
      StopSmallMotor();
      StartL1Motor();
    }
    if (temperatureC > 29 && temperatureC <= 31) {
      StartSmallMotor();
      StopL2Motor();
      StartL1Motor();
    }
    if (temperatureC > 31 && temperatureC <= 33) {
      StopL1Motor();
      StopSmallMotor();
      StartL2Motor();
    }
  }

  Serial.print("=== temperature ");
  Serial.print(temperatureC);
  Serial.print(" temperature rising: ");
  Serial.print(temperatureRising);
  Serial.print(", status motors based on variables: small motor: ");
  Serial.print(smallMotorRelayActive);
  Serial.print(" L1: ");
  Serial.print(L1MotorRelayActive);
  Serial.print(" L2: ");
  Serial.print(L2MotorRelayActive);
  Serial.print(" Common: ");
  Serial.print(commonCurrentRelayActive);
  Serial.println(" ===");
}

// bool commonCurrentRelayActive = false;
// bool L1RelayActive = false;
// bool L2RelayActive = false;
// bool smallMotorRelayActive = false;
void StopEverything() {
  StopL2Motor();
  StopL1Motor();
  StopSmallMotor();
  SetCommonCurrentRelayOff();
}

void StartSmallMotor() {
  SetSmallMotorRelay(true);
}

void StopSmallMotor() {
  SetSmallMotorRelay(false);
}

void StartL1Motor() {
  //Stop L2 if active since they will short circut if on at the same time
  SetL2MotorRelay(false);
  delay(750);
  SetL1MotorRelay(true);
}

void StopL1Motor() {
  SetL1MotorRelay(false);
}

void StartL2Motor() {
  SetL1MotorRelay(false);
  delay(750);
  SetL2MotorRelay(true);
}

void StopL2Motor() {
  SetL2MotorRelay(false);
}

void SetCommonCurrentDependingOnTemperature(float temperatureC) {
  if (temperatureC >= 25)
    SetCommonCurrentRelayOn();
  if (temperatureC < 25)
    SetCommonCurrentRelayOff();
}

void SetCommonCurrentRelayOn() {
  SetCommonCurrentRelay(true);
}

void SetCommonCurrentRelayOff() {
  SetCommonCurrentRelay(false);
}

bool IsTemperatureIsRising(float temperatureC) {
  if (temperatureC > previousTemperatureC)
    return true;
  return false;
}

void SetL1MotorRelay(bool setActive) {
  if (setActive && !L1MotorRelayActive) {
    Serial.println("Setting L1 motor relay ON");
    digitalWrite(L1MotorRelay, HIGH);  // Set the pin HIGH (On)
  } else if (!setActive && L1MotorRelayActive) {
    Serial.println("Setting L1 motor relay OFF");
    digitalWrite(L1MotorRelay, LOW);  // Set the pin HIGH (On)
  }
  L1MotorRelayActive = setActive;
}

void SetL2MotorRelay(bool setActive) {
  if (setActive && !L2MotorRelayActive) {
    Serial.println("Setting L2 motor relay ON");
    digitalWrite(L2MotorRelay, HIGH);  // Set the pin HIGH (On)
  } else if (!setActive && L2MotorRelayActive) {
    Serial.println("Setting L2 motor relay OFF");
    digitalWrite(L2MotorRelay, LOW);  // Set the pin HIGH (On)
  }
  L2MotorRelayActive = setActive;
}


void SetSmallMotorRelay(bool setActive) {
  if (setActive && !smallMotorRelayActive) {
    Serial.println("Setting small motor relay ON");
    digitalWrite(SmallMotorRelay, HIGH);  // Set the pin HIGH (On)
  } else if (!setActive && smallMotorRelayActive) {
    Serial.println("Setting small motor relay OFF");
    digitalWrite(SmallMotorRelay, LOW);  // Set the pin HIGH (On)
  }
  smallMotorRelayActive = setActive;
}

void SetCommonCurrentRelay(bool setActive) {
  if (!setActive && OneOrMoreMotorRelaysAreStillActive() && !commonCurrentRelayActive) {
    Serial.println("One or more relays are ON, keeping common current relay ON");
    digitalWrite(CommonCurrentRelay, HIGH);
    commonCurrentRelayActive = true;
  }
  if (!setActive && !OneOrMoreMotorRelaysAreStillActive() && commonCurrentRelayActive) {
    Serial.println("Turning common current relay OFF");
    digitalWrite(CommonCurrentRelay, LOW);
    commonCurrentRelayActive = false;
  } else if (setActive && !commonCurrentRelayActive) {
    Serial.println("Setting common current relay ON");
    digitalWrite(CommonCurrentRelay, HIGH);
    commonCurrentRelayActive = true;
  }
}

bool OneOrMoreMotorRelaysAreStillActive() {
  if (smallMotorRelayActive)
    return true;
  if (L1MotorRelayActive)
    return true;
  if (L2MotorRelayActive)
    return true;
  return false;
}

void SendDataWithUdp() {
  JsonDocument doc;
  doc["name"] = "GreenHouseFanData";
  doc["Temperature"] = previousTemperatureC;
  doc["CommonCurrent"] = (int)commonCurrentRelayActive;
  doc["SmallMotor"] = (int)smallMotorRelayActive;
  doc["L1Motor"] = (int)L1MotorRelayActive;
  doc["L2Motor"] = (int)L2MotorRelayActive;
  doc["TempRising"] = (int)TemperatureRising;


  // String msg = "GreenhouseFanData&Temperature=";
  // msg.concat(previousTemperatureC);
  // msg.concat("&CommonCurrent=");
  // msg.concat(commonCurrentRelayActive);
  // msg.concat("&SmallMotor=");
  // msg.concat(smallMotorRelayActive);

  // msg.concat("&L1Motor=");
  // msg.concat(L1MotorRelayActive);
  // msg.concat("&L2Motor=");
  // msg.concat(L2MotorRelayActive);
  // msg.concat("&TempRising=");
  // msg.concat(TemperatureRising);

  String output;
  serializeJson(doc, output);



  int str_len = output.length() + 1;
  char char_array[str_len];
  output.toCharArray(char_array, str_len);
  //strcpy(ReplyBuffer);
  Serial.println("Trying to send with UDP");
  Serial.println(char_array);
  IPAddress ip;
  ip.fromString(UDP_IP);

  Udp.beginPacket(ip, UDP_PORT);
  Udp.print(char_array);
  Udp.endPacket();
}