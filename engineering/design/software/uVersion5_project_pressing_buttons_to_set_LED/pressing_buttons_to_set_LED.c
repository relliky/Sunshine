#include <reg52.h>
#include <intrins.h>

// Timer interrupt parameters
#define INTERRUPT_ON    					1
#define INTERRUPT_OFF   					0 
#define TMOD_TIMER0_16BIT_TIMER1_16BIT 		0x11  
// CYCLE_FOR_TIMER1_UNDER_11_0952MHZ (5ms)= (5000us)/(1/11.0592 us/clock)/12 clock/machine_cycle = 5000*11.0592/12=4608   
#define CYCLE_FOR_TIMER1_UNDER_11_0952MHZ 	4608 
// Count parameter for 1s 
#define COUNT_1S_FOR_5MS_TIMER  			200
// Count parameter for 5s 
#define COUNT_5S_FOR_1S_TIMER					5

// Test LED cathode state value. ON has to be 0. OFF has to be 1
#define TEST_LED_ON	   0
#define TEST_LED_OFF   1

// 7-segment LED cathode inverted value, ON has to be 1, OFF has to be 0.
#define LED_DIGIT_OFF      				0
#define LED_DIGIT_ON       				1
#define LED_DIGIT_BLINK    				0x2
#define LED_DIGIT_NOT_BLINK 			0x3
#define LED_DIGIT_UNINTIALISED_STATE 	0x4
// 7-segment mask for LED cathode inverted value to display LED off state
#define NOTHING				28

// Buttons debouncing parameters
#define DEBOUNCED_TIME 10
#define UNDEFINED_DEBOUNCE_TIME 99
// The button debounce time is 10ms, timer1 interrupt happened every 5ms, and therefore BUTTON_DEBOUNCE_TIME_FOR_TIMER1 is 2 here. 
#define BUTTON_DEBOUNCE_TIME_FOR_TIMER1 2 
#define RELEASED	   1
#define PRESSED		   0

// LED display table macros
#define DIGIT_H        16
#define DIGIT_C        0xC
#define DIGIT_DEGREE   17
#define DECIMAL_POINT 18

// LED_BLINK_TIME is set as 40ms*8 = 320ms, which is running at 3.125Hz. 
// LED blink function took 4 timer1 interrupt (Every time DigitNumber ==4, the LED blink function is enabled), 
// and LED needs to be ON and then OFF, and hence LED took 8 timer1 interrupt to finish one 4-digit LED blink as fast as possible.
// For a timer1 running at 5ms, one blink is 5ms*8 = 40ms.
#define LED_BLINK_TIME 8
// N.B. LedBlinkTimer can never reach the value of UNDEFINED_BLINK_TIMER(set to 200 here), otherwise the programme will regard the timer is undefined, i.e. blink function disabled while the blinking is actually running.
// Therefore there is LED_BLINK_TIME < UNDEFINED_BLINK_TIMER all the time.
// UNDEFINED_BLINK_TIMER cannot be over 127 since unsigned char is regarded with maximum value of 127 by the compiler
#define UNDEFINED_BLINK_TIMER 120


//
#define USER_INPUT_STATE_NORMAL_DISPLAY  	1
#define USER_INPUT_STATE_SET_TIME 			2
#define USER_INPUT_STATE_SET_TEMPERATURE  	3
#define USER_INPUT_STATE_IDLE			  	4

//#define RADIATOR_MAX_HOURS 12
#define MAXIMUM_HOURS_RADIATOR_ON 12

// Macros for general usage
#define YES 1
#define NO  0

// Clock parameters	
#define MAXIMUM_SECOND_FOR_A_MINUTE	60
#define MAXIMUM_MINUTE_FOR_AN_HOUR	60

// Temperature Set parameters
#define MINIMUM_TEMPERATURE_SET 20
#define MAXIMUM_TEMPERATURE_SET 45

sbit LedDigit1Switch = P1^0;
sbit LedDigit2Switch = P1^1;
sbit LedDigit3Switch = P1^2;
sbit LedDigit4Switch = P1^3;
sbit ButtonMinus	 = P1^4;
sbit ButtonPlus		 = P1^5;
sbit ButtonSelect	 = P1^6;
sbit ButtonAlt		 = P1^7;
sfr  LedDigitDisplay = 0xA0; //Port2 declaration
sbit TestLedState	 = P3^7;

unsigned char LedDigit1   	   = 8;
unsigned char LedDigit2   	   = 8;
unsigned char LedDigit3   	   = 8;
unsigned char LedDigit4   	   = 8;
unsigned char LedDigit1En 	   = LED_DIGIT_OFF;
unsigned char LedDigit2En 	   = LED_DIGIT_OFF;
unsigned char LedDigit3En 	   = LED_DIGIT_OFF;
unsigned char LedDigit4En 	   = LED_DIGIT_OFF;
unsigned char DigitNumber 	   = 1;
unsigned char LedDigit1BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LedDigit2BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LedDigit3BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LedDigit4BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit1BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit2BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit3BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit4BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LedDigit1State 	= LED_DIGIT_OFF;
unsigned char LedDigit2State 	= LED_DIGIT_OFF;
unsigned char LedDigit3State 	= LED_DIGIT_OFF;
unsigned char LedDigit4State 	= LED_DIGIT_OFF;
unsigned char BlinkReferenceLedState = LED_DIGIT_UNINTIALISED_STATE;

