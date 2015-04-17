#define IDLE_MODE	0x01

/*--------------------------------------------------------------------------------------------------------------------------------------------
	Description: Put microcontroller into idle mode to save power. The microcontroller will still run its oscillator and its peripherals
				 And it will wake up and back to normal operation once an interrupt has to be responsed. e.g. a timer timing out
  --------------------------------------------------------------------------------------------------------------------------------------------*/
void PutMicrocontrollerIntoIdleMode(void);