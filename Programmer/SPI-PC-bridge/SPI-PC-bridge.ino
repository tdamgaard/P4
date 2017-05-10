#include "AudioSamples.h"
// Sikrer lige det rigtige board er valgt
#if !defined(ARDUINO_SAM_DUE)
  #error Forkert board, brug Arduino Due!
#endif

#define DEBUG 1

#if DEBUG == 1
  #define DUE_CLOCK   10
  #define DUE_MISO    12
  #define DUE_MOSI    11
  #define DUE_SS      13
  #define DUE_BP       9
#else 
  #define DUE_CLOCK   13
  #define DUE_MISO    12
  #define DUE_MOSI    11
  #define DUE_SS      10
  #define DUE_BP       9
#endif

#define ERROR_MSG       0xEE
#define OK_MSG          0x10
#define CMD_MSG         0x11
#define SS_HIGH_MSG     0x12
#define SS_LOW_MSG      0x13
#define LOAD_DATA_MSG   0x14
#define LOAD_ADD_MSG    0x15
#define WRITE_DATA_MSG  0x16
#define SAMPLE_NR_MSG   0x17

#define SAMPLE_LENGTH   16



bool instruks = false;
byte sampleNr = 0; // DEC

byte arrayToWriteToSPI[SAMPLE_LENGTH];



/*
     Navn                   Formål                Kommando  
 ------------ ---------------------------------- ---------- 
  ERROR         Fejl i coms                            0xEE         
  OK            Ping-pong OK                           0x10         
  Instruks      Sende kommando til arduino             0x11         
  SS High       Sætte SS høj                           0x12         
  SS Low        Sætte SS lav                           0x13         
  Load data     Overføre 256 byte til arduino'en       0x14         
  Load adress   Giv arduino en adresse at skrive til   0x15         
  Write data    Skriv data via. SPI                    0x16         
  Sample nr.    Overfør sample nr.                     0x17         
   
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(DUE_CLOCK,  OUTPUT);
  pinMode(DUE_MISO,   INPUT_PULLUP); 
  pinMode(DUE_MOSI,   OUTPUT);
  pinMode(DUE_SS,     OUTPUT);
  pinMode(DUE_BP,     OUTPUT);
  pinMode(2, OUTPUT);
  
  // Serial.write(0xFF);
  digitalWrite(DUE_CLOCK, LOW);
  digitalWrite(DUE_MISO, LOW);
  digitalWrite(DUE_MOSI, LOW);
  digitalWrite(DUE_SS, LOW);
  digitalWrite(DUE_BP, LOW);

  highSS();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){
    // highSS();
    byte temp = Serial.read();
    /*
      lowSS();
      transmitOneByteSPI(temp);
      highSS();
    */
    #if DEBUG
      // Serial.write(temp);
    #endif
    
    switch(temp){
      case OK_MSG:
        // OK
        break;

      case CMD_MSG:
        // Instruks
        instruks = true;
        sendOK();
        break;

      case SS_HIGH_MSG:
        // SS High
        highSS();
        instruks = false;
        sendOK();
        break;

      case SS_LOW_MSG:
        // SS Low
        lowSS();
        instruks = false;
        sendOK();
        break;

      case LOAD_DATA_MSG:
        // Load data
        if(instruks){
          getDataFromPC();
        }
        sendOK();
        instruks = false;
        break;

      case LOAD_ADD_MSG:
        // Load address
        break;

        
      case WRITE_DATA_MSG:
        // Write data
        break;

      case SAMPLE_NR_MSG:
        // Update sample nr
        if(instruks){
          getSampleNr();
        }
        instruks = false;
        break;

      default:
        // highSS();
        Serial.write(ERROR_MSG);
        break;  
        
    }




    
  } else {
    // highSS();
    Serial.flush();
  }
}

void getDataFromPC(){
  
  int i = 0;
  while(!Serial.available());
  for(int i = 0; i < 256; i++){
    while(!Serial.available());
    arrayToWriteToSPI[i] = Serial.read();
  }
  delay(200);
  digitalWrite(2, HIGH);
  Serial.write(0xBB);
  for(int i = 0; i < SAMPLE_LENGTH; i++){
    Serial.write(arrayToWriteToSPI[i]);
    
  }
  digitalWrite(2, LOW);
  delay(1000);
}
void getSampleNr(){
  while(!Serial.available());

  sampleNr = Serial.read();
  Serial.write(sampleNr);
}


