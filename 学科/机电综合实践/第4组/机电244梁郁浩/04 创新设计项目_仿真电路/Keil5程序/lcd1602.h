#ifndef _LCD1602_H_
#define _LCD1602_H_
#include "main.h"
//向LCD写入命令或数据************************************************************
#define LCD_COMMAND				0      		// Command
#define LCD_DATA					1      		// Data
#define LCD_CLEAR_SCREEN	0x01      // 清屏
#define LCD_HOMING  			0x02      // 光标返回原点

//Port Definitions**********************************************************
sbit LcdRs		= P2^0;
sbit LcdRw		= P2^1;
sbit LcdEn  	= P2^2;
sfr  DBPort 	= 0x80;		//P0=0x80,P1=0x90,P2=0xA0,P3=0xB0.数据端口

//函数原型声明
uchar LCD_Wait(void);
void LCD_Write(bit style, uchar input);
void LCD_SetDisplay(uchar DisplayMode);
void LCD_SetInput(uchar InputMode);
void LCD_Initial();
void GotoXY(uchar x, uchar y);
void Print(uchar *str);

#endif