/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_NRF24L0.h
 * @brief      无线调试  函数
 * @author     野火科技
 * @version    v5.0
 * @date       2014-01-04
 */

#ifndef _NRF24L0_H_
#define _NRF24L0_H_     1


//以下是硬件配置
#define NRF_SPI         SPI1
#define NRF_CS          SPI_PCS0

#define NRF_CE_PTXn     PTE5
#define NRF_IRQ_PTXn    PTC18

//以下是用户配置的选项

#define DATA_PACKET             32      //一次传输最大可支持的字节数（1~32）
#define RX_FIFO_PACKET_NUM      80      //接收 FIFO 的 包 数目 ( 总空间 必须要大于 一副图像的大小，否则 没法接收完 )
#define ADR_WIDTH               5       //定义地址长度（3~5）
#define IS_CRC16                1       //1表示使用 CRC16，0表示 使用CRC8 (0~1)


//配置到这里结束

typedef enum
{
    NRF_TXING,              //发送中
    NRF_TX_ERROR,           //发送错误
    NRF_TX_OK,              //发送完成
} nrf_tx_state_e;


//函数声明
extern  uint8   nrf_init(void);              //初始化NRF24L01+

extern  uint8   nrf_link_check(void);        //检测NRF24L01+与单片机是否通信正常

extern  uint32  nrf_rx(uint8 *rxbuf, uint32 len);   //接收
extern  uint8   nrf_tx(uint8 *txbuf, uint32 len);   //发送

extern  nrf_tx_state_e nrf_tx_state ();             //发送状态(发送数据后查询是否发送成功)


extern  void    nrf_handler(void);                  //NRF24L01+ 的 中断服务函数

//下面的函数 留给 无线消息处理机制 的函数使用，一般用户用不着
extern  uint8  nrf_rx_fifo_check(uint32 offset,uint16 * val);    //获取 接收FIFO 的数据


#endif      //_FIRE_NRF24L0_H_
