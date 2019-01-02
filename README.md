# PWM-Playtune
A modified version of the playtune library to just use 1 pin (pin 9) on the arduino
This is a library for arduino that gives the capability to play music that is store on the PROGMEM of the arduino in a form of a bytestream (that you can get by converting a midi file by the use of miditones https://github.com/LenShustek/miditones or MidiGUI https://github.com/theawsomeavr/midigui)
Advantages:

1°- It only uses 2 timers (timer1 and timer2 leaving timer0 alone) meaning that you cna use the delay, millis(), and micros() functions.
2°- It uses PWM (pin 9) to generate the waveform that meaning you can eliminate the resistors from your circuit and even use a MOSFET for more power.

3°- Since it uses PWM instead of only making square wave you can also make other wave forms like sine, saw, triangle, etc.. (to change the wave form go to the util folder of this library and replace the contents of the waveform.h file with the contents of the wave form that you want) and that makes a better sounds.

4°- It has 4 voices of polyphony rather than the 3 voices of the original playtune library.

5°- Bonus: you can attach leds for visual representation on any pin of the arduino (except pin 9).

Disadvanteges:

1°- It only works with versions of the arduino IDE from 1.6.x and higher.

2°- It only works on the arduino Uno and arduino mega (atmega2560) (atmega328p).

Thanks a lot to LenShustek and to Joe Marshall's for making this library possible
Original playtune library:
https://github.com/LenShustek/arduino-playtune

Arduino music using Direct Digital Synthesis:
http://www.cs.nott.ac.uk/~pszjm2/projects/other/Arduino_music_using_Direct_Digital_Synthesis.html
