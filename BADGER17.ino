#include <AddicoreRFID.h>
#include <SPI.h>
#include <RTClib.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Wire.h>
#include <EEPROM.h>
#include "Arduino.h"


//---EEPROM
int USER = 0;



//---RFID
AddicoreRFID myRFID;
const int chipSelectPin = 10;
const int NRSTPD = 9;
#define  uchar unsigned char
#define uint  unsigned int
uchar fifobytes;
uchar fifoValue;
String TAG;
String TAG1;
String TAG2;
String TAG3;
String TAG4;
String ZERO;

//Lunghezza max array
#define MAX_LEN 16


//---RTC
RTC_DS1307 rtc;

//--DISPLAY
#define SSD1306_128_32 //Matrice display
#define OLED_ADDR 0x3C //Indirizzo display
Adafruit_SSD1306 display(-1); //Reset Pin non utilizzato

String TXT;
String TXTone;
String TXTM;
String TXTMone;
int COLOR;
int COLOR2;
String KEY;
String KEY2;
byte trigger;
byte strigger;
byte mtrigger;
byte flag = 1;
byte stopp;
String INGR;
bool sign1;
bool sign2;
bool sign3;
bool sign4;
bool sign5;
bool sign6;
bool sign7;
bool sign8;
bool sign9;
bool sign10;
String NDAY;
String NMON;
String NDAYM;
String NMONM;
int DAYW;
String DAY;
String CH2;
String CH;
String CH3;
String CHHR;
String CHMN;
String CHHRt;
String CHMNt;
String NYR;
byte reception;
String TXTH;

//---------------TOUCH
int sVal;

int count =0;

void setup() {
  pinMode(7,OUTPUT);
  digitalWrite(7,HIGH);
  Serial.begin(9600);
  Wire.begin();
  SPI.begin();
  pinMode(chipSelectPin, OUTPUT);             // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin
  digitalWrite(chipSelectPin, LOW);         // Activate the RFID reader
  pinMode(NRSTPD, OUTPUT);                    // Set digital pin 10 , Not Reset and Power-down
  digitalWrite(NRSTPD, HIGH);
  pinMode(4, INPUT);
  myRFID.AddicoreRFID_Init();
  //pinMode(buttonPin, INPUT);
  //pinMode(potPin, INPUT);
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setFont(&FreeSerif12pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15, 20);
  display.print("badger");
  display.display();
  display.setFont();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(90, 14);
  display.print("v1.7"); //VERSIONE
  display.display();
  delay(5000);
  display.setFont();
  trigger = 4;
  stopp = 2;
  strigger = 11;
  sign1 = EEPROM.read(500);
  sign2 = EEPROM.read(499);
  sign3 = EEPROM.read(498);
  sign4 = EEPROM.read(497);
  sign5 = EEPROM.read(496);
  sign6 = EEPROM.read(495);
  sign7 = EEPROM.read(494);
  sign8 = EEPROM.read(493);
  sign9 = EEPROM.read(492);
}

/*void sleep(){
  Serial.println("Sleeping");
  digitalWrite(7,LOW); 
  KEY = " BUFFERING";
  KEY2 = "";
  COLOR = 1;
  COLOR2 = 0;
  displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
  delay(1000);
}*/

void displ(String TXT, String TXTM, String KEY, int COLOR, String KEY2, int COLOR2) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(COLOR);
  display.setCursor(35 , 16);
  display.print(TXT + ":" + TXTM);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(27, 3);
  display.print(KEY);
  display.display();
  display.setFont(&FreeSans9pt7b);
  display.setTextSize(1);
  display.setTextColor(COLOR2);
  display.setCursor(25, 28);
  display.print(KEY2);
  display.display();
  display.setFont();
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------REGISTRAZIONE GIORNALIERA
void reg(int USER, String INGR, int DAYW, String DAY, String TXT, String TXTM, String NDAYM, String NMONM) {
    Serial.println(INGR);
    int INGRc;
    int TXTi = TXT.toInt();
    EEPROM.write(USER * USER + 1, TXTi);
    int TXTMi = TXTM.toInt();
    EEPROM.write(USER * USER + 2, TXTMi);
    if (INGR == "EN ") {
      INGRc = 1;
      EEPROM.write(USER * USER + 3, INGRc);
    }
    if (INGR == "US ") {
      INGRc = 2;
      EEPROM.write(USER * USER + 3, INGRc);
    }
    String USR = String(USER);
    String US1 = USR + "/" + TXT + ":" + TXTM + "/" + DAY + "/" + INGR;
    Serial.println(US1);
    if (INGR == "EN ") {
      String US1BYTE = USR + TXT + TXTM + NDAYM + NMONM + "1" + "/";
      char* buf = (char*) malloc(sizeof(char) * US1BYTE.length() + 1);
      US1BYTE.toCharArray(buf, US1BYTE.length() + 1);
      Serial.println(buf);
      Wire.beginTransmission(8);
      Wire.write(buf);
      Wire.endTransmission();
      free(buf);

    }
    if (INGR == "US ") {
      String US1BYTE = USR + TXT + TXTM + NDAYM + NMONM + "2" + "/";
      char* buf = (char*) malloc(sizeof(char) * US1BYTE.length() + 1);
      US1BYTE.toCharArray(buf, US1BYTE.length() + 1);
      Serial.println(buf);
      Wire.beginTransmission(8);
      Wire.write(buf);
      Wire.endTransmission();
      free(buf);
    }

}

