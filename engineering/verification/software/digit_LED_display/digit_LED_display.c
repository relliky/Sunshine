// This will display on LED "36Co"

#include <reg52.h>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char

#define INTERRUPT_ON    					1
#define INTERRUPT_OFF   					0 
#define TIMER0_16BIT_TIMER1_16BIT 		   	0x11  
// CYCLE_FOR_TIMER1_UNDER_11_0952MHZ (5ms)= (5000us)/(1/11.0592 us/clock)/12 clock/machine_cycle = 5000*11.0592/12=4608    
#define CYCLE_FOR_TIMER1_UNDER_11_0952MHZ 	4608  


#define TEST_LED_ON	   0
#define TEST_LED_OFF   1

// 7-segment LED cathode inverted value, ON has to be 0x1, OFF has to be 0x0.
#define LED_DIGIT_OFF      	0x0
#define LED_DIGIT_ON       	0x1
#define LED_DIGIT_BLINK    	0x2
#define LED_DIGIT_NOT_BLINK 0x3

// LED blinks twice per 1s, which means LED needs to be enable and disable every 500ms, and the time between enable and disable is 250ms, which is 50 times of 5ms timer interrupt
// N.B. LedBlinkTimer can never reach the value of UNDEFINED_BLINK_TIMER(set to 200 here), otherwise the programme will regard the timer is undefined, i.e. blink function disabled while the blinking is actually running.
// Therefore there is LED_BLINK_TIME < UNDEFINED_BLINK_TIMER all the time.
// UNDEFINED_BLINK_TIMER cannot be over 127 since unsigned char is regarded with maximum value of 127 by the compiler
#define LED_BLINK_TIME 25
#define UNDEFINED_BLINK_TIMER 120

#define DIGIT_H        16
#define DIGIT_C        0xC
#define DIGIT_DEGREE   17
#define DECIMAL_NUMBER 18
#define NOTHING		   28

sbit LedDigit1Switch=P1^0;
sbit LedDigit2Switch=P1^1;
sbit LedDigit3Switch=P1^2;
sbit LedDigit4Switch=P1^3;

sfr  LedDigitDisplay = 0xA0; //P2 declearation

sbit TestLedState	 = P3^7;

unsigned char LedDigit1 		= 8;
unsigned char LedDigit2 		= 8;
unsigned char LedDigit3 		= 8;
unsigned char LedDigit4 		= 8;
unsigned char LedDigit1En 		= LED_DIGIT_OFF;
unsigned char LedDigit2En 		= LED_DIGIT_OFF;
unsigned char LedDigit3En 		= LED_DIGIT_OFF;
unsigned char LedDigit4En 		= LED_DIGIT_OFF;
unsigned char DigitNumber 		= 1;
unsigned int LedBlinkTimer 	= UNDEFINED_BLINK_TIMER;
unsigned char LedDigit1BlinkEn 	= LED_DIGIT_NOT_BLINK;
unsigned char LedDigit2BlinkEn 	= LED_DIGIT_NOT_BLINK;
unsigned char LedDigit3BlinkEn 	= LED_DIGIT_NOT_BLINK;
unsigned char LedDigit4BlinkEn 	= LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit1BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit2BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit3BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit4BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LedDigit1State 	= LED_DIGIT_OFF;
unsigned char LedDigit2State 	= LED_DIGIT_OFF;
unsigned char LedDigit3State 	= LED_DIGIT_OFF;
unsigned char LedDigit4State 	= LED_DIGIT_OFF;
unsigned char BlinkReferenceLedState = 0x3;


const unsigned char LedDisplayTable[]={~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,	    	// Table for 7-segment LED to display 0-F,h,o(degree),"0." "1." "2." "3." "4." "5." "6." "7." "8." "9." and OFF.
							~0x07,~0x7f,~0x6f,~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71,
							~0x74,~0x63,
							~(0x3f+0x80),~(0x06+0x80),~(0x5b+0x80),~(0x4f+0x80),
							~(0x66+0x80),~(0x6d+0x80),~(0x7d+0x80),~(0x07+0x80),
							~(0x7f+0x80),~(0x6f+0x80),
							~(0x00)};


int ind = 0;

int test_slow_counter = 0;

void LedDigitControl(unsigned char LedDigitNumber, unsigned char LedDigitState, unsigned char LedDigitChar);

