#include "common.h"
#include "include.h"

///////////////
u8 dolphin_k = 0;
s16 expect_speed=75;//存放速度期望值75
u32 zhongzhi;
s16 pulse_count=0,distance_0=0;
u32 i=0;
extern  u8  this_is_sanjiao;
extern  u8 podao_0;
extern  u8 danbian;
float kp_duoji_G;
float kp_duoji_S;
int mode=0; //是停车 
u8 startflage_houche=0; 
extern  u8 this_is_jiakuan;
extern u8 this_is_sanjiao;
extern u8 kaiche;
void speed_ctrol(void);   
s16 hongwai=0;  
u8 err=0;
u8 stop=0;
 void  main(void)
{  
    main_initial(); //所有设备初始化 1510    
    zhongzhi=600;   //100hz  1500     300hz 4515  hongche674   baiche692
    u8 error;
    s16 angle;
    u8 Sanflag;      
 
     if(kaiche)
     {
        while(1)    //开机时的死循环！！！
      {
        
         if( key_get(KEY_U)==KEY_DOWN ) 
        {
           DELAY_MS(100);//1500
         if( key_get(KEY_U)==KEY_DOWN)
         {
           uart_putchar   (UART0 , '1');                    //发送字符1为前车启动位
           Buzzer_Bi();   
         }   
        }
      
        if(mode==1)//
        {
         DELAY_MS(1500);//1500  
         startflage_houche=1;   

        break;
        }  
      
      } 
      //DELAY_MS(1000);
        Buzzer_Mie(); 
     }
     else
     {
     startflage_houche=1;  
     }
    while(1)
    { 
       camera_get_img();  
          
       if(dolphin_k==1)
        {  
             
             Sanflag= 0;
             dolphin_k = 0;  
             Img_extract();   //   解压图像 
             hongwai = adc_once(ADC0_SE8, ADC_8bit);  //获取红外的值！
             Sanflag=Sanjiao_panduan();//0;
             
             speed_ctrol(); //前后车速度的选择！！
             
           
             if(podao_0==1||mode==7||mode==8||danbian==1) //作为后面的车要超车了
             {
             Buzzer_Bi();
             Sanflag=0;
             }
             else
             {Buzzer_Mie();
             }
             if(Sanflag)//Sanflag
             {
             GetWay();
             duoji_sanjiao();
             this_is_sanjiao=0;
             led_turn(LED1);   //灯闪看频率  
             continue;
             }
             else
             {
             i=camera_2_num(); // 提取中线  计算偏差  返回找到的行数
             angle = duoji(i, expect_speed, pulse_count);   //舵机控制  10MS+控制一次舵机
            // expect_speed = 75;//change_speed(hongwai,i ,angle); //计算期望速度      angle为角度微分项！！！！！！！！！！！angle
             led_turn(LED3);   //灯闪看频率 
             if(i>50)//i>=54
             {
             err++;   
             }
        
            }    
            if(err>3)      //出界停车   >=5
             {
            // stop=1;  
             }    
        }
     distance_0=distance_0+pulse_count;
     if(distance_0>60000)
     distance_0=60000;
   // LED_P6x8Str(2,6,"distance_0");
   // LED_PrintValueF(72,6,   distance_0/100, 1);
   // LED_P6x8Str(2,6,"hongwai");
   // LED_PrintValueF(72,6,   hongwai, 1);
   /// LED_P6x8Str(2,7,"expect_speed");
   // LED_PrintValueF(72,7,   expect_speed, 1); 
     
        //  OutData[0] = expect_speed;
        //  OutData[1] = pulse_count; 
        //  OutData[2] = expect_speed-pulse_count;       
        //  OutData[3] =0;
        //  OutPut_Data(); 
 
       
       
     #if 0   
     LED_P6x8Str(2,3,"zhongzhi");
     LED_PrintValueF(72,3,   zhongzhi, 1);
       
    if( key_get(KEY_U)==KEY_DOWN)
    {
       DELAY_MS(100);
     if( key_get(KEY_U)==KEY_DOWN)
       zhongzhi=zhongzhi+2; 
    } 
     if( key_get(KEY_D)==KEY_DOWN)
    {
       DELAY_MS(100);
     if( key_get(KEY_D)==KEY_DOWN)
      zhongzhi=zhongzhi-2;
    } 
    #endif
    
    #if 0
          car_show();
    #endif
       
    #if 0
           Camera_data_show();        //串口送图像数据
    #endif
   
    }
   
}

