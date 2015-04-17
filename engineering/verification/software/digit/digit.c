#include <reg52.h>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char


sbit LedDigit1=P1^0;
sbit LedDigit2=P1^1;
sbit LedDigit3=P1^2;
sbit LedDigit4=P1^3;

sfr digit_segment_port = 0xA0; //P2 declearation

void delay(uchar x)
{
    uchar j;
    while(x--){
        for(j=0;j<125;j++)
            {;}
        }   
}

/******************************************************************/
/*                   main                                       */
/******************************************************************/
main()
{
	uchar sled_disp_buff[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,	    	// Table for 7-segment LED to display 0-F
                        0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
	int i; 

    // The table has to be invented as LED is drived by an inventor
	for(i=0;i<0xF;i++)
	{ sled_disp_buff[i] = ~sled_disp_buff[i];
	}

 while(1)
 {

	for(i=0;i<0xF;i++)
    {
	    digit_segment_port = sled_disp_buff[0];	
	    P10=1;
		delay(5);
        P10=0;
	    digit_segment_port = sled_disp_buff[1];	
	    P11=1;
		delay(5);
        P11=0;
	    digit_segment_port = sled_disp_buff[2];	
	    P12=1;
		delay(5);
        P12=0;
	    digit_segment_port = sled_disp_buff[3];	
	    P13=1;
		delay(5);
        P13=0;
   	}
  }

}
