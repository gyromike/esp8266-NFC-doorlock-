/*
   NFC Door lock
   Auther Michael Burton

   Creative Commons License
   NFC Door lock
   by Michael Dale Burton
   is licensed under a Creative Commons Attribution-NonCommercial
   4.0 International License.

*/
//#define BLYNK_PRINT Serial  // Comment this line out to disable prints and save space 
//#define BLYNK_DEBUG

// only uncoment one at a time
//#define MASTER_LOCK true
//#define LOCK1_LOCK  true
#define LOCK2_LOCK  true
/*
   hardware defines
*/
#define wirespeed 400000L   // Ic2 buss speed
#define serialspeed 115200  // serial port speed
#define nfcretries 0x0f     // number of passive retries before timeout on nfc hardware 

/*
                                PIN MAP for ESP8266
*/
#define DATASTART   0x0           // start address of data buffer
#define IC2_FRAM_ADDRESS 0x50     // address of the fram board
#define SDA_PIN     2             // I2C SDA pin
#define SLC_PIN     14            // I2C SCL pin

#define RTS_PIN     4             // RTS line for C17A - DB9 pin 7
#define DTR_PIN     5             // DTR line for C17A - DB9 pin 4

#define RELAY_PIN   8             // relay output pin
#define LED_PIN     16            // attached LED



/********************************************************************************************

   BLYNK V PIN MAP

 ********************************************************************************************/
#define clockpin        V1
#define resetallpin     V2
#define namedisplay     V3
#define cardiddisplay   V4
#define indexdisplay    V5
#define terminaldisplay V6
#define copybutton      V11
#define copyallbutton   V12
#define bridgewrite     V7
#define lockbutton      V8
#define bridgedata      V9
#define deletebutton    V10
#define eventorpin      V14
#define vcount          V24
#define DATASTORE       V25
#define datapins        V25, V26, V27, V28, V29, V30, V31, V32, V33, V34, V35, V36, V37, V38, V39, V40, V41, V42, V43, V44, V45


#define maxcards      20
#define maxnamelength 20
#define maxuidlength  8
#define flagslength   1
#define recordlength    (4 + maxnamelength + maxuidlength)   // 4 bytes fir the two ints + the max string lengths  for the name and uid

const char*  daystrings[7]  =  {"Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat"};

/* BLYNK tokens

*/
char reader_token[] = "token1";
char lock1_token[] = "token2";
char lock2_token[] = "token3";


#if defined(MASTER_LOCK)

char blynk_token[] = "token1";

#elif defined(LOCK1_LOCK)

char blynk_token[] = "token2";

#else

char blynk_token[] = "token3";

#endif
/*
    WiFi credentials.
*/

char ssid[] = "wifi";
char pass[] = "password";

/*
    Global data
*/
String  cardId[maxcards + 1];
String  cardHolder[maxcards + 1];
int     accessFlags[maxcards + 1];
String  SThour;
String  STmin;
String  SPhour;
String  SPmin;
int     days[7];
int     flag = 0;

int     cardIndex = 0;
int     rowIndex = 0;
String  currentTime;
String  currentDate;
int talktome = 0;

//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "8080";
//char blynk_token[34] = "YOUR_BLYNK_TOKEN";

//flag for saving wifi data
bool shouldSaveConfig = false;

uint32_t versiondata = 0; // NFC hardware version

Adafruit_FRAM_I2C   fram        = Adafruit_FRAM_I2C();
uint16_t            framAddr    = 0;
int                 validfram   = 0;

String carddatafile = "/carddata.txt";
File datafile;



