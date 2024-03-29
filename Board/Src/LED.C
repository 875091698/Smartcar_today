 /*!
  *     COPYRIGHT NOTICE
  *     Copyright (c) 2013,野火科技
  *     All rights reserved.
  *     技术讨论：野火初学论坛 http://www.chuxue123.com
  *
  *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
  *     修改内容时必须保留野火科技的版权声明。
  *
  * @file       FIRE_LED.c
  * @brief      led驱动函数实现
  * @author     野火科技
  * @version    v5.0
  * @date       2013-07-9
  */

/*
 * 包含头文件
 */
#include "common.h"
#include "MKL_port.h"
#include "MKL_gpio.h"
#include "LED.H"


/*
 * 定义LED 编号对应的管脚
 */
PTXn_e LED_PTxn[LED_MAX] = {PTD4,PTD5,PTD6,PTD7};


/*!
 *  @brief      初始化LED端口
 *  @param      LED_e    LED编号
 *  @since      v5.0
 *  Sample usage:       LED_init (LED0);    //初始化 LED0
 */
void    led_init(LED_e ledn)
{
    if(ledn < LED_MAX)
    {
        gpio_init(LED_PTxn[ledn],GPO,LED_OFF);
    }
    else
    {
        ledn = LED_MAX;
        while(ledn--)
        {
            gpio_init(LED_PTxn[ledn],GPO,LED_OFF);
        }

    }
}



/*!
 *  @brief      设置LED灯亮灭
 *  @param      LED_e           LED编号（LED0、LED1、LED2、LED3）
 *  @param      LED_status      LED亮灭状态（LED_ON、LED_OFF）
 *  @since      v5.0
 *  Sample usage:       LED (LED0,LED_ON);    //点亮 LED0
 */
void    led(LED_e ledn,LED_status status)
{
    gpio_set(LED_PTxn[ledn],status);
}

/*!
 *  @brief      设置LED灯亮灭反转
 *  @param      LED_e           LED编号（LED0、LED1、LED2、LED3）
 *  @since      v5.0
 *  Sample usage:       LED_turn (LED0);    // LED0灯亮灭反转
 */
void led_turn(LED_e ledn)
{
    gpio_turn(LED_PTxn[ledn]);
}


void Led_turn_all(void)
{
    led_turn(LED1);
    led_turn(LED2);
    led_turn(LED3);
}
