
#define useleds 1
#ifdef useleds
const int leds[4]={4,5,6,7};
#endif
#include "music.h"
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
//triangle
char wave256 []  __attribute__ ((aligned(256)))={
  -128,-126,-124,-122,-120,-118,-116,-114,-112,-110,-108,-106,-104,-102,-100,
  -98,-96,-94,-92,-90,-88,-86,-84,-82,-80,-78,-76,-74,-72,-70,-68,
  -66,-64,-62,-60,-58,-56,-54,-52,-50,-48,-46,-44,-42,-40,-38,-36,
  -34,-32,-30,-28,-26,-24,-22,-20,-18,-16,-14,-12,-10,-8,-6,-4,
  -2,0,2,4,6,8,10,12,14,16,18,20,22,24,26,
  28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,
  60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,
  92,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,
  124,126,127,125,123,121,119,117,115,113,111,109,107,105,103,101,
  99,97,95,93,91,89,87,85,83,81,79,77,75,73,71,69,
  67,65,63,61,59,57,55,53,51,49,47,45,43,41,39,37,
  35,33,31,29,27,25,23,21,19,17,15,13,11,9,7,5,
  3,1,-1,-3,-5,-7,-9,-11,-13,-15,-17,-19,-21,-23,-25,-27,
  -29,-31,-33,-35,-37,-39,-41,-43,-45,-47,-49,-51,-53,-55,-57,-59,
  -61,-63,-65,-67,-69,-71,-73,-75,-77,-79,-81,-83,-85,-87,-89,-91,
  -93,-95,-97,-99,-101,-103,-105,-107,-109,-111,-113,-115,-117,-119,-121,-123,-125,-127
  };


volatile char* curWave=wave256;

#include "avr/pgmspace.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))


byte poly[4];
// set both timer to do phase correct PCM at
// a nominal frequency of 31372.549
void setupTimers() 
{
  
// Timer1 and 2 Clock Prescaler to : 1, set WGM22 to zero, and WGM12,13
// for phase correct PCM
  TCCR1B = (TCCR1B & 0b11100000) | 0b00001;
  TCCR2B = (TCCR2B & 0b11110000) | 0b0001;

// set it to clear compare match mode
// on both pins, and phase correct PCM (WGM21 and WGM20)
  TCCR1A = (TCCR1A &0b00001100)| 0b10100001;
  TCCR2A = (TCCR2A &0b00001100)| 0b10100001;

// set overflow interrupts enabled for timers 1 and 2
  TIMSK2 |= (1<<TOIE2);
  TIMSK1 |= (1<<TOIE1);
  //TIMSK0 |= (1<<TOIE0);
  
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

volatile unsigned int scorewait_interrupt_count;
volatile byte *score_start;
volatile byte *score_cursor;
volatile bool tune_playing;
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
     
     oscillators[chan].volume=0;
     #ifdef useleds
digitalWrite(leds[chan],0);

#endif
    }
    else if (opcode == 0x90) {
    
     // tune_playnote (chan, );
  oscillators[chan].phaseStep=hzToPhaseStep(freq[pgm_read_byte(score_cursor++)-12]);
  oscillators[chan].volume=25;
#ifdef useleds
digitalWrite(leds[chan],1);

#endif
    }
    else if (opcode == 0xe0) {

      score_cursor = score_start;
    }
    else if (opcode == 0xf0) {
      
    tune_playing = false;
    oscillators[3].volume=0;
    oscillators[2].volume=0;
    oscillators[1].volume=0;
    oscillators[0].volume=0;
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
void tune_playscore (byte *score) {
    if (tune_playing){    oscillators[3].volume=0;
    oscillators[2].volume=0;
    oscillators[1].volume=0;
    oscillators[0].volume=0;}
    score_start = score;
    score_cursor = score;
    tune_stepscore(); /* execute initial commands */
    tune_playing = true;  /* release the interrupt routine */
}
const int delayy=31;
long tick;
ISR(TIMER1_OVF_vect) 
{
  tick++;
  if(tick-delayy>0){
    
  
  if (tune_playing && scorewait_interrupt_count && --scorewait_interrupt_count == 0) {
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

  OCR1A=(poly[0]+poly[1]+poly[2]+poly[3])/4;
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

unsigned int hzToPhaseStep(float hz)
{
  float phaseStep= hz *2.0886902978652881446683197032183;
//  (pow(2,16) * frequency) / 31376.6
  return (unsigned int)phaseStep;
}

void setup()
{
         
    
   
   // pinMode(3,INPUT_PULLUP);
   // pinMode(3, OUTPUT);     // pin11= PWM  output / frequency output
//Serial.begin(115200);
    pinMode(9, OUTPUT);     // pin11= PWM  output / frequency output
pinMode(13,1);
   // pin11= PWM  output / frequency output
//  Serial.begin(9600);        // connect to the serial port

#ifdef useleds
pinMode(leds[0],1);
pinMode(leds[2],1);
pinMode(leds[3],1);
pinMode(leds[1],1);
#endif

  setupTimers();

}

void loop()
{   

  tune_playscore (score); 
      while (tune_playing) {
  
          //tune_stopscore();
       
        } 
    delay(5000);
  // byte pwm=

}
