
#include "adjust.h"
#include "common.h"
#include "OLED.h"
#include "key.h"

u8 servo_row; 
u8 D1=0,D2=0,D3=0,D4=0,D5=0,D6=0,D7=0,D8=0;
float kd_duoji,kp_duoji,Kp,Ki,Kd;
float kd_duoji_SB,kp_duoji_SB;
//void adjust()；
s16 boma_max,boma_min;
void adjust_init(void)                               
{   
   servo_row=26;   //越大越切内道26
 
                       
 
   //普通时的pid
   kp_duoji=3.3;                 //         (3.3)        
   kd_duoji=12.8;                //         (12.8)            
   /////////////////////////////////////
    boma_max=75;
    boma_min=38;
   //sb为过黑三角时的pd
   kp_duoji_SB=2.9;              //        (3.2)3.3
   kd_duoji_SB=0.5;              //        (14)0.5
   
   
   //////////////////////////////////////////
   
   
   Kp=85.5;                        //         (83)
   Ki=0.1;                       //         (0.85)0.1
   Kd=2.5;                         //         ( 3)
   
   gpio_init(PTB20,GPI,0);   
   gpio_init(PTB19,GPI,0);   
   gpio_init(PTE26,GPI,0);
   gpio_init(PTE1,GPI,0);
   gpio_init(PTE2,GPI,0);
   gpio_init(PTE3,GPI,0);
   gpio_init(PTE4,GPI,0);
   gpio_init(PTE5,GPI,0);
    
   D1=gpio_get (PTB20);
   D2=gpio_get (PTB19);
   D3=gpio_get (PTE26);
   D4=gpio_get (PTE5);
   D5=gpio_get (PTE1);
   D6=gpio_get (PTE2); 
   D7=gpio_get (PTE3);
   D8=gpio_get (PTE4); 
   LED_Fill(0x00);
   

   adjust(); 
}

u8  you_zhuan=0;
u8  chaoche_jiakuan=0; 
u8 chaoche_jiakuan_7=0;
u8 kaiche=0;
u8 tingche=0;
u8 shibiesanjiao=0;
void adjust(void)
{  if(D1!=1)
  {
    
   servo_row=27;   //越大越切内道21     25  23
   
   boma_max=85;
   boma_min=48;                  
 
   //普通时的pid
   kp_duoji=3.1;                 //         (3.3)     2.95    
   kd_duoji=8.8;                //         (14.8)            
   /////////////////////////////////////
    
   //sb为过黑三角时的pd
   kp_duoji_SB=3.3;              //        (3.2)3.3
   kd_duoji_SB=0.5;              //        (14)0.5
   
   
   //////////////////////////////////////////
   
   
   Kp=85.5;                        //         (83)
   Ki=0.1;                       //         (0.85)0.1
   Kd=2.5;                         //         ( 3)
   
  
  }
  if(D2!=1)
  {
    
   servo_row=35;   //越大越切内道21     25  23
   
   boma_max=80;
   boma_min=48;                  
 
   //普通时的pid
   kp_duoji=3.1;                 //         (3.3)     2.95    
   kd_duoji=4.8;                //         (14.8)            
   /////////////////////////////////////
    
   //sb为过黑三角时的pd
   kp_duoji_SB=3.3;              //        (3.2)3.3
   kd_duoji_SB=0.5;              //        (14)0.5
   
   
   //////////////////////////////////////////
   
   
   Kp=85.5;                        //         (83)
   Ki=0.1;                       //         (0.85)0.1
   Kd=2.5;                         //         ( 3)
   
  
  }
    if(D3==1)
  {
    
  tingche=1;
  
  }
  if(D4==1)
  {
    
  kaiche=1;
  
  }
    if(D6==1)
  {
    
  shibiesanjiao=1;
  
  }
 if(D7==1)
  {
    
  chaoche_jiakuan_7=1;
  
  }
 if(D8==1)
  {
    
  chaoche_jiakuan=1;
  
  }

 enable_irq(PIT_IRQn);
 
 
}




