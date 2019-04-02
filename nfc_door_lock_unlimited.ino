#include <FS.h>

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

    20 cards are supported per lock
    unlimited locks are supported
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
    V25 , V26 are bridge data exchange

*/

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <BlynkSimpleEsp8266.h>
#include <U8g2lib.h>
#include "SSD1306Wire.h"
#include <TimeLib.h>
#include <WidgetRTC.h>
#include "globals.h"                    // tab for other defines and globals

/*
 * Saves the time data into global data
 */
// get the time and the day
void getCurrentTime() {
  currentTime = String(String(hour()) + ":" + twoDigits(minute()) + ":" + twoDigits(second()));
  currentMonth = monthstrings[month() - 1];
  currentDate = String(day());
  currentDay = daystrings[weekday() - 1];
}

bool checkactivedoor() {
  long nowseconds = ((hour() * 3600) + (minute() * 60) + second());
  int today = weekday();
  return days[today];
}

void lockDoor() {
  digitalWrite(RELAY_PIN, LOW);             // turn off pin this causes the door to lock
  Blynk.virtualWrite(lockbutton, 0);      // display door locked on the app
  flag = 0;
  display.clear();
}

void unlockDoor() {
    if (!flag) {
      flag = 1;
      digitalWrite(RELAY_PIN, HIGH);              // send power to unlock the door
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
  setup OLED display
  restore nfc data

**************************************/
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  Wire.setClock(wirespeed);                     // set the Ic2 buss speed
  Serial.begin(serialspeed);                    // open the serial port
  Wire.pins(SDA_PIN, SLC_PIN);                  // SDA, SLC pins
  Wire.begin();                                 // start the Ic2 buss
  nfc.begin();                                  // init the NFC Hardware
  SPIFFS.begin();

  Serial.print("Connected at " + currentTime);                  // send start time to serial

  startmywifi();                                //connect to the wifi
  Blynk.config(lock_token);                     // config Blynk
  getdatafromfile();                            //restore card data
  setupdisplay();                               // setup the OLED display
  timer.setInterval(1000l, scanNfc);            // scan the NFC on a timer

  ArduinoOTA.setHostname(module_name);          // give a name to our module
  ArduinoOTA.begin();                           // OTA initialization
  /*
   * Lambda timer
   * place code here that you need to run in sync
   */
  timer.setInterval(700L, [] () { //  <<=== set how often the code should run 700L = Run every .7 seconds
    ArduinoOTA.handle();                        // check for OTA update
  });  // END Timer Function
}

/**************************************
  main loop
**************************************/
void loop()
{
  if (Blynk.connected()) {
    Blynk.run();
  } else Blynk.connect(1000l);                // timeout 1 seconds
  timer.run();
}


