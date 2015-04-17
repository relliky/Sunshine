#include <reg52.h>

/* --------------------------------------------------------------------------------------------------------------------------------------------
	Timer Interrupt Macros
 --------------------------------------------------------------------------------------------------------------------------------------------*/
// Timer interrupt parameters
#define INTERRUPT_ON    					1
#define INTERRUPT_OFF   					0 
#define TMOD_TIMER0_16BIT_TIMER1_16BIT 		0x11  
// CYCLE_FOR_TIMER1_UNDER_11_0952MHZ (5ms)= (5000us)/(1/11.0592 us/clock)/12 clock/machine_cycle = 5000*11.0592/12=4608   
#define CYCLE_FOR_TIMER1_UNDER_11_0952MHZ 	4132  
// Count parameter for 1s 
#define COUNT_1S_FOR_5MS_TIMER  			200
// Count parameter for 5s 
#define COUNT_5S_FOR_1S_TIMER				5

// 4608,4000 is so slow that providing less force



// 3599, 3686, 4032 provides enough force to move but still quite small

// 3000, 2764 provides much less force

unsigned char code F_Rotation[]={0x20,0x10,0x08,0x04}; //forward 0010 0000£¬0001 0000£¬0000 1000£¬0000 0100
unsigned char code B_Rotation[]={0x04,0x08,0x10,0x20}; //backward 0000 0100£¬0000 1000£¬0001 0000£¬0010 0000

//unsigned char  LITAI[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

int k = 0;


/******************************************************************/
/*                   main                                       */
/******************************************************************/
main()
{
 // unsigned char i;

	// Timers interrupt initialisations 
	EA   = INTERRUPT_ON;
	TMOD = TMOD_TIMER0_16BIT_TIMER1_16BIT;

    ET1  = INTERRUPT_ON;
	TH1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)/0xFF;  
	TL1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)%0xFF;
	TR1  = INTERRUPT_ON;

	while(1){}

// while(1)
// {
//  for(i=0;i<4;i++)      //4 phase
//     {
   
    // P3=F_Rotation[i];  

//	P3 = LITAI[i];

//     Delay(500);        //speed
//   	 }
//  }
}


void timer5ms (void) interrupt 3 
{



	
	TH1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)/0xFF;  
	TL1  = (0xFFFF - CYCLE_FOR_TIMER1_UNDER_11_0952MHZ)%0xFF;

	k++;
	if(k==4) k=0;

	P3 = B_Rotation[k];

}


