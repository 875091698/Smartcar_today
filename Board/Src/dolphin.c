
#include "common.h"
#include "MKL_uart.h"
#include "MKL_adc.h"
#include "dolphin.h"
#include "l3g4200d.h"


extern s16 ACCEL_OFFSET;   //��С��ǰ�� ���ٶȼƾ�ƫ


u16 A_x;
u16 G_y;
u32 a_sum = 0;
u32 g_sum = 0;        //ע������ķ��Ű�
float a_value = 0;                           //�������ٶ�
float g_value = 0;                           //�����ǽ��ٶ�
float g_fAngle;                              // �����ǽǶ�
float a_fAngle, g_fAngleSpeed;               // �����Ƕ�(���ٶȼƽǶ�)�������ǽ��ٶ�
float g_fAngleIntegral;                      // �����ǽ��ٶȻ���ֵ
volatile u8 mpu_finish_flag=0;

float g_fAngleSpeedIntegral = 0;    //������ֱ�ӻ��ֵõ��ĽǶ�
s32 GYRO_OFFSET = 0;  //�����ǵľ�ƫ      �������



u16 G_dolphin = 0;      //��һ��������
float g_fAngleSpeed_dolphin = 0;
float g_fAngleSpeedIntegral_dolphin = 0;
s32 GYRO_OFFSET_dolphin = 0;



//LOW_PASS,Fs=200,HAMMING,wp=3,ws=9
#define FILTERTAPS_A 5
float coef_a[FILTERTAPS_A] = {0.001193,0.010720,0.026164,0.026164,0.010720};
float gain_a=13.340246;    
float values_a[FILTERTAPS_A] = {0}; 

float FIR_A(float in){
   static u8 k;
   u8 i = 0; 
   float out = 0;
   values_a[k] = in; 
   for (i=0; i<FILTERTAPS_A; i++) {            
     out += coef_a[i] * values_a[(i + k) % FILTERTAPS_A];                          
    }
    out *= gain_a;                        
    k = (k+1) % FILTERTAPS_A;                  
    return out;                        
}





void AngleCalculate() 
{
    float fDeltaValue;
    
    u8 i = 0;
    a_sum = 0;
    g_sum = 0;
    
    A_x = adc_once(ADC0_SE8, ADC_10bit); //���ٶȼ�
    G_y = adc_once(ADC0_SE9, ADC_10bit); //������  ֱ��  X��
    G_dolphin = adc_once(ADC0_SE12, ADC_10bit); //ת��������
    for(i = 0; i<20; i+=1)  //��Ϊ40����ƽ��  ��һ�β��
    {
        A_x = A_x*0.2 + adc_once(ADC0_SE8, ADC_10bit)*0.8;  //B0
        G_y = G_y*0.5 + adc_once(ADC0_SE9, ADC_10bit)*0.5;
        G_dolphin = G_dolphin*0.5 +  adc_once(ADC0_SE12, ADC_10bit)*0.5;   
    }
       
    a_sum = A_x;
    g_sum = G_y;
    
    
    a_value = a_sum; 
    a_value = FIR_A(a_value); //�˲�  ���ٶȼ�
    
    g_value = g_sum;  //������1  ��ֱ��
      
    G_dolphin = G_dolphin; //������2  ת��  Z��
    
         
    
        a_fAngle = -(a_value - ACCEL_OFFSET ) * ACCEL_ANGLE_RATIO;        
                                            // ���ٶȽǶ�=(���ٶ�ֵ-���ٶ�ƫ����) * ���ٶȱ�������       
	g_fAngleSpeed = -(s16)(g_value - GYRO_OFFSET) * GYRO_ANGLE_RATIO;  
                                            // �����Ƿ�������ٶȼ��෴,��GYRO_OFFSET - G_y    
                                            // �����ǽ��ٶ�=(������ֵ-������ƫ����) * �����Ǳ�������
                                            // �����Ǳ������������,���ڵ�������ֱ�ӻ��ֵõ��ĽǶȺͼ��ٶȼƵĽǶ�һ��
        //�����������
        g_fAngleSpeed_dolphin =  -(s16)(G_dolphin - GYRO_OFFSET_dolphin)*ACCEL_ANGLE_RATIO;
        g_fAngleSpeedIntegral_dolphin += (g_fAngleSpeed_dolphin* FREQ_TIME);

        
	
       // �����ǽǶ�ȷ��
	g_fAngle = g_fAngleIntegral;            // �����ǽǶ� = ����ֵ
	fDeltaValue = ( a_fAngle - g_fAngle)/ACCEL_ADJUST_TIME_CONSTANT;
                                            // �Ƕ�ƫ�� = (���ٶȽǶ� - �����ǽǶ�) / ʱ�䳣��,
                                            // ʱ�䲹������ë�̵ĳ̶ȣ����
	
	//g_fAngleIntegral += (g_fAngleSpeed + fDeltaValue) / GYRO_ANGLE_SIGMA_FREQUENCY;
                                            // ����ֵ += (�����ǽ��ٶ�+�Ƕ�ƫ��)/����Ƶ��
                                            // ������200����ʵ���ǳ���5ms�����ٶȳ���ʱ����ǽǶȣ�Ȼ���ÿ��5ms���֣��������ڵĽǶ�
    
    g_fAngleIntegral += (g_fAngleSpeed + fDeltaValue) * FREQ_TIME;
                                                  // ����ʱ��,0.005,�������Ͷ�ʱ��ʱ��һ��
    
    g_fAngleSpeedIntegral += (g_fAngleSpeed * FREQ_TIME);  // �������ǻ��ֵõ��ĽǶȣ���ANGLE_DOLPHIN,�����Ĳ���
    
    
//  mpu_finish_flag = 1;   �ŵ��жϺ�����
}



