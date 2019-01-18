/*


void startfram() {
  if (validfram) {
    return;
  } else {
    validfram = fram.begin(IC2_FRAM_ADDRESS);
    if (!validfram) {
      yield();
    } else {         
      terminal.println("fram started: ");
      terminal.flush();
      getdatafromfram();
    }
  }
}

void readrawdata(uint16_t whatbyte, int howlong, char databuffer[]){
    for (int j = 0; j < howlong; j++){
      databuffer[j] = fram.read8(DATASTART+whatbyte+j);
      yield();
    }
}

void writerawdata(uint16_t whatbyte, int howlong, char databuffer[]){
    for (int j = 0; j < howlong; j++){
       fram.write8(DATASTART+whatbyte+j, databuffer[j]);
      yield();
    }
}

void writestringdata(uint16_t whatbyte, int howlong, String databuffer){
   
    for (int j = 0; j < howlong; j++){
       fram.write8(DATASTART+whatbyte+j,  databuffer.charAt(j));
       yield();
    }
}

String readstringdata(uint16_t whatbyte, int howlong){
  char s[256];
    for (int j = 0; j < howlong; j++){
      s[j] = fram.read8(DATASTART+whatbyte+j);
      yield();
    }
    return String(s);
}

uint16_t read16data(uint16_t whatbyte){
  uint8_t high = fram.read8(DATASTART+whatbyte);
  uint8_t low = fram.read8(DATASTART+whatbyte+1);
  uint16_t out;
  out = high << 8;
  out += low;
  return out;
 }

void write16data(uint16_t whatbyte, uint16_t databuffer){
  uint8_t high = highByte(databuffer);
  uint8_t low = lowByte(databuffer);
  fram.write8(DATASTART+whatbyte, high);
  fram.write8(DATASTART+whatbyte+1, low);
 }
 
void getdatafromfram(){
  rowIndex = read16data(0);                                                   //read the card count from fram 
  uint16_t buffer_offset = recordlength;                                    //start at record 1
    Blynk.virtualWrite(vcount, rowIndex);       // write the rowIndex to the server
    
  for (int i = 1; i <= rowIndex; i++){                                             // read all the card data
//    tempindex = read16data(buffer_offset);                                       // skip the index
    buffer_offset += 2;
    cardHolder[i] = readstringdata(buffer_offset, maxnamelength);
    buffer_offset += maxnamelength;
    cardId[i] = readstringdata(buffer_offset, maxuidlength);
    buffer_offset += maxuidlength;
    accessFlags[i] = read16data(buffer_offset);
    buffer_offset += 2;
    yield();
  }
}

void dumpdatafromfram(){
  String ch;
  String cId;
  int af;
  int ri;
  int tempIndex;
  uint16_t buffer_offset = recordlength;                                    //start at record 1


    tempIndex = read16data(0);                                                   //read the card count from fram address 0
  for (int i = 1; i <= tempIndex; i++){                                             // read all the card data
    ri = read16data(buffer_offset);
    buffer_offset += 2;
    ch = readstringdata(buffer_offset, maxnamelength);
    buffer_offset += maxnamelength;
    cId = readstringdata(buffer_offset, maxuidlength);
    buffer_offset += maxuidlength;
    af = read16data(buffer_offset);
    buffer_offset += 2;
    terminal.println("index: " + String(ri) + " holder:  " + ch + " ID:  " + cId + " flags: " + String(af));
    yield();
  }
  terminal.flush();
}


*/