s16 sudu=0;//0
u8  count_00=0;
u8  count=0;
s16 distance_sudu=0;
s16 distance_jiasu_sudu=0;
void speed_ctrol()
{   
      s8 sanjiao_jiasu=7; 
   
     /*****************************/
      if(this_is_jiakuan==0)
       {
        if(podao_0==1)
        {
         expect_speed=75;
        }
        else
        {
      
          expect_speed =hongwai_ceju(hongwai); 
        }
       }       
      else
       {                                                 // 宽白超车区加速
         expect_speed=0;
       }
      if(mode==2)  
      {
         expect_speed=120;  
      }
       if(stop==1)
       {
       expect_speed=0;
       }
       /*****************************/

      /*****************************/
      
      /*****************************/
  /*    if(distance_0<=1000)
      {
        if(hongwai>70)
        {
        count_00++;           //开车时谁在后谁加速
        }  
        if( count_00>50)
        {
           sudu=4;
        }
      }

      
      ///////////////////////////////
      if(this_is_jiakuan==1)
      {
          if(sudu<=4)
          {
            sudu=4;
          }  
      } */
      if(sudu<0)//通过一段距离后再开始检测有没有加宽区域！
     { 
      distance_sudu=distance_sudu+pulse_count;
       if(distance_sudu>=3500)//distance_of_sanjiao>=1000
       {
         distance_sudu=0;
         sudu=0;
       }
     } 
      if(mode==9)
      {
         sudu=sanjiao_jiasu;
      }
     if(sudu==sanjiao_jiasu)  
     {
        distance_jiasu_sudu=distance_jiasu_sudu+pulse_count;
       if(distance_jiasu_sudu>=8000)//distance_of_sanjiao>=1000
       {
         distance_jiasu_sudu=0;
         sudu=4;
       }
     
     }
     /*****************************/   
      
   // LED_P6x8Str(2,6,"count_00");
   // LED_PrintValueF(72,6,   count_00, 1);
   // LED_P6x8Str(2,7,"sudu");
   // LED_PrintValueF(72,7,   sudu, 1); 

}
extern u8 *img_bin_buff;  //指向原始二值图像

void main_initial(void)
{
  
    #define INT_COUNT 0xFFFF                            //LPT 产生中断的计数次数
    set_vector_handler(UART0_VECTORn,uart0_handler);   // 设置中断服务函数到中断向量表里
    uart_rx_irq_en (UART0);                            //开串口接收中断
    uart_init(UART0,115200);
    Buzzer_Init;
    adc_init(ADC0_SE8);              //ADC初始化
    led_init(LED_MAX);                                 //初始化LED端口
    LED_Init();                                        //OLED初始化
    tpm_pwm_init(TPM0, TPM_CH0,15000, 0);              //电机初始化   初始化 TPM0_CH0 为频率 15000Hz 的PWM, 占空比为 0/TPM0_PRECISON
    tpm_pwm_init(TPM0, TPM_CH1,15000, 0);
    Buzzer_Init;    
    tpm_pwm_init(TPM1, TPM_CH0,50, zhongzhi);         //舵机100HZ比300HZ的好      
    Camera_data_init();                               //摄像头数据初始化，很关键
    camera_init(img_bin_buff);                       //摄像头初始化  源地址PTE_B2_IN ， PCLK:A7  场中断：A6  SCCB_SDA:A16  SCCB_SCL:A17
    set_vector_handler(PORTA_VECTORn ,PORTA_IRQHandler);    //设置场中断复位函数为 PORTA_IRQHandler
    set_vector_handler(DMA0_VECTORn ,DMA0_IRQHandler);      //设置DMA的中断复位函数为 PORTA_IRQHandler
    camera_get_img();  //开始取图像
    tpm_pulse_init(TPM2,TPM_CLKIN0,TPM_PS_1); // C12右轮 初始化 TPM2 为脉冲累加，输入管脚为 TPM_CLKIN0_PIN ，分频系数为 1   
    tpm_pulse_clean(TPM2);  //清空脉冲计数器计算值（马上清空，这样才能保证计数值准确）   
    pit_init_ms(PIT0, 5);    //定时器0 5ms 定时
    set_vector_handler(PIT_VECTORn, PIT_IRQHandler);
   //enable_irq(PIT_IRQn);     
    key_init(KEY_MAX);  
    adjust_init();
    
}

