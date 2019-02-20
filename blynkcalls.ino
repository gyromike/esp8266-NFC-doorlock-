/*
   NFC Door lock
   Auther Michael Burton

   Creative Commons License
   NFC Door lock
   by Michael Dale Burton
   is licensed under a Creative Commons Attribution-NonCommercial
   4.0 International License.

*/
BLYNK_CONNECTED() {
  rtc.begin();                              // Synchronize time on connection
  bridge_master.setAuthToken(reader_token); // Token of reader
}

BLYNK_WRITE(vcount) {
  //  openspiffs();
  //  rowIndex = read16data(0);                                                 //save the card count in offset 0
  //  closespiffs();
}


BLYNK_READ(vcount)
{
  Blynk.virtualWrite(vcount, rowIndex);                                       // write the rowIndex to the server
  //  openspiffs();
  //  write16data(0, rowIndex);                                                 //save the card count in fram at offset 0
  //  closespiffs();
}


BLYNK_WRITE(flagpin)
{
  accessFlags[0] = param.asInt();
}

BLYNK_READ(flagpin)
{
  Blynk.virtualWrite(flagpin, accessFlags[0]);                                //update the flag display on the app
}

BLYNK_WRITE(updatepin) {
  if (param.asInt()) {
    int tempid = findUid(cardId[0]);
    if (!tempid) {                                                            // if no card has this id then see if we can add a card
      if (rowIndex < maxcards) {                                              // if the maximum number of cards has not been stored then add a card
        storeCard(++rowIndex, cardId[0], cardHolder[0], accessFlags[0]);      //copy temp data into card memory
      }
    } else storeCard(tempid, cardId[0], cardHolder[0], accessFlags[0]);       //replace the data
  }
}

// called when the name nameDisplay has had input
BLYNK_WRITE(namedisplay)
{
  String textIn = param.asStr();                                              // Text Input Widget - Strings
  cardHolder[0] = textIn;                                                     //get the Text Input Widget - String
}


BLYNK_READ(namedisplay)
{
  Blynk.virtualWrite(namedisplay, cardHolder[0]);                             //update the name display on the app
}


BLYNK_WRITE(cardiddisplay)
{
  String textIn = param.asStr();                                              // Text Input Widget - Strings
  cardId[0] = textIn;                                                         //set the temp data for the card ID
}

/*
   cycle through the cards

*/
BLYNK_WRITE(indexdisplay) {
  cardIndex = param.asInt();
  if (cardIndex > rowIndex)
    cardIndex = 1;
  else if (cardIndex = 0)
    cardIndex = rowIndex;
  getCard(cardIndex);
  Blynk.virtualWrite(namedisplay, cardHolder[0]);
  Blynk.virtualWrite(cardiddisplay, cardId[0]);
  Blynk.virtualWrite(flagpin, accessFlags[0]);
  Blynk.virtualWrite(indexdisplay, cardIndex);
}

BLYNK_WRITE(terminaldisplay)
{
  dumpdatafromfile();
}

//BLYNK_WRITE_DEFAULT()
//{
//  int pin = request.pin;      // Which pin is handled?
//  if (pin = terminaldisplay) {

//    dumpdatafromfile();

//  }

//}

/*
   this will copy data to the device with the id sent as param[1]
   param[0] determines if we copy all data or just 1 card
*/
BLYNK_WRITE(bridgewrite)
{
  String stoken = param[1].asStr();   // get the lock token string
  char token[34];
  byte tokenbytes[34];

  stoken.getBytes(tokenbytes, 34);    // copy the token string to a char array
  for ( int i = 0; i < 34; i++) {
    token[i] = tokenbytes[i];
  }

  bridge_lock.setAuthToken(token);    // set the bridge token to the lock

  bridge_lock.virtualWrite(bridgedata, cardHolder[0], cardId[0], accessFlags[0]);     //copy current card from reader

}

/*
   this will copy data to the device with the id sent as param[1]
   param[0] determines if we copy all data or just 1 card
*/
BLYNK_WRITE(bridgewriteall)
{
  String stoken = param[1].asStr();   // get the lock token string
  char token[34];
  byte tokenbytes[34];

  stoken.getBytes(tokenbytes, 34);    // copy the token string to a char array
  for ( int i = 0; i < 34; i++) {
    token[i] = tokenbytes[i];
  }

  bridge_lock.setAuthToken(token);    // set the bridge token to the lock

  for (int i = 1; i <= rowIndex; i++) {
    bridge_lock.virtualWrite(bridgedata, cardHolder[i], cardId[i], accessFlags[i]);    //copy all cards from reader
    yield();
  }
}