unsigned int  LedBlinkTimer     = UNDEFINED_BLINK_TIMER;

unsigned char ButtonMinusState	  		= RELEASED;
unsigned char ButtonPlusState	 		= RELEASED;
unsigned char ButtonSelectState	 		= RELEASED;
unsigned char ButtonAltState	 		= RELEASED;
unsigned char ButtonPlusDebounced  		= NO;
unsigned char ButtonMinusDebounced  	= NO;
unsigned char ButtonSelectDebounced  	= NO;
unsigned char ButtonAltDebounced  		= NO;
unsigned int  ButtonPlusDebounceTime    = UNDEFINED_DEBOUNCE_TIME;
unsigned int  ButtonMinusDebounceTime   = UNDEFINED_DEBOUNCE_TIME;
unsigned int  ButtonSelectDebounceTime  = UNDEFINED_DEBOUNCE_TIME;
unsigned int  ButtonAltDebounceTime  	= UNDEFINED_DEBOUNCE_TIME;

// Table for 7-segment LED to display 0-F,h,o(degree),"0." "1." "2." "3." "4." "5." "6." "7." "8." "9." and blank.
const unsigned char LedDisplayTable[]={
							~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,~0x07,~0x7f,~0x6f,  // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71,						  // A-F
							~0x74,~0x63, 			  									  // h,o(degree)
							~(0x3f+0x80),~(0x06+0x80),~(0x5b+0x80),~(0x4f+0x80),		  // "0." "1." "2." "3." 
							~(0x66+0x80),~(0x6d+0x80),~(0x7d+0x80),~(0x07+0x80),  		  // "4." "5." "6." "7."
							~(0x7f+0x80),~(0x6f+0x80),									  // "8." "9."
							~(0x00)};													  // blank


unsigned char NumberOfHalfAnHourRadiatorOn = 0;
unsigned char TimerTenthsDigit = 0;
unsigned char TimerUnitsDigit = 0;
unsigned char TimerTensDigit = 0;

unsigned int OneSecCounterForTimer = 0;
unsigned int SecondForClock		  = 0;
unsigned int MinuteForClock		  = 0;
unsigned int HourForClock		  = 0;	
unsigned int Display5sCounter     = 0;
unsigned int TemperatureSet		  = MINIMUM_TEMPERATURE_SET;


unsigned char UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;

void LedDigitControl(unsigned char LedDigitNumber, unsigned char LedDigitState, unsigned char LedDigitChar);
void DisplayStaticTimer (unsigned char NumberOfHalfAnHourRadiatorOn_Local);
void DisplayBlinkingTimer (unsigned char NumberOfHalfAnHourRadiatorOn_Local);
void DisplayStaticTemperature (unsigned char TemperatureSet_Local);
void DisplayBlinkingTemperature (unsigned char TemperatureSet_Local);

