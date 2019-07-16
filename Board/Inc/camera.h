/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,Ұ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�Ұ���ѧ��̳ http://www.chuxue123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����Ұ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��Ұ��Ƽ��İ�Ȩ������
 *
 * @file       FIRE_camera.h
 * @brief      ����ͷ�����ӿ��ض���
 * @author     Ұ��Ƽ�
 * @version    v5.0
 * @date       2013-09-01
 */


#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "common.h"

#define CAMERA_OV7725_EAGLE         2       //Ұ��ӥ��
#define CAMERA_OV7725_WOLF          3       //Ұ������


#define USE_CAMERA      CAMERA_OV7725_EAGLE   //ѡ��ʹ�õ� ����ͷ

typedef struct
{
    uint8 addr;                 /*�Ĵ�����ַ*/
    uint8 val;                   /*�Ĵ���ֵ*/
} reg_s;

//����ͼ��ɼ�״̬
typedef enum
{
    IMG_NOTINIT = 0,
    IMG_FINISH,             //ͼ��ɼ����
    IMG_FAIL,               //ͼ��ɼ�ʧ��(�ɼ���������)
    IMG_GATHER,             //ͼ��ɼ���
    IMG_START,              //��ʼ�ɼ�ͼ��
    IMG_STOP,               //��ֹͼ��ɼ�
} IMG_STATUS_e;



#include  "SCCB.h"
#include  "OV7725_Eagle.h"


// ����ͷ �ӿ�ͳһ�ĳ� ����ģʽ

//  camera_init(imgaddr);
//  camera_get_img();
//  camera_cfg(rag,val)


//  camera_vsync()  //���ж�
//  camera_href()   //���ж�
//  camera_dma()    //DMA�ж�

// ��Ҫ �ṩ ���� �궨��
// #define  CAMERA_USE_HREF    1     //�Ƿ�ʹ�� ���ж� (0 Ϊ ��ʹ�ã�1Ϊʹ��)
// #define  CAMERA_COLOR       1     //����ͷ�����ɫ �� 0 Ϊ �ڰ׶�ֵ��ͼ�� ��1 Ϊ �Ҷ� ͼ�� ��2 Ϊ RGB565 ͼ��
// #define  CAMERA_POWER       0     //����ͷ ��Դѡ�� 0 Ϊ 3.3V ,1 Ϊ 5V





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


