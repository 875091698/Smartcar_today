/*
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_KEY.h
 * @brief      KEY驱动头文件
 * @author     野火科技
 * @version    v5.0
 * @date       2013-07-10
 */

#ifndef _KEY_H__
#define _KEY_H__


//下面是定义按键的时间，单位为 ： 10ms（中断时间）
#define KEY_DOWN_TIME           1       //消抖确认按下时间
#define KEY_HOLD_TIME           50      //长按hold确认时间，最多253，否则需要修改 keytime 的类型
                                        //如果按键一直按下去，则每隔 KEY_HOLD_TIME - KEY_DOWN_TIME 时间会发送一个 KEY_HOLD 消息

//定义按键消息FIFO大小
#define KEY_MSG_FIFO_SIZE       20      //最多 255，否则需要修改key_msg_front/key_msg_rear类型

//按键端口的枚举
typedef enum
{
    KEY_U,  //上
    KEY_D,  //下

    
    KEY_A,  //选择
    KEY_B,  //取消

    KEY_START,  //开始
//  KEY_STOP,   //停止
    
    KEY_DIAL_1,  //拨码开关
    KEY_DIAL_2,
    KEY_DIAL_3,
    KEY_DIAL_4,
   // KEY_DIAL_5,
//    KEY_DIAL_6,
//    KEY_DIAL_7,
//    KEY_DIAL_8,

    KEY_MAX,
} KEY_e;


//key状态宏定义
typedef enum
{
    KEY_DOWN  =   0,         //按键按下时对应电平
    KEY_UP    =   1,         //按键弹起时对应电平

    KEY_HOLD,

} KEY_STATUS_e;

//按键消息结构体
typedef struct
{
    KEY_e           key;
    KEY_STATUS_e    status;
} KEY_MSG_t;


void            key_init(KEY_e key);            // KEY初始化函数(key 小于 KEY_MAX 时初始化 对应端口，否则初始化全部端口)
KEY_STATUS_e    key_get(KEY_e key);
KEY_STATUS_e    key_check(KEY_e key);           //检测key状态（带延时消抖）


//定时扫描按键
uint8   get_key_msg(KEY_MSG_t *keymsg);         //获取按键消息，返回1表示有按键消息，0为无按键消息
void    key_IRQHandler(void);                   //需要定时扫描的中断复位函数（定时时间为10ms）


#endif  //__FIRE_KEY_H__