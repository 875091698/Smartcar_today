
#ifndef _5110_H_
#define _5110_H_

#include "MKL_gpio.h"

#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    //置寄存器的一位
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  //清寄存器的一位
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  //获得寄存器一位的状态

#define CE    PTC17        // 默认端口为PTE，若要修
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


// 摄像头只能车显示部分
void lcd_write_byte(u8 dat,u8 command);     // 写入字节，common表示数据/命令
void lcd_set_XY(u8 X,u8 Y);                 // 写入位置
void lcd_clear();                           // 清屏函数
void lcd_write_image(void);                 // 画图函数
void lcd_init();                            //LCD初始化
void lcd_clear_line();
extern void car_show();
extern void Car_show_handstand(void);


// 5110显示函数
void lcd_write_char(u8 c);                      // 字符显示
void lcd_write_char2(u8 X, u8 Y,u8 c);          // 在特定位置显示字符
void lcd_write_str(u8 X, u8 Y,u8 *s);           // 在特定位置显示字符串
void lcd_write_num (u8 X,u8 Y,u16 number);      // 在特定位置显示一个数字
void lcd_write_num2 (u8 X,u8 Y,u16 number);      // 在特定位置显示一个数字

#endif