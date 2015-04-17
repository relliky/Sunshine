
#include<reg51.h>
#include <intrins.h>


#define uchar unsigned char

#define DEBOUNCED_TIME 10

#define RELEASED	   1
#define PRESSED		   0
#define TEST_LED_ON	   0
#define TEST_LED_OFF   1
#define SHOWING_SUCCESS_TIME  1000

sbit ButtonMinus	 = P1^4;
sbit ButtonPlus		 = P1^5;
sbit ButtonSelect	 = P1^6;
sbit ButtonAlt		 = P1^7;

sbit TestLedState	 = P3^7;

unsigned int ButtonMinusState	  =	 RELEASED;
unsigned int ButtonPlusState	  =	 RELEASED;
unsigned int ButtonSelectState	  =	 RELEASED;
unsigned int ButtonAltState		  =	 RELEASED;

unsigned int i,j,k;


void Delay1ms(unsigned int i)
{
  for(j=i;j>0;j--)
	for(k=125;k>0;k--);
}

/*
	Description: Scan and debounce the keys in software for the minus button, plus button, select button, and alternative button.
*/
void ScanKeys()
{
  if(ButtonMinus == PRESSED)  Delay1ms(DEBOUNCED_TIME);
  if(ButtonMinus == PRESSED)  ButtonMinusState = PRESSED;
 
  if(ButtonPlus == PRESSED)  Delay1ms(DEBOUNCED_TIME);
  if(ButtonPlus == PRESSED)  ButtonPlusState = PRESSED;
 
  if(ButtonSelect == PRESSED)  Delay1ms(DEBOUNCED_TIME);
  if(ButtonSelect == PRESSED)  ButtonSelectState = PRESSED;
 
  if(ButtonAlt == PRESSED)  Delay1ms(DEBOUNCED_TIME);
  if(ButtonAlt == PRESSED)  ButtonAltState = PRESSED;
 
}

void main()
{


 while(1){
 
	 ScanKeys();
	 
	 if ((ButtonMinusState == PRESSED) || (ButtonPlusState == PRESSED) || (ButtonSelectState == PRESSED) || (ButtonAltState == PRESSED) )
	 {
	    //--- Code for testing --------
		TestLedState = TEST_LED_ON;
		Delay1ms(SHOWING_SUCCESS_TIME);
        //--- End of code for testing --------

		ButtonMinusState = RELEASED;
		ButtonPlusState = RELEASED;
		ButtonSelectState = RELEASED;
		ButtonAltState = RELEASED;

		ButtonMinus = RELEASED;
		ButtonPlus = RELEASED;
		ButtonSelect = RELEASED;
		ButtonAlt = RELEASED;
		
		
     }else{
			//--- Code for testing --------
	 		TestLedState = TEST_LED_OFF;
			//--- End of code for testing --------
			}
 }
}