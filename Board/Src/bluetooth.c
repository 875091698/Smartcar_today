
#include "bluetooth.h"
#include "OLED.h"


extern s16 car_speed;
extern float camera_error;

char vaule_uart_rxd[4] = {0,1,1,0};
char * blue_ch = vaule_uart_rxd;
char blue_flag = 0;



void bluetooth_uart(void)
{
    u32 i = 0xffff; 
    u8 temp = 0;
     
    while( (--i>0x00)&&(blue_flag==0) )
    {       
         temp = uart_querystr (FIRE_PORT, blue_ch, 10);
         if (temp == 5)
         {
            if (vaule_uart_rxd[0]==0xff)
            {
                blue_flag=1; 
            }              
         }
    }
    
    
    LED_PrintValueC(1 ,1,vaule_uart_rxd[0]);
    LED_PrintValueC(1 ,2,vaule_uart_rxd[1]);
    LED_PrintValueC(1 ,3,vaule_uart_rxd[2]);
    LED_PrintValueC(1 ,4,vaule_uart_rxd[3]);
      
    if (blue_flag==1)
    {
        blue_flag = 0;               
    } 
    blue_flag = 0;
    
    
}