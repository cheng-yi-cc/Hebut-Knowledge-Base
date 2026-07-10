#include <main.h>
#include <lcd1602.h>
#include <delay.h>
#include <timer0.h>
#include <ds18.h>
/*
SDAU课程设计 
所选题目:    电子钟设计
实现功能:    电子时钟的基本功能，显示时、分、秒，用户可以设置时间
             秒表功能
						 使用DS18B20测量温度
						 LCD1602液晶屏驱动，通过液晶屏实现数据交互
TIPS:基于普中A2开发板 AT89C52
DESIGNED BY : 电子2002 PGL 
*/

//定义所需变量
unsigned char MenuSelect,LastKey,NowKey;
unsigned char	MenuChange= 1;
unsigned char T0Count = 0;
unsigned int  T1Count = 0;
unsigned int  T2Count = 0;
unsigned char	SCount = 0;
unsigned char	KeyNumber = 0;
unsigned long	time_value =0;
unsigned long	time_change_value =0;
unsigned char str[12];
unsigned char S_Flag = 0;
unsigned long	second_value =0;
unsigned char	h_m_s=0;
long temp; //不是无符号数
int DS_int; 
unsigned char Dsdata[7] ="_______";
bit  fg=1;//用于判断温度正负

//读取温度
int ReadTemperature(void)
{
	unsigned char a=0;//低8位
	unsigned char b=0;//高8位
	unsigned int  t=0;

	Init_DS18B20();
	WriteOneChar(0xCC); // 跳过读序号列号的操作
	WriteOneChar(0x44); // 启动温度转换
	delay_18B20(100); //至少750ms
	Init_DS18B20();
	WriteOneChar(0xCC); //跳过读序号列号的操作
	WriteOneChar(0xBE); //读取温度寄存器
	a=ReadOneChar();
	b=ReadOneChar();
	if((b&0xf8)==0xf8) //位为1 时温度是负
	{
		b=~b;
		a=~a+1; //补码转换，取反加一
		if(a==0)b++;//如果发生进位
		fg=0; //读取温度为负时fg=0
	}
	else{
		fg=1;//正数
	}
	  t=((b*256+a)*25)>>2;//合成整数
		return(t);
}

//对读取到的温度数据进行处理
void Tran(int temp)
{
	unsigned char k=0;  
  Dsdata[1] = ' ';Dsdata[2] = '0';Dsdata[3] = '0';Dsdata[4] = '0';Dsdata[5] = '0';//清缓存
	if(fg){
		Dsdata[0] = ' ';
	}
	else{
		Dsdata[0] = '-';
	}
	do
  {
   Dsdata[4-k]=temp%10 + '0';//取最后一位
   k++;
  }while(temp/=10);
		Dsdata[5]=Dsdata[4];Dsdata[4]=Dsdata[3];Dsdata[3]='.';//后移2位数据，加上小数点
	Dsdata[6]='\0';//END
}

//按键扫描
void Scan_Key()
{
	unsigned char KEY_Temp = 0;
	if(KEY_1==0){KEY_Temp=1;}
	if(KEY_2==0){KEY_Temp=2;}
	if(KEY_3==0){KEY_Temp=3;}
	if(KEY_4==0){KEY_Temp=4;}
	LastKey	=	NowKey;
	NowKey	= KEY_Temp;
}

//根据扫描结果判断有无按键按下，可以实现长按检测
unsigned char Judge_Key(){
	if(NowKey!= 0 ){
		T2Count++;
	}else if(LastKey!=0 && NowKey==0){//边沿检测
		
			if(T2Count>25){
				KeyNumber=LastKey+4;
			}else{
				KeyNumber=LastKey;
			}
			T2Count=0;
	}
}

