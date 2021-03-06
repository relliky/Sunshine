/*-----------------------------------------------
  名称：四相五线减速步进电机
  日期：2009.5
  修改：无
  内容：1、本程序用于测试4相步进电机常规驱动 
        2、需要用跳帽或者杜邦线把信号输出端和对应的步进电机信号输入端连接起来
        3、速度不可以调节的过快，不然就没有力矩转动了
 ------------------------------------------------*/

#include <reg52.h>

sbit WELA=P2^7;//数码管位选锁存器信号

unsigned char code F_Rotation[4]={0x20,0x10,0x08,0x04}; //正转表格，换算成二进制 0010 0000，0001 0000，0000 1000，0000 0100
unsigned char code B_Rotation[4]={0x04,0x08,0x10,0x20}; //反转表格，换算成二进制 0000 0100，0000 1000，0001 0000，0010 0000

/******************************************************************/
/*                    延时函数                                    */
/******************************************************************/
void Delay(unsigned int i)//延时
{
 while(--i);
}

/******************************************************************/
/*                   主函数                                       */
/******************************************************************/
main()
{
  unsigned char i;
////////////////////////////////////////////////////////////////
	P0=0XFF;//关掉数码管的位选信号。阻止数码管受到P0口信号的影响。
	Delay(500);
	WELA=1;
	Delay(500);
	WELA=0;
////////////////////////////////////////////////////////////////

 while(1)
 {
  for(i=0;i<4;i++)      //4相
     {
     P0=F_Rotation[i];  //输出对应的相 可以自行换成反转表格
     Delay(500);        //改变这个参数可以调整电机转速 ,数字越小，转速越大
	 }
  }
}

