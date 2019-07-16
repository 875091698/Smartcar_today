 
#include "pid.h"
#include "common.h"
#include "MKL_uart.h"
#include "LED.h"
#include  "MKL_tpm.h"           //TPM（类似K60的 FTM ，pwm、脉冲计数）
#include "MKL_pit.h"
#include "camera.h"
 
#include "adjust.h"
#define CAMERA_LINE (60)
#include "OLED.h"
extern u32 zhongzhi ;

s32 motoroutput2;
extern float camera_error ;
extern s8  this_is_sanjiao;
extern s8 this_is_jiakuan_zx;
extern u8 mode;
extern u8 ding2; 
extern   float kp_duoji,kd_duoji;
extern   float kp_duoji_SB,kd_duoji_SB;
extern   float kp_duoji1;
float ang_1;
float ang;
u16 servoRow;
extern u16 ding1;
extern s16 sudu;
extern  u8 image_start;
extern  u8 you_zhuan;
extern s16 pulse_count;
u8 NtpBrakeCount_1=85;
u8 count_1=0;
s16 duoji(lost,expect_speed, pulse_count)
{

    static s16 old,old_n;
   
/***************普通pid********************/

        old_n=camera_error*kp_duoji;

	ang_1=camera_error-old;       //更新误差
 
	ang=old_n+(ang_1)*kd_duoji*1.2;
        
        old=camera_error;

        

      if(this_is_jiakuan_zx!=0 && mode==4&&count_1<=1)//
    {
             if(this_is_jiakuan_zx==1)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi+120));
                          mode=0;
                          sudu=6;
              this_is_jiakuan_zx==0;
              DELAY_MS(240);    // 420

            }
          
            else if(this_is_jiakuan_zx==-1)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi-120));
                          mode=0;
                          sudu=6;
              this_is_jiakuan_zx==0;
              DELAY_MS(240);    // 

            } 
            mode=0;
            count_1++;
    }
    
      /* if(ang>180)
         {
          ang=180;
         }
         else if(ang<-180)//右打角
         {
          ang=-180;
         }
      */  
     
      motoroutput2=((u32)(zhongzhi-ang));//输出值-ang
       
      tpm_pwm_duty(TPM1, TPM_CH0, (u32)motoroutput2);
  
      return camera_error+2.2*ang_1;//*2.2
         
}
extern  s16 expect_speed;
void duoji_sanjiao()
{
       static s16 S_old,S_old_n;     
       s16 dajiao_z=0,dajiao_y=0,shijian=0;
/***************普通pid********************/
        S_old_n=camera_error*kp_duoji_SB;

	ang_1=camera_error-S_old;       //更新误差
 
	ang=S_old_n+(ang_1)*kd_duoji_SB;
        
        S_old=camera_error;
        if(pulse_count>190) 
        {
        dajiao_z=155;
        dajiao_y=155;
        shijian=420;
        }
        else if(pulse_count>85)
        {
                  dajiao_z=145;
                  dajiao_y=155;
        shijian=420;
        }
        else if(pulse_count>80)
        {
                  dajiao_z=135;
                  dajiao_y=170;
        shijian=420;
        }
        
    /////////    ///////////////////////////////////////////////////////////////
        else if(pulse_count>70)
        {
                  dajiao_z=125;
                  dajiao_y=135;
        shijian=420;
        }
        
        
         else if(pulse_count>60)
        {
                  dajiao_z=125;
                  dajiao_y=135;
        shijian=420;
        }
         else if(pulse_count>50)
        {
                  dajiao_z=100;
                  dajiao_y=120;
        shijian=420;
        }
         else if(pulse_count>40)
        {
                  dajiao_z=100;
                  dajiao_y=120;
        shijian=380;
        }
         else
        {
                  dajiao_z=100;
                  dajiao_y=120;
        shijian=380;
        }
    if(this_is_sanjiao!=0&&(mode==5||mode==6))//this_is_sanjiao==1&&mode==7 &&(mode==5||mode==6)  并接收到前车的标志位才能转向
        { 
            if(mode==5)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi+dajiao_y));
            
              DELAY_MS(shijian);    // 420
              sudu=-30;
              uart_putchar   (UART0 , '9');                    //发送字符9到后车 后面的车快点跑！
            }
           
            else if(mode==6)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi-dajiao_z));
            
              DELAY_MS(shijian);    // 
              sudu=-30;
              uart_putchar   (UART0 , '9');                    //发送字符9到后车 后面的车快点跑！
            } 
            mode=0;
       }      
         
  
     /*    if(this_is_sanjiao!=0)//this_is_sanjiao==1&&mode==7 &&(mode==5||mode==6)  并接收到前车的标志位才能转向
        { 
            if(this_is_sanjiao==1)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi+dajiao_y));
            
              DELAY_MS(shijian);    // 420
              sudu=0;
              uart_putchar   (UART0 , '9');                    //发送字符9到后车 后面的车快点跑！
            }
           
            else if(this_is_sanjiao==-1)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi-dajiao_z));
            
              DELAY_MS(shijian);    // 
              sudu=0;
              uart_putchar   (UART0 , '9');                    //发送字符9到后车 后面的车快点跑！
            } 
            mode=0;
       } 
      */ 
        
       
      if(this_is_jiakuan_zx!=0 && mode==4&&count_1<=1)//
    {
             if(this_is_jiakuan_zx==1)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi+120));
                          mode=0;
                          sudu=6;
              this_is_jiakuan_zx==0;
              DELAY_MS(240);    // 420

            }
          
            else if(this_is_jiakuan_zx==-1)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi-120));
                          mode=0;
                          sudu=6;
              this_is_jiakuan_zx==0;
              DELAY_MS(240);    // 

            } 
            mode=0;
            count_1++;
    }
     
   /*  if(this_is_sanjiao!=0)
      {
      Buzzer_Bi();
      }
      else
      {
      Buzzer_Mie();
      }*/
   /*  if(this_is_jiakuan_zx!=0 && mode==4)
    {
             if(this_is_jiakuan_zx==1)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi+35));
                          mode=0;
              this_is_jiakuan_zx==0;
              DELAY_MS(120);    // 420

            }
          
            else if(this_is_jiakuan_zx==-1)
            {
              tpm_pwm_duty(TPM1, TPM_CH0, (u32)(zhongzhi-35));
                          mode=0;
              this_is_jiakuan_zx==0;
              DELAY_MS(120);    // 

            } 
            mode=0;
    }*/
     
     
     //    LED_P6x8Str(2,6,"this_is_sanjiao");
          //LED_PrintValueF(72,6,   this_is_sanjiao, 1);
          
   /*    if(ang>180)
         {
          ang=180;
         }
         else if(ang<-180)//右打角
         {
          ang=-180;
         }*/
        motoroutput2=((u32)(zhongzhi-ang));//输出值-ang
       
        tpm_pwm_duty(TPM1, TPM_CH0, (u32)motoroutput2);
     
}