/******************************************************************/
/*                   main                                       */
/******************************************************************/
int main()
{

    LedDigit1En = LED_DIGIT_OFF;
    LedDigit2En = LED_DIGIT_OFF;
    LedDigit3En = LED_DIGIT_OFF;
    LedDigit4En = LED_DIGIT_OFF;
	LedDigit1Switch = LED_DIGIT_OFF;
	LedDigit2Switch = LED_DIGIT_OFF;
	LedDigit3Switch = LED_DIGIT_OFF;
	LedDigit4Switch = LED_DIGIT_OFF;

	
	// Timers interrupt initialisations 
	EA   = INTERRUPT_ON;
	TMOD = TMOD_TIMER0_16BIT_TIMER1_16BIT;

    ET1  = INTERRUPT_ON;
	TH1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)/0xFF;  
	TL1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)%0xFF;
	TR1  = INTERRUPT_ON;
	
	OneSecCounterForTimer = 0;
	SecondForClock		  = 0;
	MinuteForClock		  = 0;
	HourForClock		  = 0;	
	Display5sCounter  = 0 ;
	

	
 while(1)
 {
	/*
		Clock
	*/
	if(OneSecCounterForTimer == COUNT_1S_FOR_5MS_TIMER)
	{
		OneSecCounterForTimer = 0;
		SecondForClock++;
		
		// For USER_INPUT_STATE_NORMAL_DISPLAY to display time and temperature and switch between them every 5s 
		Display5sCounter++;
		if(Display5sCounter == COUNT_5S_FOR_1S_TIMER*2 + 1)
		{
			Display5sCounter = 1;
		}
	}// end of 	if(OneSecCounterForTimer == COUNT_1S_FOR_5MS_TIMER)



	
	if(SecondForClock == MAXIMUM_SECOND_FOR_A_MINUTE)
	{
		SecondForClock = 0;
		MinuteForClock++;
	}
 
	if(MinuteForClock == MAXIMUM_MINUTE_FOR_AN_HOUR)
	{
		MinuteForClock = 0;
		HourForClock++;
	}
	
	



	/*
	
	
	*/
 

	
	
	
/*     LedDigit1En = LED_DIGIT_ON;
	LedDigit1 = TimerTensDigit;
    LedDigit2En = LED_DIGIT_ON;
	LedDigit2 = TimerUnitsDigit;
    LedDigit3En = LED_DIGIT_ON;
	LedDigit3 = TimerTenthsDigit;
 
    LedDigit4En = LED_DIGIT_ON;
	LedDigit4 = DIGIT_H;
	
 */

/* 	 
	Description: Setting the user input state machine. 
	1. The product will interface with users in four modes: normal display mode, setting time mode, setting temperature mode, idle mode.

	2. Normal display mode
		i)   When the product is powered on, the default mode is the normal display mode.
		ii)  It displays the time set and the temperature set
		iii) It displays the time set for 5s and then switches to display the temperature set for another 5s and then back to display the time set.
		iv)  when no any button is pressed for 20s, the user input state switches to the idle mode.

	3. Setting time mode
		i)   The time digits, i.e. first, second and third digits, are blinking at the frequency twice every 1s.
		ii)  The four digits are "xx.xh", where x is a number, "h" is just a letter standing for hours.
		iii) The first digit can be set to "0", and "1" only.
		iv)  The second digit can be set to "0." "1." "2." "3." "4." "5." "6." "7." "8." "9." only.
		v)   The third digit can be set to "0" and "5" only
		vi)  The forth digit can be set to letter "h" only
		vii) The time can be set from "0.0h" to "12.0h" with each step 0.5h by pressing plus button and minus button. For example "0.0h" ->(press plus button) "0.5h" -> (press plus button) "1.0h", or "12.0h" ->(press plus button) "12.0h"
		viii)If MSB of the time is "0", the LED of MSB should be turned off.
		ix)  When the system is reset, the time is set to the default value, 0.0h.
		x)  The user input state switches to the normal display mode 20s after none of four buttons has been pressed.
		
	4. Setting temperature mode
		i)   The temperature digits, i.e. first and second digits, are blinking at the frequency twice every 1s.
		ii)  The four digits are "xx.Co", where x is a number, "Co" is just a letter and a symbol standing for degree Celsius.
		iii) The first digit can be set to "2", "3", "4" only.
		iv)  The second digit can be set to "0" "1" "2" "3" "4" "5" "6" "7" "8" "9" only
		v)   The third digit can be set to letter "C" only
		vi)  The forth digit can be set to symbol degree "o" only
		vii) The temperature can be set to from 20Co to 40Co with each step 1Co by pressing plus button and minus button. For example "20Co" ->(press plus button) "21Co" -> (press plus button) "22Co", or "40Co" ->(press plus button) "40Co"
		viii)When the system is reset, the temperature is set to the default value, 20Co.
		ix)  The user input state switches to the normal display mode 20s after none of four buttons has been pressed.
		
	5. Idle mode
		i)   The microcontroller is still running in the normal-on mode, but the LED screen is off.
		ii)  Any key pressed will put the user input state to the normal display mode.
		iii) When the system is in the sleep mode, the user input state has to be in the idle mode.
	
 */
 	switch(UserInputState)
	{
		case USER_INPUT_STATE_NORMAL_DISPLAY:
		{
			// Action to be done in this state
			if(Display5sCounter <= COUNT_5S_FOR_1S_TIMER)
			{
				DisplayStaticTemperature(TemperatureSet);
			}else
				{
					DisplayStaticTimer(NumberOfHalfAnHourRadiatorOn);
				}
				
			// Transition condition of next state
			if(ButtonSelectState == PRESSED) 
			{
				UserInputState = USER_INPUT_STATE_SET_TIME;
				ButtonSelectState = RELEASED;	
			}
			break;			
		}// end of case USER_INPUT_STATE_NORMAL_DISPLAY
		
		case USER_INPUT_STATE_SET_TIME:
		{
			// Action to be done in this state
			// If value of time is greater than MAXIMUM_HOURS_RADIATOR_ON, it will be set to MAXIMUM_HOURS_RADIATOR_ON.
			if(NumberOfHalfAnHourRadiatorOn >= MAXIMUM_HOURS_RADIATOR_ON*2) NumberOfHalfAnHourRadiatorOn = MAXIMUM_HOURS_RADIATOR_ON*2;
			
			if(ButtonPlusState == PRESSED)
			{
				ButtonPlusState = RELEASED; 
				if(NumberOfHalfAnHourRadiatorOn != MAXIMUM_HOURS_RADIATOR_ON*2) NumberOfHalfAnHourRadiatorOn++;
			}

			if(ButtonMinusState == PRESSED)
			{
				ButtonMinusState = RELEASED;
				if(NumberOfHalfAnHourRadiatorOn != 0)	NumberOfHalfAnHourRadiatorOn--;
			}
  
			DisplayBlinkingTimer(NumberOfHalfAnHourRadiatorOn);
			
			// Transition condition of next state			
			if(ButtonSelectState == PRESSED)
			{
				UserInputState = USER_INPUT_STATE_SET_TEMPERATURE;
				ButtonSelectState = RELEASED;
			}	
			break;
		}// end of case USER_INPUT_STATE_SET_TIME

		case USER_INPUT_STATE_SET_TEMPERATURE:
		{

			// If value of temperature is greater than MAXIMUM_TEMPERATURE_SET, it will be set to MAXIMUM_TEMPERATURE_SET.
			if(TemperatureSet >= MAXIMUM_TEMPERATURE_SET) TemperatureSet = MAXIMUM_TEMPERATURE_SET;
			
			if(ButtonPlusState == PRESSED)
			{
				ButtonPlusState = RELEASED; 
				if(TemperatureSet != MAXIMUM_TEMPERATURE_SET) TemperatureSet++;
			}

			if(ButtonMinusState == PRESSED)
			{
				ButtonMinusState = RELEASED;
				if(TemperatureSet != MINIMUM_TEMPERATURE_SET) TemperatureSet--;
			}
			
			DisplayBlinkingTemperature(TemperatureSet);
		
			if(ButtonSelectState == PRESSED)
			{
				UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;
				ButtonSelectState = RELEASED;
			}
			break;
		}// end of case USER_INPUT_STATE_SET_TEMPERATURE

		case USER_INPUT_STATE_IDLE:
		{
		// For test propose
			LedDigitControl(4, LED_DIGIT_BLINK, 1			);
			LedDigitControl(2, LED_DIGIT_OFF  , NOTHING 	);
			LedDigitControl(3, LED_DIGIT_OFF  , NOTHING 	);
			LedDigitControl(1, LED_DIGIT_OFF  , NOTHING 	);
		// End of test propose code					
		
			if(ButtonSelectState == PRESSED)
			{
				UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;
				ButtonSelectState = RELEASED;
			}				
			if(ButtonPlusState == PRESSED)
			{
				UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;
				ButtonPlusState = RELEASED;
			}
			if(ButtonMinusState == PRESSED)
			{
				UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;
				ButtonMinusState = RELEASED;
			}
			if(ButtonAltState == PRESSED)
			{
				UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;
				ButtonAltState = RELEASED;
			}
			break;
		}// end of case USER_INPUT_STATE_IDLE			

		default:
		{
		
			UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;
			break;
		}
		
	}// end of switch(UserInputState)

/* 
 	LedDigitControl(1, LED_DIGIT_BLINK, 3			);
	LedDigitControl(2, LED_DIGIT_BLINK, 6			);
	LedDigitControl(3, LED_DIGIT_ON   , DIGIT_C		);
	LedDigitControl(4, LED_DIGIT_ON	  , DIGIT_DEGREE);
	
 */
 }// end of while(1) loop
 

 return(0);
}



