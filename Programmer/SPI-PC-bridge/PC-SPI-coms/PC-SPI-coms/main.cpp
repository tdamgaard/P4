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

#define PATH_TO_ARDUINO "/dev/cu.usbmodem16"









int main(int argc, const char * argv[]) {
    cout << "Hello world!\n";
    FILE *arduinoInd;
    FILE *arduinoUd;
    arduinoInd = fopen(PATH_TO_ARDUINO, "r");
    arduinoUd  = fopen(PATH_TO_ARDUINO, "w");
    
    fflush(arduinoUd);
    fflush(arduinoInd);
    
    sleep(2);
    
    
    for(int i = 0; i < 20; i++){
        while(1){
        fputc(0xAD, arduinoUd);
        int temp = 20;
        temp = fgetc(arduinoInd);
        
        if(temp == 0xAD){
            cout << temp;
        }
            // sleep(1);
        
        
        cout << "\n";
        }
    }
    
    fclose(arduinoInd);
    fclose(arduinoUd);
    return 0;
}
