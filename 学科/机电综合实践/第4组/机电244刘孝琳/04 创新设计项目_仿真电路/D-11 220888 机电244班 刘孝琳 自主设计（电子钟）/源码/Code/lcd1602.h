#ifndef __LCD1602_H_
#define __LCD1602_H_

#include<reg52.h>
//定义LCD接口
#define LCD_DATAPINS P0
sbit		LCD_EN = P2^7;
sbit		LCD_RS = P2^6;
sbit 		LCD_WR = P2^5;
//定义与初始化有关的寄存器初值
#define LCD_MODE_PIN8		0X38 //8位数据口，2行，5*8点阵
#define LCD_MODE_PIN4		0X28 //4位数据口，2行，5*8点阵
#define LCD_SCREEN_CLR	0X01 //清屏
#define LCD_CURSOR_RST	0X02 //光标复位
#define LCD_CURSOR_SHOW	0x0C//显示开/关控制
#define LCD_CURSOR_MOVE	0x06//光标或显示移位
//显示或者光标移位
#define LCD_CURSOR_LEFT		0x10
#define LCD_SHOW_LEFT			0x1C
#define LCD_CURSOR_RIGHT	0x14
#define LCD_SHOW_RIGHT		0x18

//初始化
void ReadBusy();
void WriteCmd(unsigned char cmd);
void WriteData(unsigned char dat);
void LcdInit();
void LCD_Show_String(unsigned char x,unsigned char y,unsigned char *p);
void LCD_CGRAM(void);
void LCD_Show_CHAR( unsigned x,unsigned char y,unsigned char dat);
void LCD_Start();
#endif