#include <reg52.h>


sbit P01 = P1^0;  


/******************************************************************/
/*                   delay                                        */
/******************************************************************/
void Delay(long i)
{
 while(--i);
}

/******************************************************************/
/*                   main                                       */
/******************************************************************/

void main(void)
{
	P01 = 0; 
	Delay(30000);

	P01 = 1; 
	Delay(30000);

}
