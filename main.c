// ******************************************************************************************* //
//  This comment is a test for the github repository. test again.
// File:         lab2p1.c
// Date:         
// Authors:      
//
// Description: Part 1 for lab 1
// ******************************************************************************************* //

#include <xc.h>
#include <sys/attribs.h>
#include "config.h"
#include "constants.h"
#include "leds.h"
#include "interrupt.h"
#include "switch.h"
#include "button.h"
#include "timer.h"
#include "lcd.h"
#include "keypad.h"
#include "ADC.h"
#include "PWM.h"
#include "control.h"


typedef enum state_enum {
    INIT, FORWARD, RIGHT, LEFT, PIVOT, TURN_RIGHT, SET_DIRECTION, PRINT_LCD, RESUME
} state_t;

volatile state_t myState;
volatile int read;
volatile int read_reset;    /*integer to store value of the reset button*/
volatile char timer_flag;   /*The timer flag increments every 10ms*/
/* Please note that the configuration file has changed from lab 0.
 * the oscillator is now of a different frequency.
 */
int main(void)
{
    int i = 0;
    state_t direction = FORWARD;
    char pivot = 1;
    int turnedRight = 0;
    int voltageADC = 0;
    int lastVoltage = 0;
    char charToWrite = 0;
    char numberToPrint[5] = {' ', ' ',  ' ',  ' ',  '\0'};
    char sensors = 0b00000001;
    SYSTEMConfigPerformance(10000000);
    enableInterrupts();
    myState = INIT;
    initButton();
    initLEDs();
    initTimers();
    initLCD();
    initKeypad();
    initPWM();
    initADC();
    
    LATDbits.LATD5 = ENABLED;                  //#Assuming that D5 moves it backward when set high 
    LATDbits.LATD11 = ENABLED;                 //#Assuming that D11 moves it backward when set high 
    LATGbits.LATG13 = DISABLED;                  //#Assuming that G13 moves it forward when set high 
    LATGbits.LATG15 = DISABLED;                  //#Assuming that G15 moves it forward when set high 
    
    //format 0b0000[Right][Top][Left][Middle]
    /* Green wire: TOP sensor J11 pin 32
    ** Yellow wire: LEFT sensor J11 pin 30
    ** Orange wire: MIDDLE sensor   J10 pin
    ** Red wire: RIGHT sensor   J11 pin 34
    */
    while(1)
    {
        sensors = readSensors();
        if(frontTriggered(sensors)) {
            LATDbits.LATD2 = ENABLED;
        }
        else {
            LATDbits.LATD2 = DISABLED;
        }
        if(rightTriggered(sensors)) {
            LATDbits.LATD0 = ENABLED;
        }
        else {
            LATDbits.LATD0 = DISABLED;
        }
        if(leftTriggered(sensors)) {
            //LATDbits.LATD2 = ENABLED;
        }
        else {
            //LATDbits.LATD2 = DISABLED;
        }
        
        /*clearLCD();
        for (i = 0; i<10; i++) delayUs(1000);
        moveCursorLCD(0, 2);
        printStringLCD("R    T    L");
        moveCursorLCD(1, 0);
        voltageADC = testSensor(0b00001000);
        itoa(numberToPrint, voltageADC, 10);
        printStringLCD(numberToPrint);
        printStringLCD(" ");
        voltageADC = testSensor(0b00000100);
        itoa(numberToPrint, voltageADC, 10);
        printStringLCD(numberToPrint);
        printStringLCD(" ");
        voltageADC = testSensor(0b00000010);
        itoa(numberToPrint, voltageADC, 10);
        printStringLCD(numberToPrint);
        for (i = 0; i<30; i++) delayUs(1000);
         */
        
        switch(myState) {
            case INIT:
                voltageADC = testSensor(0b00000100);
                if(myState != SET_DIRECTION) {
                    myState = INIT;
                }
                break;
            case RIGHT:
                turnRight();
                sensors = readSensors();
                while(!frontTriggered(sensors)) {
                    sensors = readSensors();
                }
                delayUs(1000);
                myState = FORWARD;
                break;
            case FORWARD:
                sensors = readSensors();
                //goForward();
                if(frontTriggered(sensors)) {
                    goForward();
                }
                else if(rightTriggered(sensors)) {
                    turnRight();
                }
                else if(leftTriggered(sensors)) {
                    turnLeft();
                }
                
                if(frontTriggered(sensors) && rightTriggered(sensors) && leftTriggered(sensors) && middleTriggered(sensors)) {
                    if(pivot == 1) {
                        LATDbits.LATD5 = DISABLED;                  //#Assuming that D5 moves it backward when set high 
                        LATDbits.LATD11 = ENABLED;                 //#Assuming that D11 moves it backward when set high 
                        LATGbits.LATG13 = ENABLED;                  //#Assuming that G13 moves it forward when set high 
                        LATGbits.LATG15 = DISABLED;                  //#Assuming that G15 moves it forward when set high 
                        
                        OC3RS = 800;
                        OC4RS = 800;
                        //for(i = 0; i < 5000; ++i) delayUs(1000);
                        pivot = 1;
                    }
                }
                
                myState = FORWARD;
                break;
            case LEFT:
                /*
                turnLeft();
                sensors = readSensors();
                while(!frontTriggered(sensors)) {
                    sensors = readSensors();
                }
                delayUs(1000);
                myState = FORWARD;
                 */
                turnLeft();
                break;
            case TURN_RIGHT:
                turnRight();
                sensors = readSensors();

                while(frontTriggered(sensors)) {
                    sensors = readSensors();
                }

                turnedRight = 1;
                for (i = 0; i<10; i++) delayUs(1000);
                myState = FORWARD;
                break;
            case PIVOT:
                if(myState != SET_DIRECTION) {
                    myState = PRINT_LCD;
                }
                break;
            case SET_DIRECTION:
                sensors = readSensors();
                myState = FORWARD;
                break;
            case RESUME:
                if(myState != SET_DIRECTION) {
                    switch(sensors) {
                        case 0b00001000:
                            myState = RIGHT;
                            break;
                        case 0b00000100:
                            myState = INIT;
                            break;
                        case 0b00000010:
                            myState = LEFT;
                            break;
                        case 0b00000001:
                            myState = FORWARD;
                            break;
                        default:
                            myState = INIT;
                            break;
                    }
                }
                break;
            case PRINT_LCD:
                itoa(numberToPrint, voltageADC, 10);
                clearLCD();
                for (i = 0; i<10; i++) delayUs(1000);
                moveCursorLCD(0, 5);
                printStringLCD(numberToPrint);
                for (i = 0; i<30; i++) delayUs(1000);
                if(myState != SET_DIRECTION) {
                    myState = INIT;
                }
                break;
        }
    }
    
    return 0;
}

void __ISR(_TIMER_1_VECTOR, IPL7SRS) timer1Handler(void){
    IFS0bits.T1IF = FLAG_DOWN;
    TMR1 = 0;
}

/*
** The change notification interrupt handles the start/stop button and the 
** reset button logic, hence its length. There are really only two switch statements
** in the ISR, so it really doesn't have to make many decisions that would 
** bloat the ISR.
*/

void __ISR(_CHANGE_NOTICE_VECTOR, IPL7SRS) _CNInterrupt(void){
    //TODO: Implement the interrupt to capture the press of the button
    PORTD;
    IFS1bits.CNDIF = FLAG_DOWN;
    IFS0bits.T4IF = 0;      //lower timer 4 flag for delay
    TMR4 = 0;  
    PR4 = 15000;//manually clear timer 4 register
    T4CONbits.ON = 1;       //turn timer 4 on.
    while(IFS0bits.T4IF != 1){};    //5ms delay
    T4CONbits.ON = 0;       //turn off timer 4
    IFS0bits.T4IF = 0;
    read = PORTD;
    read = PORTDbits.RD7;
    if(read == 0){
        switch(myState) {
            default:
                myState = SET_DIRECTION;
                break;
        }
    } else {
        myState = myState;
    }
}
