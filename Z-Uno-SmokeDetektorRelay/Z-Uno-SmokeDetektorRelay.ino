// LED pin number
// 13 pin - user LED of Z-Uno board
// button pin number
// 18 pin - button(BTN) of Z-Uno board

#define SWITCH_NEW 7
#define SWITCH_OLD 8
#define READ_OLD_ALARM 9
#define READ_NEW_ALARM 10
#define READ_NEW_FAULT 11
#define READ_NEW_CO 12
#define LED_PIN 13
const int DELAY_TIME=200; //delay after reports

// variable to store current button state
byte lastStateSwitchNewSmoke;
byte lastStateSwitchOldSmoke;
byte lastStateAlarmOldSmoke;
byte lastStateAlarmNewSmoke;
byte lastStateCONewSmoke;
byte lastStateFaultNewSmoke;

ZUNO_SETUP_SLEEPING_MODE(ZUNO_SLEEPING_MODE_ALWAYS_AWAKE);

//I need switches for the following: Turn on alarm in old smoke detektors, read status of old smoke detektors
//Turn on alarm in new smoke detektors, read status of new smoke detektors, read status of new smoke detektors CO, read status of new smoke detektors fault
// a total of 6 channels. After reading forums it is smart to keep it at max 8 channels when comunicating with HomeSeer (read in a forum post)
ZUNO_SETUP_CHANNELS(  
  ZUNO_SWITCH_BINARY(getterSwitchSmokeNew, setterSwitchSmokeNew),//lastStateSwitchNewSmoke
  ZUNO_SWITCH_BINARY(getterSwitchSmokeOld, setterSwitchSmokeOld),//lastStateSwitchOldSmoke
  ZUNO_SWITCH_BINARY(lastStateAlarmOldSmoke, NULL), //lastStateAlarmOldSmoke
  ZUNO_SWITCH_BINARY(lastStateAlarmNewSmoke, NULL),//lastStateAlarmNewSmoke
  ZUNO_SWITCH_BINARY(lastStateCONewSmoke, NULL),//lastStateCONewSmoke
  ZUNO_SWITCH_BINARY(lastStateFaultNewSmoke, NULL)//lastStateFaultNewSmoke
  );

// the setup routine runs once when you press reset:
void setup() {
  pinMode(SWITCH_NEW, OUTPUT); 
  pinMode(SWITCH_OLD, OUTPUT); 
  pinMode(LED_PIN , OUTPUT); 
  pinMode(READ_OLD_ALARM , INPUT_PULLUP); 
  pinMode(READ_NEW_ALARM , INPUT_PULLUP); 
  pinMode(READ_NEW_FAULT, INPUT_PULLUP); 
  pinMode(READ_NEW_CO, INPUT_PULLUP); 
  //pinMode(BTN_PIN, INPUT_PULLUP); // set button pin as input
 //SendInitialStatus();
}

//void SendInitialStatus(){
//  //Will send inital status for all channels
//   for (int i = 1; i <= 6; i++) {
//    zunoSendReport(i);
//    delay(DELAY_TIME); //delay  just to give room for any messages
//   }    
//}

// the loop routine runs over and over again forever:
void loop() {
  //Read the state for the old smoke switch/sensor state
  byte currenStateAlarmOldSmoke= digitalRead(READ_OLD_ALARM);
  if(lastStateAlarmOldSmoke!=currenStateAlarmOldSmoke)
  {
    lastStateAlarmOldSmoke=currenStateAlarmOldSmoke;
    zunoSendReport(3);
    delay(DELAY_TIME); //delay  just to give room for any messages
  }
  
  //Read the state for the new smoke switch/sensor state
  byte currenStateAlarmNewSmoke= digitalRead(READ_NEW_ALARM);
  if(lastStateAlarmNewSmoke!=currenStateAlarmNewSmoke)
  {
    lastStateAlarmNewSmoke=currenStateAlarmNewSmoke;
    zunoSendReport(4);
    delay(DELAY_TIME); //delay  just to give room for any messages
  }

  //Read the state for the new smoke switch/sensor state
  byte currenStateCONewSmoke= digitalRead(READ_NEW_CO);
  if(lastStateCONewSmoke!=currenStateCONewSmoke)
  {
    lastStateCONewSmoke=currenStateCONewSmoke;
    zunoSendReport(5);
    delay(DELAY_TIME); //delay  just to give room for any messages
  }

  //Read the state for the new smoke switch/sensor state
  byte currenStateFaultNewSmoke= digitalRead(READ_NEW_FAULT);
  if(lastStateFaultNewSmoke!=currenStateFaultNewSmoke)
  {
    lastStateFaultNewSmoke=currenStateFaultNewSmoke;
    zunoSendReport(6);
    delay(DELAY_TIME); //delay  just to give room for any messages
  }
  //delay(1000); //One second delay between checks
  if(lastStateFaultNewSmoke==0 || 
      lastStateAlarmNewSmoke==0 ||
      lastStateCONewSmoke==0 ||
      lastStateAlarmOldSmoke==0){
    digitalWrite (LED_PIN , LOW);
  }
  else
  {
    digitalWrite (LED_PIN , HIGH);
  }
  
  
}

byte setterSwitchSmokeNew(byte value) 
{
   if (value > 0) 
   {               // if greater then zero
    digitalWrite (SWITCH_NEW, HIGH); //turn the switch on        
  } 
  else 
  {                         // if equals zero
    digitalWrite(SWITCH_NEW, LOW);   //turn the switch off    
  } 
  lastStateSwitchNewSmoke=value;     
}

byte setterSwitchSmokeOld(byte value) 
{
  if (value > 0) 
  {               // if greater then zero
    digitalWrite (SWITCH_OLD, HIGH); //turn the switch on        
  } 
  else 
  {                         // if equals zero
    digitalWrite(SWITCH_OLD, LOW);   //turn the switch off    
  } 
  lastStateSwitchOldSmoke=value;     
}

byte getterSwitchSmokeNew() 
{
  return lastStateSwitchNewSmoke;
}

byte getterSwitchSmokeOld() 
{
  return lastStateSwitchOldSmoke;
}
