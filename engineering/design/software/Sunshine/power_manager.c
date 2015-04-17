#ifndef INTRINS_H
	#include <intrins.h>
	#define INTRINS_H
#endif
	
#ifndef REG52_H
	#include <reg52.h>	
	#define REG52_H
#endif	

#ifndef POWER_MANAGER_H
	#include <power_manager.h>
	#define POWER_MANAGER_H
#endif

/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Put microcontroller into idle mode to save power. The microcontroller will still run its oscillator and its peripherals
				 And it will wake up and back to normal operation once an interrupt has to be responsed. e.g. a timer timing out
  --------------------------------------------------------------------------------------------------------------------------------------------*/

void PutMicrocontrollerIntoIdleMode(void)
{
	PCON = IDLE_MODE;
}	
	