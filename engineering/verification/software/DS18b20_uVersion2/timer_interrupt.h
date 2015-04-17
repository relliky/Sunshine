



/* --------------------------------------------------------------------------------------------------------------------------------------------
	Timer Interrupt Macros
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// Timer interrupt parameters
#define INTERRUPT_ON    					1
#define INTERRUPT_OFF   					0 
#define TMOD_TIMER0_16BIT_TIMER1_16BIT 		0x11  
// CYCLE_FOR_TIMER1_UNDER_11_0952MHZ (5ms)= (5000us)/(1/11.0592 us/clock)/12 clock/machine_cycle = 5000*11.0592/12=4608   
#define CYCLE_FOR_TIMER1_UNDER_11_0952MHZ 	4608 
// CYCLE_FOR_TIMER1_UNDER_11_0952MHZ 4132 is the best parameter for stepper to provide maximum force to press the buttom, which is a 4.48ms timer interrupt   
#define CYCLE_FOR_TIMER0_UNDER_11_0952MHZ   4132
// Count parameter for 1s 
#define COUNT_1S_FOR_5MS_TIMER  			200
// Count parameter for 5s 
#define COUNT_5S_FOR_1S_TIMER				5



/* --------------------------------------------------------------------------------------------------------------------------------------------
	Button Macros
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// Buttons debouncing parameters
#define DEBOUNCED_TIME 						10
#define UNDEFINED_DEBOUNCE_TIME 			99
// The button debounce time is 10ms, timer1 interrupt happened every 5ms, and therefore BUTTON_DEBOUNCE_TIME_FOR_TIMER1 is 2 here. 
#define BUTTON_DEBOUNCE_TIME_FOR_TIMER1 	2 
#define RELEASED	   						1
#define PRESSED		   						0

/* --------------------------------------------------------------------------------------------------------------------------------------------
	Test LED Macros
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// Test LED cathode state value. ON has to be 0. OFF has to be 1
#define TEST_LED_ON	   0
#define TEST_LED_OFF   1

/* --------------------------------------------------------------------------------------------------------------------------------------------
	LED Display Macros
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// 7-segment LED cathode inverted value, ON has to be 1, OFF has to be 0.
#define LED_DIGIT_OFF      				0
#define LED_DIGIT_ON       				1
#define LED_DIGIT_BLINK    				0x2
#define LED_DIGIT_NOT_BLINK 			0x3
// LED display table macros
#define DIGIT_H        					16
#define DIGIT_C        					0xC
#define DIGIT_DEGREE   					17
#define DECIMAL_POINT 					18
// 7-segment mask for LED cathode inverted value to display LED off state
#define NOTHING							28
// LED_BLINK_TIME is set as 40ms*8 = 320ms, which is running at 3.125Hz. 
// LED blink function took 4 timer1 interrupt (Every time DigitNumber ==4, the LED blink function is enabled), 
// and LED needs to be ON and then OFF, and hence LED took 8 timer1 interrupt to finish one 4-digit LED blink as fast as possible.
// For a timer1 running at 5ms, one blink is 5ms*8 = 40ms.
#define LED_BLINK_TIME 8
// N.B. LedBlinkTimer can never reach the value of UNDEFINED_BLINK_TIMER(set to 200 here), otherwise the programme will regard the timer is undefined, i.e. blink function disabled while the blinking is actually running.
// Therefore there is LED_BLINK_TIME < UNDEFINED_BLINK_TIMER all the time.
// UNDEFINED_BLINK_TIMER cannot be over 127 since unsigned char is regarded with maximum value of 127 by the compiler
#define UNDEFINED_BLINK_TIMER 120

/* --------------------------------------------------------------------------------------------------------------------------------------------
	Stepper Motor  Macros
 --------------------------------------------------------------------------------------------------------------------------------------------*/
#define STEPPER_ON					1
#define STEPPER_OFF					0
#define STEPPER_FORWARD				1
#define STEPPER_BACKWARD			0
#define STEPPER_PHASE_ON			1
#define STEPPER_PHASE_OFF			0
#define STEPPER_INITIAL_PHASE 		1
#define STEPPER_LAST_PHASE  		4
#define STEPPER_PHASE_INDEX_ALL_OFF 0

/* --------------------------------------------------------------------------------------------------------------------------------------------
	Misc Macros
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// Macros for general usage
#define YES			1
#define NO  		0
#define FINISHED	1
#define CONTINUOUS 	0
//#define NOT_RUN		0
//#define RUNNING		1
/* --------------------------------------------------------------------------------------------------------------------------------------------
	LED display external variables declarations
 --------------------------------------------------------------------------------------------------------------------------------------------*/
extern unsigned char LedDigit1   	  ;
extern unsigned char LedDigit2   	  ;
extern unsigned char LedDigit3   	  ;
extern unsigned char LedDigit4   	  ;
extern bit LedDigit1En 	  ;
extern bit LedDigit2En 	  ;
extern bit LedDigit3En 	  ;
extern bit LedDigit4En 	  ;
extern unsigned char LedDigit1BlinkEn ;
extern unsigned char LedDigit2BlinkEn ;
extern unsigned char LedDigit3BlinkEn ;
extern unsigned char LedDigit4BlinkEn ;

/* --------------------------------------------------------------------------------------------------------------------------------------------
	Buttons external variables declarations
 --------------------------------------------------------------------------------------------------------------------------------------------*/
extern bit ButtonMinusState ;
extern bit ButtonPlusState  ;
extern bit ButtonSelectState;
extern bit ButtonAltState	;

/* --------------------------------------------------------------------------------------------------------------------------------------------
	Global counters variables declarations
 --------------------------------------------------------------------------------------------------------------------------------------------*/
extern unsigned int OneSecCounterForTimer;


/* --------------------------------------------------------------------------------------------------------------------------------------------
	Stepper external variables declarations
 --------------------------------------------------------------------------------------------------------------------------------------------*/
extern unsigned int 	TimerSetForStepperLastingInOneMovementInTimer1;
extern bit				StepperRotationDirection					  ;
extern bit 				StepperForwardStart							  ;
extern bit 				StepperForwardStop							  ;
extern bit 				StepperBackwardStart						  ;
extern bit 				StepperBackwardStop							  ;