/*
void getSampleNr(){
  // Serial.flush();
  
  byte waitingForCmd = 0x00;
  byte commandFromPC = 0x00;
  
  bool transmissionOK = false;
  do{
    // Venter på der kommer data
    while(Serial.available() == 0);
    
    // får en kommando
    do{
      waitingForCmd = Serial.read();
    }while(waitingForCmd != CMD_MSG);

    while(!Serial.available());
    // efterfulgt af sample-nr
    sampleNr = Serial.read();
    // Verificerer data er OK
      // Skriver samplenr til PC
      Serial.write(sampleNr);
  
      // Venter på PC svarer
      while(!Serial.available());
      // Læser svar
      commandFromPC = Serial.read();
  
      // Tjekker om svaret er OK
      if(commandFromPC == OK_MSG){
        sendOK();
        transmissionOK = true;
      } else {
        sendError();
        sampleNr = 0;
        transmissionOK = false;
      }
    
  } while (!transmissionOK); 
}
*/




void sendError(){
  Serial.write(ERROR_MSG);
}

void sendOK(){
  Serial.write(OK_MSG);  
}


byte readOneByteSPI(){
  /*
   * Denne funktion læser én byte fra SPI
   */
   
  byte tempInputData = 0x00;
  
  highMosi();
  
  #if defined(ARDUINO_AVR_UNO)
    
    for(int k = 7; k >= 0; k--){
      highClock();
      // Hvis data in er HIGH efter falling-edge clock
      if(bitRead(PINB, 4)){
        bitSet(tempInputData, k);  // Sæt den pågældende bit high
      } 
      lowClock();
    }// for
    
  #elif defined(ARDUINO_SAM_DUE)
    
    for(int k = 7; k >= 0; k--){
      highClock();
      if(digitalRead(DUE_MISO)){
        bitSet(tempInputData, k);
      }
      lowClock();
    }
    
  #endif
  
  lowMosi();
  return tempInputData;
}

void transmitOneByteSPI(byte data){
  /*
   * Sender én byte via. SPI
   */
  
  // DDRB = DDRB|B00101111; // Set as output - Overflødig
  for(int i = 7; i >= 0; i--){
    if(bitRead(data,i) == 1){
      highMosi();
    } else {
      lowMosi();
    }
    cycleClock();   
  }// for
 
}






/*
#################################################################################################
  ____                _____   _____    _____         ______   _    _   _   _    _____     
 |  _ \      /\      / ____| |_   _|  / ____|       |  ____| | |  | | | \ | |  / ____|    
 | |_) |    /  \    | (___     | |   | |            | |__    | |  | | |  \| | | |         
 |  _ <    / /\ \    \___ \    | |   | |            |  __|   | |  | | | . ` | | |         
 | |_) |  / ____ \   ____) |  _| |_  | |____        | |      | |__| | | |\  | | |____   _ 
 |____/  /_/    \_\ |_____/  |_____|  \_____|       |_|       \____/  |_| \_|  \_____| (_)
#################################################################################################
*/
/* PINOUT
#define DUE_CLOCK   13
#define DUE_MISO    12
#define DUE_MOSI    11
#define DUE_SS      10
 */

void pulseBreakPin(){
  digitalWrite(DUE_BP,   HIGH);
  delayMicroseconds(1);
  digitalWrite(DUE_BP, LOW);
}

void cycleSS(){
  highSS();
  lowSS();
}

void cycleClock(){
  highClock();
  delayMicroseconds(1); // tCH: Min. 9 ns
  lowClock();
  delayMicroseconds(1); // tCL: Min. 9 ns
}

void lowMosi(){
  digitalWrite(DUE_MOSI, LOW);
}

void highMosi(){
  digitalWrite(DUE_MOSI, HIGH);
}

void lowSS(){
  digitalWrite(DUE_SS, LOW);
}

void highSS(){
  digitalWrite(DUE_SS, HIGH);
}

void highClock(){
  digitalWrite(DUE_CLOCK, HIGH);
}

void lowClock(){
  digitalWrite(DUE_CLOCK, LOW);
}
