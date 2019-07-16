
#include "common.h"
#include "MKL_uart.h"
#include "MKL_adc.h"
#include "dolphin.h"
#include "l3g4200d.h"


extern s16 ACCEL_OFFSET;   //变小往前跑 加速度计静偏


u16 A_x;
u16 G_y;
u32 a_sum = 0;
u32 g_sum = 0;        //注意变量的符号啊
float a_value = 0;                           //重力加速度
float g_value = 0;                           //陀螺仪角速度
float g_fAngle;                              // 陀螺仪角度
float a_fAngle, g_fAngleSpeed;               // 重力角度(加速度计角度)，陀螺仪角速度
float g_fAngleIntegral;                      // 陀螺仪角速度积分值
volatile u8 mpu_finish_flag=0;

float g_fAngleSpeedIntegral = 0;    //陀螺仪直接积分得到的角度
s32 GYRO_OFFSET = 0;  //陀螺仪的静偏      变大往下



u16 G_dolphin = 0;      //另一个陀螺仪
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
    
    A_x = adc_once(ADC0_SE8, ADC_10bit); //加速度计
    G_y = adc_once(ADC0_SE9, ADC_10bit); //陀螺仪  直立  X轴
    G_dolphin = adc_once(ADC0_SE12, ADC_10bit); //转向陀螺仪
    for(i = 0; i<20; i+=1)  //改为40次求平均  和一次差不多
    {
        A_x = A_x*0.2 + adc_once(ADC0_SE8, ADC_10bit)*0.8;  //B0
        G_y = G_y*0.5 + adc_once(ADC0_SE9, ADC_10bit)*0.5;
        G_dolphin = G_dolphin*0.5 +  adc_once(ADC0_SE12, ADC_10bit)*0.5;   
    }
       
    a_sum = A_x;
    g_sum = G_y;
    
    
    a_value = a_sum; 
    a_value = FIR_A(a_value); //滤波  加速度计
    
    g_value = g_sum;  //陀螺仪1  ，直立
      
    G_dolphin = G_dolphin; //陀螺仪2  转向  Z轴
    
         
    
        a_fAngle = -(a_value - ACCEL_OFFSET ) * ACCEL_ANGLE_RATIO;        
                                            // 加速度角度=(加速度值-加速度偏移量) * 加速度比例因子       
	g_fAngleSpeed = -(s16)(g_value - GYRO_OFFSET) * GYRO_ANGLE_RATIO;  
                                            // 陀螺仪方向与加速度计相反,故GYRO_OFFSET - G_y    
                                            // 陀螺仪角速度=(陀螺仪值-陀螺仪偏移量) * 陀螺仪比例因子
                                            // 陀螺仪比例因子需调节,调节到陀螺仪直接积分得到的角度和加速度计的角度一致
        //另外的陀螺仪
        g_fAngleSpeed_dolphin =  -(s16)(G_dolphin - GYRO_OFFSET_dolphin)*ACCEL_ANGLE_RATIO;
        g_fAngleSpeedIntegral_dolphin += (g_fAngleSpeed_dolphin* FREQ_TIME);

        
	
       // 陀螺仪角度确定
	g_fAngle = g_fAngleIntegral;            // 陀螺仪角度 = 积分值
	fDeltaValue = ( a_fAngle - g_fAngle)/ACCEL_ADJUST_TIME_CONSTANT;
                                            // 角度偏差 = (加速度角度 - 陀螺仪角度) / 时间常数,
                                            // 时间补偿，跟毛刺的程度，需调
	
	//g_fAngleIntegral += (g_fAngleSpeed + fDeltaValue) / GYRO_ANGLE_SIGMA_FREQUENCY;
                                            // 积分值 += (陀螺仪角速度+角度偏差)/工作频率
                                            // 除的是200，其实就是乘以5ms，角速度乘以时间才是角度，然后对每个5ms积分，就是现在的角度
    
    g_fAngleIntegral += (g_fAngleSpeed + fDeltaValue) * FREQ_TIME;
                                                  // 积分时间,0.005,不调，和定时器时间一致
    
    g_fAngleSpeedIntegral += (g_fAngleSpeed * FREQ_TIME);  // 单陀螺仪积分得到的角度，调ANGLE_DOLPHIN,看她的波形
    
    
//  mpu_finish_flag = 1;   放到中断函数里
}



s32 GYRO_OFFSET_k = 0;

void gyro_off(void)     //陀螺仪开机自检
{   
    u16 i = 0;
    
   GYRO_OFFSET_k = 0;  //陀螺仪1
    for (i=0; i<400; i+=1)
    {
        GYRO_OFFSET_k += adc_once(ADC0_SE9, ADC_10bit);
        DELAY_MS(1);     //适当延时，
    }
    GYRO_OFFSET_k  = GYRO_OFFSET_k/400;
       
    for (i=0; i<100; i+=1)
    {
        GYRO_OFFSET_k = GYRO_OFFSET_k*0.5 + adc_once(ADC0_SE9, ADC_10bit)*0.5;
        DELAY_MS(1);
    }
    GYRO_OFFSET = GYRO_OFFSET_k;
    G_y = GYRO_OFFSET;
    //////////////////////////////////////////直立陀螺仪自检完毕
    
   
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
    ////////////////////////////////////////////加速度计校正，其实没用 
    
       
    
    //另一个陀螺仪自检
    
    GYRO_OFFSET_k = 0;
    for (i=0; i<200; i+=1)
    {
        GYRO_OFFSET_k += adc_once(ADC0_SE12, ADC_10bit);
        DELAY_MS(1);  //适当延时，必须
    }
    GYRO_OFFSET_k  = GYRO_OFFSET_k/200;
    
    for (i=0; i<100; i+=1)
    {
        GYRO_OFFSET_k = GYRO_OFFSET_k*0.5 + adc_once(ADC0_SE12, ADC_10bit)*0.5;
        DELAY_MS(1);
    }
    GYRO_OFFSET_dolphin = GYRO_OFFSET_k;  
    G_dolphin = GYRO_OFFSET_dolphin; 
    ///////////////////////////////////////////////////////////////////转向陀螺仪自检 校正完毕   
}


/* 
    陀螺仪比例系数  GYRO_ANGLE_RATIO

    Rgyro参数确定：(看曲线)
    选择Rgyro分别为0、0.5、1.0做实验，判断Rgyro应该在0~0.5之间(假设)
    Rgyro分别为0.25、0.75做实验，判断Rgyro应该在0.25~0.5之间
    Rgyro分别为0.3、0.35做实验，判断Rgyro应该在0.3~0.35之间
    Rgyro为0.325，可以判断该参数比较适合
    
    加速度值输出曲线有很多噪声
    而陀螺仪积分值则很光滑
    (这就是用陀螺仪测量角度的原因，加速度值只是作为一个参考量使陀螺仪积分值与它保持一致)
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
    零偏确定，包括加速度值最大值与最小值

    加速度x轴最大值：水平向右垂直90°，静止时读取数值
    加速度x轴最小值：水平向左垂直90°，静止时读取数值
    加速度计比例因子：最大值-最小值

    Accel=            // 零偏
    Gyro=
    Accel=
    Gyro=

    Accel=         // 最大值
    Gyro=
    Accel=
    Gyro=

    Accel=        // 最小值
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


/*   这个函数在 math.h中声明了  不注释就会出错 
也不知道为什么在这里，干嘛的
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
