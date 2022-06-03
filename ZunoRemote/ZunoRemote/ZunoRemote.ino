#define BTN_INT_PIN 18   // INT pin
#define BTN_PIN1 0       // button pin
#define BTN_PIN2 20      // button pin
#define CONTROL_GROUP1 1  // number of Association Group 
#define CONTROL_GROUP2 2  // number of Association Group
#define SWITCH_ON 0xff
#define SWITCH_OFF 0

const int DELAY_TIME=250;

//ZUNO_SETUP_CHANNELS(  
//  ZUNO_SWITCH_BINARY(lastStateColdWater, NULL), //lastStateAlarmOldSmoke
//  ZUNO_SWITCH_BINARY(lastStateWarmWater, NULL),//lastStateAlarmNewSmoke
//);
//ZUNO_SETUP_ASSOCIATIONS(ZUNO_ASSOCIATION_GROUP_SET_VALUE),ZUNO_ASSOCIATION_GROUP_SET_VALUE);
//ZUNO_SETUP_SLEEPING_MODE(ZUNO_SLEEPING_MODE_SLEEPING); // SLEEPING MODE

byte lastStateColdWater=0;
byte lastStateWarmWater=0;

byte buttonColdOnPressed = 0;
byte buttonColdOffPressed = 0;

byte buttonWarmOnPressed = 0;
byte buttonWarmOffPressed = 0;

void setup() {
  pinMode(19, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
}

void loop() {
  //Cold water
  if (digitalRead(20) == LOW) {    
    lastStateColdWater=SWITCH_ON;
      zunoSendToGroupSetValueCommand(CTRL_GROUP_1, SWITCH_ON);            
      zunoSendReport(1);
      delay(DELAY_TIME);
      buttonColdOnPressed = 1;    
  }
  else {
    buttonColdOnPressed = 0;
  }
  if (digitalRead(19) == LOW) {
    lastStateColdWater=SWITCH_OFF;
    zunoSendToGroupSetValueCommand(CTRL_GROUP_1, SWITCH_OFF);  
    zunoSendReport(1);  
    delay(DELAY_TIME);        
      buttonColdOffPressed = 1;
  }
  else {
    buttonColdOffPressed = 0;
  }
  //Warm water
 if (digitalRead(1) == LOW) {  
      lastStateWarmWater  =SWITCH_ON;
      zunoSendToGroupSetValueCommand(CTRL_GROUP_2, SWITCH_ON);            
      zunoSendReport(2);
      delay(DELAY_TIME);
      buttonWarmOnPressed = 1;    
  }
  else {
    buttonWarmOnPressed = 0;
  }
  if (digitalRead(0) == LOW) {
      lastStateWarmWater  =SWITCH_OFF;
    zunoSendToGroupSetValueCommand(CTRL_GROUP_2, SWITCH_OFF);            
    zunoSendReport(2);
    delay(DELAY_TIME);
    buttonWarmOffPressed = 1;
  }
  else {
    buttonWarmOffPressed = 0;
  }
  //Go to sleep if all buttons are 0
  if (buttonColdOnPressed == 0 && buttonColdOffPressed == 0 && buttonWarmOnPressed == 0 && buttonWarmOffPressed == 0 ) {
    //zunoSendDeviceToSleep(); // Send into sleep once buttons are released
    
  }
  delay(10);
}
