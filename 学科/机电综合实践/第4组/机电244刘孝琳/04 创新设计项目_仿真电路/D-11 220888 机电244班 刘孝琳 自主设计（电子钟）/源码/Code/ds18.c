#include <ds18.h>
#include <delay.h>
//初始化函数
bit Init_DS18B20(void)
{
	unsigned char x=0;
	DQ = 1;
	delay_18B20(8);//延时
	DQ = 0;
	delay_18B20(80); //应大于 480us
	DQ = 1;//拉高总线
	delay_18B20(14);
	x=DQ;//稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	delay_18B20(20);
	return(x); 
}

unsigned char ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{
		DQ = 0; 
		dat>>=1;
		DQ = 1; 
		if(DQ)
			dat|=0x80;
		delay_18B20(4);
	}
	return(dat);
}

void WriteOneChar(unsigned char dat)
{
	unsigned char i=0;
	for (i=8; i>0; i--)
	{
		DQ = 0;
		
		DQ = dat&0x01; //输出位为零时实现长0短1；输出位为1时短0，短0长1，datasheet
		delay_18B20(5);

		DQ = 1;
		dat>>=1;
	}
}
