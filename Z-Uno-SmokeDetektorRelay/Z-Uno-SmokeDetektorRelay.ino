#define EiOutput   9     // pin connection  
#define EiInput 18//10     // pin connection 
#define DeltronicsOutput  11     // pin connection 
#define DeltronicsInput 12     // pin connection 

#define ledTest 8     // pin connection 

byte currentEiOutpuValue;
byte currentDeltronicsOutputValue;
byte lastStateEiInputValue;
byte lastStatDeltronicsInputValue;

#define ZUNO_CHANNEL_NUMBER_ONE   1

ZUNO_SETUP_CHANNELS(
      ZUNO_SWITCH_BINARY(getEiOutput, setEiOutput),
      ZUNO_SWITCH_BINARY(getDeltronicsOutput, setDeltronicsOutput),
      ZUNO_SENSOR_BINARY(ZUNO_SENSOR_BINARY_TYPE_SMOKE, getEiInput),
      ZUNO_SENSOR_BINARY(ZUNO_SENSOR_BINARY_TYPE_SMOKE, getDeltronicsInput)
);

//Always on
ZUNO_SETUP_SLEEPING_MODE(ZUNO_SLEEPING_MODE_ALWAYS_AWAKE);

void setup() {
  // put your setup code here, to run once:
  pinMode(EiOutput, OUTPUT);
  pinMode(DeltronicsOutput, OUTPUT);
  pinMode(EiInput, INPUT_PULLUP);
  pinMode(DeltronicsInput, INPUT_PULLUP);
  digitalWrite(EiOutput, LOW);
  digitalWrite(DeltronicsOutput, LOW);

  pinMode(ledTest, OUTPUT);
  //digitalWrite(ledTest, HIGH);
  //delay(1000);
  //digitalWrite(ledTest, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
 bool sendReport=false;
 byte fetchedStateDeltronicsInput=digitalRead(DeltronicsInput);
 byte fetchedStateEiInput=digitalRead(EiInput);
 if(fetchedStateDeltronicsInput!=lastStatDeltronicsInputValue){  
  lastStatDeltronicsInputValue=fetchedStateDeltronicsInput;    
  sendReport=true;
 }
 if(fetchedStateEiInput!=lastStateEiInputValue){  
  lastStateEiInputValue=fetchedStateEiInput;
  sendReport=true;
  
 }
 if(sendReport){
  digitalWrite(ledTest, HIGH);
  delay(1000);
  digitalWrite(ledTest, LOW);
  zunoSendReport(ZUNO_CHANNEL_NUMBER_ONE);
 }
}

BYTE getEiOutput() {
    return currentEiOutpuValue;
}

void setEiOutput(BYTE newValue) {
  if (newValue > 0) {               // if greater then zero
    digitalWrite (EiOutput, HIGH); //turn on (HIGH is the voltage level)
  } else {                         // if equals zero
    digitalWrite(EiOutput, LOW);   //turn off by making the voltage LOW
  }
  // we'll save our value for the situation, when the controller will ask us about it
  currentEiOutpuValue = newValue;
}

BYTE getDeltronicsOutput() {
    return currentDeltronicsOutputValue;
}

void setDeltronicsOutput(BYTE newValue) {
  if (newValue > 0) {               // if greater then zero
    digitalWrite (DeltronicsOutput, HIGH); //turn on (HIGH is the voltage level)
  } else {                         // if equals zero
    digitalWrite(DeltronicsOutput, LOW);   //turn off by making the voltage LOW
  }
  // we'll save our value for the situation, when the controller will ask us about it
  currentDeltronicsOutputValue = newValue;
}

BYTE getEiInput() {
   if (lastStateEiInputValue == 0) { // if button is pressed
    return 0xff;              // return "Triggered" state to the controller
  } else {                    // if button is released
    return 0;                 // return "Idle" state to the controller
  }
    //return lastStateEiInputValue;
}

BYTE getDeltronicsInput() {
    return lastStatDeltronicsInputValue;
}
