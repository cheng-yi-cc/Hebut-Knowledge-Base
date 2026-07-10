#include <lcd1602.h>

//内部等待函数**************************************************************************
uchar LCD_Wait(void)
{
	LcdRs=0;
	LcdRw=1;	_nop_();
	LcdEn=1;	_nop_();
	//while(DBPort&0x80);//在用Proteus仿真时，注意用屏蔽此语句，在调用GotoXY()时，会进入死循环，
						 //可能在写该控制字时，该模块没有返回写入完备命令，即DBPort&0x80==0x80
						 //实际硬件时打开此语句
	LcdEn=0;
	return DBPort;		
}



void LCD_Write(bit style, uchar input)
{
	LcdEn=0;
	LcdRs=style;
	LcdRw=0;		_nop_();
	DBPort=input;	_nop_();//注意顺序
	LcdEn=1;		_nop_();//注意顺序
	LcdEn=0;		_nop_();
	LCD_Wait();	
}

//设置显示模式************************************************************
#define LCD_SHOW				0x04    //显示开
#define LCD_HIDE				0x00    //显示关	  

#define LCD_CURSOR			0x02 		//显示光标
#define LCD_NO_CURSOR		0x00    //无光标		     

#define LCD_FLASH				0x01    //光标闪动
#define LCD_NO_FLASH		0x00    //光标不闪动

void LCD_SetDisplay(uchar DisplayMode)
{
	LCD_Write(LCD_COMMAND, 0x08|DisplayMode);	
}

//设置输入模式************************************************************
#define LCD_AC_UP				0x02
#define LCD_AC_DOWN			0x00      // default

#define LCD_MOVE				0x01      // 画面可平移
#define LCD_NO_MOVE			0x00      //default

void LCD_SetInput(uchar InputMode)
{
	LCD_Write(LCD_COMMAND, 0x04|InputMode);
}



//初始化LCD************************************************************
void LCD_Initial()
{
	LcdEn=0;
	LCD_Write(LCD_COMMAND,0x38);           			//8位数据端口,2行显示,5*7点阵
	LCD_Write(LCD_COMMAND,0x38);
	LCD_SetDisplay(LCD_SHOW|LCD_NO_CURSOR);    	//开启显示, 无光标
	LCD_Write(LCD_COMMAND,LCD_CLEAR_SCREEN);   	//清屏
	LCD_SetInput(LCD_AC_UP|LCD_NO_MOVE);       	//AC递增, 画面不动
}

//************************************************************************
void GotoXY(uchar x, uchar y)
{
	if(y==0)
		LCD_Write(LCD_COMMAND,0x80|x);
	if(y==1)
		LCD_Write(LCD_COMMAND,0x80|(x-0x40));
}

void Print(uchar *str)
{
	while(*str!='\0')
	{
		LCD_Write(LCD_DATA,*str);
		str++;
	}
}