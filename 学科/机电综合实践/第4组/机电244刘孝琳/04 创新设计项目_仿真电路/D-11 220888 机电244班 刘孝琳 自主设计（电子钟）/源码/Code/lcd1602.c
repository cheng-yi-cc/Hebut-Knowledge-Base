#include<reg52.h>
#include<delay.h>
#include<lcd1602.h>

unsigned char code	table_1[]={
0x03,0x07,0x0f,0x1f,0x1f,0x1f,0x1f,0x1f,
0x18,0x1E,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,
0x07,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,
0x10,0x18,0x1c,0x1E,0x1E,0x1E,0x1E,0x1E,
0x0f,0x07,0x03,0x01,0x00,0x00,0x00,0x00,
0x1f,0x1f,0x1f,0x1f,0x1f,0x0f,0x07,0x01,
0x1f,0x1f,0x1f,0x1f,0x1f,0x1c,0x18,0x00,
0x1c,0x18,0x10,0x00,0x00,0x00,0x00,0x00};//心形图的字符模型


//检测LCD1602是不是在忙
void ReadBusy(){
	unsigned char state = 0xff;	//用于存放读取到的状态
	unsigned char i = 0x00 ;	 // 计数器
	LCD_DATAPINS = 0xFF;	//IO作为输入，先写1
	LCD_RS = 0;					
	LCD_WR = 1;
	do{
	LCD_EN = 1;
	state = LCD_DATAPINS;
	LCD_EN = 0; //为下一次交互准备
		i++;
		if(i>50){
			break;
		}
	}while(state & 0X80); //使用do while循环
	//避免液晶屏损坏，读出结果一直为1
}

//写命令
void WriteCmd(unsigned char cmd){
	ReadBusy();//等待不忙
	LCD_RS = 0;
	LCD_WR = 0;//写操作
	LCD_DATAPINS = cmd;
	LCD_EN = 1;
	LCD_EN = 0;
}

//写命令
void WriteData(unsigned char dat){
	ReadBusy();//等待不忙
	LCD_RS = 1;
	LCD_WR = 0;//写操作
	LCD_DATAPINS = dat;
	LCD_EN = 1;
	LCD_EN = 0;
}

void LcdInit(){
	WriteCmd(LCD_MODE_PIN8);//8位数据口，2行，5*8点阵
	WriteCmd(LCD_CURSOR_SHOW);//显示光标以及光标设置
	WriteCmd(LCD_CURSOR_MOVE);//显示开/关控制
	WriteCmd(LCD_SCREEN_CLR);
}

//DDRAM,列从0开始
void LCD_Show_String(unsigned char x,unsigned char y,unsigned char *p){
	if(y == 0){
		WriteCmd(0x80|x);
	}else{
		WriteCmd(0x80|(0x40+x));
	}
	//写数据
	while(*p != '\0'){
		WriteData(*p);
		p++;
	}
}

//一行只能显示16个字符，通过移屏进行显示后面的，em...好像用不到


void LCD_CGRAM(void){
	unsigned int i=0,j=0,k=0;
	unsigned char tmp=0x40;//设置CGRAM地址
for(j=0;j<8;j++)
{
	for(i=0;i<8;i++)
	{
		WriteCmd(tmp+i); // 设置自界说字符的 CGRAM 地址
		//delay_nms(2);
		WriteData(table_1[k]); // 向CGRAM写入自界说字符表的数据
		k++;
		//delay_nms(2);等待忙检测
	}
		tmp=tmp+8;
}
}


void LCD_Show_CHAR(unsigned x,unsigned char y,unsigned char dat)
{
	if(y == 0){
		WriteCmd(0x80|x);
	}else{
		WriteCmd(0x80|(0x40+x));
	}
	//写数据
	WriteData(dat);
}


void LCD_Start(){
	unsigned char i;
	for (i=0;i<4;i++)//显现心图画的上半部
	{
		LCD_Show_CHAR(i,0,i);
	}
	for (i=4;i<8;i++)//显现心图画的下半部
	{
		LCD_Show_CHAR(i-4,1,i);
	}
	LCD_Show_String(8,0,"SDAU MCU");
	LCD_Show_String(8,1,"2022 6 6");
	delayx10ms(300);	//保持3秒
	WriteCmd(0x01);//清屏
}
