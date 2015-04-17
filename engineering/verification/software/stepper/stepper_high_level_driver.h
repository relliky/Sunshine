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



bit RunStepperForwardForTime( unsigned int RunningTime );

bit RunStepperBackwardForTime( unsigned int RunningTime );

bit RunStepperForwardAndBackwardForTime ( unsigned int RunningTimeForTwoCycle);

void PressingButtonsByStepper( unsigned int RunningTimeForTwoCycle, unsigned int *NumberOfButtonToPressByStepper);

     