void handleChr() {
  if (EEPROM.read(USER * USER + 1) < 10) {
    CHHRt = String(EEPROM.read(USER * USER + 1));
    CHHR = "0";
    CHHR += CHHRt;
  } else {
    CHHR = String(EEPROM.read(USER * USER + 1));
  }
  if (EEPROM.read(USER * USER + 2) < 10) {
    CHMNt = String(EEPROM.read(USER * USER + 2));
    CHMN = "0";
    CHMN += CHMNt;
  } else {
    CHMN = String(EEPROM.read(USER * USER + 2));
  }
  CH = CHHR + ":" + CHMN + "/" + String(EEPROM.read(USER * USER + 3));
  CH3 = CHHR + ":" + CHMN;
  if (CH.indexOf("/1") > 0) {
    CH2 = " E";
    CH += CH2;
  }
  if (CH.indexOf("/2") > 0) {
    CH2 = " U";
    CH += CH2;
  }
  KEY = "  ULT. MOV:";
  KEY2 = "  " + CH3 + CH2;
  COLOR = 0;
  COLOR2 = 1;
  trigger = 4;
  strigger = 11;
  stopp = 2;
}


void handleUser1() {
  if (sign1 == false && sVal == 0) {
    INGR = "EN ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sign1 == true && sVal == 0) {
    INGR = "US ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sVal == 1) {
    handleChr();
  }
}

void handleUser2() {
  if (sign2 == false && sVal == 0) {
    INGR = "EN ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sign2 == true && sVal == 0) {
    INGR = "US ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sVal == 1) {
    handleChr();
  }
}
void handleUser3() {
  if (sign3 == false && sVal == 0) {
    INGR = "EN ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sign3 == true && sVal == 0) {
    INGR = "US ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sVal == 1) {
    handleChr();
  }
}

void handleUser4() {
  if (sign4 == false && sVal == 0) {
    INGR = "EN ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sign4 == true && sVal == 0) {
    INGR = "US ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sVal == 1) {
    handleChr();
  }
}

void handleUser5() {
  if (sign5 == false && sVal == 0) {
    INGR = "EN ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sign5 == true && sVal == 0) {
    INGR = "US ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sVal == 1) {
    handleChr();
  }
}


void handleUser6() {
  if (sign6 == false && sVal == 0) {
    INGR = "EN ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sign6 == true && sVal == 0) {
    INGR = "US ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sVal == 1) {
    handleChr();
  }
}

void handleUser7() {
  if (sign7 == false && sVal == 0) {
    INGR = "EN ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sign7 == true && sVal == 0) {
    INGR = "US ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sVal == 1) {
    handleChr();
  }
}

void handleUser8() {
  if (sign8 == false && sVal == 0) {
    INGR = "EN ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sign8 == true && sVal == 0) {
    INGR = "US ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sVal == 1) {
    handleChr();
  }
}

void handleUser9() {
  if (sign9 == false && sVal == 0) {
    INGR = "EN ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sign9 == true && sVal == 0) {
    INGR = "US ";
    KEY = "  " + TAG;
    KEY2 = INGR + TXT + ":" + TXTM;
    COLOR = 0;
    COLOR2 = 1;
    trigger = 4;
    strigger = 11;
    reg(USER, INGR, DAYW, DAY, TXT, TXTM, NDAYM, NMONM);
    stopp = 2;
  }
  if (sVal == 1) {
    handleChr();
  }
}

void getTouch () {
  sVal = digitalRead(4);
}

void getRFID(String &TAG) {
  uchar i, tmp, checksum1;
  uchar status;
  uchar str[MAX_LEN];
  uchar RC_size;
  uchar blockAddr;  //Selection operation block address 0 to 63
  String mynum = "";
  str[1] = 0x4400;
  status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);  status = myRFID.AddicoreRFID_Anticoll(str);

  if (status == MI_OK)
  {
    checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
    TAG1 = String(str[0]);
    TAG2 = String(str[1]);
    TAG3 = String(str[2]);
    TAG4 = String(str[3]);
    TAG = TAG1 + TAG2 + TAG3 + TAG4;
    int LENT = TAG.length();
    int DIFF = LENT - 12;
    if (LENT >= 9) { //Trim 8 cifre
      TAG.remove(8);
      LENT = TAG.length();
    }
    Serial.println(TAG);
    trigger = 4; //trigger per menu
    mtrigger = 1;
    stopp = 1;
    delay(1000);
  }
}

