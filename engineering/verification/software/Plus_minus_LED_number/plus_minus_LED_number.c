#include <reg52.h>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char

#define INTERRUPT_ON    					1
#define INTERRUPT_OFF   					0 
#define TMOD_TIMER0_16BIT_TIMER1_16BIT 		   	0x11  
// CYCLE_FOR_TIMER1_UNDER_11_0952MHZ (5ms)= (5000us)/(1/11.0592 us/clock)/12 clock/machine_cycle = 5000*11.0592/12=4608    
#define CYCLE_FOR_TIMER1_UNDER_11_0952MHZ 	4608  


#define TEST_LED_ON	   0
#define TEST_LED_OFF   1

#define LED_DIGIT_ON     1
#define LED_DIGIT_OFF    0

#define DEBOUNCED_TIME 10
#define UNDEFINED_DEBOUNCE_TIME 99
// The button debounce time is 10ms, timer1 interrupt happened every 5ms, and therefore BUTTON_DEBOUNCE_TIME_FOR_TIMER1 is 2 here.
#define BUTTON_DEBOUNCE_TIME_FOR_TIMER1 2 

#define RELEASED	   1
#define PRESSED		   0
#define TEST_LED_ON	   0
#define TEST_LED_OFF   1
#define DIGIT_H        16


#define RADIATOR_MAX_HOURS 12

#define YES 1
#define NO  0

sbit LedDigit1Switch=P1^0;
sbit LedDigit2Switch=P1^1;
sbit LedDigit3Switch=P1^2;
sbit LedDigit4Switch=P1^3;

sfr  LedDigitDisplay = 0xA0; //P2 declearation

sbit TestLedState	 = P3^7;

unsigned char LedDigit1 = 8;
unsigned char LedDigit2 = 8;
unsigned char LedDigit3 = 8;
unsigned char LedDigit4 = 8;
unsigned char LedDigit1En = LED_DIGIT_OFF;
unsigned char LedDigit2En = LED_DIGIT_OFF;
unsigned char LedDigit3En = LED_DIGIT_OFF;
unsigned char LedDigit4En = LED_DIGIT_OFF;
unsigned char DigitNumber = 1;

sbit ButtonMinus	 = P1^4;
sbit ButtonPlus		 = P1^5;
sbit ButtonSelect	 = P1^6;
sbit ButtonAlt		 = P1^7;


unsigned char ButtonMinusState	  =	 RELEASED;
unsigned char ButtonPlusState	  =	 RELEASED;
unsigned char ButtonSelectState	  =	 RELEASED;
unsigned char ButtonAltState	  =	 RELEASED;

unsigned char ButtonPlusDebounceTime    = UNDEFINED_DEBOUNCE_TIME;
unsigned char ButtonMinusDebounceTime   = UNDEFINED_DEBOUNCE_TIME;
unsigned char ButtonSelectDebounceTime  = UNDEFINED_DEBOUNCE_TIME;
unsigned char ButtonAltDebounceTime  	= UNDEFINED_DEBOUNCE_TIME;

unsigned char ButtonPlusDebounced  		= NO;
unsigned char ButtonMinusDebounced  	= NO;
unsigned char ButtonSelectDebounced  	= NO;
unsigned char ButtonAltDebounced  		= NO;


const unsigned char LedDisplayTable[]={~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,	    	// Table for 7-segment LED to display 0-F,h
                        ~0x07,~0x7f,~0x6f,~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71,~0x74};


unsigned char RadiatorOnTime = 0;
unsigned char TimerTenthsDigit = 0;
unsigned char TimerUnitsDigit = 0;
unsigned char TimerTensDigit = 0;



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
	
	
	
	
	
 while(1)
 {
 
	TimerTensDigit  = RadiatorOnTime%100/10;
	TimerUnitsDigit = RadiatorOnTime%10;
    TimerTenthsDigit = 5;
 
 
    LedDigit1En = LED_DIGIT_ON;
	LedDigit1 = TimerTensDigit;
    LedDigit2En = LED_DIGIT_ON;
	LedDigit2 = TimerUnitsDigit;
    LedDigit3En = LED_DIGIT_ON;
	LedDigit3 = TimerTenthsDigit;
 
    LedDigit4En = LED_DIGIT_ON;
	LedDigit4 = DIGIT_H;
	
    
	if(ButtonPlusState == PRESSED)
	{
		if(RadiatorOnTime <= RADIATOR_MAX_HOURS-1)
		{
			RadiatorOnTime++;
		}else{RadiatorOnTime = RADIATOR_MAX_HOURS;}
		
		ButtonPlusState = RELEASED;
	}
	
	if(ButtonMinusState == PRESSED)
	{
		if(RadiatorOnTime != 0)
			RadiatorOnTime--;	
		ButtonMinusState = RELEASED;
	}	
	

 }// end of while(1) loop


 return(0);
}


void Timer5ms_with_ID_timer1() interrupt 3
{
   // To measure how long does the program takes by observe the waveform of test LED in an oscillator
   // Results: It took about 40us, and therefore the duty cycle is about less than 1% by measured. Theoretically, the duty cycle is 40us/5000us = 0.8%
    TestLedState = TEST_LED_ON;

	TH1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)/0xFF;  
	TL1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)%0xFF;

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
  
 

	if( !((LedDigit1En == LED_DIGIT_OFF) && (LedDigit2En == LED_DIGIT_OFF) && (LedDigit3En == LED_DIGIT_OFF) && (LedDigit4En == LED_DIGIT_OFF)) )
	{
		DigitNumber ++;
		if (DigitNumber >= 5)
			DigitNumber = 1;
			
		switch(DigitNumber)
		{
			case 1 :	LedDigitDisplay = LedDisplayTable [LedDigit1];
						LedDigit4Switch = LED_DIGIT_OFF;
						if(LedDigit1En == LED_DIGIT_ON) 
							{LedDigit1Switch = LED_DIGIT_ON;}
							else{LedDigit1Switch = LED_DIGIT_OFF;}
						break;
						
			case 2 :	LedDigitDisplay = LedDisplayTable [LedDigit2];
						LedDigit1Switch = LED_DIGIT_OFF;
						if(LedDigit2En == LED_DIGIT_ON) 
							{LedDigit2Switch = LED_DIGIT_ON;}
							else{LedDigit2Switch = LED_DIGIT_OFF;}
						break;

			case 3 :	LedDigitDisplay = LedDisplayTable [LedDigit3];
						LedDigit2Switch = LED_DIGIT_OFF;
						if(LedDigit3En == LED_DIGIT_ON) 
							{LedDigit3Switch = LED_DIGIT_ON;}
							else{LedDigit3Switch = LED_DIGIT_OFF;}
						break;

			case 4 :	LedDigitDisplay = LedDisplayTable [LedDigit4];
						LedDigit3Switch = LED_DIGIT_OFF;
						if(LedDigit4En == LED_DIGIT_ON) 
							{LedDigit4Switch = LED_DIGIT_ON;}
							else{LedDigit4Switch = LED_DIGIT_OFF;}
						break;						
					
		}// end of switch(DigitNumber)


	}// end of if( (LedDigit1 == DIGIT_LED_OFF) && (LedDigit2 == DIGIT_LED_OFF) && (LedDigit3 == DIGIT_LED_OFF) && (LedDigit4 == DIGIT_LED_OFF) )
   
   
   
   

   // To measure how long does the program takes by observe the waveform of test LED in an oscillator
   // Results: It took about 40us, and therefore the duty cycle is about less than 1% by measured. Theoritically, the duty cycle is 40us/5000us = 0.8%
	TestLedState = TEST_LED_OFF;
}
