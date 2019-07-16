
#ifndef _5110_H_
#define _5110_H_

#include "MKL_gpio.h"

#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    //�üĴ�����һλ
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  //��Ĵ�����һλ
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  //��üĴ���һλ��״̬

#define CE    PTC17        // Ĭ�϶˿�ΪPTE����Ҫ��
#define SCLK  PTD2
#define SDIN  PTD1
#define RST   PTC16
#define DC    PTD0

//#define LCD_PORT    XXX  
//#define LCD_PDOR    XXX

//#define CE_0        BCLR(CE,LCD_PDOR)
//#define CE_1        BSET(CE,LCD_PDOR)
//#define SCLK_0      BCLR(SCLK,LCD_PDOR)
//#define SCLK_1      BSET(SCLK,LCD_PDOR)
//#define SDIN_0      BCLR(SDIN,LCD_PDOR)
//#define SDIN_1      BSET(SDIN,LCD_PDOR)
//#define RST_0       BCLR(RST,LCD_PDOR)
//#define RST_1       BSET(RST,LCD_PDOR)
//#define DC_0        BCLR(DC,LCD_PDOR)
//#define DC_1        BSET(DC,LCD_PDOR)

#define CE_0        GPIO_PDOR_REG(GPIOX_BASE(CE)) &= ~(1 << PTn(CE));
#define CE_1        GPIO_PDOR_REG(GPIOX_BASE(CE))  |= (1 << PTn(CE));
#define SCLK_0      GPIO_PDOR_REG(GPIOX_BASE(SCLK)) &= ~(1 << PTn(SCLK));
#define SCLK_1      GPIO_PDOR_REG(GPIOX_BASE(SCLK))  |= (1 << PTn(SCLK));
#define SDIN_0      GPIO_PDOR_REG(GPIOX_BASE(SDIN)) &= ~(1 << PTn(SDIN));
#define SDIN_1      GPIO_PDOR_REG(GPIOX_BASE(SDIN))  |= (1 << PTn(SDIN));
#define RST_0       GPIO_PDOR_REG(GPIOX_BASE(RST)) &= ~(1 << PTn(RST));
#define RST_1       GPIO_PDOR_REG(GPIOX_BASE(RST))  |= (1 << PTn(RST));
#define DC_0        GPIO_PDOR_REG(GPIOX_BASE(DC)) &= ~(1 << PTn(DC));
#define DC_1        GPIO_PDOR_REG(GPIOX_BASE(DC))  |= (1 << PTn(DC));


// ����ͷֻ�ܳ���ʾ����
void lcd_write_byte(u8 dat,u8 command);     // д���ֽڣ�common��ʾ����/����
void lcd_set_XY(u8 X,u8 Y);                 // д��λ��
void lcd_clear();                           // ��������
void lcd_write_image(void);                 // ��ͼ����
void lcd_init();                            //LCD��ʼ��
void lcd_clear_line();
extern void car_show();
extern void Car_show_handstand(void);


// 5110��ʾ����
void lcd_write_char(u8 c);                      // �ַ���ʾ
void lcd_write_char2(u8 X, u8 Y,u8 c);          // ���ض�λ����ʾ�ַ�
void lcd_write_str(u8 X, u8 Y,u8 *s);           // ���ض�λ����ʾ�ַ���
void lcd_write_num (u8 X,u8 Y,u16 number);      // ���ض�λ����ʾһ������
void lcd_write_num2 (u8 X,u8 Y,u16 number);      // ���ض�λ����ʾһ������

#endif