s32 GYRO_OFFSET_k = 0;

void gyro_off(void)     //�����ǿ����Լ�
{   
    u16 i = 0;
    
   GYRO_OFFSET_k = 0;  //������1
    for (i=0; i<400; i+=1)
    {
        GYRO_OFFSET_k += adc_once(ADC0_SE9, ADC_10bit);
        DELAY_MS(1);     //�ʵ���ʱ��
    }
    GYRO_OFFSET_k  = GYRO_OFFSET_k/400;
       
    for (i=0; i<100; i+=1)
    {
        GYRO_OFFSET_k = GYRO_OFFSET_k*0.5 + adc_once(ADC0_SE9, ADC_10bit)*0.5;
        DELAY_MS(1);
    }
    GYRO_OFFSET = GYRO_OFFSET_k;
    G_y = GYRO_OFFSET;
    //////////////////////////////////////////ֱ���������Լ����
    
   
    GYRO_OFFSET_k = 0;
    for (i=0; i<100; i+=1)
    {
        GYRO_OFFSET_k += adc_once(ADC0_SE8, ADC_10bit);
        DELAY_MS(1);
    }
    A_x = GYRO_OFFSET_k/100;
    for (i=0; i<100; i+=1)
    {
        A_x = A_x*0.5 + adc_once(ADC0_SE8, ADC_10bit)*0.5;
        DELAY_MS(1);
    }
    ////////////////////////////////////////////���ٶȼ�У������ʵû�� 
    
       
    
    //��һ���������Լ�
    
    GYRO_OFFSET_k = 0;
    for (i=0; i<200; i+=1)
    {
        GYRO_OFFSET_k += adc_once(ADC0_SE12, ADC_10bit);
        DELAY_MS(1);  //�ʵ���ʱ������
    }
    GYRO_OFFSET_k  = GYRO_OFFSET_k/200;
    
    for (i=0; i<100; i+=1)
    {
        GYRO_OFFSET_k = GYRO_OFFSET_k*0.5 + adc_once(ADC0_SE12, ADC_10bit)*0.5;
        DELAY_MS(1);
    }
    GYRO_OFFSET_dolphin = GYRO_OFFSET_k;  
    G_dolphin = GYRO_OFFSET_dolphin; 
    ///////////////////////////////////////////////////////////////////ת���������Լ� У�����   
}