//长整数转换成时间（模60）
unsigned char IntergerToTime(long dat,unsigned char*str){
	//中间变量
	unsigned char i = 0;
	unsigned char h = 0;
	unsigned char m = 0;
	unsigned char s = 0;
	unsigned char buff[11];
    h = dat/ 3600;
    m = dat% 3600 / 60;
    s = dat% 60;
	
		buff[i]=s%10 + '0';
		i++;
		buff[i]=s/10 + '0';
		i++;
	
		buff[i]=m%10 + '0';
		i++;
		buff[i]=m/10 + '0';
		i++;
	
		buff[i]=h%10 + '0';
		i++;
		buff[i]=h/10 + '0';
		i++;
		
	while(i-- > 0){
		*str = buff[i];
		str++;
	}
	*str='\0';//取内容
	
	return 1;
}

//秒表计数值转换
unsigned char MsecondToTime(long dat,unsigned char*str){
	//中间变量
	unsigned char i = 0;
	unsigned char h = 0;
	unsigned char m = 0;
	unsigned char s = 0;
	unsigned char buff[11];
    h = dat/ 6000;
    m = (dat-h*6000)/100;
    s = dat-h*6000-m*100;
	
		buff[i]=s%10 + '0';
		i++;
		buff[i]=s/10 + '0';
		i++;
	
		buff[i]=m%10 + '0';
		i++;
		buff[i]=m/10 + '0';
		i++;
	
		buff[i]=h%10 + '0';
		i++;
		buff[i]=h/10 + '0';
		i++;
		
	while(i-- > 0){
		*str = buff[i];
		str++;
	}
	*str='\0';//取内容
	
	return 1;
}

//主菜单
void Main_menu(){
	LCD_Show_String(0,0,"Time:");
	LCD_Show_CHAR(6,0,str[0]);
	LCD_Show_CHAR(7,0,str[1]);
	LCD_Show_CHAR(8,0,':');
	LCD_Show_CHAR(9,0,str[2]);
	LCD_Show_CHAR(10,0,str[3]);
	LCD_Show_CHAR(11,0,':');
	LCD_Show_CHAR(12,0,str[4]);
	LCD_Show_CHAR(13,0,str[5]);
	LCD_Show_String(0,1,"Temp:");//温度
	Tran(DS_int);
	LCD_Show_String(5,1,Dsdata);
	//LCD_Show_CHAR(7,1, '0');
}

//秒表菜单
void Second_menu(){
	LCD_Show_String(0,0,"Stopwatch:");
	LCD_Show_CHAR(6,1,str[0]);
	LCD_Show_CHAR(7,1,str[1]);
	LCD_Show_CHAR(8,1,':');
	LCD_Show_CHAR(9,1,str[2]);
	LCD_Show_CHAR(10,1,str[3]);
	LCD_Show_CHAR(11,1,':');
	LCD_Show_CHAR(12,1,str[4]);
	LCD_Show_CHAR(13,1,str[5]);
}

//时间调整菜单
void Change_menu(){
	LCD_Show_String(0,0,"Change Time");
	if(h_m_s == 0){
		LCD_Show_CHAR(0,1,'H');
	}else if(h_m_s == 1){
		LCD_Show_CHAR(0,1,'M');
	}else if(h_m_s == 2){
		LCD_Show_CHAR(0,1,'S');
	}
	LCD_Show_CHAR(6,1,str[0]);
	LCD_Show_CHAR(7,1,str[1]);
	LCD_Show_CHAR(8,1,':');
	LCD_Show_CHAR(9,1,str[2]);
	LCD_Show_CHAR(10,1,str[3]);
	LCD_Show_CHAR(11,1,':');
	LCD_Show_CHAR(12,1,str[4]);
	LCD_Show_CHAR(13,1,str[5]);
}