void getClock() {
  DateTime now = rtc.now();
  DAY = String(now.day()) + "/" + String(now.month());
  DAYW = now.dayOfTheWeek();
  TXT = String(now.hour());
  NMONM=String(now.month());
  if (NMONM.toInt()>=4 && NMON.toInt()<=9){
      TXTH=String(TXT.toInt()+1);
    }
  if (NMONM=="3" && NDAY <= "25"){
      TXTH=String(TXT.toInt()+1);
    }
  if (NMONM=="10" && NDAY >= "28"){
      TXTH=String(TXT.toInt()+1);
    }
  
  if (TXTH=="023"){
    TXTH="00";
  }
  if (TXTH=="024"){
    TXTH="01";
  }
  if (TXTH=="009"){
    TXTH="09";
  }

  
  if (TXTM.toInt() <= 9) {
    TXTMone = String(now.minute());
    TXTM = "0";
    TXTM += TXTMone;
  } else {
    TXTM = String(now.minute());
  }
  if (TXT.toInt() <= 8) {
    TXT = "0";
    TXT += TXTH;
  } else {TXT = TXTH;}
  if (now.day() <= 9) {
    NDAY = String(now.day());
    NDAYM = "0";
    NDAYM += NDAY;
  } else {
    NDAYM = String(now.day());
  }
  if (now.month() <= 9) {
    NMON = String(now.month());
    NMONM = "0";
    NMONM += NMON;
  } else {
    NMONM = String(now.month());
  }
}


/*  void receive(){
    if(reception == 1){
     KEY = " BUFFER OK";
     KEY2 = "";
     COLOR = 1;
     COLOR2 = 0;
     displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
     }
  }*/


//-------------------------------------------LOOP-----------------------------------------------------------------------------------LOOP-------------------------------------------

void loop() {
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  getRFID(TAG);
  getTouch();
  getClock();
  /*count+=1;
  if (count >= 100 && TXTH == "22" || count >= 100 && TXTH == "01" || count >= 100 && TXTH == "06" || count >= 100 && TXTH == "13"){
    count=0;
    Serial.println("RFID Sleeping");
    sleep();
    digitalWrite(chipSelectPin, LOW);         // Activate the RFID reader
    digitalWrite(NRSTPD, HIGH);
    myRFID.AddicoreRFID_Init();
  }*/
  
 // Wire.onReceive(receive);
  
  if (TAG == "24527250" && stopp == 1) {
    USER = 1;
    handleUser1();
    if (sVal != 1) {
      sign1 = !sign1;
      EEPROM.write(500, sign1);
    }
    trigger = 4;
    strigger = 11;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
    delay(1000);
  }
  if (TAG == "16314654" && stopp == 1) {
    USER = 2;
    handleUser2();
    if (sVal != 1) {
      sign2 = !sign2;
      EEPROM.write(499, sign2);
    }
    trigger = 4;
    strigger = 11;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
    delay(1000);
  }
  if (TAG == "99190159" && stopp == 1) {
    USER = 3;
    handleUser3();
    if (sVal != 1) {
      sign3 = !sign3;
      EEPROM.write(498, sign3);
    }
    trigger = 4;
    strigger = 11;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
    delay(1000);
  }
  if (TAG == "22720316" && stopp == 1) {
    USER = 4;
    handleUser4();
    if (sVal != 1) {
      sign4 = !sign4;
      EEPROM.write(497, sign4);
    }
    trigger = 4;
    strigger = 11;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
    delay(1000);
  }
  if (TAG == "13122623" && stopp == 1) {
    USER = 5;
    handleUser5();
    if (sVal != 1) {
      sign5 = !sign5;
      EEPROM.write(496, sign5);
    }
    trigger = 4;
    strigger = 11;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
    delay(1000);
  }
  if (TAG == "21127336" && stopp == 1) {
    USER = 6;
    handleUser6();
    if (sVal != 1) {
      sign6 = !sign6;
      EEPROM.write(495, sign6);
    }
    trigger = 4;
    strigger = 11;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
    delay(1000);
  }
  if (TAG == "70149200" && stopp == 1) {
    USER = 7;
    handleUser7();
    if (sVal != 1) {
      sign7 = !sign7;
      EEPROM.write(494, sign7);
    }
    trigger = 4;
    strigger = 11;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
    delay(1000);
  }
  if (TAG == "35152144" && stopp == 1) {
    USER = 8;
    handleUser8();
    if (sVal != 1) {
      sign8 = !sign8;
      EEPROM.write(493, sign8);
    }
    trigger = 4;
    strigger = 11;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
    delay(1000);
  }
  if (TAG == "11594147" && stopp == 1) {
    USER = 9;
    handleUser9();
    if (sVal != 1) {
      sign9 = !sign9;
      EEPROM.write(492, sign9);
    }
    trigger = 4;
    strigger = 11;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
    delay(1000);
  }
  if (trigger == 4 && strigger == 11 && stopp == 2) {
    KEY = "  DPG VIGG.";
    KEY2 = "";
    COLOR = 1;
    COLOR2 = 0;
    displ(TXT, TXTM, KEY, COLOR, KEY2, COLOR2);
  }


  //Serial.println(TXTH);
  myRFID.AddicoreRFID_Halt();
  //11888224
}
