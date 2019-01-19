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
  // Synchronize time on connection
  rtc.begin();
  bridge1.setAuthToken(reader_token); // Token of reader
  bridge2.setAuthToken(lock1_token);  // Token of lock1
  bridge3.setAuthToken(lock2_token);  // Token of lock2
}

BLYNK_WRITE(vcount) {
  rowIndex =  param.asInt();    //restore the rowIndex
}


BLYNK_READ(vcount)
{
  Blynk.virtualWrite(vcount, rowIndex);       // write the rowIndex to the server
}

// called when the name nameDisplay has had input
BLYNK_WRITE(namedisplay)
{
  String textIn = param.asStr();  // Text Input Widget - Strings
  cardHolder[0] = textIn;   //get the Text Input Widget - String
  if (!findUid(cardId[0]))         // if now card has this id then see if we can add a card
  {
    if (rowIndex < maxcards)      // if the maximum number of cards has not been stored then add a card
    {
      storeCard(++rowIndex, cardId[0], cardHolder[0], accessFlags[0]);                //copy temp data into card memory                                    // display card index on server
    }
  }
}


BLYNK_READ(namedisplay)
{
  Blynk.virtualWrite(namedisplay, cardHolder[0]);   //update the name display on the app
}


BLYNK_WRITE(cardiddisplay)
{
  String textIn = param.asStr();  // Text Input Widget - Strings
  cardId[0] = textIn;           //set the temp data for the card ID
}

/*
   cycle through the cards

*/
BLYNK_WRITE(indexdisplay) {
  int ci = param.asInt();

  if (ci <= rowIndex) {
    cardIndex = ci;
    getCard(cardIndex);
    Blynk.virtualWrite(namedisplay, cardHolder[0]);
    Blynk.virtualWrite(cardiddisplay, cardId[0]);
  } else {
    cardIndex = rowIndex;
  }
  Blynk.virtualWrite(indexdisplay, cardIndex);
}

/*
    this is where the data gets restored
*/
BLYNK_WRITE_DEFAULT()
{
  int pin = request.pin;      // Which pin is handled?

  if ((pin >=  DATASTORE) & (pin <= (DATASTORE + maxcards))) {        // if it is a data pin then  restore the data
    int i = param[0].asInt();
    cardHolder[i] = param[1].asStr();
    cardId[i] = param[2].asStr();
    accessFlags[i] = param[3].asInt();
  } else if (pin = terminaldisplay){

//      dumpdatafromfram();
      dumpdatafromfile();

  }
  
}

/*
   Need to be able to copy only to the selected lock

*/
BLYNK_WRITE(bridgewrite)
{
  int b =  param[0].asInt();
  Serial.print("bridgewrite b ");
  Serial.println(b);
  if (b=1){
    bridge2.virtualWrite(bridgedata, cardHolder[0], cardId[0], accessFlags[0],0);      //copy current card to lock 1
    bridge3.virtualWrite(bridgedata, cardHolder[0], cardId[0], accessFlags[0],0);      //copy current card to lock 2
  }
  if (b=2){
    for (int i = 1; rowIndex >= i; i++){
      getCard(i);
      bridge2.virtualWrite(bridgedata, cardHolder[0], cardId[0], accessFlags[0], (i < rowIndex));      //copy current card to lock 1
      bridge3.virtualWrite(bridgedata, cardHolder[0], cardId[0], accessFlags[0], (i < rowIndex));      //copy current card to lock 2
    }  
  }
}

BLYNK_WRITE(copybutton)
{
 if (param.asInt()){
  talktome = 1;                                 //set talktome to the number of cards to copy
  bridge1.virtualWrite(bridgewrite, 1);         //tell masterlock to copy card to current lock 
 }
}

BLYNK_WRITE(copyallbutton)
{  
  if (param.asInt()){
      talktome = 2;                                //set talktome to copy all cards
      bridge1.virtualWrite(bridgewrite, 2);        //tell masterlock to copy all cards to current lock
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
      write16data(0, rowIndex);                                                                //save the card count in fram at offset 0
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

BLYNK_WRITE(bridgedata) // copy card to lock hardware
{
  if (talktome) {
      cardHolder[0] = param[0].asStr();
      cardId[0] = param[1].asStr();
      accessFlags[0] = param[2].asInt();
      talktome = param[3].asInt();   //clear the flag when the last card is coppied
      terminal.println("holder " + cardHolder[0]);
      terminal.println("id " + cardId[0]);
      terminal.print("flags " );
      terminal.println(accessFlags[0]);     
      terminal.print("talktome ");
      terminal.println(talktome);
      terminal.flush();     
      if (!findUid(cardId[0]))
     {
        if (rowIndex < maxcards)
       {
          storeCard(++rowIndex, cardId[0], cardHolder[0], accessFlags[0]);                                    //put card in memory    
          getCard(cardIndex);
          if (!talktome){
            Blynk.virtualWrite(namedisplay, cardHolder[0]);
            Blynk.virtualWrite(cardiddisplay, cardId[0]);
          }
        }
      }
    }
}

BLYNK_WRITE(eventorpin) {
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
  days[0] =  t.isWeekdaySelected(1);
  days[1] =  t.isWeekdaySelected(2);
  days[2] =  t.isWeekdaySelected(3);
  days[3] =  t.isWeekdaySelected(4);
  days[4] =  t.isWeekdaySelected(5);
  days[5] =  t.isWeekdaySelected(6);
  days[6] =  t.isWeekdaySelected(7);

  terminal.println(SThour + ":" + STmin);
  terminal.println(SPhour + ":" + SPmin);
  for (int i = 0; i++; i < 7) {
//    if (days[i]) {
      terminal.print(String(i) + " ");
//    }
terminal.println(" ");
  }
  terminal.flush();
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
  write16data(0, rowIndex);                                                 //save the card count in fram at offset 0

}