/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Display Timer on LED screen with always on setting, mainly used in USER_INPUT_STATE_NORMAL_DISPLAY
	Input:		 unsigned char NumberOfHalfAnHourRadiatorOn_Local
  --------------------------------------------------------------------------------------------------------------------------------------------*/
void DisplayStaticTimer (unsigned char NumberOfHalfAnHourRadiatorOn_Local)
{
	unsigned char Tens;
	unsigned char Units;
	unsigned char Tenths;
	
	Tens   = NumberOfHalfAnHourRadiatorOn_Local/2/10;
	Units  = NumberOfHalfAnHourRadiatorOn_Local/2%10;
    Tenths = NumberOfHalfAnHourRadiatorOn_Local%2*5;
 
	if(Tens != 0)
	{
		LedDigitControl(1, LED_DIGIT_ON, Tens	);
	}else
		{
			LedDigitControl(1, LED_DIGIT_OFF, NOTHING		);
		}
	LedDigitControl(2, LED_DIGIT_ON, Units + DECIMAL_POINT	);
	LedDigitControl(3, LED_DIGIT_ON, Tenths					);
	LedDigitControl(4, LED_DIGIT_ON, DIGIT_H				);
} // end of void DisplayStaticTimer (unsigned char NumberOfHalfAnHourRadiatorOn_Local)

/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Display Timer on LED screen with blinking numbers, mainly used in USER_INPUT_STATE_SET_TIME
	Input:		 unsigned char NumberOfHalfAnHourRadiatorOn_Local
  --------------------------------------------------------------------------------------------------------------------------------------------*/
void DisplayBlinkingTimer (unsigned char NumberOfHalfAnHourRadiatorOn_Local)
{
	unsigned char Tens;
	unsigned char Units;
	unsigned char Tenths;

	Tens   = NumberOfHalfAnHourRadiatorOn_Local/2/10;
	Units  = NumberOfHalfAnHourRadiatorOn_Local/2%10;
    Tenths = NumberOfHalfAnHourRadiatorOn_Local%2*5;
 
	if(Tens != 0)
	{
		LedDigitControl(1, LED_DIGIT_BLINK, Tens	);
	}else
		{
			LedDigitControl(1, LED_DIGIT_OFF, NOTHING		);
		}
	LedDigitControl(2, LED_DIGIT_BLINK, Units + DECIMAL_POINT	);
	LedDigitControl(3, LED_DIGIT_BLINK, Tenths					);
	LedDigitControl(4, LED_DIGIT_ON	  , DIGIT_H					);
} // end of void DisplayBlinkingTimer (unsigned char NumberOfHalfAnHourRadiatorOn_Local)

/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Display Temperature on LED screen with always on numbers, mainly used in USER_INPUT_STATE_SET_TEMPERATURE
	Input:		 unsigned char TemperatureSet_Local
  --------------------------------------------------------------------------------------------------------------------------------------------*/
void DisplayStaticTemperature (unsigned char TemperatureSet_Local)
{
	LedDigitControl(1, LED_DIGIT_ON	  , TemperatureSet_Local/10	);
	LedDigitControl(2, LED_DIGIT_ON   , TemperatureSet_Local%10	);
	LedDigitControl(3, LED_DIGIT_ON	  ,	DIGIT_C					);
	LedDigitControl(4, LED_DIGIT_ON	  , DIGIT_DEGREE			);
} // end of void DisplayStaticTemperature (unsigned char TemperatureSet_Local)

