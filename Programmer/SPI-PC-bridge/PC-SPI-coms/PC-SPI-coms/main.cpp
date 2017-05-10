//
//  main.cpp
//  PC-SPI-coms
//
//  Created by Mathias Lyngklip Kjeldgaard on 05/05/2017.
//  Copyright © 2017 Mathias Lyngklip Kjeldgaard. All rights reserved.
//


#include <unistd.h> // sleep()
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h> // exit() and EXIT_FAILURE
#include <iostream>
using namespace std;

#define PATH_TO_ARDUINO "/dev/cu.usbmodem1411"


#define ERROR_MSG       0xEE
#define OK_MSG          0x10
#define CMD_MSG         0x11
#define SS_HIGH_MSG     0x12
#define SS_LOW_MSG      0x13
#define LOAD_DATA_MSG   0x14
#define LOAD_ADD_MSG    0x15
#define WRITE_DATA_MSG  0x16
#define SAMPLE_NR_MSG   0x17






/*
 ╔═════════════╦══════════════════════════════════════╦══════════╗
 ║    Navn     ║                Formål                ║ Kommando ║
 ╠═════════════╬══════════════════════════════════════╬══════════╣
 ║ ERROR       ║ Fejl i coms                          ║ 0xEE     ║
 ║ OK          ║ Ping-pong OK                         ║ 0x10     ║
 ║ Instruks    ║ Sende kommando til arduino           ║ 0x11     ║
 ║ SS High     ║ Sætte SS høj                         ║ 0x12     ║
 ║ SS Low      ║ Sætte SS lav                         ║ 0x13     ║
 ║ Load data   ║ Overføre 256 byte til arduino'en     ║ 0x14     ║
 ║ Load adress ║ Giv arduino en adresse at skrive til ║ 0x15     ║
 ║ Write data  ║ Skriv data via. SPI                  ║ 0x16     ║
 ╚═════════════╩══════════════════════════════════════╩══════════╝
*/


void turnLEDOff();
void turnLEDOn();
void putSampleNr(int sampleNr);
void writeToArduino(uint8_t data);
uint8_t readFromArduino();





unsigned int delayTime = 0050000;
FILE *arduinoInd;
FILE *arduinoUd;

int main(int argc, const char * argv[]) {
    
    /*
     * HELE MAIN:
     *
     * 1. Adresse
     * 2. 256 byte data
     *     Gentag til done
     * 3. Verificer data
     * 4. Giv SPI-WRITE kommando
     */
    
    
    
    
    
    
    
    arduinoInd = fopen(PATH_TO_ARDUINO, "r");
    arduinoUd  = fopen(PATH_TO_ARDUINO, "w");
    
    fflush(arduinoUd);
    fflush(arduinoInd);
    
    sleep(2);
    /*
    for(uint8_t i = 0; i < 10; i++){
        // cout << "Loop\n";
        // turnLEDOn();
        // usleep(delayTime);
        
        // turnLEDOff();
        // usleep(delayTime);
        // fflush(arduinoUd);
        // fflush(arduinoInd);
        
    }
    */
    // putSampleNr(1);
    
    while(1){
        writeToArduino(0xAA);
    }
    /*
    while(1){
        cout << hex << uppercase << fgetc(arduinoInd) << "\n";
    }
    */
    fclose(arduinoInd);
    fclose(arduinoUd);
    return 0;
}








void putSampleNr(int sampleNr){
    bool done = false;
    writeToArduino(CMD_MSG);
    writeToArduino(SAMPLE_NR_MSG);
    do{
        writeToArduino(CMD_MSG);
        writeToArduino(sampleNr);
        if(readFromArduino() == sampleNr){
            done = true;
        } else {
            
        }
        usleep(delayTime);
    }while (!done);
    
}

uint8_t readFromArduino(){
    uint8_t readDataFromArduino = 0x00;
    
    readDataFromArduino = (int)fgetc(arduinoInd);
    
    return readDataFromArduino;
}

void writeToArduino(uint8_t data){
    // arduinoUd  = fopen(PATH_TO_ARDUINO, "w");
    
    fputc(data, arduinoUd);
    
    // fclose(arduinoUd);
}

void turnLEDOff(){
    cout << "\tStuff\n";
    uint8_t temp = 0x00;
    do{
        writeToArduino(0x11);
        writeToArduino(0x13);
        // fprintf(arduinoUd, &temp);
        // fprintf(arduinoUd, &temp2);
        /*
        fputc(0x00, arduinoUd);
        fputc(0x11, arduinoUd);
        //fprintf(arduinoUd, 0x11);
        char temp = 0x11;
        */
    
    
        
        temp = fgetc(arduinoInd);
        cout << hex << uppercase << (int)temp << "\n";
    } while (temp != 0x10);
    
}

void turnLEDOn(){
    cout << "\tOther Stuff\n";
    char temp = 0x00;
    do{
        writeToArduino(0x11);
        writeToArduino(0x12);
        
        
        
        temp = fgetc(arduinoInd);
        //cout << hex << temp;
    } while (temp != 0x10);
    
}