u16 ding1=0,ding2=0,ding3=0,ding4=0;
extern u8 chaoche_jiakuan_7;
void PIT_IRQHandler()
{

  if (PIT_TFLG(PIT0) == 1 )
    {
      
      if(mode==5||mode==6||mode==7||mode==8)
      {
      ding1++;
      if(ding1>500)
      {
      mode=0;
      ding1=0;
      }
      }
         if(mode==9)
      {
       ding2++;
      if(ding2>750)
      {   
        sudu=6;
        ding2=0;
      }
      }
      if(chaoche_jiakuan_7)
      {
        if(this_is_jiakuan==1)
        {
          ding3++;
          if(ding3>350)
          {
          this_is_jiakuan=0;
          ding3=0;
          }
        }
      }
              if(this_is_jiakuan==1)
        {
          ding4++;
          if(ding4>2500)
          {
          this_is_jiakuan=0;
          ding4=0;
          }
        }
     if(startflage_houche==1)
     {     
      pulse_count=0;
      pulse_count=tpm_pulse_get(TPM2);            // 当前速度   
      tpm_pulse_clean(TPM2);  //清空脉冲计数器计算值 
      
      set_motor(expect_speed,pulse_count);   //让电机转！！！！！！
     }
     
    PIT_Flag_Clear(PIT0);       //清中断标志位
    } 
}

void PORTA_IRQHandler(void)
{
    uint8  n = 0;    //引脚号
    uint32 flag = PORTA_ISFR;
  PORTA_ISFR  = ~0;                                   //清中断标志位，放在camera_vsync();里面，因为不用行中断先
    n = 2;                                              //场中断
    if(flag & (1 << n))                                 //PTA6触发中断
    {       
      camera_vsync();   //（ PORTA_ISFR & ( 1 << n )）: PTA6触发中断 ；  （ PORTA_ISFR = ( 1 << n )）  写 1 清终端标志位
    }
#if 0             //鹰眼直接全速采集，不需要行中断
    n = 7;
    if(flag & (1 << n))                                 //PTA5触发中断
    {
        camera_href();
    }
#endif
}

/*!
 *  @brief      DMA0中断服务函数
 *  @since      v5.0
 */
void DMA0_IRQHandler(void)
{  
  camera_dma(); 
  dolphin_k = 1;
   this_is_sanjiao=0;       //关键，史上最前标志位
}
 char str[2]={0,0}; u8 j=0,h=0;
void uart0_handler(void)
{     
    char ch;    //用户需要处理接收数据
/**************************************/    
/*    
       MODE=3是前车停好给后车通过的标志                 
       MODE=4是前车找到双边让停下的车动
       MODE=5是发送标志让后面的车左转
       MODE=6是发送标志让后面的车右转
       MODE=7是前面的车看到黑块发送标志到后车，后车再次看到黑角才会转向
 */  
/**************************************/  
    if(uart_query    (UART0) == 1)   //接收数据寄存器满
    {  
        //用户需要处理接收数据
       uart_getchar   (UART0, &ch);                    //无限等待接受1个字节
       
          if(ch=='1')
          {mode=1;}
          if(ch=='2')
          {mode=2;}
          if(ch=='3')
          {mode=3;}    //等于3的时候它作为后面的车通过加宽超车区 
          if(ch=='4')   //等于4的时候解除加宽区停车
          {
          mode=4;
          this_is_jiakuan=0;
          }
          if(ch=='5')
          {mode=5;}
          if(ch=='6')
          {mode=6;}
          if(ch=='7')  //左边
          {
            mode=7;
       //   sudu==-10;
          }
          if(ch=='8')  //右边
          {
            mode=8;
        //  sudu==-10;
          }
          
           if(ch=='9')
          {mode=9;}
          
    }
}