void main(){
	delayx10ms(1);
	LcdInit();
	LCD_CGRAM();
	LCD_Start();
	Timer0_Init();
	//循环
	while(1){
		if(KeyNumber){
			if(KeyNumber == 1){
				if(MenuSelect == 1){
					if(S_Flag == 1){
						S_Flag = 0;
					}else{
						S_Flag = 1;
					}
				}
				if(MenuSelect==2){
					switch(h_m_s){
						case 0 : 

							temp = time_change_value + 60*60;
							if(temp<86400-1){
								time_change_value = temp;
							}break;
						case 1 : 

							temp = time_change_value + 60;
							if(temp<86400-1){
								time_change_value = temp;
							}break;
						case 2 : 

							temp = time_change_value + 1;
							if(temp<86400-1){
								time_change_value = temp;
							}break;
					}
				}
			}else if(KeyNumber == 2){
				if(MenuSelect==2){
					switch(h_m_s){
						case 0 : 

							temp = time_change_value - 60*60;
							if(temp>=0){
								time_change_value = temp;
							}break;
						case 1 : 

							temp = time_change_value - 60;
							if(temp>=0){
								time_change_value = temp;
							}break;
						case 2 : 

							temp = time_change_value - 1;
							if(temp>=0){
								time_change_value = temp;
							}break;
					}
				}
			}
			else if(KeyNumber == 3){//3键切换菜单
					if(MenuSelect<2){
							MenuSelect++;
							MenuChange=1;
							if(MenuSelect>1){
								MenuSelect=0;
							}
					}else if(MenuSelect==2){
							h_m_s++;
							if(h_m_s>=3){
								h_m_s=0;
							}
					}
			}else if(KeyNumber == 5){
						if(MenuSelect == 1){
							//清零计数值
							second_value=0;
						}
						//LED_2 = ~LED_2;
			}else if(KeyNumber == 6){
						if(MenuSelect == 2){
							//确定校时
							time_value=time_change_value;
							//--
							LED_2 = ~LED_2;//LED闪烁表示 效时成功了哦
							//--
						}
			}else if(KeyNumber == 7){
					if(MenuSelect != 2){
						MenuChange=1;
						//time_change_value=time_value;
						time_change_value=0;
						MenuSelect=2;
					}else{
						MenuChange=1;
						MenuSelect=0;
					}
			}
			

			KeyNumber=0;
		}
		//清屏命令，防止重复清屏，卡哎
		if(MenuChange==1){
			WriteCmd(0x01);//清屏
			MenuChange = 0;
		}
		//切换菜单
		switch(MenuSelect){
			case 0 :	IntergerToTime(time_value,str);Main_menu();break;
			case 1 :	MsecondToTime(second_value,str);Second_menu();break;
			case 2 :	IntergerToTime(time_change_value,str);Change_menu();
								if(h_m_s==0){
									Delay(260);
									LCD_Show_CHAR(6,1,' ');
									LCD_Show_CHAR(7,1,' ');
									Delay(260);
								}else if(h_m_s==1){
									Delay(260);
									LCD_Show_CHAR(9,1,' ');
									LCD_Show_CHAR(10,1,' ');
									Delay(260);
								}else if(h_m_s==2){
									Delay(260);
									LCD_Show_CHAR(12,1,' ');
									LCD_Show_CHAR(13,1,' ');
									Delay(260);
								}break;
			}

	}
	
}

//计数器0用于按键扫描（不要加delay延时！）同时实现计数
void Timer0_interrupt() interrupt 1
{
	TL0 = 0x18;		
	TH0 = 0xFC;		
	T0Count++;T1Count++;
	if(T0Count>=20)					//每20ms扫描一次按键
	{
		T0Count=0;
		SCount++;
		Scan_Key();
		Judge_Key();
	}
	if(T1Count>=10){
		T1Count=0;
		if(S_Flag==1){
			if(second_value<60001){
				second_value++;
			}else{
				second_value=0;
			}
		}
	}
	if(SCount >= 50 ){
		SCount=0;
		if(time_value >86400-1){
					time_value=0;
			}else{
					time_value++;
				//---
					DS_int = ReadTemperature();//读取温度

				//---
			}

	}
}


