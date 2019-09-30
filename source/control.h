// Shiwei Sun, Jose Manila, Michael Wong 
// Includes needed to run the program
#include <unistd.h>
#include "GPIO_INIT.h"
#include <wiringPi.h>
#include <stdio.h>

// Declaration of functions and some variables
unsigned int *gpio;
void Init_GPIO(unsigned linenum, unsigned fcode);
void Write_Latch(int output);
void Write_Clock(int output);
int Read_Data();
void Read_SNES();
int tempvalue;
int value[16]; //Declare an array to store data from controller.


/**
 * Take the line number and function codes as two parameters.
 * It will initialize the GPIO line but does not return anything.
 * */
void Init_GPIO(unsigned int linenum, unsigned int fcode){
	gpio[linenum/10] = (gpio[linenum/10] & ~(0b111<<(linenum-(linenum/10)*10)*3)) | ((fcode) << (linenum-(linenum/10)*10)*3) ;
}
/**
 *Write a bit to the SNES latch line.
 * The parameter output indicate whether it will do set or clear.
 **/
void Write_Latch(int output){
    if(output){
        gpio[7] = 1<<9; //SET
    }
    else{
        gpio[10] = 1<<9;    //CLEAR
    }
    return;
}
/**
 *Write a bit to the SNES clock line.
 * The parameter output indicate whether it will do set or clear.
 **/
void Write_Clock(int output){
    if(output){
        gpio[7] = 1<<11; //SET
    }
    else{
        gpio[10] = 1<<11;    //CLEAR
    }
    return;
}
/**
 * Read a bit from SNES data line.
 * It will return the value of data line.
 **/
int Read_Data(){
    return ((gpio[13] >> 10) & 1);
}
/**
 *Main SNES function that read input from a SNES controller.
 *It will return the codes of a pressed button.
 **/
void Read_SNES(){
    
    
    Write_Latch(1);//Write latch line to indicate that we start to read data from controller.
    delayMicroseconds(12);//Delay for 12 us, then we can read the first button B later.
    Write_Latch(0);//Drop latch to zero.
    Write_Clock(1);// Start clock pulse, after this, we can read buttons
    for(int i = 0;i<16;i++){
        delayMicroseconds(6); // Delay for 6um
        value[i] = Read_Data(); //then we read buttons.
        Write_Clock(0);// The falling edge in one clock cycle.
        delayMicroseconds(6); //delay for another 6um.
        Write_Clock(1);// The rising edge in one clock cycle.
    }
}
