/*******************************************************************************
程序功能：实时采集当前环境温度值，并显示于数码管上。
*****************************************************************************/
#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

unsigned int uint_a = 0;
unsigned char uchar_a = 0;

void main (void){

int a = 1;

//while(1){ 

//a = 2;

//}

	uint_a 	= 542;  					 // 0d542, 0x21E
	uint_a 	= uint_a/10; 				 // 0d54 , 0x36
	uchar_a	= (unsigned char)(uint_a & 0x00FF);  	 // 0d54 , 0x36



}
