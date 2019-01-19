#include "FS.h"

/*

   Creative Commons License
   NFC Door lock
   by Michael Dale Burton
   is licensed under a Creative Commons Attribution-NonCommercial
   4.0 International License.

    Impliments a door lock system using NFC reader and the Blynk app
    Blynk is used to store the data on the cloud and provide
    a smart phone interface for adding, removing and assigning cards
    Device selector is used to choose the lock/door

    20 cards are supported
    2 slaved locks are supported
    1 master  is used as the scanner for adding cards it can also be used as a lock

    Nodemcu esp8266 and PN532 hardware is used

    V1 is a timer
    V2 is a reset button
    V3 ia a text input for the user name assigned to a card
    V4 is a value display for the card id
    V5 current card index "vcount"
    V6 is a terminal window for debug information
    V10 is the delete button
    V24 is a value display for the number of valid cards
    V25 - V45 are reserved for data storage

*/

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>

/*included for wifi manager
  #include <DNSServer.h>            //Local DNS Server for redirecting all requests to the configuration portal
  #include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
  #include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

  #include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
*/

#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include "globals.h"                    // tab for other defines and globals


PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

//create terminal widget instance and attach it to the pin
WidgetTerminal terminal(terminaldisplay);

//Create the real time clock instance
WidgetRTC rtc;

//create timer instance
BlynkTimer timer;

//create bridge instance and assign pins
WidgetBridge bridge1(51);
WidgetBridge bridge2(52);
WidgetBridge bridge3(53);



// get the time and the day
void getCurrentTime() {
  currentTime = String(hour() + ":" + minute());
  currentDate = String(day());
}

void lockDoor() {
  digitalWrite(LED_PIN, LOW);             // turn off LED this causes the door to lock
  Blynk.virtualWrite(lockbutton, 0);      // display door locked on the app
  flag = 0;
}

void unlockDoor() {
  if (!flag) {
    flag = 1;
    digitalWrite(LED_PIN, HIGH);              // send power to unlock the door
    timer.setTimeout(5000L, lockDoor);        // lock the door after 5 seconds
  }
  Blynk.virtualWrite(lockbutton, 1);          // update the app
}


/**************************************
  connect to wifi
  connect to blynk server
  create timer
  start nfc
  start I2c
  set I2c rate and pins
  start serial and set speed

**************************************/
void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RTS_PIN, OUTPUT);
  pinMode(DTR_PIN, OUTPUT);

  Wire.setClock(wirespeed);                     // set the Ic2 buss speed
  Serial.begin(serialspeed);                    // open the serial port
  Wire.pins(SDA_PIN, SLC_PIN);                  // SDA, SLC pins
  Wire.begin();                                 // start the Ic2 buss
  nfc.begin();                                  // init the NFC Hardware
  SPIFFS.begin();

  Serial.print(currentTime);
  
  Blynk.begin(blynk_token, ssid, pass);         // start Blynk
  getdatafromfile();
//  Blynk.syncVirtual(vcount, datapins);          // restore count and card data  This is how all the card data gets restored from the cloud
//  Blynk.syncVirtual(vcount);                    // restore count
  timer.setInterval(500l, scanNfc);             // scan the NFC every .5 seconds
  ArduinoOTA.setHostname(modulename); // give an name to our module
  ArduinoOTA.begin(); // OTA initialization 
}

/**************************************
  main loop
**************************************/
void loop()
{
  if (Blynk.connected()){
    Blynk.run();
  } else Blynk.connect(2500l);                // timeout 2.5 seconds
  timer.run();
  ArduinoOTA.handle();
}


