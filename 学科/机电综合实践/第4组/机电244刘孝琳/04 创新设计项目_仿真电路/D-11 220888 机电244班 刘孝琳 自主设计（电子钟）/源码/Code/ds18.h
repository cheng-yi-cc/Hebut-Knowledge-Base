#ifndef __DS18_H_
#define __DS18_H_
#include <reg52.h>

sbit DQ = P3^7;//定义通信端口

bit Init_DS18B20(void);
unsigned char ReadOneChar(void);
void WriteOneChar(unsigned char dat);
#endif