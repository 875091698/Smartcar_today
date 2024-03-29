/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_camera.h
 * @brief      摄像头函数接口重定向
 * @author     野火科技
 * @version    v5.0
 * @date       2013-09-01
 */


#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "common.h"

#define CAMERA_OV7725_EAGLE         2       //野火鹰眼
#define CAMERA_OV7725_WOLF          3       //野火狼眼


#define USE_CAMERA      CAMERA_OV7725_EAGLE   //选择使用的 摄像头

typedef struct
{
    uint8 addr;                 /*寄存器地址*/
    uint8 val;                   /*寄存器值*/
} reg_s;

//定义图像采集状态
typedef enum
{
    IMG_NOTINIT = 0,
    IMG_FINISH,             //图像采集完毕
    IMG_FAIL,               //图像采集失败(采集行数少了)
    IMG_GATHER,             //图像采集中
    IMG_START,              //开始采集图像
    IMG_STOP,               //禁止图像采集
} IMG_STATUS_e;



#include  "SCCB.h"
#include  "OV7725_Eagle.h"


// 摄像头 接口统一改成 如下模式

//  camera_init(imgaddr);
//  camera_get_img();
//  camera_cfg(rag,val)


//  camera_vsync()  //场中断
//  camera_href()   //行中断
//  camera_dma()    //DMA中断

// 需要 提供 如下 宏定义
// #define  CAMERA_USE_HREF    1     //是否使用 行中断 (0 为 不使用，1为使用)
// #define  CAMERA_COLOR       1     //摄像头输出颜色 ， 0 为 黑白二值化图像 ，1 为 灰度 图像 ，2 为 RGB565 图像
// #define  CAMERA_POWER       0     //摄像头 电源选择， 0 为 3.3V ,1 为 5V





extern void Camera_data_init(void);

extern void Img_extract(void);
extern u8 Camera_line(void);

extern u8  dn;
extern void camera_draw_line_error(void);

extern void display_bin_img_uart(void);
extern void display_buf_img_uart(void);

extern void display_yy(void);

extern void Camera_data_show(void);


u8 track_judge(u8);

u8 servo_control(void);

s16 draw_brick(void);
s16 draw_back_road(void);
 





#endif


