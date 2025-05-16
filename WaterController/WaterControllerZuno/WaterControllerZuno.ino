// pin number, where relay is connected
//Transformer

#define TRANSFORMER_PIN     10

#define VALVE_ONE_PIN     20
#define VALVE_TWO_PIN     21
#define VALVE_THREE_PIN     22

// variable to store current relay state
byte transformerLastSetValue;
byte valveOneLastSetValue;
byte valveTwoLastSetValue;
byte valveThreeLastSetValue;

//Transformer on/Off
ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(getterTransformer, setterTransformer),

  ZUNO_SWITCH_BINARY(getterValveOne, setterValveOne),

  ZUNO_SWITCH_BINARY(getterValveTwo, setterValveTwo),

  ZUNO_SWITCH_BINARY(getterValveThree, setterValveThree)  
);

void setup() {
  // put your setup code here, to run once:
  pinMode(TRANSFORMER_PIN, OUTPUT); // set up relay pin as output
  pinMode(VALVE_ONE_PIN, OUTPUT); // set up relay pin as output
  pinMode(VALVE_TWO_PIN, OUTPUT); // set up relay pin as output
  pinMode(VALVE_THREE_PIN, OUTPUT); // set up relay pin as output

  transformerLastSetValue=0;
  valveOneLastSetValue=0;
  valveTwoLastSetValue=0;
  valveThreeLastSetValue=0;
}

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
