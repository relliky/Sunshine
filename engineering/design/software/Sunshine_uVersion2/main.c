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
	
#ifndef LED_DISPLAY_HIGH_LEVEL_DRIVER_H
	#include <led_display_high_level_driver.h>
	#define LED_DISPLAY_HIGH_LEVEL_DRIVER_H
#endif

#ifndef STEPPER_HIGH_LEVEL_DRIVER_H
	#include <stepper_high_level_driver.h>
	#define STEPPER_HIGH_LEVEL_DRIVER_H
#endif


// User input state macros
#define USER_INPUT_STATE_NORMAL_DISPLAY  	1
#define USER_INPUT_STATE_SET_TIME 			2
#define USER_INPUT_STATE_SET_TEMPERATURE  	3
#define USER_INPUT_STATE_IDLE			  	4

// Timer parameter
#define MAXIMUM_SECONDS_RADIATOR_ON (12*60*60)
#define SECONDS_IN_HALF_AN_HOUR 	(1*30*60)

// Clock parameters	
#define MAXIMUM_SECOND_FOR_A_MINUTE	60
#define MAXIMUM_MINUTE_FOR_AN_HOUR	60

// Temperature set parameters
#define MINIMUM_TEMPERATURE_SET 20
#define MAXIMUM_TEMPERATURE_SET 45

// Idle system transitions time parameters
#define SETTING_STATE_TO_NORMAL_DISPLAY_STATE_TIME 10
#define NORMAL_DISPLAY_STATE_TO_IDLE_STATE_TIME    20

unsigned int NumberOfSecondRadiatorOn = 0;

unsigned char SecondForClock		  = 0;
unsigned char MinuteForClock		  = 0;
unsigned char HourForClock		  	  = 0;	
unsigned char Display5sCounter        = 0;
unsigned char TemperatureSet		  = MINIMUM_TEMPERATURE_SET;
unsigned char IdleButtonTime 	      = 0;

unsigned char UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;

static unsigned int NumberOfButtonToPressByStepper = 0;

unsigned int TimeForPressingAButton			= 3000;



/******************************************************************/
/*                   main                                       */
/******************************************************************/
int main(void)
{
	

	/*  LedDigit1En = LED_DIGIT_OFF;
    LedDigit2En = LED_DIGIT_OFF;
    LedDigit3En = LED_DIGIT_OFF;
    LedDigit4En = LED_DIGIT_OFF;
	LedDigit1Switch = LED_DIGIT_OFF;
	LedDigit2Switch = LED_DIGIT_OFF;
	LedDigit3Switch = LED_DIGIT_OFF;
	LedDigit4Switch = LED_DIGIT_OFF;
 */
	
	// Timers interrupt initialisations 
	EA   = INTERRUPT_ON;
	TMOD = TMOD_TIMER0_16BIT_TIMER1_16BIT;
	
    ET0  = INTERRUPT_ON;
	TH0  = (0xFFFF - CYCLE_FOR_TIMER0_UNDER_11_0952MHZ)/0xFF;  
	TL0  = (0xFFFF - CYCLE_FOR_TIMER0_UNDER_11_0952MHZ)%0xFF;
	TR0  = INTERRUPT_ON;
	
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
	
	// Alternative press
	if(ButtonAltState == PRESSED) 
	{
		NumberOfButtonToPressByStepper++;
		ButtonAltState = RELEASED;	
	}	
	PressingButtonsByStepper(TimeForPressingAButton, &NumberOfButtonToPressByStepper);
	
	
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
		
		//Dimming screen counter when buttons are not pressed 
		IdleButtonTime++;
		if(IdleButtonTime >= NORMAL_DISPLAY_STATE_TO_IDLE_STATE_TIME) 
			IdleButtonTime = NORMAL_DISPLAY_STATE_TO_IDLE_STATE_TIME;
			

		// Timer counting down
		if(NumberOfSecondRadiatorOn != 0)	NumberOfSecondRadiatorOn--;
		
	}// end of if(OneSecCounterForTimer == COUNT_1S_FOR_5MS_TIMER)


	
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
	

	// Keep the LED display on when keys are being pressed
	if((ButtonPlusState == PRESSED) || (ButtonMinusState == PRESSED) || (ButtonSelectState == PRESSED) || (ButtonAltState == PRESSED) )
	{
		IdleButtonTime = 0;
	}
			

