#ifndef LED_DISPLAY_HIGH_LEVEL_DRIVER_H
	#include <led_display_high_level_driver.h>
	#define LED_DISPLAY_HIGH_LEVEL_DRIVER_H
#endif

/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Display Timer on LED screen with always on setting, mainly used in USER_INPUT_STATE_NORMAL_DISPLAY
				 1. For any extra time greater than 0 min but less then 30 min period, time will be rounded up. 
					For example, 1:27:30 will be display as 1.5h, but 1:30:00 will also be displayed as 1.5h.
	Input:		 unsigned int NumberOfSecondRadiatorOn_Local
  --------------------------------------------------------------------------------------------------------------------------------------------*/
void DisplayStaticTimer(unsigned int NumberOfSecondRadiatorOn_Local)
{
	DisplayTimer(NumberOfSecondRadiatorOn_Local, LED_DIGIT_ON);
} // end of void DisplayStaticTimer (unsigned char NumberOfHalfAnHourRadiatorOn_Local)


/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Display Timer on LED screen with blinking numbers, mainly used in USER_INPUT_STATE_SET_TIME
				 1. For any extra time greater than 0 min but less then 30 min period, time will be rounded up. 
					For example, 1:27:30 will be display as 1.5h, but 1:30:00 will also be displayed as 1.5h.
	Input:		 unsigned int NumberOfSecondRadiatorOn_Local
  --------------------------------------------------------------------------------------------------------------------------------------------*/
void DisplayBlinkingTimer (unsigned int NumberOfSecondRadiatorOn_Local)
{
	DisplayTimer(NumberOfSecondRadiatorOn_Local, LED_DIGIT_BLINK);
} // end of void DisplayBlinkingTimer (unsigned char NumberOfHalfAnHourRadiatorOn_Local)


/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Display Timer on LED screen with blinking/always numbers, mainly used in USER_INPUT_STATE_SET_TIME
				 1. For any extra time greater than 0 min but less then 30 min period, time will be rounded up. 
					For example, 1:27:30 will be display as 1.5h, but 1:30:00 will also be displayed as 1.5h.
	Input:		 unsigned int NumberOfSecondRadiatorOn_Local
				 unsigned char BlinkOrStatic
  --------------------------------------------------------------------------------------------------------------------------------------------*/
void DisplayTimer(unsigned int NumberOfSecondRadiatorOn_Local, unsigned char BlinkOrStatic)
{
	unsigned int  NumberOfHalfAnHourRadiatorOn_Local;
	unsigned char Tens;
	unsigned char Units;
	unsigned char Tenths;
	
	
	NumberOfHalfAnHourRadiatorOn_Local = NumberOfSecondRadiatorOn_Local/30/60;
	
	if(NumberOfSecondRadiatorOn_Local - NumberOfHalfAnHourRadiatorOn_Local*30*60 > 0)
		NumberOfHalfAnHourRadiatorOn_Local++;
	
	Tens   = NumberOfHalfAnHourRadiatorOn_Local/2/10;
	Units  = NumberOfHalfAnHourRadiatorOn_Local/2%10;
    Tenths = NumberOfHalfAnHourRadiatorOn_Local%2*5;
 
	if(Tens != 0)
	{
		LedDigitControl(1, BlinkOrStatic, Tens	);
	}else
		{
			LedDigitControl(1, LED_DIGIT_OFF, NOTHING		);
		}
	LedDigitControl(2, BlinkOrStatic, Units + DECIMAL_POINT	);
	LedDigitControl(3, BlinkOrStatic, Tenths				);
	LedDigitControl(4, BlinkOrStatic, DIGIT_H				);
}

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

/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Turn off LED display when system is idle/sleep
	Input:		 N.A.
  --------------------------------------------------------------------------------------------------------------------------------------------*/
void TurnOffLedDisplay(void)
{
	LedDigitControl(1, LED_DIGIT_OFF  , NOTHING	);
	LedDigitControl(2, LED_DIGIT_OFF  , NOTHING	);
	LedDigitControl(3, LED_DIGIT_OFF  , NOTHING	);
	LedDigitControl(4, LED_DIGIT_OFF  , NOTHING	);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Changing a state of a LED digit
	Input:		 unsigned char LedDigitNumber , i.e. which LED digit is controlled by this function)
	             unsigned char LedDigitChar   , i.e. what character is sent to this digit
				 unsigned char LedDigitState  , i.e. what state is for this LED digit: ON/OFF/BLINK
	N.B. For some reason, Keil uVersion2 does not compile one of the assignment to assembly even though there is no any brunch before.
		 Therefore, when using Keil to simulate the program running, breakpoints cannot be set at these assignments in C.		 
		 More specifically, LedDigit1BlinkEn = LED_DIGIT_NOT_BLINK; in the LedDigitState == LED_DIGIT_ON is not regarded as compiled, which means there is no correlated assembly code there and hence an user cannot set a breakpoint there..
		 And therefore LED display could not display well when digit state changed from ON/OFF to blink or otherwise.
		 Solution is an update of the compiler. Using latest version of Keil solved the issue of code not getting compiled, i.e. Keil uVersion5.
  --------------------------------------------------------------------------------------------------------------------------------------------*/
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