/******************************************************************/
/*                   main                                       */
/******************************************************************/
int main(void)
{

 
	long  time_t;
//	double int flag_t;    


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
	TMOD = TIMER0_16BIT_TIMER1_16BIT;

  ET1  = INTERRUPT_ON;
	TH1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)/256;  // CYCLE_FOR_10MS_UNDER_11_0952MHZ = (10000us)/(1/11.0592 us/clock)/12 clock/machine_cycle = 10000*11.0592/12=9216 
	TL1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)%256;
	TR1  = INTERRUPT_ON;
	
	
	
	
	
 while(1)
 {
//    LedDigit1En = LED_DIGIT_ON;
// 	 LedDigit1 = DIGIT_H;
//    LedDigit2En = LED_DIGIT_ON;
//	LedDigit2 = DIGIT_DEGREE;
//   LedDigit3En = LED_DIGIT_ON;
//	LedDigit3 = DIGIT_C;
//   LedDigit4En = LED_DIGIT_ON;
//	LedDigit4 = (DECIMAL_NUMBER+5);
	
//	LedDigit1BlinkEn = LED_DIGIT_BLINK;
//	LedDigit2BlinkEn = LED_DIGIT_BLINK;
//	LedDigit3BlinkEn = LED_DIGIT_BLINK;

	/*	All of the trasitions have to be tested. 
		OFF -> Blink -> OFF, 
		ON -> Blink -> ON, 
		ON -> OFF -> ON
	*/
  	for(time_t = 0;  time_t<0xFFFF/2*50; time_t++ )
	{
		if(time_t >0xFFFF/4*50)
		{
			LedDigitControl(1, LED_DIGIT_OFF  , NOTHING		);
			LedDigitControl(2, LED_DIGIT_ON	  , 2		 	);
			LedDigitControl(3, LED_DIGIT_ON   , 7 			);
			LedDigitControl(4, LED_DIGIT_OFF  , NOTHING	 	);
	 	}else
		{
			LedDigitControl(1, LED_DIGIT_BLINK, 4			);
			LedDigitControl(2, LED_DIGIT_BLINK, 5 			);
			LedDigitControl(3, LED_DIGIT_OFF  , NOTHING	 	);
			LedDigitControl(4, LED_DIGIT_BLINK, 2		 	);
	 	}
	
	}

 
 
	// 4 ON -> Blink -> ON, 
/*  	for(time_t = 0;  time_t<0xFFFF/2*50; time_t++ )
	{
		if(time_t >0xFFFF/4*50)
		{
			LedDigitControl(1, LED_DIGIT_ON  , 1		);
			LedDigitControl(2, LED_DIGIT_ON	  , 2		 	);
			LedDigitControl(3, LED_DIGIT_ON   , 3 			);
			LedDigitControl(4, LED_DIGIT_ON   , 4	 	);
	 	}else
		{
			LedDigitControl(1, LED_DIGIT_BLINK, 5			);
			LedDigitControl(2, LED_DIGIT_BLINK, 6 			);
			LedDigitControl(3, LED_DIGIT_BLINK  , 7	 	);
			LedDigitControl(4, LED_DIGIT_BLINK, 8		 	);
	 	}
	
	}
 */
 
/* 	LedDigitControl(1, LED_DIGIT_BLINK, 3			);
	LedDigitControl(2, LED_DIGIT_BLINK, 6			);
	LedDigitControl(3, LED_DIGIT_ON   , DIGIT_C		);
	LedDigitControl(4, LED_DIGIT_ON	  , DIGIT_DEGREE);
 */	
/* 	
			LedDigitControl(2, LED_DIGIT_BLINK, 1			);
			LedDigitControl(1, LED_DIGIT_OFF  , NOTHING 	);
			LedDigitControl(3, LED_DIGIT_OFF  , NOTHING 	);
			LedDigitControl(4, LED_DIGIT_OFF  , NOTHING 	);
 */
 }// end of while(1) loop

}

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


void Timer5ms_with_ID_timer1(void) interrupt 3
{

	TH1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)/256;  
	TL1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)%256;
	
// For test usage, slow the function down 100 times. Each LED digit shows 500ms
//	test_slow_counter++;
//	if(test_slow_counter == 10)
//	{ 	test_slow_counter = 0;
// end of code for test usage.
	 DigitNumber ++;
//	}

	if (DigitNumber >= 5)
		DigitNumber = 1;

	ind ++;
	
    if(ind == 100)
	{
		TestLedState = TEST_LED_ON;
	}	

	if(ind == 200)
	{
		TestLedState = TEST_LED_OFF;
		ind = 0;
	}

	/*------------------------------------------------------------------------------------------------------------------------------------------------
	Description: Blink LED at a frequency twice per second.
	1. If any of four LED blink property is on, this function gets executed and toggle the state of blinking LED to blink the LED.
	2. If none of LED blink property is on, this function is disabled.
	3. LedBlinkTimer can never reach the value of UNDEFINED_BLINK_TIMER(set to 200 here), otherwise the programme will regard the timer is undefined, i.e. blink function disabled while the blinking is actually running. Therefore LED_BLINK_TIME has to be less then UNDEFINED_BLINK_TIMER all the time.
	4. It has to be ensured that all of the digit states being synchronised at the moment any digit changes from not blinking to blinking.
	5. It has to be ensured that all of the blinking digit toggles with same transitions while blinking, i.e. all ON -> OFF, or all OFF -> ON.
	   It is likely that first digit blink from ON -> 
	------------------------------------------------------------------------------------------------------------------------------------------------*/

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

}// end of void Timer5ms_with_ID_timer1(void) interrupt 3
