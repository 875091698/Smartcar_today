#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include  "common.h"

/*
 * Include 用户自定义的头文件
 */
#include  "MKL_BME.h"           //位操作
#include  "MKL_wdog.h"          //看门狗
#include  "MKL_gpio.h"          //IO口操作
#include  "MKL_uart.h"          //串口
#include  "outputdata.h"       //滴答定时器
#include  "MKL_lptmr.h"         //低功耗定时器(延时、脉冲计数、定时、计时)
#include  "math.h"           //I2C
//#include  "MKL_spi.h"           //SPI
#include  "MKL_tpm.h"           //TPM（类似K60的 FTM ，pwm、脉冲计数）
#include  "MKL_pit.h"           //PIT
#include  "MKL_adc.h"           //ADC
//#include  "MKL_dac.h"           //DAC
#include  "MKL_dma.h"           //DMA
#include  "MKL_FLASH.h"         //FLASH


#include  "LED.H"          //LED
#include  "KEY.H"          //KEY
//#include  "MMA7455.h"      //三轴加速度MMA7455
//#include  "NRF24L0.h"      //无线模块NRF24L01+

//#include  "LCD.h"          //液晶总头文件

//#include  "TSL1401.h"      //线性CCD
//#include  "key_event.h"    //按键消息处理
//#include  "NRF24L0_MSG.h"  //无线模块消息处理

#include  "dolphin.h"
#include  "bluetooth.h"
#include  "camera.h"
#include  "OV7725_REG.h"
#include  "OV7725_Eagle.h"
#include  "OLED.h"
#include  "adjust.h"

#include "l3g4200d.h"
#include "l3g4200d_i2c.h"




#endif  //__INCLUDE_H__
