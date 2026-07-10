#ifndef __MAIN_H__
#define __MAIN_H__

#include<reg52.h>
sbit	KEY_1 = P3^1;
sbit	KEY_2 = P3^0;
sbit	KEY_3 = P3^2;
sbit	KEY_4 = P3^3;

sbit	LED_1 = P2^0;
sbit	LED_2 = P2^1;
sbit	LED_3 = P2^2;
sbit	LED_4 = P2^3;

void Scan_Key();
unsigned char Judge_Key()	;
unsigned char IntergerToTime(long dat,unsigned char*str);
unsigned char MsecondToTime(long dat,unsigned char*str);
void Main_menu();
void	Second_menu();
void Change_menu();


int ReadTemperature(void);
#endif