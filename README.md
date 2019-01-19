# esp8266-NFC-doorlock-

Creative Commons License
   NFC Door lock
   by Michael Dale Burton
   is licensed under a Creative Commons Attribution-NonCommercial
   4.0 International License.

    Impliments a door lock system using NFC reader and the Blynk app
    Blynk is used to store the data on the cloud and provide
    a smart phone interface for adding, removing and assigning cards
    Device selector is used to choose the lock/door
    in this version the card / nfc data is stored in the SPIFFS memory

    20 cards are supported
    2 slaved locks are supported
    1 master  is used as the scanner for adding cards it can also be used as a lock

you will need three blynk tokens.  
One for each lock.  If you have fewer than three it will still work.  
You must have at least one.  It will act as the master and the lock.

    Nodemcu esp8266 and PN532 hardware is used

BLYNK pin map

    V1 is a timer
    V2 is a reset button
    V3 ia a text input for the user name assigned to a card
    V4 is a value display for the card id
    V5 current card 
    V6 is a terminal window for debug information
    V10 is the delete button
    v11 is the copy button
    v12 is the copy all button -- it may not be working
    V24 is a value display for the number of valid cards
    V25 - V45 are reserved for data storage
