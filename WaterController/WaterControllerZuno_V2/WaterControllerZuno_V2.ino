// pin number, where relay is connected
//Transformer
#define TRANSFORMER_PIN     9

#define VALVE_ONE_PIN     13
#define VALVE_TWO_PIN     14
#define VALVE_THREE_PIN     15
#define VALVE_FOUR_PIN     16
#define VALVE_FIVE_PIN     17
#define VALVE_SIX_PIN     18

// variable to store current relay state
byte transformerLastSetValue;

byte valveOneLastSetValue;
byte valveTwoLastSetValue;
byte valveThreeLastSetValue;
byte valveFourLastSetValue;
byte valveFiveLastSetValue;
byte valveSixLastSetValue;

//Transformer on/Off
ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(getterTransformer, setterTransformer),
  //Valves on/off
  ZUNO_SWITCH_BINARY(getterValveOne, setterValveOne),
  ZUNO_SWITCH_BINARY(getterValveTwo, setterValveTwo),
  ZUNO_SWITCH_BINARY(getterValveThree, setterValveThree),
  ZUNO_SWITCH_BINARY(getterValveFour, setterValveFour),
  ZUNO_SWITCH_BINARY(getterValveFive, setterValveFive),
  ZUNO_SWITCH_BINARY(getterValveSix, setterValveSix)
);

void setup() {
  // put your setup code here, to run once:
  pinMode(TRANSFORMER_PIN, OUTPUT); // set up relay pin as output
  pinMode(VALVE_ONE_PIN, OUTPUT); // set up relay pin as output
  pinMode(VALVE_TWO_PIN, OUTPUT); // set up relay pin as output
  pinMode(VALVE_THREE_PIN, OUTPUT); // set up relay pin as output
  pinMode(VALVE_FOUR_PIN, OUTPUT); // set up relay pin as output
  pinMode(VALVE_FIVE_PIN, OUTPUT); // set up relay pin as output
  pinMode(VALVE_SIX_PIN, OUTPUT); // set up relay pin as output
  //pinMode(LED_BUILTIN, OUTPUT);
  //Init(); //Removed due to 
}

//void Init(){
//  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
//    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
//
//  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(500);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(500);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
//    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(500);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
//  
//    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
//    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
//    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
//  }

void loop() {
  // put your main code here, to run repeatedly:

}




byte getterTransformer() {
  return transformerLastSetValue;
}

void setterTransformer(byte newValue) {
  // newValue is a variable, holding a "value"
  // which came from the controller or other Z-Wave device
  if (newValue > 0) { // if greater then zero
    digitalWrite(TRANSFORMER_PIN, HIGH); //turn relay on
  } else {            // if equals zero
    digitalWrite(TRANSFORMER_PIN, LOW);  //turn relay off
  }
  // save the new value in a variable
  transformerLastSetValue = newValue;
}

byte getterValveOne() {
  return valveOneLastSetValue;
}

void setterValveOne(byte newValue) {
  // newValue is a variable, holding a "value"
  // which came from the controller or other Z-Wave device
  if (newValue > 0) { // if greater then zero
    digitalWrite(VALVE_ONE_PIN, HIGH); //turn relay on
  } else {            // if equals zero
    digitalWrite(VALVE_ONE_PIN, LOW);  //turn relay off
  }
  // save the new value in a variable
  valveOneLastSetValue = newValue;
}

byte getterValveTwo() {
  return valveTwoLastSetValue;
}

void setterValveTwo(byte newValue) {
  // newValue is a variable, holding a "value"
  // which came from the controller or other Z-Wave device
  if (newValue > 0) { // if greater then zero
    digitalWrite(VALVE_TWO_PIN, HIGH); //turn relay on
  } else {            // if equals zero
    digitalWrite(VALVE_TWO_PIN, LOW);  //turn relay off
  }
  // save the new value in a variable
  valveTwoLastSetValue = newValue;
}

byte getterValveThree() {
  return valveThreeLastSetValue;
}

void setterValveThree(byte newValue) {
  // newValue is a variable, holding a "value"
  // which came from the controller or other Z-Wave device
  if (newValue > 0) { // if greater then zero
    digitalWrite(VALVE_THREE_PIN, HIGH); //turn relay on
  } else {            // if equals zero
    digitalWrite(VALVE_THREE_PIN, LOW);  //turn relay off
  }
  // save the new value in a variable
  valveThreeLastSetValue = newValue;
}

byte getterValveFour() {
  return valveFourLastSetValue;
}

void setterValveFour(byte newValue) {
  // newValue is a variable, holding a "value"
  // which came from the controller or other Z-Wave device
  if (newValue > 0) { // if greater then zero
    digitalWrite(VALVE_FOUR_PIN, HIGH); //turn relay on
  } else {            // if equals zero
    digitalWrite(VALVE_FOUR_PIN, LOW);  //turn relay off
  }
  // save the new value in a variable
  valveFourLastSetValue = newValue;
}

byte getterValveFive() {
  return valveFiveLastSetValue;
}

void setterValveFive(byte newValue) {
  // newValue is a variable, holding a "value"
  // which came from the controller or other Z-Wave device
  if (newValue > 0) { // if greater then zero
    digitalWrite(VALVE_FIVE_PIN, HIGH); //turn relay on
  } else {            // if equals zero
    digitalWrite(VALVE_FIVE_PIN, LOW);  //turn relay off
  }
  // save the new value in a variable
  valveFiveLastSetValue = newValue;
}

byte getterValveSix() {
  return valveSixLastSetValue;
}

void setterValveSix(byte newValue) {
  // newValue is a variable, holding a "value"
  // which came from the controller or other Z-Wave device
  if (newValue > 0) { // if greater then zero
    digitalWrite(VALVE_SIX_PIN, HIGH); //turn relay on
  } else {            // if equals zero
    digitalWrite(VALVE_SIX_PIN, LOW);  //turn relay off
  }
  // save the new value in a variable
  valveSixLastSetValue = newValue;
}
