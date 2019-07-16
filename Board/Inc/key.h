/*
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,Ұ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�Ұ���ѧ��̳ http://www.chuxue123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����Ұ��Ƽ����У�δ������������������ҵ��;��
 *     �޸�����ʱ���뱣��Ұ��Ƽ��İ�Ȩ������
 *
 * @file       FIRE_KEY.h
 * @brief      KEY����ͷ�ļ�
 * @author     Ұ��Ƽ�
 * @version    v5.0
 * @date       2013-07-10
 */

#ifndef _KEY_H__
#define _KEY_H__


//�����Ƕ��尴����ʱ�䣬��λΪ �� 10ms���ж�ʱ�䣩
#define KEY_DOWN_TIME           1       //����ȷ�ϰ���ʱ��
#define KEY_HOLD_TIME           50      //����holdȷ��ʱ�䣬���253��������Ҫ�޸� keytime ������
                                        //�������һֱ����ȥ����ÿ�� KEY_HOLD_TIME - KEY_DOWN_TIME ʱ��ᷢ��һ�� KEY_HOLD ��Ϣ

//���尴����ϢFIFO��С
#define KEY_MSG_FIFO_SIZE       20      //��� 255��������Ҫ�޸�key_msg_front/key_msg_rear����

//�����˿ڵ�ö��
typedef enum
{
    KEY_U,  //��
    KEY_D,  //��

    
    KEY_A,  //ѡ��
    KEY_B,  //ȡ��

    KEY_START,  //��ʼ
//  KEY_STOP,   //ֹͣ
    
    KEY_DIAL_1,  //���뿪��
    KEY_DIAL_2,
    KEY_DIAL_3,
    KEY_DIAL_4,
   // KEY_DIAL_5,
//    KEY_DIAL_6,
//    KEY_DIAL_7,
//    KEY_DIAL_8,

    KEY_MAX,
} KEY_e;


//key״̬�궨��
typedef enum
{
    KEY_DOWN  =   0,         //��������ʱ��Ӧ��ƽ
    KEY_UP    =   1,         //��������ʱ��Ӧ��ƽ

    KEY_HOLD,

} KEY_STATUS_e;

//������Ϣ�ṹ��
typedef struct
{
    KEY_e           key;
    KEY_STATUS_e    status;
} KEY_MSG_t;


void            key_init(KEY_e key);            // KEY��ʼ������(key С�� KEY_MAX ʱ��ʼ�� ��Ӧ�˿ڣ������ʼ��ȫ���˿�)
KEY_STATUS_e    key_get(KEY_e key);
KEY_STATUS_e    key_check(KEY_e key);           //���key״̬������ʱ������


//��ʱɨ�谴��
uint8   get_key_msg(KEY_MSG_t *keymsg);         //��ȡ������Ϣ������1��ʾ�а�����Ϣ��0Ϊ�ް�����Ϣ
void    key_IRQHandler(void);                   //��Ҫ��ʱɨ����жϸ�λ��������ʱʱ��Ϊ10ms��


#endif  //__FIRE_KEY_H__