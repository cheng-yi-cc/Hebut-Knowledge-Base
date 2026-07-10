#include <main.h>

sbit ZD=P3^2;//左队加分按键
sbit YD=P3^3;//右队加分按键
sbit CX=P3^7;//撤回按键

sbit LEDR=P2^6;//红灯获胜指示灯
sbit LEDB=P2^7;//蓝灯获胜指示灯

uchar String[16];//定义lcd显示字符串
uint gradZ=0,gradY=0,k;//双方分数和步骤参数
uint time=30;//倒计时时间
uchar JL[60]={0};//步骤记录
uint SD=6,D=1;//赛点

//延时ms函数
void delay_ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<110;j++);
}

void Timer0Init(void)
{
	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。
	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;	
	EA=1;//打开总中断
	ET0=1;//打开定时器0中断允许
	TR0=1;//打开定时器
}

void Timer0() interrupt 1
{
	static uint i;
	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;
	i++;
	if(i==1000)//1秒到，倒计时减1
	{
		i=0;
		if(time>0)
			time--;
	}
}
void EX_ISRInit(void)//外部中断初始化
{
	IT0 = 1;//打开外部中断0，下降沿触发
	EX0 = 1;	
	IT1 = 1;//打开外部中断1，下降沿触发
	EX1 = 1;
}
void EX0_ISR(void) interrupt 0 //外部中断0
{
	delay_ms(2);
	if(ZD==0)
	{
    if(time!=0) {JL[k]=1;k++;gradZ++;}//左队分数加1
  }
}

void EX1_ISR(void) interrupt 2 //外部中断1
{
	delay_ms(2);
	if(YD==0)
	{
    if(time!=0) {JL[k]=2;k++;gradY++;}//右队分数加1
	}
}
//主函数
void main()
{  
	LCD_Initial();//lcd1602初始化
	EX_ISRInit();//外部中断初始化
	Timer0Init(); //定时器初始化
	while(1)
	{
		if(!CX)  //撤销按键按下
		{
			delay_ms(10);
			if(!CX)
			{
					if(time!=0)//如果时间没到，撤销上一步操作
					{					
						if(JL[k-1]==1&&gradZ>0)      {gradZ--;JL[k-1]=0;k--;if(D>1) D--;}
						else if(JL[k-1]==2&&gradY>0) {gradY--;JL[k-1]=0;k--;if(D>1) D--;}
					}
			}
			while(!CX);
		}
		
		 if((gradZ>=SD+D&&gradZ>gradY+1)||(gradY>=SD+D&&gradY>gradZ+1))
		 {
		   time=0;//如果一方达到胜利分数，并比对方高2分，比赛结束，时间清零
		 }
		 else if((gradZ>=SD+D&&gradZ<=gradY+1)||(gradY>=SD+D&&gradY<=gradZ+1))
		 {
		   D++;//反之，比赛继续，获胜分数加1
		 }
		 if(gradZ>=SD||gradY>=SD)//如果到达赛点
		 {
			 if(time>0)
			 {
				 sprintf(String,"MP");//lcd1602显示
				 GotoXY(12,0);
				 Print(String);
			 }
		 }
		 if(time>0)//倒计时时间大于0显示时间
		 {
			 sprintf(String,"    time:%d    ",time);//lcd1602显示
			 GotoXY(0,0);
			 Print(String);
		 }
		 else //倒计时时间结束显示获胜状态
		 {
			 if(gradZ>gradY)//左队分数高
			 {
				 sprintf(String," Left team wins");//lcd1602显示
				 GotoXY(0,0);
				 Print(String);
				 LEDR=0; LEDB=1;
			 }
			 else if(gradZ<gradY)//右队分数高
			 {
				 sprintf(String,"Right team wins");//lcd1602显示
				 GotoXY(0,0);
				 Print(String);
				 LEDR=1; LEDB=0;
			 }
			 else//两队分数相同，平局
			 {
				 sprintf(String,"    draw!!!    ");//lcd1602显示
				 GotoXY(0,0);
				 Print(String);
				 LEDR=0; LEDB=0;
			 }
		 }
     sprintf(String,"     %d : %d   ",gradZ,gradY);//lcd1602显示
		 GotoXY(0,1);
		 Print(String);
	}
}