/*
   NFC Door lock
   Auther Michael Burton

   Creative Commons License
   NFC Door lock
   by Michael Dale Burton
   is licensed under a Creative Commons Attribution-NonCommercial
   4.0 International License.

*/
#define BLYNK_PRINT Serial  // Comment this line out to disable prints and save space 
#define BLYNK_DEBUG
/*
   hardware defines
*/
#define wirespeed 400000L   // Ic2 buss speed
#define serialspeed 115200  // serial port speed
#define nfcretries 0x0f     // number of passive retries before timeout on nfc hardware 

/*
              hardware PIN MAP for ESP8266
*/
#define SDA_PIN     2             // I2C SDA pin
#define SLC_PIN     14            // I2C SCL pin
#define RELAY_PIN   D8            // relay output pin


// screen information
#define screenW  128
#define screenH  64
#define displayCenterX  screenW/2
#define displayCenterY  (screenH/2)  

/********************************************************************************************

   BLYNK V PIN MAP

 ********************************************************************************************/
#define flagpin         V30
#define updatepin       V31
#define clockpin        V1
#define resetallpin     V2
#define namedisplay     V3
#define cardiddisplay   V4
#define indexdisplay    V5
#define terminaldisplay V6
#define copybutton      V11
#define copyallbutton   V12
#define bridgewrite     V25
#define bridgewriteall  V26
#define lockbutton      V8
#define bridgedata      V9
#define deletebutton    V10
#define timeinpin       V14
#define vcount          V24
/*
 * data defines
 */
#define maxcards      50
#define maxnamelength 20
#define maxuidlength  15
#define flagslength   1
#define recordlength    (4 + maxnamelength + maxuidlength)   // 4 bytes for the two ints + the max string lengths  for the name and uid
#define DATASTART   0x0           // start address of data buffer
/* 
 *  wifi data defines
*/
#define BLYNKSIZE 34
#define MODULENAMESIZE 128
/*
    Global data
*/
String  cardId[maxcards + 1];
String  cardHolder[maxcards + 1];
int     accessFlags[maxcards + 1];
int     flag = 0;
int     cardIndex = 0;
int     rowIndex = 0;
String carddatafile = "/carddata.txt";      //data file name for NFC card data
/*
 * Calander constants
 */
const char*  daystrings[7]  =  {"Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat"};
const char*   monthstrings[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
/*
 * wifi data
 */
char reader_token[BLYNKSIZE];
char lock_token[BLYNKSIZE];
char module_name[MODULENAMESIZE+1];
//flag for saving wifi data
bool shouldSaveConfig = true;

/*
 * start and stop times
 */
long startsecondswd;            
long stopsecondswd;
//  times converted to strings
String  SThour;
String  STmin;
String  SPhour;
String  SPmin;
//valid days
int     days[7] = {0,0,0,0,0,0,0};
// current time, date, day as strings
String  currentTime;
String  currentDate;
String  currentMonth;
String  currentDay;

uint32_t versiondata = 0; // NFC hardware version
/*
 * objects
 */
//file object
File datafile;
// NFC reader objects
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);
// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, 2, 14);
//create terminal widget instance and attach it to the pin
WidgetTerminal terminal(terminaldisplay);
//Create the real time clock instance
WidgetRTC rtc;
//create timer instance
BlynkTimer timer;
//create bridge instance and assign pins
WidgetBridge bridge_master(51);
WidgetBridge bridge_lock(52);

