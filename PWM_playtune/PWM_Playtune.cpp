/**************************************************************
this is a modded library based around the playtune library and 
DDS genereted waveforms (http://www.cs.nott.ac.uk/~pszjm2/projects/other/Arduino_music_using_Direct_Digital_Synthesis.html)
you can change the waveform on the waveform.c, just replace it with some of them that i leave in the util folder
**************************************************************/
#include "PWM_Playtune.h"
#include "waveform.h"
int leds[4]={1,1,1,1};

//#endif
void PWM_Playtune::enable_leds (int led1,int led2,int led3,int led4){
#define useleds 1
leds[0]=led1;
leds[1]=led2;
leds[2]=led3;
leds[3]=led4;

pinMode(leds[0],1);
pinMode(leds[2],1);
pinMode(leds[3],1);
pinMode(leds[1],1);

}
//#define useleds 1

unsigned int hzToPhaseStep(float hz)
{
  float phaseStep= hz *2.0886902978652881446683197032183;
//  (pow(2,16) * frequency) / 31376.6
  return (unsigned int)phaseStep;
}

const int freq[128] =
{
  16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31, 33, 35, 37, 39, 41,
  44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110,
  117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233,
  247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
  523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047,
  1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729,
  3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040,
  7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544,
  13290, 14080, 14917, 15804, 16744, 17740, 18795, 19912, 21096,
  22351, 23680, 25088
};



volatile char* curWave=wave256;

#include "avr/pgmspace.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))


byte poly[4];
// set both timer to do phase correct PCM at
// a nominal frequency of 31372.549
void PWM_Playtune::begin() 
{
   pinMode(9, OUTPUT); 
   #if (defined(ARDUINO_AVR_UNO) || \
       defined(ARDUINO_AVR_DUEMILANOVE) || \
       defined(__AVR_ATmega328__) || \
       defined(__AVR_ATmega328P__) || \
       defined(__AVR_ATmega328PB__))
// Timer1 and 2 Clock Prescaler to : 1, set WGM22 to zero, and WGM12,13
// for phase correct PCM
  TCCR1B = 0b00000001;
  TCCR2B = 0b00000001;

// set it to clear compare match mode
// on both pins, and phase correct PCM (WGM21 and WGM20)
  TCCR1A = 0b10000001;
  TCCR2A = 0b00000001;

// set overflow interrupts enabled for timers 1 and 2
  TIMSK2 |= (1<<TOIE2);
  TIMSK1 |= (1<<TOIE1);
  //TIMSK0 |= (1<<TOIE0);
  #elif (defined(ARDUINO_AVR_MEGA) || \
       defined(ARDUINO_AVR_MEGA1280) || \
       defined(ARDUINO_AVR_MEGA2560) || \
       defined(__AVR_ATmega1280__) || \
       defined(__AVR_ATmega1281__) || \
       defined(__AVR_ATmega2560__) || \
       defined(__AVR_ATmega2561__))
  TCCR1B = 0b00000001;
  TCCR2B = 0b00000001;

// set it to clear compare match mode
// on both pins, and phase correct PCM (WGM21 and WGM20)
  TCCR1A = 0b00000001;
  TCCR2A = 0b00100001;

// set overflow interrupts enabled for timers 1 and 2
  TIMSK2 |= (1<<TOIE2);
  TIMSK1 |= (1<<TOIE1);
  #else
	  unsuported MCU
  #endif
}

// information about the current state of a single oscillator
struct OscillatorState
{
  unsigned int phaseStep; // frequency of this oscillator
   char volume; // volume multiplier for this oscillator
   unsigned int phaseAccu; // accumulator that steps through
                           // this oscillator
                           // high byte = current sample index
};

// our 4 oscillators
struct OscillatorState oscillators[4];

//********* interrupt routines for each of the timer overflows ******

