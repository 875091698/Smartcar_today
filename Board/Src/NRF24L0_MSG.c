/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_NRF24L0_MSG.c
 * @brief      无线调试 消息机制 函数
 * @author     野火科技
 * @version    v5.0
 * @date       2014-01-04
 */

#include "common.h"

#include "camera.h"
#include "NRF24L0.h"
#include "NRF24L0_MSG.h"
#include "TSL1401.h"

/**************************** 变量接收与发送 **********************************/




/**************************** 变量接收与发送 **********************************/

uint32 rxbuflen = 0;           //用于接收方返回接收到多少数据。（包含第一次传递进去的那个包大小）


const uint32 nrf_com_size[COM_MAX] = {CAMERA_SIZE , TSL1401_MAX *TSL1401_SIZE , 8, 0};

uint32 nrf_com_totalsize[COM_MAX];                                                                  // 所占用 包 的 总 占用空间

void nrf_msg_init()
{
    uint32 i = COM_MAX;

    while(i--)
    {
        nrf_com_totalsize[i] = (( nrf_com_size[i] + 2 * COM_LEN + DATA_PACKET - 1 ) / DATA_PACKET)  //求得包的数目
                               *DATA_PACKET;                                                      //总共占用的空间
    }

}


//发送命令。前两个字节用在命令识别，后面就是需要传输的内容
nrf_result_e nrf_msg_tx(com_e  com, uint8 *sendbuf)
{
    ASSERT(com < COM_MAX);          //命令不能超过最大的数目

    //把命令写进数据缓冲区
    sendbuf[0] =  com;
    sendbuf[1] = ~com;
    sendbuf[nrf_com_size[com] + 2 * COM_LEN - 2] = ~com;
    sendbuf[nrf_com_size[com] + 2 * COM_LEN - 1] = com;

    while( !nrf_tx(sendbuf , nrf_com_size[com] + 2 * COM_LEN)); //发送数据

    return NRF_RESULT_NULL;
}

//查询是否有接收数据，并进行处理。rebuf 为可供使用的缓冲区
//
nrf_result_e nrf_msg_rx(com_e  *com, uint8 *rebuf)
{
    com_e   comtmp;
    uint32  len;
    uint32  tmplen;
    uint32  relen;                              //接收到的数据长度
    uint8   *buftemp;
    uint32  tmp;
    uint32  totallen ;                          //总需要接收包的数目(包的整数倍)
    uint16  tmpcheck;

RE_LOOP:
    buftemp = rebuf;                            //加载起始地址

    relen = nrf_rx(buftemp, DATA_PACKET);       //接收 第一个包
    if(relen == 0)
    {
        //如果是 预校验 失败，那么 肯定可以接收到数据，不会进入此处

        //只有 一开始 进入函数 第一次接收的时候，才出现接收失败

        return NRF_RESULT_RX_NO;                //没接收到数据
    }

    comtmp = (com_e)buftemp[0];
    if(((uint8)comtmp < (uint8)COM_MAX) && (buftemp[1] ==  (uint8)~comtmp) && (comtmp != COM_RETRAN) )
    {
        //校验正确,继续接收剩余 的数据

        totallen = nrf_com_totalsize[comtmp];   //总接收参数

        if(totallen > relen )                   //数据长度 超过 已接收长度
        {
            //需要继续接收
            len = totallen - relen;             //剩余接收的 长度

            //等待接收FIFO里的数据校验正确才接收
            while( !nrf_rx_fifo_check(nrf_com_size[comtmp] + 2 * COM_LEN - relen,&tmpcheck)  );   //等待接收
            if( tmpcheck !=  (uint16)((uint8)~comtmp + (comtmp<<8)))
            {
                goto RE_LOOP;                   //校验失败 ，放弃刚才接收的 第一个 包 （重新接收数据）
            }

            tmplen = relen;
            do
            {
                buftemp += tmplen;              //移动到尚未接收数据的缓冲区
                tmplen = nrf_rx(buftemp, len);  //接收数据
                //relen += tmplen;
                len -= tmplen;
            }
            while(len);
        }

        //校验尾部数据是否正确
        if(
            (rebuf[nrf_com_size[comtmp] + 2 * COM_LEN - 2] ==   (uint8)~comtmp)
            &&  (rebuf[nrf_com_size[comtmp] + 2 * COM_LEN - 1] ==   (uint8) comtmp)   )
        {
            *com = comtmp;                          //存储命令

            //对 命令 数据进行 处理
            switch(*com)
            {
            case COM_VAR:
#if 0
                last_tab = *((uint32 *)&rebuf[COM_LEN]);                                    //读取变量编号
#else
                last_tab =       rebuf[COM_LEN]
                            +   (rebuf[COM_LEN+1]<<8)
                            +   (rebuf[COM_LEN+2]<<16)
                            +   (rebuf[COM_LEN+3]<<24);                                                  //读取变量编号
#endif
                if(last_tab < VAR_MAX)
                {
#if 0
                    save_var((var_tab_e)last_tab, *((uint32 *)&rebuf[COM_LEN + sizeof(uint32)]));          //存储 变量
#else
                    tmp = rebuf[COM_LEN + 4]
                        +(rebuf[COM_LEN + 5]<<8)
                        +(rebuf[COM_LEN + 6]<<16)
                        +(rebuf[COM_LEN + 7]<<24) ;
                    save_var((var_tab_e)last_tab, tmp);          //存储 变量
#endif
                    var_display(last_tab);                                                  //显示 变量
                }
                else
                {
                    return NRF_RESULT_RX_NOVALID;
                }
                break;
            default:
                break;
            }

            return NRF_RESULT_RX_VALID;             //接收有效数据
        }
    }

    //有接收到数据，但接收数据无效
    return NRF_RESULT_RX_NOVALID;
}