BLYNK_WRITE(copybutton)
{
  if (param.asInt()) {
    bridge_master.virtualWrite(bridgewrite, 1, lock_token);         //tell masterlock to copy current card to current lock
  }
}



BLYNK_WRITE(copyallbutton)
{
  if (param.asInt()) {
    bridge_master.virtualWrite(bridgewriteall, 2, lock_token);        //tell masterlock to copy all cards to current lock
  }
}

/**************************************
  remove a card from valid card list
  moves the last card into the place of the removed card
  decraments the card count
**************************************/
BLYNK_WRITE(deletebutton)  //Remove
{
  int b =  param.asInt();
  if (b) {                                                //only do this on the high state of the button press
    if (rowIndex > 1) {                                   // if there is more than one card
      if (cardIndex != rowIndex) {                        // if the current card is not the last card
        getCard(rowIndex);                                // get the last card
        storeCard(cardIndex, cardId[0], cardHolder[0], accessFlags[0]);   //move the data for the last card to the deleted cards position
      } else {
        getCard(--cardIndex);                           //get the previous card and decrament the card index
      }
      --rowIndex;           // decrament the card count

    } else {                                            // remove the last card
      rowIndex = 0;                                     // set the indexes to 0 and clear the strings
      cardIndex = 0;
      cardHolder[0] = " ";
      cardId[0] = " ";
    }
    openspiffs();
    write16data(0, rowIndex);                                                 //save the card count in fram at offset 0
    closespiffs();                                                                //save the card count in fram at offset 0
    Blynk.virtualWrite(vcount, rowIndex);           // store the card count on server
    Blynk.virtualWrite(indexdisplay, cardIndex);    // display the card index
  }
  Blynk.virtualWrite(namedisplay, cardHolder[0]);
  Blynk.virtualWrite(cardiddisplay, cardId[0]);
}



BLYNK_WRITE(lockbutton) {                       //unlock door button pressed
  if (param.asInt()) {
    unlockDoor();
  }
}

BLYNK_WRITE(bridgedata) { // copy card to lock hardware

  cardHolder[0] = param[0].asStr();
  cardId[0] = param[1].asStr();
  accessFlags[0] = param[2].asInt();

  terminal.println("holder " + cardHolder[0]);
  terminal.println("id " + cardId[0]);
  terminal.print("flags " );
  terminal.println(accessFlags[0]);
  terminal.flush();

  int tempid = findUid(cardId[0]);
  if (!tempid) {
    if (rowIndex < maxcards) {
      storeCard(++rowIndex, cardId[0], cardHolder[0], accessFlags[0]);                                    //put card in memory
      getCard(cardIndex);
      Blynk.virtualWrite(namedisplay, cardHolder[0]);
      Blynk.virtualWrite(cardiddisplay, cardId[0]);
    }
  } else storeCard(tempid, cardId[0], cardHolder[0], accessFlags[0]);    //replace the data
}

BLYNK_WRITE(timeinpin) {
  TimeInputParam t(param);

  if (t.hasStartTime())   // Process start time
  {
    SThour = String(t.getStartHour());
    STmin = String(t.getStartMinute());
  }


  if (t.hasStopTime())    // Process stop time
  {
    SPhour = String(t.getStopHour());
    SPmin = String(t.getStopMinute());
  }

  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)
  days[1] =  t.isWeekdaySelected(0);
  days[2] =  t.isWeekdaySelected(1);
  days[3] =  t.isWeekdaySelected(2);
  days[4] =  t.isWeekdaySelected(3);
  days[5] =  t.isWeekdaySelected(4);
  days[6] =  t.isWeekdaySelected(5);
  days[0] =  t.isWeekdaySelected(6);

  terminal.println(SThour + ":" + STmin);
  terminal.println(SPhour + ":" + SPmin);
  for (int i = 0; i++; i < 7) {
    //    if (days[i]) {
    terminal.print(String(i) + " ");
    //    }
    terminal.println(" ");
  }
  terminal.flush();

  startsecondswd = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
  stopsecondswd = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
}



/**************************************
  remove all cards from valid card list
**************************************/
BLYNK_WRITE(resetallpin)  //reset
{
  rowIndex = 0;
  cardIndex = 0;
  Blynk.virtualWrite(namedisplay, " ");
  Blynk.virtualWrite(cardiddisplay, " ");
  Blynk.virtualWrite(vcount, rowIndex);
  Blynk.virtualWrite(indexdisplay, rowIndex);
  openspiffs();
  write16data(0, rowIndex);                                                 //save the card count in fram at offset 0
  closespiffs();

}
