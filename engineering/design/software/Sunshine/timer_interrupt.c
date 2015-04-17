#ifndef INTRINS_H
	#include <intrins.h>
	#define INTRINS_H
#endif

#ifndef REG52_H
	#include <reg52.h>	
	#define REG52_H
#endif	

#ifndef TIMER_INTERRUPT_H
	#include <timer_interrupt.h>
	#define TIMER_INTERRUPT_H
#endif
	
	
/* --------------------------------------------------------------------------------------------------------------------------------------------
   STC80c52 pin-out, pins/ports declarations 
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// Variables visible from outside world 
/*----------------N.A.------------------*/
// Variables invisible from outside world 
// Port 0
/*----------------N.A.------------------*/
// Port 1
sbit LedDigit1Switch = P1^0;
sbit LedDigit2Switch = P1^1;
sbit LedDigit3Switch = P1^2;
sbit LedDigit4Switch = P1^3;
sbit ButtonMinus	 = P1^4;
sbit ButtonPlus		 = P1^5;
sbit ButtonSelect	 = P1^6;
sbit ButtonAlt		 = P1^7;
// Port 2
sfr  LedDigitDisplay = 0xA0; //Port declaration
// Port 3
sbit StepperPhase1Switch  	= P3^2;
sbit StepperPhase2Switch  	= P3^3;
sbit StepperPhase3Switch  	= P3^4;
sbit StepperPhase4Switch  	= P3^5;
sbit TestLedState	 		= P3^7;

 
/* --------------------------------------------------------------------------------------------------------------------------------------------
	LED display variables declarations 
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// Variables visible from outside world
unsigned char LedDigit1   	   = 8;
unsigned char LedDigit2   	   = 8;
unsigned char LedDigit3   	   = 8;
unsigned char LedDigit4   	   = 8;
bit  	   	  LedDigit1En	   = LED_DIGIT_OFF;
bit  	   	  LedDigit2En      = LED_DIGIT_OFF;
bit  	   	  LedDigit3En 	   = LED_DIGIT_OFF;
bit   	   	  LedDigit4En 	   = LED_DIGIT_OFF;
unsigned char LedDigit1BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LedDigit2BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LedDigit3BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LedDigit4BlinkEn = LED_DIGIT_NOT_BLINK;
// Variables invisible from outside world 
unsigned char LastCycleOfLedDigit1BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit2BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit3BlinkEn = LED_DIGIT_NOT_BLINK;
unsigned char LastCycleOfLedDigit4BlinkEn = LED_DIGIT_NOT_BLINK;
bit 		  BlinkReferenceLedState      = LED_DIGIT_OFF;
unsigned char DigitNumber 	    		  = 1;
unsigned int  LedBlinkTimer     		  = UNDEFINED_BLINK_TIMER;
// Table for 7-segment LED to display 0-F,h,o(degree),"0." "1." "2." "3." "4." "5." "6." "7." "8." "9." and blank.
const unsigned code LedDisplayTable[]={
							~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,~0x07,~0x7f,~0x6f,  // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71,						  // A-F
							~0x74,~0x63, 			  									  // h,o(degree)
							~(0x3f+0x80),~(0x06+0x80),~(0x5b+0x80),~(0x4f+0x80),		  // "0." "1." "2." "3." 
							~(0x66+0x80),~(0x6d+0x80),~(0x7d+0x80),~(0x07+0x80),  		  // "4." "5." "6." "7."
							~(0x7f+0x80),~(0x6f+0x80),									  // "8." "9."
							~(0x00)};													  // blank


/* --------------------------------------------------------------------------------------------------------------------------------------------
	Buttons variables declarations
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// Variables visible from outside world 
bit ButtonMinusState	  	= RELEASED;
bit ButtonPlusState	 		= RELEASED;
bit ButtonSelectState	 	= RELEASED;
bit ButtonAltState	 		= RELEASED;
// Variables invisible from outside world 
bit ButtonPlusDebounced  	= NO;
bit ButtonMinusDebounced  	= NO;
bit ButtonSelectDebounced  	= NO;
bit ButtonAltDebounced  	= NO;
unsigned char  ButtonPlusDebounceTime    = UNDEFINED_DEBOUNCE_TIME;
unsigned char  ButtonMinusDebounceTime   = UNDEFINED_DEBOUNCE_TIME;
unsigned char  ButtonSelectDebounceTime  = UNDEFINED_DEBOUNCE_TIME;
unsigned char  ButtonAltDebounceTime   	 = UNDEFINED_DEBOUNCE_TIME;

/* --------------------------------------------------------------------------------------------------------------------------------------------
	Stepper motor variables declarations
	Comments: Time parameter set for stepper motor for one movement in timer1, i.e. the value deciding how long does it take stepper motor 
	move from the end to another end. The time is set as 5s, each timer interrupt is 5ms for timer1, and hence it is 1000 here.
	It is not constant here because the stepper driver function actually uses it as a parameter, which is able to adjust the time
	if necessary in later verification phase.
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// Variables visible from outside world 
unsigned int 		TimerSetForStepperLastingInOneMovementInTimer1 	= 10000;
unsigned int 		StepperRemainingCyclesOfTimer1Interrupt 		= 0;
bit 				StepperForwardStart								= NO;
bit 				StepperForwardStop								= NO;
bit 				StepperBackwardStart							= NO;
bit 				StepperBackwardStop								= NO;
// Variables invisible from outside world 
unsigned char 		StepperPhaseIndex								= STEPPER_INITIAL_PHASE;
bit			 		StepperEnable									= STEPPER_OFF;
bit		 			LastStateOfStepperEnable						= STEPPER_OFF;
bit					StepperRotationDirection						= STEPPER_FORWARD;

/* --------------------------------------------------------------------------------------------------------------------------------------------
	Global counters variables declarations
 --------------------------------------------------------------------------------------------------------------------------------------------*/
