#define DUE_CLOCK   13
#define DUE_MISO    12
#define DUE_MOSI    11
#define DUE_SS      10
#define DUE_BP       9

// Sikrer lige det rigtige board er valgt
#if !defined(ARDUINO_SAM_DUE)
  #error Forkert board, brug Arduino Due!
#endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(DUE_CLOCK,  OUTPUT);
  pinMode(DUE_MISO,   INPUT_PULLUP); 
  pinMode(DUE_MOSI,   OUTPUT);
  pinMode(DUE_SS,     OUTPUT);
  pinMode(DUE_BP,     OUTPUT);
  // Serial.write(0xFF);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){
    byte temp = Serial.read();
    lowSS();
    transmitOneByteSPI(temp);
    highSS();
    Serial.write(temp);
  } else {
    // Serial.flush();
  } delay(1);
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
