#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <FineOffset.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DEVICE_ID 7

#define ONE_WIRE_BUS 2
#define CURRENT_PIN 0
#define TX_PIN 1
#define LED_PIN 3
#define MOISTURE_PIN A2

const int AirValue = 593;   //you need to replace this value with Value_1
const int WaterValue = 230;  //you need to replace this value with Value_2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

FineOffset tx(TX_PIN,3);

byte saveADCSRA;                      // variable to save the content of the ADC for later. if needed.
volatile byte counterWD = 0; // Count how many times WDog has fired. Used in the timing of the
                                           // loop to increase the delay before the LED is illuminated. For example,

void setup() {  
  pinMode(ONE_WIRE_BUS,INPUT_PULLUP);
  pinMode ( LED_PIN, OUTPUT );   
  pinMode ( CURRENT_PIN, OUTPUT );   
  pinMode(MOISTURE_PIN,INPUT);
  
  DoBlink();
  StopSensorCurrent();
  //sensors.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
   if ( counterWD >= 7 )                 // if the WDog has fired 10 times......
  {
    DoBlink();
    counterWD = 0;                        // reset the counterWD for another 10 WDog firings
    StartSensorCurrent();
    delay(1000);
    //Reading of moisture
    //int humidity=GetHumidity();
    int humidity=88;
    //Reading of temperature
    //float temp=GetTemperature();
    float temp=22.2;
    //Sending of readResults
    SendTemperatureAndMoisture(temp,humidity);
    delay(2000);
    StopSensorCurrent();
    DoBlink();
  } // end of if counterWD
 
  sleepNow ();                          // then set up and enter sleep mode
  

}

int GetHumidity(){
  delay(2000);
  int soilMoistureValue = analogRead(MOISTURE_PIN);
  int soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  return soilmoisturepercent;
}

float GetTemperature(){
  sensors.begin();
  delay(2000);
  sensors.requestTemperatures(); 
  delay(2000);
  float temp=sensors.getTempCByIndex(0);
  return temp;
}

void SendTemperatureAndMoisture(float temperature, int humidity){
  FineOffset tx(TX_PIN,1);
  delay(200);
  tx.send(DEVICE_ID, temperature, humidity);
}

void StartSensorCurrent(){
  digitalWrite(CURRENT_PIN,LOW);
}
void StopSensorCurrent(){
  digitalWrite(CURRENT_PIN,HIGH);
}

void DoBlink() {
  digitalWrite ( LED_PIN, HIGH );        // flash the led on for 100ms
  delay ( 100 );                        // hate using delay(), but hey! it's only a demo
  digitalWrite ( LED_PIN, LOW );
}

void sleepNow ()
{
  set_sleep_mode ( SLEEP_MODE_PWR_DOWN ); // set sleep mode Power Down
  saveADCSRA = ADCSRA;                    // save the state of the ADC. We can either restore it or leave it turned off.
  ADCSRA = 0;                             // turn off the ADC
  power_all_disable ();                   // turn power off to ADC, TIMER 1 and 2, Serial Interface
 
  noInterrupts ();                        // turn off interrupts as a precaution
  resetWatchDog ();                       // reset the WatchDog before beddy bies
  sleep_enable ();                        // allows the system to be commanded to sleep
  interrupts ();                          // turn on interrupts
 
  sleep_cpu ();                           // send the system to sleep, night night!

  sleep_disable ();                       // after ISR fires, return to here and disable sleep
  power_all_enable ();                    // turn on power to ADC, TIMER1 and 2, Serial Interface
 
  // ADCSRA = saveADCSRA;                 // turn on and restore the ADC if needed. Commented out, not needed.
 
} // end of sleepNow ()


void resetWatchDog ()
{
  MCUSR = 0;
  WDTCR = bit ( WDCE ) | bit ( WDE ) | bit ( WDIF ); // allow changes, disable reset, clear existing interrupt
  WDTCR = bit ( WDIE ) | bit ( WDP3 )| bit ( WDP0 ); // set WDIE ( Interrupt only, no Reset ) and 1 second TimeOut
                                                     
  wdt_reset ();                            // reset WDog to parameters
 
} // end of resetWatchDog ()

ISR ( WDT_vect )
{
  wdt_disable ();                           // until next time....
  counterWD ++;                             // increase the WDog firing counter. Used in the loop to time the flash
                                            // interval of the LED. If you only want the WDog to fire within the normal
                                            // presets, say 2 seconds, then comment out this command and also the associated
                                            // commands in the if ( counterWD..... ) loop, except the 2 digitalWrites and the
                                            // delay () commands.
} // end of ISR
