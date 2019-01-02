/**************************************************************************
* 
* Playtune: An Arduino Tune Generator
* 
* Plays a polyphonic musical score
* For documentation, see the Playtune.cpp source code file
* 
*   (C) Copyright 2011,2016 Len Shustek
*    Modified by David Rubio December 2018 
*
**************************************************************************/
/*
* Change log
*  19 January 2011, L.Shustek, V1.0: Initial release.
*  10 June 2013, L. Shustek, V1.3
*     - change for compatibility with Arduino IDE version 1.0.5
*   6 April 2015, L. Shustek, V1.4
*     - change for compatibility with Arduino IDE version 1.6.x
*  28 May 2016, T. Wasiluk
*     - added support for ATmega32U4
*  10 July 2016, Nick Shvelidze
*     - Fixed include file names for Arduino 1.6 on Linux.
*  08/09/18, David Rubio
*     added support for the popular attiny85 based on L.Shustek attiny4313 menorah November 25, 2013 
*  20/12/18, David Rubio
*     modified the library so that it works with a PWM pin (pin 9) and it supports custom waveforms (for the atmega328p only)
*/

#ifndef PWM_Playtune_h
#define PWM_Playtune_h

#include <Arduino.h>

class PWM_Playtune
{
public:
 void set_volume(byte vol);      //use this to adjust the volume from 0/100
 void enable_leds (int led1,int led2,int led3,int led4); //call this function for using leds
 void tune_playscore (const byte *score);	// start playing a polyphonic score
 volatile static bool tune_playing;	// is the score still playing?
 void begin (void);			// starts timers
 //void tune_delay (unsigned msec);		update: instead of using this function you can use the normal "delay" function 
 void tune_stopscore (void);
 };

#endif
