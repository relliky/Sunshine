

/****************************************************************
                                                              
            ��������:   ֱ������޼�����
            �汾:      VER1.0
            ���ð屾:  PL-51ѧϰ��      
			���ö�ʱ�����Ʋ���ռ�ձȿɱ��PWM����ֱ����������޼�����
			��S2��PWMֵ���ӣ���ռ�ձȼ�С,������١�(��ֵ���Ϊ��Сֵ)
			��S3��PWMֵ��С����ռ�ձ�����,������١�
			��PWMֵ���ӵ����ֵ���С����Сֵʱ����������������

            ע:ֱ������İ�װ,��鿴���ļ��еİ�װ˵��.

*****************************************************************/

#include<reg51.h>
#include<intrins.h>
sbit  S2 =P3^4 ;        //PWMֵ���ټ�
sbit  S3 =P3^5 ;       //PWMֵ���Ӽ�
sbit  BEEP =P2^3 ;         //������
unsigned char PWM=0x03 ;   //����ֵ

///////////////////////////////////////
sbit dula=P2^6;   //����ܵĶ�ѡ�ź�
sbit wela=P2^7;   //����ܵ�λѡ�ź�
void delay1 (void) //�ر��������ʱ����
{
	int k;
	for (k=0; k<1000; k++);

}
//////////////////////////////////////

void Beep();
void delayms(unsigned char ms);
void delay(unsigned char t);

/*********************************************************/
void main()
{   
    P1=0xff;
    TMOD=0x00 ;
 	TH0=0x00 ;           //��ʱ����
    TL0=0x00 ;           //Ƶ�ʵ���
    TH1=PWM ;            //��������
    TL1=0xff ;
	EA=1;
	ET0=1;
	ET1=1;
    TR0=1 ;

	/////////////////////////////////////////////////////////////////
	P0=0x00;//�ص�����ܵ��źš���ֹ������ܵ�P0���źŵ�Ӱ�졣
	dula=1;
	wela=0;
	delay1();
	dula=0;
	wela=0;
	delay1();
	////////////////////////////////////////////////////////////////

   while(1)
   {
 do{
     if(PWM!=0xff)
    {PWM++ ;delayms(10);}
        else Beep() ; 
   }
    while(S3==0);

 do{
      if(PWM!=0x02)
     {PWM-- ;delayms(10);}
      else Beep() ; 
   }
    while(S2==0);
  }
}

/*********************************************************/
// ��ʱ��0�жϷ������  ��Ƶ�ʣ�
/*********************************************************/
void timer0() interrupt 1 
{  
    TR1=0 ;
    TH0=0x00;
    TL0=0x00 ;
    TH1=PWM ;
    TR1=1 ;
    P0=0x00 ;      //�������
	
}

/*********************************************************/
// ��ʱ��1�жϷ������ ��������
/*********************************************************/
void timer1() interrupt 3 
{ 
    TR1=0 ;
    P0=0xff ;     //�������
}

/*********************************************************/
//�������ӳ���
/*********************************************************/

void Beep()     
  {
    unsigned char i  ;
    for (i=0  ;i<100  ;i++)
      {
        delay(100)  ;
        BEEP=!BEEP  ;                //Beepȡ��
      } 
    BEEP=1  ;                        //�رշ�����
 delayms(100);
  } 

/*********************************************************/
// ��ʱ�ӳ���
/*********************************************************/  
void delay(unsigned char t)
 { 
   while(t--)   ;
 }

/*********************************************************/
// ��ʱ�ӳ���
/*********************************************************/
void delayms(unsigned char ms) 

{
   unsigned char i ;
   while(ms--)
    {
      for(i = 0 ; i < 120 ; i++) ;
    }
}

/*********************************************************/ 