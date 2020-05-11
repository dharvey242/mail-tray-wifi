
 

// Your GPRS credentials (leave empty, if not needed)
const char apn[]      = "wap.vodafone.co.uk"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "wap"; // GPRS User
const char gprsPass[] = "wap"; // GPRS Password

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = ""; 

// Server details
// The server variable can be just a domain name or it can have a subdomain. It depends on the service you are using
const char server[] = "wldpc.co.uk"; // domain name: example.com, maker.ifttt.com, etc
const char resourceempty[] = "/tcp/clientb/t1e.php";         // resource path, for example: /post-data.php
const char resourcefull[] = "/tcp/clientb/t1f.php";         // resource path, for example: /post-data.php
const int  port = 80;                             // server port number


// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

////////////////////////////////light sensor pins

int light_sensor_pin1 = 34;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider

int fullempty = 0;
int light1;
int state1 = 2;
//light thresholds per tray
int t1t = 10;
int t1tt = 11;
//timedelay
int tdelay = 500;
int afterpub = 3000;


//// publish times set to 1 hour intervals from on time//
unsigned long previousMillis = 0;
const unsigned long interval = 3600000;///minute 60000 second 1000 3600000 1 hour
////////////////////////////////////////////////

///led pins
int led1 = 32;  
int led2 = 33;
///////////


// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS

#include <Wire.h>
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif


// I2C for SIM800 (to keep it running when powered from battery)
TwoWire I2CPower = TwoWire(0);


// TinyGSM Client for Internet connection
TinyGsmClient client(modem);

#define uS_TO_S_FACTOR 1000000     /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3600        /* Time ESP32 will go to sleep (in seconds) 3600 seconds = 1 hour */

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

bool setPowerBoostKeepOn(int en){
  I2CPower.beginTransmission(IP5306_ADDR);
  I2CPower.write(IP5306_REG_SYS_CTL0);
  if (en) {
    I2CPower.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  } else {
    I2CPower.write(0x35); // 0x37 is default reg value
  }
  return I2CPower.endTransmission() == 0;
}


void setup() {
  // Set serial monitor debugging window baud rate to 115200
  SerialMon.begin(115200);

  // Start I2C communication
  I2CPower.begin(I2C_SDA, I2C_SCL, 400000);


  // Keep power when running from battery
  bool isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // use modem.init() if you don't need the complete restart

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }

//////////////////////////////  
pinMode(led1, OUTPUT);  //led pins
pinMode(led2, OUTPUT);  //led pins


digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led



 //set the measurement on startup and send first data
    float light_measurement1 = analogRead(light_sensor_pin1);
   light1 = (int)(light_measurement1/4096*100);
   Serial.println(light_measurement1);
   Serial.println(light1);
   if (light1 <= t1t){
     sendfull();
     } 
    else {
      if (light1 >= t1tt){
      sendempty();
      }
    }

greenledflash();
digitalWrite(led1, LOW); ///blank the led
digitalWrite(led2, LOW); ///blank the led
Serial.println(light_measurement1);
   Serial.println(light1);
state1 = 2;///clear status

/////////////////////////
}

void loop() {

///////////CHECK TIME AND SUBMIT DATA EVERY HOUR/////////////
unsigned long currentMillis = millis();

if (currentMillis - previousMillis >= interval) {
    // save the time 
    previousMillis += interval;

if (state1 == 1) {
    sendempty();
                }
 
if (state1 == 0) {
    sendfull();
                } 
                                                }
  checktray();
}

void checktray()
  {
    float light_measurement1 = analogRead(light_sensor_pin1);
        light1 = (int)(light_measurement1/4096*100);
        
         
    if (light1 <= t1t && state1 != 0){
      state1 = 0;
      redled();
      Serial.println(light1);
      //sendfull();  ///realtime sending
      
     } 
    else {
      if (light1 >= t1tt && state1 != 1){
      state1 = 1;
      greenled();
      Serial.println(light1);
      
      //sendempty(); //reatime sending
      
      }
  
  }
  }
       
      

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


void sendempty(){
  
  SerialMon.print("Connecting to APN empty: ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
  }
  else {
    SerialMon.println(" OK");
    
    SerialMon.print("Connecting to ");
    SerialMon.print(server);
    if (!client.connect(server, port)) {
      SerialMon.println(" fail");
    }
    else {
      SerialMon.println(" OK");
    
      // Making an HTTP POST request
      SerialMon.println("Performing HTTP POST request...");
      // Prepare your HTTP POST request data (Temperature in Celsius degrees)
     String httpRequestData = "";
     
    
      client.print(String("POST ") + resourceempty + " HTTP/1.1\r\n");
      client.print(String("Host: ") + server + "\r\n");
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(httpRequestData.length());
      client.println();
      client.println(httpRequestData);
      SerialMon.println(httpRequestData);

      unsigned long timeout = millis();
      while (client.connected() && millis() - timeout < 10000L) {
        // Print available data (HTTP response from server)
        while (client.available()) {
          char c = client.read();
          SerialMon.print(c);
          timeout = millis();
        }
      }
      SerialMon.println();
    
      // Close client and disconnect
      client.stop();
      SerialMon.println(F("Server disconnected"));
      modem.gprsDisconnect();
      SerialMon.println(F("GPRS disconnected"));
    }
  }
 

  redled();
  delay(afterpub); ///slow it down once detection takes place to avoid mixed messages

}


void sendfull(){
  SerialMon.print("Connecting to APN full: ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
  }
  else {
    SerialMon.println(" OK");
    
    SerialMon.print("Connecting to ");
    SerialMon.print(server);
    if (!client.connect(server, port)) {
      SerialMon.println(" fail");
    }
    else {
      SerialMon.println(" OK");
    
      // Making an HTTP POST request
      SerialMon.println("Performing HTTP POST request...");
      // Prepare your HTTP POST request data (Temperature in Celsius degrees)
     String httpRequestData = "";
     
    
      client.print(String("POST ") + resourcefull + " HTTP/1.1\r\n");
      client.print(String("Host: ") + server + "\r\n");
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(httpRequestData.length());
      client.println();
      client.println(httpRequestData);
      SerialMon.println(httpRequestData);

      unsigned long timeout = millis();
      while (client.connected() && millis() - timeout < 10000L) {
        // Print available data (HTTP response from server)
        while (client.available()) {
          char c = client.read();
          SerialMon.print(c);
          timeout = millis();
        }
      }
      SerialMon.println();
    
      // Close client and disconnect
      client.stop();
      SerialMon.println(F("Server disconnected"));
      modem.gprsDisconnect();
      SerialMon.println(F("GPRS disconnected"));
    }
  }
 

  greenled();
  delay(afterpub); ///slow it down once detection takes place to avoid mixed messages

}
