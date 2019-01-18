// setupnfc does not block or wait for a success
// you will need to call it again if the global var versiondata is not set
// once it sees the hardware it sets the number of retries and configures the board
// it then prints the current time and "nfc started to the terminal
void setupnfc() {
  Serial.println("looking for NFC");
  //search for the NFC hardware
  versiondata = nfc.getFirmwareVersion();
  if (versiondata){
    nfc.setPassiveActivationRetries(nfcretries);  // Set the max number of retry attempts to read from a card
    nfc.SAMConfig();                              // configure board to read RFID tags
    terminal.println("NFC started");
    terminal.print(currentTime);
  }

}

/**************************************
  scanNfc
  Get card data from  NFC hardware
**************************************/
void scanNfc() {

  uint8_t uid[maxuidlength] = {0, 0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
  String  suid;
  uint8_t uidLength = 0;                          // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  char holder[maxnamelength];

  if (versiondata) {
    nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
    if (uidLength) {                                      // found a card
      for (int i = 0; i < uidLength; i++) {
        suid += String(uid[i], HEX);                      // convert the uid to a hex string
      }
      Blynk.virtualWrite(cardiddisplay, suid);           // display the uid
      cardIndex = findUid(suid);
      if (cardIndex) {
        unlockDoor();                                     // unlock the door if the card was valid
        Blynk.virtualWrite(namedisplay, cardHolder[cardIndex]); //display the card holder name on the app
        terminal.print(cardHolder[cardIndex]);  // send debug data to the terminal
        terminal.print(" ");
        terminal.print(currentTime);
        terminal.print(" ");
        terminal.println(currentDate);
        terminal.println(cardId[cardIndex]);
        terminal.flush();
        Blynk.virtualWrite(indexdisplay, cardIndex);  // display the card index on the app
        return;                                       // card found exit
      }
      storeCard(0, suid, String(" "), 0);             // no card found clear card display
      Blynk.virtualWrite(namedisplay, " ");           // clear name display on the app
      Blynk.virtualWrite(indexdisplay, 0);            // set index display to 0 on the app
    }
  } else {
    setupnfc();                                       // if hardware not found then start it now.
  }
}
