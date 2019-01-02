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
pt.set_volume(50);//OPTIONAL: set the volume between 0/100, if not called the volume will be 100%
pt.enable_leds(13,12,11,10); // OPTIONAL: call this function for using leds and give them the pins where they are going to be

}

void loop () {
 pt.tune_playscore (score);
  
  while (pt.tune_playing);
  delay(5000);
          
}