/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Display Temperature on LED screen with blinking numbers, mainly used in USER_INPUT_STATE_SET_TEMPERATURE
	Input:		 unsigned char TemperatureSet_Local
  --------------------------------------------------------------------------------------------------------------------------------------------*/
void DisplayBlinkingTemperature (unsigned char TemperatureSet_Local)
{
	LedDigitControl(1, LED_DIGIT_BLINK, TemperatureSet_Local/10	);
	LedDigitControl(2, LED_DIGIT_BLINK, TemperatureSet_Local%10	);
	LedDigitControl(3, LED_DIGIT_ON	  ,	DIGIT_C					);
	LedDigitControl(4, LED_DIGIT_ON	  , DIGIT_DEGREE			);
} // end of void DisplayBlinkingTemperature (unsigned char TemperatureSet_Local)


/*
	Description: Changing a state of a LED digit
	Input:		 unsigned char LedDigitNumber , i.e. which LED digit is controlled by this function)
	             unsigned char LedDigitChar   , i.e. what character is sent to this digit
				 unsigned char LedDigitState  , i.e. what state is for this LED digit: ON/OFF/BLINK
	N.B. For some reason, Keil uVersion2 does not compile one of the assignment to assembly even though there is no any brunch before.
		 Therefore, when using Keil to simulate the program running, breakpoints cannot be set at these assignments in C.		 
		 More specifically, LedDigit1BlinkEn = LED_DIGIT_NOT_BLINK; in the LedDigitState == LED_DIGIT_ON is not regarded as compiled, which means there is no correlated assembly code there and hence an user cannot set a breakpoint there..
		 And therefore LED display could not display well when digit state changed from ON/OFF to blink or otherwise.
		 Solution is an update of the compiler. Using latest version of Keil solved the issue of code not getting compiled, i.e. Keil uVersion5.

*/
void LedDigitControl(unsigned char LedDigitNumber, unsigned char LedDigitState, unsigned char LedDigitChar)
{
/* 	switch(LedDigitNumber)
	{
		case 1:
				breaks;
	
	
	}// end of switch(LedDigitNumber)
	
 */
 	switch(LedDigitNumber)
	{
		case 1:
			{
				LedDigit1 = LedDigitChar;
				if (LedDigitState == LED_DIGIT_BLINK)
				{
					LedDigit1BlinkEn = LED_DIGIT_BLINK;
				}
					
				if(LedDigitState == LED_DIGIT_ON)
				{
					LedDigit1En = LED_DIGIT_ON;
					LedDigit1BlinkEn = LED_DIGIT_NOT_BLINK;
				}
				
				if(LedDigitState == LED_DIGIT_OFF)
				{
					LedDigit1En = LED_DIGIT_OFF;
					LedDigit1BlinkEn = LED_DIGIT_NOT_BLINK;
				}								
					
				break;	
			}// end of case 1

		case 2:
			{
				LedDigit2 = LedDigitChar;
				if (LedDigitState == LED_DIGIT_BLINK)
				{
					LedDigit2BlinkEn = LED_DIGIT_BLINK;
				}
					
				if(LedDigitState == LED_DIGIT_ON)
				{
					LedDigit2En = LED_DIGIT_ON;
					LedDigit2BlinkEn = LED_DIGIT_NOT_BLINK;
				}
				
				if(LedDigitState == LED_DIGIT_OFF)
				{
					LedDigit2En = LED_DIGIT_OFF;
					LedDigit2BlinkEn = LED_DIGIT_NOT_BLINK;
				}								
					
				break;	
			}// end of case 2

		case 3:
			{
				LedDigit3 = LedDigitChar;
				if (LedDigitState == LED_DIGIT_BLINK)
				{
					LedDigit3BlinkEn = LED_DIGIT_BLINK;
				}
					
				if(LedDigitState == LED_DIGIT_ON)
				{
					LedDigit3En = LED_DIGIT_ON;
					LedDigit3BlinkEn = LED_DIGIT_NOT_BLINK;
				}
				
				if(LedDigitState == LED_DIGIT_OFF)
				{
					LedDigit3En = LED_DIGIT_OFF;
					LedDigit3BlinkEn = LED_DIGIT_NOT_BLINK;
				}								
					
				break;	
			}// end of case 3
						
 		case 4:
			{
				LedDigit4 = LedDigitChar;
				if (LedDigitState == LED_DIGIT_BLINK)
				{
					LedDigit4BlinkEn = LED_DIGIT_BLINK;
				}
					
				if(LedDigitState == LED_DIGIT_ON)
				{
					LedDigit4En = LED_DIGIT_ON;
					LedDigit4BlinkEn = LED_DIGIT_NOT_BLINK;
				}
				
				if(LedDigitState == LED_DIGIT_OFF)
				{
					LedDigit4En = LED_DIGIT_OFF;
					LedDigit4BlinkEn = LED_DIGIT_NOT_BLINK;
				}								
					
				break;	
			}// end of case 4
			
 	}// end of switch(LedDigitNumber)
	
}//end of LedDigitControl function


