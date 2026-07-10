#include<delay.h>
void delay10ms(void){ 
   unsigned char i,j,k;
   for(i=5;i>0;i--)
      for(j=4;j>0;j--)
        for(k=248;k>0;k--);
}

void delayx10ms(unsigned int n){  
	unsigned int i;
	for(i = 0;i<n;i++){
		delay10ms();
	}
}

void Delay(unsigned int xms)		//@11.0592MHz
{
	unsigned char i, j;
	while(xms--)
	{
		_nop_();
	i = 2;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
	}
	
}


//晶振12MHz时，用于18B20延时
void delay_18B20(unsigned int i)
{
while(i--);
}