//**************************************************
//
// Test of PWM Playtune library
//
// L. Shustek, 31 Jan 2011; initial release.
// L. Shustek, 27 Feb 2011; new score data
// L. Shustek, 6 April 2015; change for compatibility with Arduino IDE v1.6.x
// T. Wasiluk, 28 May 2016; moved to appropriate library examples subdirectory
// David Rubio, 9 September 2018; added support for the popular attiny85 based on L.Shustek attiny4313 menorah November 25, 2013
// David Rubio, 20 December 2018; modified the library so that it works with a PWM pin (pin 9) and it supports custom waveforms (for the atmega328p only) (it also supports LEDs per channel!!!)
//**************************************************

#include <PWM_Playtune.h>


/**********************************************************************
put down here your bytestream from miditones 
***********************************************************************/

PWM_Playtune pt;

void setup() {

pt.begin(); //initialize the timers
pt.enable_leds(13,12,11,10); // call this function for using leds and give them the pins where they are going to be

}

int analog;
void loop () {
  pt.tune_playscore (score); //play score
  while (pt.tune_playing){
    if(analogRead(A1)<analog-4||analogRead(A1)>analog+4){
      byte v=map(analogRead(A1),0,1023,0,100); // set the volume between 0/100
     pt.set_volume(v); 
     analog=analogRead(A1);
    }
  }
  
  delay(5000); //wait 5 seconds
  
          
}