unsigned int OneSecCounterForTimer = 0;



void Timer_4_48ms_with_ID_timer0() interrupt 1
{
	TH0  = (0xFFFF - CYCLE_FOR_TIMER0_UNDER_11_0952MHZ)/0xFF;  
	TL0  = (0xFFFF - CYCLE_FOR_TIMER0_UNDER_11_0952MHZ)%0xFF;
	
	/* ------------------------------------------------------------------------------------------------------------------------------------------------
		Description: Low level driver to drive stepper motor moving forward/backward for amount of time, set by TimerSetForStepperLastingInOneMovementInTimer1
					 It will drive each phase to ON, and drive other phase to OFF every timer1 interrupt called, i.e. 4.48ms.
					 It is tested that the stepper motor will be driven to provide the maximum force to press the button with 4.48ms between phase changes.
					 1.	When StepperForwardStart contains the value of YES, the driver clear StepperForwardStart to NO and start to drive motor forward for a period, set by TimerSetForStepperLastingInOneMovementInTimer1.
					 2. StepperForwardStart must not be set to YES again until the StepperForwardStop is set YES and then back to NO. When StepperForwardStop is YES, this means the forward movement of the stepper motor has completed.
					 3. During the forward movement of the stepper, each phase of the stepper will be energised in order with interval of 4.48ms.
					 4. Note that the initial phase that stepper motor will energise actually is phase 2, i.e. STEPPER_INITIAL_PHASE+1, and it will stop at phase 2 (STEPPER_INITIAL_PHASE+1) when it completed a forward/backward movement.
					 5. Backward movement is similar to forward movement.
					 6. Since it is a low-level driver in a timer interrupt, and it has to handshake with high-level driver by using handshaking signals StepperForwardStart/StepperForwardStop for forward movements,StepperBackwardStart/StepperBackwardStop for backward movements,  See RunStepperForwardForTime, RunStepperBackwardForTime for detail of handshaking.
	
		Input:		
					unsigned int 	TimerSetForStepperLastingInOneMovementInTimer1;
					bit				StepperRotationDirection					  ;
					bit 			StepperForwardStart							  ;
					bit 			StepperForwardStop							  ;
					bit 			StepperBackwardStart						  ;
					bit 			StepperBackwardStop							  ;
		Output:
					
	  
	  
	  ------------------------------------------------------------------------------------------------------------------------------------------------*/
	if(StepperForwardStart == YES)
	{
		StepperForwardStart = NO;
		StepperForwardStop  = NO;
		StepperEnable = STEPPER_ON;
		StepperRotationDirection = STEPPER_FORWARD;
		StepperPhaseIndex = STEPPER_INITIAL_PHASE;
	}

	if(StepperBackwardStart == YES)
	{
		StepperBackwardStart = NO;
		StepperBackwardStop  = NO;
		StepperEnable = STEPPER_ON;
		StepperRotationDirection = STEPPER_BACKWARD;
		StepperPhaseIndex = STEPPER_INITIAL_PHASE;
	}
	
	if(StepperEnable == STEPPER_ON)
	{	
		if(LastStateOfStepperEnable == STEPPER_OFF)  
			StepperRemainingCyclesOfTimer1Interrupt = TimerSetForStepperLastingInOneMovementInTimer1;
		if(StepperRemainingCyclesOfTimer1Interrupt != 0) StepperRemainingCyclesOfTimer1Interrupt--;
			
		if(StepperRotationDirection == STEPPER_FORWARD)
		{
			StepperPhaseIndex++;
			if(StepperPhaseIndex == STEPPER_LAST_PHASE + 1) StepperPhaseIndex = STEPPER_INITIAL_PHASE;		
			if(StepperRemainingCyclesOfTimer1Interrupt == 0) 
			{
				if(StepperPhaseIndex == STEPPER_INITIAL_PHASE + 1)  // Stepper has to stop at the phase 0, otherwise, if it stops anywhere else and the motor might not drive gear.
				{
					StepperEnable 	   = STEPPER_OFF;
					StepperForwardStop = YES;
					StepperPhaseIndex  = STEPPER_PHASE_INDEX_ALL_OFF;
				}
			}
		}
		
		if(StepperRotationDirection == STEPPER_BACKWARD)
		{
			if(StepperPhaseIndex == STEPPER_INITIAL_PHASE) StepperPhaseIndex = STEPPER_LAST_PHASE + 1;		
			StepperPhaseIndex--;
			if(StepperRemainingCyclesOfTimer1Interrupt == 0) 
			{
				if(StepperPhaseIndex == STEPPER_INITIAL_PHASE + 1)
				{
					StepperEnable 	   = STEPPER_OFF;
					StepperBackwardStop = YES;
					StepperPhaseIndex  = STEPPER_PHASE_INDEX_ALL_OFF;
				}
			}
		}
		
	}else
		{
			StepperPhaseIndex = STEPPER_PHASE_INDEX_ALL_OFF;
		}// end of if(StepperEnable = STEPPER_ON)
	
	
	
	switch(StepperPhaseIndex)
	{
		case 1:
				StepperPhase1Switch = STEPPER_PHASE_ON;
				StepperPhase2Switch = STEPPER_PHASE_OFF;
				StepperPhase3Switch = STEPPER_PHASE_OFF;
				StepperPhase4Switch = STEPPER_PHASE_OFF;
				break;

		case 2:
				StepperPhase1Switch = STEPPER_PHASE_OFF;
				StepperPhase2Switch = STEPPER_PHASE_ON ;
				StepperPhase3Switch = STEPPER_PHASE_OFF;
				StepperPhase4Switch = STEPPER_PHASE_OFF;
				break;
	
		case 3:
				StepperPhase1Switch = STEPPER_PHASE_OFF;
				StepperPhase2Switch = STEPPER_PHASE_OFF;
				StepperPhase3Switch = STEPPER_PHASE_ON;
				StepperPhase4Switch = STEPPER_PHASE_OFF;
				break;	

		case 4:
				StepperPhase1Switch = STEPPER_PHASE_OFF;
				StepperPhase2Switch = STEPPER_PHASE_OFF;
				StepperPhase3Switch = STEPPER_PHASE_OFF;
				StepperPhase4Switch = STEPPER_PHASE_ON;
				break;	
				
		default:
				StepperPhase1Switch = STEPPER_PHASE_OFF;
				StepperPhase2Switch = STEPPER_PHASE_OFF;
				StepperPhase3Switch = STEPPER_PHASE_OFF;
				StepperPhase4Switch = STEPPER_PHASE_OFF;
				break;	
	}// end of switch(StepperPhaseIndex)

	LastStateOfStepperEnable = StepperEnable;	


}

							
void Timer5ms_with_ID_timer1() interrupt 3
{
   // To measure how long does the program takes by observe the waveform of test LED in an oscillator
   // Results: It took about 40us, and therefore the duty cycle is about less than 1% by measured. Theoretically, the duty cycle is 40us/5000us = 0.8%
    TestLedState = TEST_LED_ON;

	TH1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)/0xFF;  
	TL1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)%0xFF;

	// Counter for counting 1s
	OneSecCounterForTimer++;
	
	// Counter for counting down the time that 
	
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
							else{LedDigit1Switch = LED_DIGIT_OFF;}
						break;
						
			case 2 :	LedDigit1Switch = LED_DIGIT_OFF;
						LedDigitDisplay = LedDisplayTable [LedDigit2];
						if(LedDigit2En == LED_DIGIT_ON) {LedDigit2Switch = LED_DIGIT_ON;}
							else{LedDigit2Switch = LED_DIGIT_OFF;}
						break;
						
			case 3 :	LedDigit2Switch = LED_DIGIT_OFF;
						LedDigitDisplay = LedDisplayTable [LedDigit3];
						if(LedDigit3En == LED_DIGIT_ON) {LedDigit3Switch = LED_DIGIT_ON;}
							else{LedDigit3Switch = LED_DIGIT_OFF;}
						break;
						
			case 4 :	LedDigit3Switch = LED_DIGIT_OFF;
						LedDigitDisplay = LedDisplayTable [LedDigit4];
						if(LedDigit4En == LED_DIGIT_ON) {LedDigit4Switch = LED_DIGIT_ON;}
				 			else{LedDigit4Switch = LED_DIGIT_OFF;}
						break;
					
		}// end of switch(DigitNumber)
		
		
   // To measure how long does the program takes by observe the waveform of test LED in an oscillator
   // Results: It took about 40us, and therefore the duty cycle is about less than 1% by measured. Theoritically, the duty cycle is 40us/5000us = 0.8%
	TestLedState = TEST_LED_OFF;

		
	/* 			
		0.00745226s
		-
		0.00748372s
		=0.00003s = 0.03ms

		0.17415582s
		-
		0.17423611s

		= 0.00008s = 0.08ms 

		In total = 0.11ms

		Max Duty cycle = 0.11ms/5ms = 11/500 = 10/500 = 1/50 = 2%
			
		*/
	

		
}// end of void Timer5ms_with_ID_timer1() interrupt 3