inline byte getByteLevel(int accumulator)
{
    char valOut=((unsigned int)(accumulator))>>7;
    valOut+=128;
    return (byte)valOut;
}
byte volumen=100;
bool chanon[4];
void PWM_Playtune::set_volume(byte vol){
	volumen=vol;
	oscillators[0].volume=volumen*chanon[0];
	oscillators[1].volume=volumen*chanon[1];
	oscillators[2].volume=volumen*chanon[2];
	oscillators[3].volume=volumen*chanon[3];
}
volatile unsigned int scorewait_interrupt_count;
volatile byte *score_start;
volatile byte *score_cursor;
volatile bool PWM_Playtune::tune_playing;
void tune_stepscore (void) {
  byte cmd, opcode, chan;

  while (1) {
    cmd = pgm_read_byte(score_cursor++);
  
    if (cmd < 0x80) {

  
      scorewait_interrupt_count = ((unsigned)cmd << 8) | (pgm_read_byte(score_cursor++)+1);
      break;
    }
    opcode = cmd & 0xf0;
    chan = cmd & 0x0f;
    if (opcode == 0x80) {
     if(chan<4){
     oscillators[chan].volume=0;
	 chanon[chan]=0;
     #ifdef useleds
digitalWrite(leds[chan],0);

#endif
	 }
    }
    else if (opcode == 0x90) {
    if(chan<4){
     // tune_playnote (chan, );
	 chanon[chan]=1;
  oscillators[chan].phaseStep=hzToPhaseStep(freq[pgm_read_byte(score_cursor++)-12]);
  oscillators[chan].volume=volumen;
#ifdef useleds
digitalWrite(leds[chan],1);

#endif
	}
	else score_cursor++;
    }
    else if (opcode == 0xe0) {

      score_cursor = score_start;
    }
    else if (opcode == 0xf0) {
      
    PWM_Playtune::tune_playing = false;
    oscillators[3].volume=0;
    oscillators[2].volume=0;
    oscillators[1].volume=0;
    oscillators[0].volume=0;
	chanon[0]=0;
	chanon[1]=0;
	chanon[2]=0;
	chanon[3]=0;
    #ifdef useleds
digitalWrite(leds[0],0);
digitalWrite(leds[1],0);
digitalWrite(leds[2],0);
digitalWrite(leds[3],0);

#endif
      break;
    }
  }
}
// interrupt for timer 1 overflow (pins 9 and 10)
// in this, we set the next value for the PWM for those pins
// ie. we set the sample value
void PWM_Playtune::tune_playscore (const byte *score) {
    if (tune_playing){    
	oscillators[3].volume=0;
    oscillators[2].volume=0;
    oscillators[1].volume=0;
    oscillators[0].volume=0;
	chanon[0]=0;
	chanon[1]=0;
	chanon[2]=0;
	chanon[3]=0;}
    score_start = score;
    score_cursor = score;
    tune_stepscore(); /* execute initial commands */
    PWM_Playtune::tune_playing = true;  /* release the interrupt routine */
}
const int delayy=31;
long tick;
void PWM_Playtune::tune_stopscore (void) {
    oscillators[3].volume=0;
    oscillators[2].volume=0;
    oscillators[1].volume=0;
    oscillators[0].volume=0;
	chanon[0]=0;
	chanon[1]=0;
	chanon[2]=0;
	chanon[3]=0;
	#ifdef useleds
digitalWrite(leds[0],0);
digitalWrite(leds[1],0);
digitalWrite(leds[2],0);
digitalWrite(leds[3],0);

#endif
 
  PWM_Playtune::tune_playing = false;
}
ISR(TIMER1_OVF_vect) 
{
  tick++;
  if(tick-delayy>0){
    
  
  if (PWM_Playtune::tune_playing && scorewait_interrupt_count && --scorewait_interrupt_count == 0) {
        // end of a score wait, so execute more score commands
        tune_stepscore ();  // execute commands
    }
    tick=0;
    }
    // decrement delay wait counter
   
  // update sample position (ignore overflow, as 
  // we use the top byte to index into a 256 byte buffer
  // and the overflow means it loops through the buffer)

  //oscillator 0 update 
  oscillators[0].phaseAccu+=oscillators[0].phaseStep;
  int valOut0=curWave[oscillators[0].phaseAccu>>8]*oscillators[0].volume;

  //oscillator 1 update 
  oscillators[1].phaseAccu+=oscillators[1].phaseStep;
  int valOut1=curWave[oscillators[1].phaseAccu>>8]*oscillators[1].volume;

// write these two oscillators out
// to individual pins

// write to pin 9
  poly[0]=getByteLevel(valOut0);
  // write to pin 10
  poly[1]=getByteLevel(valOut1);
     #if (defined(ARDUINO_AVR_UNO) || \
       defined(ARDUINO_AVR_DUEMILANOVE) || \
       defined(__AVR_ATmega328__) || \
       defined(__AVR_ATmega328P__) || \
       defined(__AVR_ATmega328PB__))
  OCR1A=(poly[0]+poly[1]+poly[2]+poly[3])/4;
    #elif (defined(ARDUINO_AVR_MEGA) || \
       defined(ARDUINO_AVR_MEGA1280) || \
       defined(ARDUINO_AVR_MEGA2560) || \
       defined(__AVR_ATmega1280__) || \
       defined(__AVR_ATmega1281__) || \
       defined(__AVR_ATmega2560__) || \
       defined(__AVR_ATmega2561__))
   OCR2B=(poly[0]+poly[1]+poly[2]+poly[3])/4;
   #else 
	   unsuported MCU
   #endif
}
// interrupt for timer 2 overflow (pins 11 and 3)
// in this, we set the next value for the PWM for those pins
// ie. we set the sample value
ISR(TIMER2_OVF_vect) 
{
  // update sample position (ignore overflow, as 
  // we use the top byte to index into a 256 byte buffer
  // and the overflow means it loops through the buffer)
  
  // update the oscillators - make sure you update the oscillator
  // in the correct interrupt function, based on what pin you're 
  // outputting from
  
  // oscillator 2 update  
  oscillators[2].phaseAccu+=oscillators[2].phaseStep;
  int valOut2=curWave[oscillators[2].phaseAccu>>8]*oscillators[2].volume;
  
  // oscillator 3 update  
  oscillators[3].phaseAccu+=oscillators[3].phaseStep;
  int valOut3=curWave[oscillators[3].phaseAccu>>8]*oscillators[3].volume;
    
// write the oscillators to individual pins
  poly[2]=getByteLevel(valOut2); // write to pin 11
  poly[3]=getByteLevel(valOut3); // write pin 3
  
// if you want to mix multiple oscillators, put it in here instead
// e.g.  OCR2A=getByteLevel(valOut2+valOut3);

}





