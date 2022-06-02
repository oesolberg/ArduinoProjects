#define CLOSET_SERVER 200

#ifndef NODEMCU_SENSOR
  #define NODEMCU_SENSOR CLOSET_SERVER 
#endif

#define WIFI_SSID "SomeWifiName"
#define WIFI_PASSWORD "TheWifiPassword"
#define MQTT_SERVER "123.45.67.89"  // MQTT address
#define SERVER_INFO_TOPIC "/sensors/info"

#ifdef NODEMCU_SENSOR  
  #if (NODEMCU_SENSOR==CLOSET_SERVER)
    //ServerCloset
    #define SERVER_FAN_TOPIC "/sensors/servercloset/fan"
    #define SERVER_TEMPERATURE_TOPIC "/sensors/servercloset/temperature"
    #define MQTT_ID "AnId"
    #define MQTT_USER "A_User"
    #define MQTT_PASSWORD "A_Password"
  #endif
#endif