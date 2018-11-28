#include <Wire.h>
#include<SPI.h>
#include<SD.h>


//---MODULO SD
Sd2Card card;
SdVolume volume;
SdFile root;
File myFile;

//---BUZZER
const int buzzer=6;

const int chipSelect = 7;
char result[11];
char allResult[142]={0};
String Result;
String oldResult = "a";
String AR;
String USERS;
String USERS2;
String HOURS;
String MINUTES;
String DAYS;
String MONTHS;
String ENTRIES;
String TOT;
String ECC;



void setup() {
   delay(5200);
   Serial.begin(9600); 
   Wire.begin(8);
   pinMode(buzzer,OUTPUT);
  /* while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
}*/
 SD.begin(chipSelect);
 if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("ERRORE 01");
    Serial.println("* Verificare che la MicroSD sia correttamente inserita.");
    Serial.println("* Controllare il cablaggio.");
    Serial.println("* Controllare eventuali modifiche al firmware.");
    return;
  } else {
    Serial.println("MicroSD rilevata.");
  }
}
  

void loop() {
 Wire.onReceive(receiveEvent);
 if(Result.length()>0&&Result!=oldResult) {
  Serial.println(Result);
  oldResult=Result;
  if(strlen(allResult)==11){
  Serial.println(allResult);
  String AR = String (allResult);
  memset(allResult,0,sizeof(allResult));
  manageAR(AR);
  tone(buzzer, 2500,1000);
    }
 }
 delay(100);
  }
  
void manageAR(String AR) {
  //String USERS = String(AR.substring(0,1))+String(AR.substring(9,10))+String(AR.substring(18,19))+String(AR.substring(27,28))+String(AR.substring(36,37))+String(AR.substring(45,46))+String(AR.substring(54,55))+String(AR.substring(63,64))+String(AR.substring(72,73))+String(AR.substring(81,82));
  //Serial.println(USERS);
  int i = 0;
  while (i<=AR.length()) {
    if (i<=AR.length()){
    i += 11;
    int count = i;
    USERS2 = String(AR.substring(count-11,count-10));
    HOURS = String(AR.substring(count-10,count-8));
    MINUTES = String(AR.substring(count-8,count-6));
    DAYS = String(AR.substring(count-6,count-4));
    MONTHS = String(AR.substring(count-4,count-2));
    ENTRIES = String(AR.substring(count-2,count-1));
    if (ENTRIES=="1"){
      int ECCh = HOURS.toInt()*60;
      int ECCm = MINUTES.toInt();
      int TOTm = ECCh+ECCm;
      ECC = String(460-TOTm);
    }
     if (ENTRIES=="2"){
      int ECCh = HOURS.toInt()*60;
      int ECCm = MINUTES.toInt();
      int TOTm = ECCh+ECCm;
      ECC = String(TOTm-990);
  }
    if (USERS2=="1"){ //----------------------------------------------SETACCIO UTENTI
      USERS2="UTENTE1";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+"\n";
    }
    if (USERS2=="2"){
      USERS2="UTENTE2";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+"\n";
    }
    if (USERS2=="3"){
      USERS2="UTENTE3";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+"\n";
    }
    if (USERS2=="4"){ 
      USERS2="UTENTE4";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+"\n";
    }
    if (USERS2=="5"){
      USERS2="UTENTE5";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+"\n";
    }
    if (USERS2=="6"){ 
      USERS2="UTENTE6";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+"\n";
    }
    if (USERS2=="7"){
      USERS2="UTENTE7";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+"\n";
    }
    if (USERS2=="8"){ 
      USERS2="UTENTE8";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+"\n";
    }
    if (USERS2=="9"){
      USERS2="UTENTE9";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+"\n"; 
    }
    if (ENTRIES=="1"){ //-------------------------------------------SETACCIO INGRESSI
      ENTRIES="ENTRATA";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+" Eccedenza: "+ECC+"\n"; 
    }
    if (ENTRIES=="2") {
      ENTRIES="USCITA";
      TOT = USERS2+" "+HOURS+":"+MINUTES+" "+DAYS+"/"+MONTHS+" "+ENTRIES+" Eccedenza: "+ECC+"\n";      
    }
   Serial.println(TOT);
   myFile = SD.open("Elenco.txt", FILE_WRITE);
   if (myFile) {
    myFile.println(TOT);
    myFile.close();
    Serial.println("File scritto");
   }
  }
  if (i>>AR.length()){
    i=0;
  }
 
}
}

void receiveEvent (int howMany) {
    process(result);
    strcat(allResult,result);
    Result = String(result);
    Result.remove(11);
    SD.begin(chipSelect);
 if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("ERRORE 01");
    Serial.println("* Verificare che la MicroSD sia correttamente inserita.");
    Serial.println("* Controllare il cablaggio.");
    Serial.println("* Controllare eventuali modifiche al firmware.");
    return;
  } else {
    Serial.println("MicroSD rilevata.");
  }
    //Serial.println(allResult);
    /*if(AR.length()==90){
     Serial.println(AR);
     memset(allResult,0,sizeof(allResult));
    }
    //Serial.println(Result); in loop*/
}

int process(char result[]){
  int i = 0;
  while(Wire.available()>0) {
    char c = Wire.read();
    if(i!=11){
      result[i] = c;
      i++;
    }
    if(i>>11){
     result[i]='\0';
     Serial.flush();
     return 0;
    }
  }
}