/* 
    �����Ǳ���ϵ��  GYRO_ANGLE_RATIO

    Rgyro����ȷ����(������)
    ѡ��Rgyro�ֱ�Ϊ0��0.5��1.0��ʵ�飬�ж�RgyroӦ����0~0.5֮��(����)
    Rgyro�ֱ�Ϊ0.25��0.75��ʵ�飬�ж�RgyroӦ����0.25~0.5֮��
    Rgyro�ֱ�Ϊ0.3��0.35��ʵ�飬�ж�RgyroӦ����0.3~0.35֮��
    RgyroΪ0.325�������жϸò����Ƚ��ʺ�
    
    ���ٶ�ֵ��������кܶ�����
    �������ǻ���ֵ��ܹ⻬
    (������������ǲ����Ƕȵ�ԭ�򣬼��ٶ�ֵֻ����Ϊһ���ο���ʹ�����ǻ���ֵ��������һ��)
*/





unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT);

u16 count=0;

void r_Gyro_display()
{       
    
        int temp[4] = {0};
        unsigned int temp1[4] = {0};
        unsigned char databuf[10] = {0};
        unsigned char i;
        unsigned short CRC16 = 0;
        float OutData[4];
        
        
    if(mpu_finish_flag == 1)
    {     
    //  mpu_finish_flag=0;
 
        OutData[0] = a_fAngle;         //   a_value  
        OutData[1] = g_fAngle;      //g_value       G_dolphin  G_dolphin
        OutData[2] = g_fAngleSpeed;//(s16)(ACCEL_OFFSET-a_sum)*ACCEL_ANGLE_RATIO   g_fAngleSpeed_dolphin
        OutData[3] = g_fAngleSpeedIntegral;    // g_fAngleSpeed-(s16)  0g_fAngleSpeedIntegral_dolphin   
        
        for(i=0;i<4;i++)
        {        
            temp[i]  = (int)OutData[i];
            temp1[i] = (unsigned int)temp[i];        
        }
        
        for(i=0;i<4;i++) 
        {
            databuf[i*2]   = (unsigned char)(temp1[i]%256);
            databuf[i*2+1] = (unsigned char)(temp1[i]/256);
        }
        
        CRC16 = CRC_CHECK(databuf,8);
        databuf[8] = CRC16%256;
        databuf[9] = CRC16/256;
        
        for(i=0;i<10;i++)
        uart_putchar(FIRE_PORT,databuf[i]);  //UART? 
    }
}



/* 
    ��ƫȷ�����������ٶ�ֵ���ֵ����Сֵ

    ���ٶ�x�����ֵ��ˮƽ���Ҵ�ֱ90�㣬��ֹʱ��ȡ��ֵ
    ���ٶ�x����Сֵ��ˮƽ����ֱ90�㣬��ֹʱ��ȡ��ֵ
    ���ٶȼƱ������ӣ����ֵ-��Сֵ

    Accel=            // ��ƫ
    Gyro=
    Accel=
    Gyro=

    Accel=         // ���ֵ
    Gyro=
    Accel=
    Gyro=

    Accel=        // ��Сֵ
    Gyro=
    Accel=
    Gyro=
*/

s32 Accel_zero=0;
s32 Gyro_zero=0;

void zero_display()
{
    if(mpu_finish_flag==1)
    {
   //   mpu_finish_flag=0;
        count++;
        
        Accel_zero += A_x;                //G_y
        Gyro_zero += g_fAngle;   //g_fAngleSpeedIntegral; g_fAngleIntegral
                         
        if(count==200)
        {
            count=0;
            
            if(Accel_zero>0)                
                printf("Accel=%d\n",Accel_zero/200);
            else
                printf("Accel=-%d\n",-Accel_zero/200);
            
            if(Gyro_zero>0)                
                printf("Gyro=%d\n",Gyro_zero/200);
            else
                printf("Gyro=-%d\n",-Gyro_zero/200);    
            
            Accel_zero=0;
            Gyro_zero=0;              
        }        
    }    
}


/*   ��������� math.h��������  ��ע�;ͻ���� 
Ҳ��֪��Ϊʲô����������
float fabs(float c)
{
     if(c<0)        
      c=-c;
    
    return c;
}
*/


unsigned short static CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0;i<CRC_CNT; i++){      
        CRC_Temp ^= Buf[i];
        for (j=0;j<8;j++) {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}