/******************电机控制******************/
/******************PID控制**********************/
extern float Kp,Ki,Kd; 
extern u8 this_is_jiakuan;
extern s16 expect_speed;
u8 NtpBrakeCount=10;
struct 
  {
    float current_error;//车轮当前误差值
    float last_error;   //车轮上次误差值
    float prevent_error;//车轮上上次误差值
  }PID_M;  
s16 ex_speed=0;       //设定速度
float P=0,I=0,D=0;  
float motor=0; 
void set_motor(speed,pulse_count_y)
{ 
  ex_speed=speed;
  
  PID_M.prevent_error=PID_M.last_error;//一下三句的作用是为了更新误差

  PID_M.last_error=PID_M.current_error;
 
  PID_M.current_error= ex_speed -pulse_count_y;
 
  P=Kp*(PID_M.current_error-PID_M.last_error);   //经典pid
  I=Ki*PID_M.current_error;
  D=Kd*(PID_M.current_error-2*PID_M.last_error+PID_M.prevent_error);
  
 if(fabs(PID_M.last_error)<115)
  {
   motor=P +I +D + motor;//增量式PID
  }
  else
  motor=P+motor;//积分分离
   
 /*******电机限幅**********/
  if(motor>8000) 
    motor=8000;  
  if(motor<-8000)
    motor=-8000; 
  
       //tpm_pwm_duty(TPM0, TPM_CH2, 0);    
       //tpm_pwm_duty(TPM0, TPM_CH3, (s16)(1500)); 
  //   LED_P6x8Str(2,3,"image_start");
  //   LED_PrintValueF(72,3,   image_start, 1);  
    // if(image_start!=0)
    //  motor=0; image_start==0

  if(image_start<=38)
 {
      //   LED_P6x8Str(2,4,"this_is_jiakuan");
     //    LED_PrintValueF(72,4,   this_is_jiakuan, 1);   
  if(this_is_jiakuan==1)//this_is_jiakuan==1
  {
    expect_speed=0;
      if (NtpBrakeCount > 0) 
        {
          tpm_pwm_duty(TPM0, TPM_CH0, 0);
         //motor_pwm(CH1, -motor_l); 
          tpm_pwm_duty(TPM0, TPM_CH1, (2000));
           NtpBrakeCount--;
        } 
         else 
       {
           if(pulse_count>=10)
             {
             tpm_pwm_duty(TPM0, TPM_CH0, 0);
             //motor_pwm(CH1, -motor_l); 
             tpm_pwm_duty(TPM0, TPM_CH1, (1500));
             }
           else
           {
        tpm_pwm_duty(TPM0, TPM_CH0, 0);
       //motor_pwm(CH1, -motor_l); 
       tpm_pwm_duty(TPM0, TPM_CH1, (0));
           }
       }
    
  }
    
  if(this_is_sanjiao!=0)//this_is_sanjiao!=0
  {
     if(motor>=0)       //motor>=0
    {
       tpm_pwm_duty(TPM0, TPM_CH0, (s16)(0)/4);    //motor
        //motor_pwm(CH3, (s16)(motor_l));
     tpm_pwm_duty(TPM0, TPM_CH1, 0);
    }
    else
    {
          tpm_pwm_duty(TPM0, TPM_CH0, 0);
         //motor_pwm(CH1, -motor_l); 
         tpm_pwm_duty(TPM0, TPM_CH1, (0)/4);

    }
  }
  else
  {
       if(motor>=0)       //motor>=0
    {
       tpm_pwm_duty(TPM0, TPM_CH0, (s16)(motor));    //motor
        //motor_pwm(CH3, (s16)(motor_l));
     tpm_pwm_duty(TPM0, TPM_CH1, 0);
    }
    else
    {
          tpm_pwm_duty(TPM0, TPM_CH0, 0);
         //motor_pwm(CH1, -motor_l); 
         tpm_pwm_duty(TPM0, TPM_CH1, (motor));

    }
  
  }
 }
   else
  {
    if(pulse_count>=10)
    {
       tpm_pwm_duty(TPM0, TPM_CH0, 0);
       //motor_pwm(CH1, -motor_l); 
       tpm_pwm_duty(TPM0, TPM_CH1, (4500));
    }  
       tpm_pwm_duty(TPM0, TPM_CH0, 0);
       //motor_pwm(CH1, -motor_l); 
       tpm_pwm_duty(TPM0, TPM_CH1, (0));

  } 

  

     

}
extern s16 sudu;
extern s16 boma_max,boma_min;
s16 set_speed;
s16 hongwai_ceju(distance_H)   //distance_H为返回的电压值！
{
  s16 Speed_Max=boma_max,Speed_Min=boma_min;
  
 // u16 _90cm=60 ,_45cm=105 ,_20cm=145 ,PS=0.6;
 u16 _90cm=40 ,_45cm=70 ,_20cm=105 ,PS=0.6;
// distance_H=distance_H;
  if(distance_H<_90cm)
  {
    
  set_speed=Speed_Max;
  
  }
  else if(distance_H<_45cm)
  {
    
  set_speed=Speed_Max-2.0*(Speed_Max-Speed_Min)*(fabs(distance_H)-_90cm)/(_45cm-_90cm+1);
  
  } 
  else
  {
    
  set_speed=Speed_Min;
  
  }
  
  if(distance_H>_20cm)  //如果比最大值还要大时
  {
    set_speed=set_speed-(distance_H-_20cm)/300.0*0.9*set_speed;
  }
  else 
  {
    
  } 
  if(set_speed<33)
  {    
  set_speed=33;
  }
  
  return set_speed+sudu;							    

}




#define CONTROL_LEVEL (2)
#define DISTANCE (3)
#define MIDSPEED_ROW (5)
#define HIGHSPEED_ROW (6)
#define SPEED_LOW (7)
#define SPEED_MID (8)
#define SPEED_HIGH (9)
#define ACC_LM (10)
#define BRAKE_COUNT (11)
#define g_HighestSpeed (12)
#define g_Mid_1Speed (13)
#define g_Mid_2Speed  (14)
#define g_LowestSpeed  (15)
#define SERVO_ROW_LOW (18)
#define SERVO_ROW_HIGH (19)


// speed merge
static s16 NtpRowHistory[4]={0,0,0,0};
static u8 NtpRowHistoryIndex=0;
static u8 NtpIsHighSpeed=0;
signed int CfgData[36];   //在这个数组里设置好各种参数


s16 change_speed( juli , lose_row , angle_set)   //
{
						    

}