void Timer5ms_with_ID_timer1() interrupt 3
{
   // To measure how long does the program takes by observe the waveform of test LED in an oscillator
   // Results: It took about 40us, and therefore the duty cycle is about less than 1% by measured. Theoretically, the duty cycle is 40us/5000us = 0.8%
    TestLedState = TEST_LED_ON;

	TH1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)/0xFF;  
	TL1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)%0xFF;

	// Counter for counting 1s
	OneSecCounterForTimer++;
	
	/* --------------------------------------------------------------------------------------------------------------------------------------------
	  Description: Debouncing Plus button, Minus button, Select button, Alternative button.
	  1. When any button is pressed, programme is going to confirm that it is pressed and 
	  it is not a noise as the button is being held for more than 10ms. 
	  2. The programme will immediately resonse  the button is pressed if it is not noise 
	  and it will not recognise any continuous pressing, which means the count of pressing 
	  this button will not increase until it is released and pressed again.
	  3. Pressing multiple buttons in the same time will not interference each other as they were denounced individually.
	  and the result of pressing multiple buttons is predictable as the order of each button is pressed.
	--------------------------------------------------------------------------------------------------------------------------------------------*/
	  if(ButtonPlus == PRESSED)  
	  {
		if(ButtonPlusDebounceTime == UNDEFINED_DEBOUNCE_TIME) // This is the first time the interrupt detects button is pressed. ButtonPlusDebounceTime has to make sure not reach UNDEFINED_DEBOUNCE_TIME!!
		{	
			ButtonPlusDebounceTime = 0;
		}else
			{	
				if(ButtonPlusDebounceTime < BUTTON_DEBOUNCE_TIME_FOR_TIMER1)
				{
					ButtonPlusDebounceTime++;
					ButtonPlusDebounced = NO;
				}		
				if(ButtonPlusDebounceTime == BUTTON_DEBOUNCE_TIME_FOR_TIMER1)
				{
					if(ButtonPlusDebounced == NO)
					{
						ButtonPlusState = PRESSED;
					}
					ButtonPlusDebounced = YES;
				}
			}// end of if(ButtonPlusDebounceTime == UNDEFINED_DEBOUNCE_TIME) else
	  }else
			{
				if(ButtonPlusDebounced == YES)
				{
					ButtonPlusDebounced = NO;
					ButtonPlusDebounceTime = UNDEFINED_DEBOUNCE_TIME;	
				}		
			}// end of if(ButtonPlus == PRESSED) else
	  
	  if(ButtonMinus == PRESSED)  
	  {
		if(ButtonMinusDebounceTime == UNDEFINED_DEBOUNCE_TIME) // This is the first time the interrupt detects button is pressed. ButtonMinusDebounceTime has to make sure not reach UNDEFINED_DEBOUNCE_TIME!!
		{	
			ButtonMinusDebounceTime = 0;
		}else
			{	
				if(ButtonMinusDebounceTime < BUTTON_DEBOUNCE_TIME_FOR_TIMER1)
				{
					ButtonMinusDebounceTime++;
					ButtonMinusDebounced = NO;
				}		
				if(ButtonMinusDebounceTime == BUTTON_DEBOUNCE_TIME_FOR_TIMER1)
				{
					if(ButtonMinusDebounced == NO)
					{
						ButtonMinusState = PRESSED;
					}
					ButtonMinusDebounced = YES;
				}
			}// end of if(ButtonMinusDebounceTime == UNDEFINED_DEBOUNCE_TIME) else
	  }else
			{
				if(ButtonMinusDebounced == YES)
				{
					ButtonMinusDebounced = NO;
					ButtonMinusDebounceTime = UNDEFINED_DEBOUNCE_TIME;	
				}		
			}// end of if(ButtonMinus == PRESSED) else
	  
	  if(ButtonSelect == PRESSED)  
	  {
		if(ButtonSelectDebounceTime == UNDEFINED_DEBOUNCE_TIME) // This is the first time the interrupt detects button is pressed. ButtonSelectDebounceTime has to make sure not reach UNDEFINED_DEBOUNCE_TIME!!
		{	
			ButtonSelectDebounceTime = 0;
		}else
			{	
				if(ButtonSelectDebounceTime < BUTTON_DEBOUNCE_TIME_FOR_TIMER1)
				{
					ButtonSelectDebounceTime++;
					ButtonSelectDebounced = NO;
				}		
				if(ButtonSelectDebounceTime == BUTTON_DEBOUNCE_TIME_FOR_TIMER1)
				{
					if(ButtonSelectDebounced == NO)
					{
						ButtonSelectState = PRESSED;
					}
					ButtonSelectDebounced = YES;
				}
			}// end of if(ButtonSelectDebounceTime == UNDEFINED_DEBOUNCE_TIME) else
	  }else
			{
				if(ButtonSelectDebounced == YES)
				{
					ButtonSelectDebounced = NO;
					ButtonSelectDebounceTime = UNDEFINED_DEBOUNCE_TIME;	
				}		
			}// end of if(ButtonSelect == PRESSED) else
	  
	  if(ButtonAlt == PRESSED)  
	  {
		if(ButtonAltDebounceTime == UNDEFINED_DEBOUNCE_TIME) // This is the first time the interrupt detects button is pressed. ButtonAltDebounceTime has to make sure not reach UNDEFINED_DEBOUNCE_TIME!!
		{	
			ButtonAltDebounceTime = 0;
		}else
			{	
				if(ButtonAltDebounceTime < BUTTON_DEBOUNCE_TIME_FOR_TIMER1)
				{
					ButtonAltDebounceTime++;
					ButtonAltDebounced = NO;
				}		
				if(ButtonAltDebounceTime == BUTTON_DEBOUNCE_TIME_FOR_TIMER1)
				{
					if(ButtonAltDebounced == NO)
					{
						ButtonAltState = PRESSED;
					}
					ButtonAltDebounced = YES;
				}
			}// end of if(ButtonAltDebounceTime == UNDEFINED_DEBOUNCE_TIME) else
	  }else
			{
				if(ButtonAltDebounced == YES)
				{
					ButtonAltDebounced = NO;
					ButtonAltDebounceTime = UNDEFINED_DEBOUNCE_TIME;	
				}		
			}// end of if(ButtonAlt == PRESSED) else
	  

  
	/*------------------------------------------------------------------------------------------------------------------------------------------------
		Description: Blink LED at a frequency twice per second.
		1. If any of four LED blink property is on, this function gets executed and toggle the state of blinking LED to blink the LED.
		2. If none of LED blink property is on, this function is disabled.
		3. LedBlinkTimer can never reach the value of UNDEFINED_BLINK_TIMER(set to 200 here), otherwise the programme will regard the timer is undefined, i.e. blink function disabled while the blinking is actually running. Therefore LED_BLINK_TIME has to be less then UNDEFINED_BLINK_TIMER all the time.
		4. It has to be ensured that all of the digit states being synchronised at the moment any digit changes from not blinking to blinking.
		5. It has to be ensured that all of the blinking digit toggles with same transitions while blinking, i.e. all ON -> OFF, or all OFF -> ON.
		6. It has to be ensured that four digits value changed at the same time, i.e. 4-digit value only get updated after successful display of four digits once
	------------------------------------------------------------------------------------------------------------------------------------------------*/
 	DigitNumber ++;

	if (DigitNumber >= 5)
		DigitNumber = 1;
		
	if(DigitNumber == 1)
	{	

		if(	((LastCycleOfLedDigit1BlinkEn == LED_DIGIT_NOT_BLINK) && (LedDigit1BlinkEn == LED_DIGIT_BLINK    ))  ||
			((LastCycleOfLedDigit2BlinkEn == LED_DIGIT_NOT_BLINK) && (LedDigit2BlinkEn == LED_DIGIT_BLINK    ))  ||
			((LastCycleOfLedDigit3BlinkEn == LED_DIGIT_NOT_BLINK) && (LedDigit3BlinkEn == LED_DIGIT_BLINK    ))  ||
			((LastCycleOfLedDigit4BlinkEn == LED_DIGIT_NOT_BLINK) && (LedDigit4BlinkEn == LED_DIGIT_BLINK    ))  )
		{
			if(LedDigit1BlinkEn == LED_DIGIT_BLINK) LedDigit1En = LED_DIGIT_ON;
			if(LedDigit2BlinkEn == LED_DIGIT_BLINK) LedDigit2En = LED_DIGIT_ON;
			if(LedDigit3BlinkEn == LED_DIGIT_BLINK) LedDigit3En = LED_DIGIT_ON;
			if(LedDigit4BlinkEn == LED_DIGIT_BLINK) LedDigit4En = LED_DIGIT_ON;
		}	
		
		if(	((LastCycleOfLedDigit1BlinkEn == LED_DIGIT_BLINK) && (LedDigit1BlinkEn == LED_DIGIT_NOT_BLINK    ))  ||
			((LastCycleOfLedDigit2BlinkEn == LED_DIGIT_BLINK) && (LedDigit2BlinkEn == LED_DIGIT_NOT_BLINK    ))  ||
			((LastCycleOfLedDigit3BlinkEn == LED_DIGIT_BLINK) && (LedDigit3BlinkEn == LED_DIGIT_NOT_BLINK    ))  ||
			((LastCycleOfLedDigit4BlinkEn == LED_DIGIT_BLINK) && (LedDigit4BlinkEn == LED_DIGIT_NOT_BLINK    ))  )
		{
			if(LedDigit1BlinkEn == LED_DIGIT_NOT_BLINK) LedDigit1En = LED_DIGIT_OFF;
			if(LedDigit2BlinkEn == LED_DIGIT_NOT_BLINK) LedDigit2En = LED_DIGIT_OFF;
			if(LedDigit3BlinkEn == LED_DIGIT_NOT_BLINK) LedDigit3En = LED_DIGIT_OFF;
			if(LedDigit4BlinkEn == LED_DIGIT_NOT_BLINK) LedDigit4En = LED_DIGIT_OFF;		
		}
			
		LastCycleOfLedDigit1BlinkEn = LedDigit1BlinkEn;
		LastCycleOfLedDigit2BlinkEn = LedDigit2BlinkEn;
		LastCycleOfLedDigit3BlinkEn = LedDigit3BlinkEn;
		LastCycleOfLedDigit4BlinkEn = LedDigit4BlinkEn;	

		if( (LedDigit1BlinkEn == LED_DIGIT_BLINK) || (LedDigit2BlinkEn == LED_DIGIT_BLINK) || (LedDigit3BlinkEn == LED_DIGIT_BLINK) || (LedDigit4BlinkEn == LED_DIGIT_BLINK) )
		{
			if(LedBlinkTimer == UNDEFINED_BLINK_TIMER)
			{
				LedBlinkTimer = 0;
			}else
				{
					LedBlinkTimer++;
					if(LedBlinkTimer == LED_BLINK_TIME)
					{
						if( ((LedDigit1En == LED_DIGIT_ON) && (LedDigit1BlinkEn == LED_DIGIT_BLINK) ) ||
							((LedDigit2En == LED_DIGIT_ON) && (LedDigit2BlinkEn == LED_DIGIT_BLINK) ) ||
							((LedDigit3En == LED_DIGIT_ON) && (LedDigit3BlinkEn == LED_DIGIT_BLINK) ) ||
							((LedDigit4En == LED_DIGIT_ON) && (LedDigit4BlinkEn == LED_DIGIT_BLINK) )  )
						{
							BlinkReferenceLedState = LED_DIGIT_ON;
						}else
							{
								BlinkReferenceLedState = LED_DIGIT_OFF;				
							}

						if(LedDigit1BlinkEn == LED_DIGIT_BLINK) 
							{if(BlinkReferenceLedState == LED_DIGIT_ON){LedDigit1En = LED_DIGIT_OFF;}else{ if(BlinkReferenceLedState == LED_DIGIT_OFF) LedDigit1En = LED_DIGIT_ON;}}
						if(LedDigit2BlinkEn == LED_DIGIT_BLINK) 
							{if(BlinkReferenceLedState == LED_DIGIT_ON){LedDigit2En = LED_DIGIT_OFF;}else{ if(BlinkReferenceLedState == LED_DIGIT_OFF) LedDigit2En = LED_DIGIT_ON;}}
						if(LedDigit3BlinkEn == LED_DIGIT_BLINK) 
							{if(BlinkReferenceLedState == LED_DIGIT_ON){LedDigit3En = LED_DIGIT_OFF;}else{ if(BlinkReferenceLedState == LED_DIGIT_OFF) LedDigit3En = LED_DIGIT_ON;}}
						if(LedDigit4BlinkEn == LED_DIGIT_BLINK) 
							{if(BlinkReferenceLedState == LED_DIGIT_ON){LedDigit4En = LED_DIGIT_OFF;}else{ if(BlinkReferenceLedState == LED_DIGIT_OFF) LedDigit4En = LED_DIGIT_ON;}}
	  
						LedBlinkTimer = 0;
					}
				}// end of if(LedBlinkTimer == UNDEFINED_BLINK_TIMER)
		}else
			{
				LedBlinkTimer = UNDEFINED_BLINK_TIMER;
			}//end of if( (LedDigit1En == LED_DIGIT_BLINK) || (LedDigit2En == LED_DIGIT_BLINK) || (LedDigit3En == LED_DIGIT_BLINK) || (LedDigit4En == LED_DIGIT_BLINK) )

	}// end of if(DigitNumber == 1)	
 	
	/*------------------------------------------------------------------------------------------------------------------------------------------------
		Description: Display LED dynamically.
		The timer interrupt happens every 5ms. And therefore display each LED for 5ms will have the a 20ms loop, i.e., digit1 for 5ms, and then digit2 for 5ms, and then digit3 for 5ms, and then digit4 for 5ms.  
		In this time setting, the LEDs are not multiplexing seemed by human eyes.
		N.B. Display function can never be turned off as there is no other function detecting the transition of LED states and call this function. 
		Once it is disabled by detected the static state of all LED states, it could not change the LED on to LED off.
	------------------------------------------------------------------------------------------------------------------------------------------------*/	

		switch(DigitNumber)
		{
			case 1 :	LedDigit4Switch = LED_DIGIT_OFF;
						LedDigitDisplay = LedDisplayTable [LedDigit1];
						if(LedDigit1En == LED_DIGIT_ON) {LedDigit1Switch = LED_DIGIT_ON;}
							else{if(LedDigit1En == LED_DIGIT_OFF) LedDigit1Switch = LED_DIGIT_OFF;}
						break;
						
			case 2 :	LedDigit1Switch = LED_DIGIT_OFF;
						LedDigitDisplay = LedDisplayTable [LedDigit2];
						if(LedDigit2En == LED_DIGIT_ON) {LedDigit2Switch = LED_DIGIT_ON;}
							else{if(LedDigit2En == LED_DIGIT_OFF) LedDigit2Switch = LED_DIGIT_OFF;}
						break;
						
			case 3 :	LedDigit2Switch = LED_DIGIT_OFF;
						LedDigitDisplay = LedDisplayTable [LedDigit3];
						if(LedDigit3En == LED_DIGIT_ON) {LedDigit3Switch = LED_DIGIT_ON;}
							else{if(LedDigit3En == LED_DIGIT_OFF) LedDigit3Switch = LED_DIGIT_OFF;}
						break;
						
			case 4 :	LedDigit3Switch = LED_DIGIT_OFF;
						LedDigitDisplay = LedDisplayTable [LedDigit4];
						if(LedDigit4En == LED_DIGIT_ON) {LedDigit4Switch = LED_DIGIT_ON;}
				 			else{if(LedDigit4En == LED_DIGIT_OFF) LedDigit4Switch = LED_DIGIT_OFF;}
						break;
					
		}// end of switch(DigitNumber)
		
		
   // To measure how long does the program takes by observe the waveform of test LED in an oscillator
   // Results: It took about 40us, and therefore the duty cycle is about less than 1% by measured. Theoritically, the duty cycle is 40us/5000us = 0.8%
	TestLedState = TEST_LED_OFF;
}
