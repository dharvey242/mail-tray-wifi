
/////////Beam start
#include "application.h"
//#include "beam.h"

/* pin definitions for Beam */
//#define RSTPIN 2        //use any digital pin
//#define IRQPIN 9        //currently not used - leave unconnected
//#define BEAMCOUNT 2    //number of beams daisy chained together

/* Iniitialize an instance of Beam */
//Beam b = Beam(RSTPIN, IRQPIN, BEAMCOUNT);
//////////////BEAM end

int fullempty = 0;
int light1;
int state1 = 2;
//offline/online veriable
int offlinevar;
int onlinevar;
//time sync
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();  
//light thresholds per tray
int t1t = 20;
int t1tt = 21;
//timedelay
int tdelay = 500;
int afterpub = 3000;
int timstate = 0;
int lastHour = 24;

int light_sensor_pin1 = A1;
int led1 = D4;  ///led pins
int led2 = D5;  ///led pins

//alarm times
int alarmtime1 = 10;


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
int currentMinute = Time.minute();
int refreshvar;
    
unsigned long previousMillis = 0; // last time update
long interval = 300000; // interval at which to do something (milliseconds)

void setup() {
    
    
pinMode(led1, OUTPUT);  //led pins
pinMode(led2, OUTPUT);  //led pins

digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led

    
    ///////beam
//Wire.begin();
WiFi.connect();
//b.begin();
    ////////////beam setup
    
    //manual refresh
Particle.function("refresh1",refreshmanual);
    
Time.zone(+1); //utc time - 1 hour behind GMT

    //set the measurement on startup and send first data
    float light_measurement1 = analogRead(light_sensor_pin1);
   light1 = (int)(light_measurement1/4096*100);
   if (light1 <= t1t){
     full1();
     } 
    else {
      if (light1 >= t1tt){
      empty1();
      }
    }
    
    ////send hour and minute to dashboard to check (only on startup)
    int currentHour = Time.hour();
     Particle.publish("current hour", String(currentHour) + " hour");
      int currentMinute = Time.minute();
     Particle.publish("current minute", String(currentMinute) + " minute");
     
     //if particle connected, light led green
    if (Particle.connected()) {
    greenledflash();
     }
    else {
        //if particle not connect, light led red
  redledflash();
    }
     
     // b.print("Provided by Hobs On-Site");
    //b.setSpeed(2);
    //b.play();
    //b.setLoops(1);
    //delay (5000);
   
     
}

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
    //sync time periodically
    if (millis() - lastSync > ONE_DAY_MILLIS) {
    // Request time synchronization from the Particle Cloud
    Particle.syncTime();
    lastSync = millis();
     }
    
int currentHour = Time.hour(); // hour() returns zero through twenty-three

if (currentHour >= 6 && currentHour <= 20) //online times
{
    
    offlinevar = 0;    //stop offline publish more than once
    if (onlinevar != 1){Particle.publish("online", String(currentHour) + "  online");
    onlinevar = 1;}

if (timstate != 1 && currentHour == alarmtime1) //change the time in two places
  {
tray1email(); //send emails for alarms

    }
    timstate = 1;
//reset the state so that it doesnt execute twice
//also change the time if required set to 20:00
if (currentHour != alarmtime1)
    {
    timstate = 0;
    } 

 //carry on doing the rest of the loop
 //check light readings
    tray1();
    
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
     previousMillis = currentMillis;  

     // do something
     if(state1 ==0){Particle.publish("tray1full", String(light1) + "%");}
     if(state1 ==1){Particle.publish("tray1empty", String(light1) + "%");}
     
     
  }
    


}

    else if(offlinevar != 1) {//while asleep turn off the leds
   ledoff();
    Particle.publish("offline", String(currentHour) + "  offline");
    offlinevar = 1;
    onlinevar = 0;
    
    delay(1000);
    
    //reset state for empty/full so that it reruns when online again.
    state1 = 2;
    }


}
    
   
    
 



void tray1()
 {
    float light_measurement1 = analogRead(light_sensor_pin1);
        light1 = (int)(light_measurement1/4096*100);
         
    if (light1 <= t1t && state1 != 0){
      state1 = 0;
      full1();
      
     } 
    else {
      if (light1 >= t1tt && state1 != 1){
      state1 = 1;
      empty1();
      
      }
    }
    
    //delay(tdelay);       
  
    }
    
    void empty1() 
{
            //Particle.publish("tray1empty", String(light1) + "%");
            fullempty = 0;
           // beamtrayisempty();
            redled();
            delay(afterpub);
           
 
}
       
      
void full1() 
{
           // Particle.publish("tray1full", String(light1) + "%");
            fullempty = 1;
           // beamtrayisfull();
            greenled();
            delay(afterpub);///slow it down once detection takes place to avoid mixed messages
            

}

void tray1email()
{
   float light_measurement1 = analogRead(light_sensor_pin1);
    light1 = (int)(light_measurement1/4096*100);
         
            if (light1 <= t1tt)     {
      
      Particle.publish("tray1fullemail", "email sent to tray1 recipient");
             // Turn ON the LED
                                } 
    else {
      if (light1 >= t1tt){
     //for later on perhaps empty email alarms once a day
                    }
        }
    //delay(100);  
}

//void welcome(){
     //  Beam can begin scrolling text with just print() and play()
 //   b.print("Welcome, collections at 10am and 2pm");
 ////   b.setSpeed(2);
 //   b.play();
    
//}

//void beamtrayisempty(){
     //  Beam can begin scrolling text with just print() and play()
//    b.print("No document detected");
  //  b.setSpeed(2);
  //  b.play();
  //  b.setLoops(1);
//}

//void beamtrayisfull(){
     //  Beam can begin scrolling text with just print() and play()
//    b.print("Document detected - Notification sent");
 //   b.setSpeed(2);
 //   b.play();
 //   b.setLoops(1);
//}



void greenled(){
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
//delay (100); //delay for safety
///then make green
digitalWrite(led1, LOW);
digitalWrite(led2, HIGH);
}

void redled(){
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
//delay (100); //delay for safety
///then make green
digitalWrite(led1, HIGH);
digitalWrite(led2, LOW);
}

void ledoff(){
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
}

void greenledflash(){
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
delay (200); //delay for safety
///then make green
digitalWrite(led1, LOW);
digitalWrite(led2, HIGH);
delay (1000);
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
delay (1000);
digitalWrite(led1, LOW);
digitalWrite(led2, HIGH);
delay (1000);
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
}

void redledflash(){
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
delay (200); //delay for safety
///then make red
digitalWrite(led1, HIGH);
digitalWrite(led2, LOW);
delay (1000);
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
delay (1000);
digitalWrite(led1, HIGH);
digitalWrite(led2, LOW);
delay (1000);
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
}



///manual refresh
 int refreshmanual(String command) {
        if(command=="refresh"){
        state1 = 2;
        tray1();
         }
        
        
        
        else {
            return -1;
            }
        }
