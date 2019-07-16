

#include "pwm.h"
#include "MKL_TPM.h"
#include "LED.h"



s16 pwm_ch0=0;
s16 pwm_ch1=0;
s16 pwm_ch2=0;
s16 pwm_ch3=0;

extern float pid_sum;
extern float pid_sum_left;
extern float pid_sum_right;


extern float g_fAngle;

extern u8 lie_down_flag;

/***PWM��ʼ��***/
void pwm_initial(void)
{        
    tpm_pwm_init(TPM0,TPM_CH3,10000,0);    //PTE30   Ϊ0
    tpm_pwm_init(TPM0,TPM_CH0,10000,1);    //PTE24   Ϊ1  ����    ����
    
    tpm_pwm_init(TPM0,TPM_CH1,10000,0);    //PTE25   Ϊ0
    tpm_pwm_init(TPM0,TPM_CH2,10000,1);    //PTE29   Ϊ1  ����    ����     ��ʱ�ٶȷ���ֵΪ��
}

extern void Buzzer_Mie(void);
extern void Buzzer_Bi(void);
extern u8 image_start_flag;

/***PWM�趨***/
void  pwm_set(void)
{     
     //�������мӵģ�Ҫ�����  ��201402161132  �о�ûʲô�ã�֮ǰһֱû��
     //�������  ,û�õģ�����̫�󷴶�����   
    if (pid_sum > 0)
    {
       pid_sum_right += 0;
       pid_sum_left += 0;
    }
    else if (pid_sum < -0)
    {
       pid_sum_right += 0;
       pid_sum_left += 0;
    }
    else
    {
       pid_sum = 0;
    }
    
      
    if(pid_sum > 0)
    {
        pwm_ch0 = (s16)pid_sum_right;
        pwm_ch1 = 0;
        
        pwm_ch2 = (s16)pid_sum_left;
        pwm_ch3 = 0;
    }
    else if(pid_sum < 0)
    {
        pwm_ch0 = 0;
        pwm_ch1 = (s16)(0 - pid_sum_right);
        
        pwm_ch2 = 0;
        pwm_ch3 = (s16)(0 - (s16)pid_sum_left);
    }
    else
    {
          pwm_ch0 = pwm_ch0;
          pwm_ch1 = pwm_ch1;
          
          pwm_ch2 = pwm_ch2;
          pwm_ch3 = pwm_ch3;     
    }  
    

   
     //PWM���ͱ���
    if(pwm_ch0 > 1000)
    {
        pwm_ch0 = 1000;
    }
    if(pwm_ch1 > 1000)
    {
        pwm_ch1 = 1000;
    }
    if(pwm_ch2 > 1000)
    {
        pwm_ch2 = 1000;
    }
    if(pwm_ch3 > 1000)
    {
        pwm_ch3 = 1000;
    }
    
    if(pwm_ch0  < 0)
    {
        pwm_ch0 = 0;
    }
    if(pwm_ch1 < 0)
    {
        pwm_ch1 = 0;
    }
    if(pwm_ch2 < 0)
    {
        pwm_ch2 = 0;
    }
    if(pwm_ch3 < 0)
    {
        pwm_ch3 = 0;
    }
    

    if (image_start_flag != 0 ) //������
    {           
        tpm_pwm_duty(TPM0,TPM_CH1,0);
        tpm_pwm_duty(TPM0,TPM_CH2,0);
        
        tpm_pwm_duty(TPM0,TPM_CH3,0);
        tpm_pwm_duty(TPM0,TPM_CH0,0);                    
    }
    else
    {
        tpm_pwm_duty(TPM0,TPM_CH3,(u16)pwm_ch0);
        tpm_pwm_duty(TPM0,TPM_CH0,(u16)pwm_ch1);
    
        tpm_pwm_duty(TPM0,TPM_CH1,(u16)pwm_ch2);
        tpm_pwm_duty(TPM0,TPM_CH2,(u16)pwm_ch3);
    }
    
    
    
    if ( lie_down_flag != 0) //�Ե�
    {     
        Buzzer_Bi();
        DELAY_MS(100);
        Buzzer_Mie();
        
        tpm_pwm_duty(TPM0,TPM_CH1,0);
        tpm_pwm_duty(TPM0,TPM_CH2,0);
        
        tpm_pwm_duty(TPM0,TPM_CH3,0);
        tpm_pwm_duty(TPM0,TPM_CH0,0);
        while(1);/////////////////////����
    }
    
//    printf("pwm_ch0  %d,  \n",(s16)pwm_ch0); 
//    printf("pwm_ch2  %d,  \n",(s16)pwm_ch1);
//    printf("pwm_ch3  %d,  \n",(s16)pwm_ch2);
//    printf("pwm_ch3  %d,  \n",(s16)pwm_ch3);   
}