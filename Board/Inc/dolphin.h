

#ifndef _DOLPHIN_H_
#define _DOLPHIN_H_


/************************* main ***************************/           

extern  s16  duoji();            // typedef   short int  s16
extern  void set_motor();
extern  void Camera_data_show();
extern  s16  change_speed();
extern  s16  hongwai_ceju();
extern  s16  camera_2_num();
extern  void   GetWay();
extern  s16  duoji_sanjiao();
#define BUZZER PTC4  //PTD0
#define Buzzer_Init gpio_init(BUZZER,GPO,0)    //GPI ����
void Buzzer_Bi(void)  {GPIO_PDOR_REG(GPIOX_BASE(BUZZER))  |= (1 << PTn(BUZZER));}   //��
void Buzzer_Mie(void)  {GPIO_PDOR_REG(GPIOX_BASE(BUZZER)) &= ~(1 << PTn(BUZZER));}  //��
void main_initial(void);
void PORTA_IRQHandler(void);     //PORT�жϷ�����
void DMA0_IRQHandler(void);
void PIT_IRQHandler(void);        //PIT��ʱ�ж�
void uart0_handler(void);
extern  u8  Sanjiao_panduan();
extern    u8  this_is_sanjiao;

#endif