/* 	 
	Description: Setting the user input state machine. 
	1. The product will interface with users in four modes: normal display mode, setting time mode, setting temperature mode, idle mode.

	2. Normal display mode
		i)   When the product is powered on, the default mode is the normal display mode.
		ii)  It displays the time set and the temperature set
		iii) It displays the time set for 5s and then switches to display the temperature set for another 5s and then back to display the time set.
		iv)  when no any button is pressed for 20s, the user input state switches to the idle mode, LED display is off.

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
		x)   The user input state switches to the normal display mode 10s after none of four buttons has been pressed.
		xi)  Time will be shown as it is rounded up, for example, 20 min and counting down is shown as 0.5h.
		xii) When time is on 0s, "+" gives 30min and counting down, "-" keeps time 0.
		xiii)When time is between two whole times of half an hour, "+" results in more 30 min than the previous displayed rounded up number, 
			 "-" results in less 30 min than the previous displayed rounded up number, 
			 for example, 1:10 will be displayed as 1.5h, "+" will result in time 2:00, showing 2.0h on screen;
			 for another example,  1:10 will be displayed as 1.5h, "-" will result in time 1:00, showing 1.0h on screen. 
		xiv) When time is greater than 11:30, for example, 11:40, showing 12.0h on display, "+" will result in time from 11:40 to 12:00, but still showing 12.0h.
		
		
	4. Setting temperature mode
		i)   The temperature digits, i.e. first and second digits, are blinking at the frequency twice every 1s.
		ii)  The four digits are "xx.Co", where x is a number, "Co" is just a letter and a symbol standing for degree Celsius.
		iii) The first digit can be set to "2", "3", "4" only.
		iv)  The second digit can be set to "0" "1" "2" "3" "4" "5" "6" "7" "8" "9" only
		v)   The third digit can be set to letter "C" only
		vi)  The forth digit can be set to symbol degree "o" only
		vii) The temperature can be set to from 20Co to 40Co with each step 1Co by pressing plus button and minus button. For example "20Co" ->(press plus button) "21Co" -> (press plus button) "22Co", or "40Co" ->(press plus button) "40Co"
		viii)When the system is reset, the temperature is set to the default value, 20Co.
		ix)  The user input state switches to the normal display mode 10s after none of four buttons has been pressed.
		
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
					DisplayStaticTimer(NumberOfSecondRadiatorOn);
				}
				
			// Transition condition of next state
			if(ButtonSelectState == PRESSED) 
			{
				UserInputState = USER_INPUT_STATE_SET_TIME;
				ButtonSelectState = RELEASED;	
			}

			if(IdleButtonTime == NORMAL_DISPLAY_STATE_TO_IDLE_STATE_TIME)
			{
				UserInputState = USER_INPUT_STATE_IDLE;
				IdleButtonTime = 0;
			}			

			break;			
		}// end of case USER_INPUT_STATE_NORMAL_DISPLAY


		
		case USER_INPUT_STATE_SET_TIME:
		{
			// Action to be done in this state
			// If value of time is greater than MAXIMUM_SECONDS_RADIATOR_ON, it will be set to MAXIMUM_SECONDS_RADIATOR_ON.
			if(NumberOfSecondRadiatorOn >= MAXIMUM_SECONDS_RADIATOR_ON) NumberOfSecondRadiatorOn = MAXIMUM_SECONDS_RADIATOR_ON;
			
			if(ButtonPlusState == PRESSED)
			{
				ButtonPlusState = RELEASED;
				if(NumberOfSecondRadiatorOn > MAXIMUM_SECONDS_RADIATOR_ON - SECONDS_IN_HALF_AN_HOUR) 
					{NumberOfSecondRadiatorOn = MAXIMUM_SECONDS_RADIATOR_ON;}
					else
						{
							if( NumberOfSecondRadiatorOn == 0  )
							{
								NumberOfSecondRadiatorOn += SECONDS_IN_HALF_AN_HOUR;
							}else
								{
									NumberOfSecondRadiatorOn += SECONDS_IN_HALF_AN_HOUR;
									if(NumberOfSecondRadiatorOn%SECONDS_IN_HALF_AN_HOUR != 0 )	
										NumberOfSecondRadiatorOn = (NumberOfSecondRadiatorOn/SECONDS_IN_HALF_AN_HOUR + 1)*(SECONDS_IN_HALF_AN_HOUR);
								}					
						}
			}

			if(ButtonMinusState == PRESSED)
			{
				ButtonMinusState = RELEASED;

				if(NumberOfSecondRadiatorOn < SECONDS_IN_HALF_AN_HOUR) 
					{NumberOfSecondRadiatorOn = 0;}
					else
						{
							NumberOfSecondRadiatorOn -= SECONDS_IN_HALF_AN_HOUR;
							if(NumberOfSecondRadiatorOn%(30*60) != 0 )
							{
								NumberOfSecondRadiatorOn = (NumberOfSecondRadiatorOn/(30*60) + 1)*(30*60);
							}
						}

 			}
			
 			DisplayBlinkingTimer(NumberOfSecondRadiatorOn);
		
/* 		//------------Code for test NumberOfSecondRadiatorOn mechanism------------------------
			LedDigitControl(1, LED_DIGIT_ON	  , NumberOfSecondRadiatorOn%3600/1000);
			LedDigitControl(2, LED_DIGIT_ON   , NumberOfSecondRadiatorOn%3600/100%10);
			LedDigitControl(3, LED_DIGIT_ON	  ,	NumberOfSecondRadiatorOn%3600/10%10);
			LedDigitControl(4, LED_DIGIT_ON	  , NumberOfSecondRadiatorOn%3600%10);
		//--------------End of testing code----------------------------------------------
 */		
			// Transition condition of next state			
			if(ButtonSelectState == PRESSED)
			{
				UserInputState = USER_INPUT_STATE_SET_TEMPERATURE;
				ButtonSelectState = RELEASED;
			}

			if(IdleButtonTime == SETTING_STATE_TO_NORMAL_DISPLAY_STATE_TIME)
			{
				UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;
				IdleButtonTime = 0;
			}
			
			break;
		}// end of case USER_INPUT_STATE_SET_TIME

		case USER_INPUT_STATE_SET_TEMPERATURE:
		{
			// Action to be done in this state
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
			
			// Transition condition of next state					
			if(ButtonSelectState == PRESSED)
			{
				UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;
				ButtonSelectState = RELEASED;
			}

			if(IdleButtonTime == SETTING_STATE_TO_NORMAL_DISPLAY_STATE_TIME)
			{
				UserInputState = USER_INPUT_STATE_NORMAL_DISPLAY;
				IdleButtonTime = 0;
			}			

			break;
		}// end of case USER_INPUT_STATE_SET_TEMPERATURE

		case USER_INPUT_STATE_IDLE:
		{
			// Action to be done in this state
			TurnOffLedDisplay();
			
			// Transition condition of next state	
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

	
	
 
	

 }// end of while(1) loop
 

 return(0);
} // end of int main(void)





