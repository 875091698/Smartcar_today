#include "common.h"
#include "include.h"

///////////////
u8 dolphin_k = 0;
s16 expect_speed=75;//����ٶ�����ֵ75
u32 zhongzhi;
s16 pulse_count=0,distance_0=0;
u32 i=0;
extern  u8  this_is_sanjiao;
extern  u8 podao_0;
extern  u8 danbian;
float kp_duoji_G;
float kp_duoji_S;
int mode=0; //��ͣ�� 
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
    main_initial(); //�����豸��ʼ�� 1510    
    zhongzhi=600;   //100hz  1500     300hz 4515  hongche674   baiche692
    u8 error;
    s16 angle;
    u8 Sanflag;      
 
     if(kaiche)
     {
        while(1)    //����ʱ����ѭ��������
      {
        
         if( key_get(KEY_U)==KEY_DOWN ) 
        {
           DELAY_MS(100);//1500
         if( key_get(KEY_U)==KEY_DOWN)
         {
           uart_putchar   (UART0 , '1');                    //�����ַ�1Ϊǰ������λ
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
             Img_extract();   //   ��ѹͼ�� 
             hongwai = adc_once(ADC0_SE8, ADC_8bit);  //��ȡ�����ֵ��
             Sanflag=Sanjiao_panduan();//0;
             
             speed_ctrol(); //ǰ���ٶȵ�ѡ�񣡣�
             
           
             if(podao_0==1||mode==7||mode==8||danbian==1) //��Ϊ����ĳ�Ҫ������
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
             led_turn(LED1);   //������Ƶ��  
             continue;
             }
             else
             {
             i=camera_2_num(); // ��ȡ����  ����ƫ��  �����ҵ�������
             angle = duoji(i, expect_speed, pulse_count);   //�������  10MS+����һ�ζ��
            // expect_speed = 75;//change_speed(hongwai,i ,angle); //���������ٶ�      angleΪ�Ƕ�΢�����������������������angle
             led_turn(LED3);   //������Ƶ�� 
             if(i>50)//i>=54
             {
             err++;   
             }
        
            }    
            if(err>3)      //����ͣ��   >=5
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
           Camera_data_show();        //������ͼ������
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
       {                                                 // ��׳���������
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
        count_00++;           //����ʱ˭�ں�˭����
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
      if(sudu<0)//ͨ��һ�ξ�����ٿ�ʼ�����û�мӿ�����
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
extern u8 *img_bin_buff;  //ָ��ԭʼ��ֵͼ��

void main_initial(void)
{
  
    #define INT_COUNT 0xFFFF                            //LPT �����жϵļ�������
    set_vector_handler(UART0_VECTORn,uart0_handler);   // �����жϷ��������ж���������
    uart_rx_irq_en (UART0);                            //�����ڽ����ж�
    uart_init(UART0,115200);
    Buzzer_Init;
    adc_init(ADC0_SE8);              //ADC��ʼ��
    led_init(LED_MAX);                                 //��ʼ��LED�˿�
    LED_Init();                                        //OLED��ʼ��
    tpm_pwm_init(TPM0, TPM_CH0,15000, 0);              //�����ʼ��   ��ʼ�� TPM0_CH0 ΪƵ�� 15000Hz ��PWM, ռ�ձ�Ϊ 0/TPM0_PRECISON
    tpm_pwm_init(TPM0, TPM_CH1,15000, 0);
    Buzzer_Init;    
    tpm_pwm_init(TPM1, TPM_CH0,50, zhongzhi);         //���100HZ��300HZ�ĺ�      
    Camera_data_init();                               //����ͷ���ݳ�ʼ�����ܹؼ�
    camera_init(img_bin_buff);                       //����ͷ��ʼ��  Դ��ַPTE_B2_IN �� PCLK:A7  ���жϣ�A6  SCCB_SDA:A16  SCCB_SCL:A17
    set_vector_handler(PORTA_VECTORn ,PORTA_IRQHandler);    //���ó��жϸ�λ����Ϊ PORTA_IRQHandler
    set_vector_handler(DMA0_VECTORn ,DMA0_IRQHandler);      //����DMA���жϸ�λ����Ϊ PORTA_IRQHandler
    camera_get_img();  //��ʼȡͼ��
    tpm_pulse_init(TPM2,TPM_CLKIN0,TPM_PS_1); // C12���� ��ʼ�� TPM2 Ϊ�����ۼӣ�����ܽ�Ϊ TPM_CLKIN0_PIN ����Ƶϵ��Ϊ 1   
    tpm_pulse_clean(TPM2);  //����������������ֵ��������գ��������ܱ�֤����ֵ׼ȷ��   
    pit_init_ms(PIT0, 5);    //��ʱ��0 5ms ��ʱ
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
      pulse_count=tpm_pulse_get(TPM2);            // ��ǰ�ٶ�   
      tpm_pulse_clean(TPM2);  //����������������ֵ 
      
      set_motor(expect_speed,pulse_count);   //�õ��ת������������
     }
     
    PIT_Flag_Clear(PIT0);       //���жϱ�־λ
    } 
}

void PORTA_IRQHandler(void)
{
    uint8  n = 0;    //���ź�
    uint32 flag = PORTA_ISFR;
  PORTA_ISFR  = ~0;                                   //���жϱ�־λ������camera_vsync();���棬��Ϊ�������ж���
    n = 2;                                              //���ж�
    if(flag & (1 << n))                                 //PTA6�����ж�
    {       
      camera_vsync();   //�� PORTA_ISFR & ( 1 << n )��: PTA6�����ж� ��  �� PORTA_ISFR = ( 1 << n )��  д 1 ���ն˱�־λ
    }
#if 0             //ӥ��ֱ��ȫ�ٲɼ�������Ҫ���ж�
    n = 7;
    if(flag & (1 << n))                                 //PTA5�����ж�
    {
        camera_href();
    }
#endif
}

/*!
 *  @brief      DMA0�жϷ�����
 *  @since      v5.0
 */
void DMA0_IRQHandler(void)
{  
  camera_dma(); 
  dolphin_k = 1;
   this_is_sanjiao=0;       //�ؼ���ʷ����ǰ��־λ
}
 char str[2]={0,0}; u8 j=0,h=0;
void uart0_handler(void)
{     
    char ch;    //�û���Ҫ�����������
/**************************************/    
/*    
       MODE=3��ǰ��ͣ�ø���ͨ���ı�־                 
       MODE=4��ǰ���ҵ�˫����ͣ�µĳ���
       MODE=5�Ƿ��ͱ�־�ú���ĳ���ת
       MODE=6�Ƿ��ͱ�־�ú���ĳ���ת
       MODE=7��ǰ��ĳ������ڿ鷢�ͱ�־���󳵣����ٴο����ڽǲŻ�ת��
 */  
/**************************************/  
    if(uart_query    (UART0) == 1)   //�������ݼĴ�����
    {  
        //�û���Ҫ�����������
       uart_getchar   (UART0, &ch);                    //���޵ȴ�����1���ֽ�
       
          if(ch=='1')
          {mode=1;}
          if(ch=='2')
          {mode=2;}
          if(ch=='3')
          {mode=3;}    //����3��ʱ������Ϊ����ĳ�ͨ���ӿ����� 
          if(ch=='4')   //����4��ʱ�����ӿ���ͣ��
          {
          mode=4;
          this_is_jiakuan=0;
          }
          if(ch=='5')
          {mode=5;}
          if(ch=='6')
          {mode=6;}
          if(ch=='7')  //���
          {
            mode=7;
       //   sudu==-10;
          }
          if(ch=='8')  //�ұ�
          {
            mode=8;
        //  sudu==-10;
          }
          
           if(ch=='9')
          {mode=9;}
          
    }
}

