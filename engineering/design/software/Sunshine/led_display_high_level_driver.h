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

void LedDigitControl(unsigned char LedDigitNumber, unsigned char LedDigitState, unsigned char LedDigitChar);

void DisplayStaticTimer(unsigned int NumberOfSecondRadiatorOn_Local);

void DisplayBlinkingTimer (unsigned int NumberOfSecondRadiatorOn_Local);

void DisplayStaticTemperature (unsigned char TemperatureSet_Local);

void DisplayBlinkingTemperature (unsigned char TemperatureSet_Local);

void DisplayTimer(unsigned int NumberOfSecondRadiatorOn_Local, unsigned char BlinkOrStatic);

void TurnOffLedDisplay(void);	
