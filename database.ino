/*
   NFC Door lock
   Auther Michael Burton

   Creative Commons License
   NFC Door lock
   by Michael Dale Burton
   is licensed under a Creative Commons Attribution-NonCommercial
   4.0 International License.

*/
/**************************************
  compairUid return bool

  in: card ids

  out: true if they are the same else false
**************************************/

bool compairUid(uint8_t uid1[], uint8_t uid2[], uint8_t j) {
  for (int i = 0; i < j; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }

  return true;
}
/*****************************************************
   findUid
   takes uid and length
   return index of uid in array or 0 if not found

*/
int findUid(String suid) {
  for (int i = 1; i <= rowIndex; i++) {
    if (!(suid.compareTo(String(&cardId[i][0])))) {
      return i; // index of card found
    }

  }

  return 0;
}
/**************************************
  storeCard
  in: row index, card Id, id length, card holder name

  places the uid and the name in the hardware memory at index location ri

  out: none
**************************************/
//put card id  in memory
void storeCard(int ri, String suid2, String holder, int af) {
  if(openspiffs()){
    uint16_t buffer_offset;
    buffer_offset = ri * recordlength;
    cardId[ri] = suid2; // save uid as a string
    cardHolder[ri] = holder;        // save name of card holder
    accessFlags[ri] = af;
    Blynk.virtualWrite(vcount, rowIndex);                                                    //save the card count on the server
    write16data(0, rowIndex);                                                                //save the card count in fram at offset 0

//  Blynk.virtualWrite(DATASTORE + ri, ri, cardHolder[ri], cardId[ri], accessFlags[ri]);     //save card data on server and in fram
    write16data(buffer_offset, ri);
    buffer_offset += 2;
    writestringdata(buffer_offset, maxnamelength, cardHolder[ri]);
    buffer_offset += maxnamelength;
    writestringdata(buffer_offset, maxuidlength, cardId[ri]);
    buffer_offset += maxuidlength;
    write16data(buffer_offset, accessFlags[ri]);
    buffer_offset += 2;
    Blynk.virtualWrite(indexdisplay, ri);
    closespiffs();
  }
}

void getCard(int index) {
  cardId[0] = cardId[index];                      //get cardId
  cardHolder[0] = cardHolder[index];              // get name of card holder
}


/***************************************
   get card id access flags and user name from the cloud

   in:  card Id, card holder name, access flags, index

   datastore is the virtual pin for the card data
**************************************/
void readFromCloud(int count) {
  getdatafromfile();
//  getdatafromfram();
//  for (int i = 0; i < count; i++) {
//    Blynk.syncVirtual(DATASTORE + count);                             //send the sync to the data pin to request card data
//  }
  getCard(1);
  Blynk.virtualWrite(namedisplay, cardHolder[0]);
  Blynk.virtualWrite(cardiddisplay, cardId[0]);
}
