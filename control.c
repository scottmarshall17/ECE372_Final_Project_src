#include <xc.h>
#include "constants.h"
#include "control.h"

void turnRight(void){
    //If we want the robot to turn right, we will stop the left tire, 
    //and have the right tire slow down until it has turned a sufficient amount
    LATDbits.LATD5 = ENABLED;                  //#Assuming that D5 moves it backward when set high 
    LATDbits.LATD11 = ENABLED;                 //#Assuming that D11 moves it backward when set high 
    LATGbits.LATG13 = DISABLED;                  //#Assuming that G13 moves it forward when set high 
    LATGbits.LATG15 = DISABLED;                  //#Assuming that G15 moves it forward when set high 
    OC3RS = 810;
    OC4RS = 870;
}

void turnLeft(void){
    //If we want the robot to turn left, we will stop the right tire, 
    //and have the left tire slow down until it has turned a sufficient amount
    LATDbits.LATD5 = ENABLED;                  //#Assuming that D5 moves it backward when set high 
    LATDbits.LATD11 = ENABLED;                 //#Assuming that D11 moves it backward when set high 
    LATGbits.LATG13 = DISABLED;                  //#Assuming that G13 moves it forward when set high 
    LATGbits.LATG15 = DISABLED;                  //#Assuming that G15 moves it forward when set high 
    OC3RS = 860;
    OC4RS = 820;
}

void goForward(void){
    //If we want the robot to go straight, we will have both tires 
    //go forward at a middle speed
    LATDbits.LATD5 = ENABLED;                  //#Assuming that D5 moves it backward when set high 
    LATDbits.LATD11 = ENABLED;                 //#Assuming that D11 moves it backward when set high 
    LATGbits.LATG13 = DISABLED;                  //#Assuming that G13 moves it forward when set high 
    LATGbits.LATG15 = DISABLED;                  //#Assuming that G15 moves it forward when set high 

    OC3RS = 810;    //890 OC3RS and 900 OC4RS seems to be a similar speed
    OC4RS = 820;
}