#include "camera.h"
#include "MKL_uart.h"
#include "LED.h"
#include  "MKL_uart.h"
#include "OLED.h"
#include "key.h"
#include "math.h"

extern s16 distance_0;
//extern void Buzzer_Mie(void);
//extern void Buzzer_Bi(void);
extern int mode; 
//u8 image_start_flag = 0;
u8 image_start = 0;
//extern u32 distance_0;
extern u8 chaoche_jiakuan;
#define CAMERA_LINE (60)
u8  image[CAMERA_SIZE + 256]={0};   //用来存放摄像头原始数据
u8 *img_bin_buff = (u8 *)(((u8 *)&image) + 2);  //指向原始二值图像

u8 image_buf[CAMERA_H][CAMERA_W] = {0};  //非压缩的二值化图像（用于识别）,
                
u8 *p_camera_buffer[CAMERA_LINE] = {&image_buf[0][0]};//定义了一个地址数组



struct LINE_Y   //Y 线
{
    u8 x;
    u8 y;   
    s8 lr_flag;    //-1 ，左边， 1 右边 2 两边，0 没
    u8 m_l;
    u8 m_r;
};
struct LINE_Y line_Y[CAMERA_LINE+1] = {0};

#define  WHITE  0
#define  BLACK  0x01
#define  www  0
#define  bbb  0x01


float camera_error = 0;            // 偏差 


u8 camera_line[CAMERA_LINE] = {     //普通程序图像取行
  
 10,13,15,17,18,20,23,25,26,27,
 28,29,30,31,32,33,34,35,36,37,
 38,39,40,41,42,43,44,45,46,47,
 48,49,50,52,54,56,58,60,62,64,
 66,68,70,72,74,76,78,80,83,86,
 87,89,91,94,96,99,104,107,109,111

};

u8 camera_line_S[CAMERA_LINE] = {   //超车程序图像取行 
   
 10,13,15,17,18,20,23,25,26,27,
 28,29,30,31,32,33,34,35,36,37,
 38,39,40,41,42,43,44,45,46,47,
 48,49,50,52,54,56,58,60,62,64,
 66,68,70,72,74,76,78,80,83,86,
 87,89,91,94,96,99,104,107,109,111, 
/*  22,23,24,25,26,27,28,29,30,31,
 32,33,34,35,36,37,38,39,40,41,
 42,43,44,45,46,47,48,49,50,51,
 52,53,54,55,56,57,58,60,62,64,
 66,68,70,72,74,76,78,80,83,86,
 87,89,91,94,96,99,104,107,109,111, */

};




u8 expect[CAMERA_LINE]={       //图像修正数组
  

11,12,13,14,14,15,15,16,16,17,
17,18,19,20,20,21,21,22,22,23,
23,24,25,25,26,26,27,27,28,29,
30,30,31,32,33,34,35,36,37,38,
39,40,41,42,43,44,45,46,47,48,
49,50,51,52,53,54,56,58,59,60,
};//赛道的宽度     

u8 expect_gaosu[CAMERA_LINE]={  //小s权重数组
  1,1,1,1,1,2,2,2,2,2,
  2,4,4,4,4,4,4,4,4,4, 
  4,4,5,5,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,
  1,1,2,2,2,2,2,2,2,2,
  1,1,1,1,1,1,1,1,1,1, 
};
/*u8 expect_disu[CAMERA_LINE]={
  1,1,1,1,1,1,1,1,1,1, 
  1,1,1,1,1,1,2,2,2,2,
  2,3,3,3,4,4,4,4,4,4, 
  2,3,3,3,4,4,4,4,4,4,
  5,4,4,5,5,5,5,5,5,5,
  5,5,5,5,5,4,4,4,2,2,
  
 
};*/



u8 image_start_flag = 0;
u8 image_start_flag_0 = 0;
s16 camera_row_num;

u8 Dflag=0;
//u8 temp3=0;
u8 Leftstart,Rightstart,danstart,danend;





void Camera_data_init(void)
{   
    u8 i = 0;
    
    for (i=0; i<CAMERA_LINE+1; i+=1) //Y线初始化
    {
        line_Y[i].x = CAMERA_W/2;//X轴的值
        line_Y[i].y = i;//Y轴的值
        line_Y[i].lr_flag = 2;//状态为2边有线
        line_Y[i].m_l = 0;//左边线的X坐标初始化为0
        line_Y[i].m_r = CAMERA_W;//右边线的坐标初始化为160
    }
    
    for (i=0; i<CAMERA_LINE; i+=1) //超关键
    {
        p_camera_buffer[i] = &image_buf[ camera_line[i] ][0]; //超关键 用来存放所需行的第一个点 
    }
       
    //line_Y[CAMERA_LINE]表示第61行的中点  根本没有第61行，是虚拟化出来的 
    line_Y[CAMERA_LINE].x =  CAMERA_W/2;   //80
    line_Y[CAMERA_LINE-1].x = CAMERA_W/2; //80
 
      
    line_Y[CAMERA_LINE].x = CAMERA_W/2;  //第120个点初始化
    line_Y[CAMERA_LINE].y = CAMERA_H;
    line_Y[CAMERA_LINE].lr_flag = 2;
    line_Y[CAMERA_LINE].m_l = CAMERA_W/2 - expect[CAMERA_LINE-1]/2;
    line_Y[CAMERA_LINE].m_r = CAMERA_W/2 + expect[CAMERA_LINE-1]/2;
       
    image_start_flag=0;
}



//解压原始二值化图像，解压后黑点为BLACK，白点为WHITE
void Img_extract(void)
{
    u8 i = 0;
    u8 j = 0;
    
    u8 colour[2] = {WHITE,BLACK}; 
    u8 temp = 0;
    
    u8 *p_img_source = img_bin_buff;
    u8 *P_img_destination = &image_buf[0][0];
    
    for(i=0;i<CAMERA_H;i+=1)
    {
         P_img_destination = &image_buf[i][0];
         for(j=0;j<CAMERA_W/8;j++)
         {              
            p_img_source++;
            temp = *p_img_source;
            
            *P_img_destination ++= colour[ (temp>>7) & 0x01 ];
            *P_img_destination ++= colour[ (temp>>6) & 0x01 ];
            *P_img_destination ++= colour[ (temp>>5) & 0x01 ];
            *P_img_destination ++= colour[ (temp>>4) & 0x01 ];
            *P_img_destination ++= colour[ (temp>>3) & 0x01 ];
            *P_img_destination ++= colour[ (temp>>2) & 0x01 ];
            *P_img_destination ++= colour[ (temp>>1) & 0x01 ]; 
            *P_img_destination ++= colour[ (temp>>0) & 0x01 ];
         }
    }
}


#define CAMERA_LINE_SB (17)  //赛道识别数组
u8 camera_line_SB[CAMERA_LINE_SB] = { 
  
32,34,36,38,41,44,47,
51,55,61,66,73,85,94,102,111,118    

};
u8 pp=0;
u8 cc=0;
u8 ccsj=0;
u8 cc_0=0;
u8 this_is_jiakuan=0;
u8 danbian=0;
u8 wrr=0;
u8 wll=0; 
u8 jiance_jiakuan=0;
s8 this_is_jiakuan_zx=0;
u8 Sanjiao_panduan() 
{
           
       int i, j, k, k1, wl[CAMERA_W], center, bottom,top,root,wml, l[ CAMERA_LINE_SB], r[ CAMERA_LINE_SB], c[ CAMERA_LINE_SB];
	
        root=80;                      
        center = 20;                        
        top = CAMERA_LINE_SB - 16;         
	bottom = CAMERA_LINE_SB- 1;     
    
		for (i = 0; i <=CAMERA_W - 1; i++)                    
		{
			wl[i] = -2;                          
		}
              
		for (i =CAMERA_W / 2 - center; i <=CAMERA_W / 2 + center; i += 5)       
		{
                  
                  
                  if (image_buf[camera_line_SB[bottom]][i]==www )                       
			{
				for (j = i; j >= 0; j--)                    
				{
					if (image_buf[camera_line_SB[bottom]][j] ==www)       
					{
						wl[j] = -1 ;                            
					}
					else
					{
						break;                      
					}
				}
                                
                                
				for (j = i; j <CAMERA_W - 1; j++)            
				{
					if (image_buf[camera_line_SB[bottom]][j] ==www)
					{
						wl[j] = -1;                           
					}
					else
					{
						break;                       
					}
				}
			}                                          
                        
		}
    
		for (i = 0; i <= CAMERA_W - 1; i++)
		{
			if (wl[i] == -1)
			{
				for (j = bottom; j >= 0; j--)                        
				{
					if (image_buf[camera_line_SB[j]][i] ==bbb)            
					{
						wl[i] = j + 1;         
						break;
					}
				}
			}
		}
                
		k = 1;                 
            
		for (i = 0; i < CAMERA_W- 1; i++)                     
		{
			k *= -1;                                              
			k1 =CAMERA_W / 2 + k*i / 2;                                    
			if (wl[k1] >= 0)                              
			{
				break;
			}
		}               
                
		wml = k1;  
                
            
			k = 1;
			for (i = 0; i <= CAMERA_W - 1; i++)
			{
				k *= -1;
				k1 = CAMERA_W / 2 + k*i / 2;                                   
				if (wl[k1] >= 0)
				if (wl[k1] < wl[wml])
				{
					wml = k1;              
				}
			}
	
                	
       root= wml;                                                        
	
       u8 d[CAMERA_LINE_SB],dw, zw, dn,  wr, xl, xw,temp1=0,temp2=0, temp3=0,temp4=0;
       wrr=0;
       wll=0;               
       dw = 10;
      
       u8  baise[ CAMERA_LINE_SB]={0};
       
       for (i = bottom; i >= top; i--)        //一场图像的白色个数                
	{
           for (j = root; j >= 0; j--)
          {

                  if (image_buf[camera_line_SB[i]][j] ==www)
                  {
                          baise[i]++;
                  }
                  if(image_buf[camera_line_SB[i]][j] ==bbb)
                  {
                          break;
                  }
          }
           for (j = root; j <= CAMERA_W - 1; j++)
          {
                  
                if (image_buf[camera_line_SB[i]][j] ==www)
                  {
                          baise[i]++;				
                  }	
                   if (image_buf[camera_line_SB[i]][j] ==bbb)
                  {
                          break;
                  }
          }   
        }
       	for (i = bottom; i >= top; i--)
	{
		for (j = root; j >= 0; j--)
		{
			if (j == 0)
			{
				l[i] = 0;           //十字偏了可以改这里吧啊
				break;
			}
			if (image_buf[camera_line_SB[i]][j] ==www&&image_buf[camera_line_SB[i]][j - 1]==bbb)
			{
				l[i] = j;
				break;
			}
		}
		for (j = root; j <= CAMERA_W - 1; j++)
		{
			if (j ==CAMERA_W - 1)
			{
				r[i] = CAMERA_W- 1;
				break;
			}
			if (image_buf[camera_line_SB[i]][j] ==www&&image_buf[camera_line_SB[i]][j + 1]==bbb)
			{
				r[i] = j;
				break;
			}
		}
	}
             for (i = bottom; i >= top; i--)
            {
               if( l[i] == 0 )
               {
                wll++;
               }
             if(r[i] == CAMERA_W  - 1 )
               {
                wrr++;
               }
            }
       //LED_P6x8Str(2,4,"wll");
       //LED_PrintValueF(72,4,   wll, 1); 
     // LED_P6x8Str(2,7,"wrr");
     //  LED_PrintValueF(72,7,   wrr, 1); 
        pp=0;
        cc=0;
        cc_0=0;
        ccsj=0;
        s32 sum=0;
        s32 sum_jiechu=0;
           for (i = bottom; i >= top; i--)        //一场图像的白色个数                
	{
          if(i!=bottom && i!=bottom-1&& i!=bottom-2&& i!=bottom-3)
          {
            if(baise[i]>=159)    //十字那
            {
             cc=1;
            // ccsj=1;
            break;
            }
           if(baise[1]>=105||baise[2]>=110||baise[3]>=115||baise[4]>=115)  
           {
              cc=1;
            //  ccsj=1;
              break;
           }
          }
            sum=baise[i]+sum;
        }
  
   if((mode==3||mode==7||mode==8))//&&see_jiakuan==1
   {  
     sum_jiechu=baise[bottom-1]+baise[bottom-2]+baise[bottom-3]+baise[bottom-4]+baise[bottom-5]+baise[bottom-6];
   }
        if(sum>=1650)
        {
        cc=1;
        }
       // if(sum_jiechu>=550&&sum_jiechu<=650)
        if(sum_jiechu>=650)
        {
        pp=1;
        }
        
        
        s32  sum1=0;
        s32  sum2=0;
        s32  sum3=0;
        
        //sum1=baise[bottom-1]+baise[bottom-0];   //近处的
    
      //  sum2=baise[bottom-12]+baise[bottom-13]; //远处的
        
      //  sum3=baise[bottom-13]+baise[bottom-14]+baise[bottom-15];  //远处是直道啊
         
       /* if(sum>=1350&&(sum1-sum2>=170&&sum1-sum2<=220)&&(root>=57&&root<=100)&&(sum3>=130&&sum3<=155))   //出超车区用   为什么这里可以不是小车拐弯
      {
        cc=1;
      }*/
     
      
     
      if(chaoche_jiakuan)//拨码开关值
     {
       if(jiance_jiakuan==1&&((wll>=5&&wrr<=3)||(wrr>=5&&wll<=3))&&mode!=7&&mode!=8)
       {
          if(sum>=1850&&wll>wrr&& mode!=3)//1870&&camera_row_num<=4&&wll>wrr
          {
          this_is_jiakuan_zx=1;
          this_is_jiakuan=1;
          jiance_jiakuan=0;
          uart_putchar   (UART0 , '8');                    //发送字符7到后车 让后面的车减速靠右通过超车区域
          }
          if(sum>=1850&&wll<wrr&& mode!=3)//1870
          { 
          this_is_jiakuan_zx=-1;
          this_is_jiakuan=1;
          jiance_jiakuan=0; 
          uart_putchar   (UART0 , '7');                    //发送字符8到后车 让后面的车减速靠左通过超车区域
          }
    
       }  
     }
     else
     {
       if((wll>=5&&wrr<=3)||(wrr>=5&&wll<=3))
       {
        if(sum>=1650)
        {
        danbian=1;
        }
       }
     }
     
        //    LED_P6x8Str(2,5,"wll");
   //   LED_PrintValueF(72,5,   wll, 1);  
    //         LED_P6x8Str(2,6,"wrr");
    //  LED_PrintValueF(72,6,   wrr, 1);  
    //        LED_P6x8Str(2,7,"sum");
    //  LED_PrintValueF(72,7,   sum, 1);  
      // LED_P6x8Str(2,7,"this_is_jiakuan");
      //LED_PrintValueF(72,7,   this_is_jiakuan, 1);  
       
  //  LED_P6x8Str(2,4,"sum_jiechu");
    //  LED_PrintValueF(72,4,   sum_jiechu, 1); 
     /* if(cc==1) 
      {
      Buzzer_Bi();
      }
      else
      {
      Buzzer_Mie();
      } */
       if(mode==7||mode==8)
   {
    Buzzer_Bi();
   }
   else
   {
    Buzzer_Mie();
   }
      return   cc;
               
} 

/*******************************************************************************************************************************************/
/*************************************************************************
* 函数名称：Get_Bound
* 功能说明：图像处理程序（）
* 参数说明：value_hang  有效行
* 函数返回：Error       偏差
*************************************************************************/
s16 vh = 65;                    
s16 vh2=65;
s16 vh3=0;
float Error_Last = 0;
float Error_Last2=0;
float Error2=0;
float Error2_Last=0;
float Zj = 0;
float Zj2 = 0;
s8 this_is_sanjiao=0;
u8 heise_sum;
int GetWayStart(int WhiteLength[], int* Bottom, int* Top)
{
	s16 i, j, k, k1, wl[CAMERA_W], center, bottom, whiteMax, whiteMaxSet, wml, whiteMax0, bottomMax, last;
	center = 20;
	whiteMaxSet = 25;
	bottom = CAMERA_LINE- 1;////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bottomMax =CAMERA_LINE- 1;
	whiteMax0 = 0;
	last = 0;
	for (;;)
	{
		for (i = 0; i <=CAMERA_W - 1; i++)
		{
			wl[i] = -2;//每列存为-2
		}
		for (i =CAMERA_W / 2 - center; i <=CAMERA_W / 2 + center; i += 5)//40、80////////////////////////////////////////////////////////////////////////////////////
		{
			if (image_buf[camera_line_S[bottom]][i]==www )
			{
				for (j = i; j >= 0; j--)
				{
					if (image_buf[camera_line_S[bottom]][j] ==www)//找最长的白线用的
					{
						wl[j] = -1;
					}
					else
					{
						break;
					}
				}
				for (j = i; j <CAMERA_W - 1; j++)//从往左右识别为白
				{
					if (image_buf[camera_line_S[bottom]][j] ==www)
					{
						wl[j] = -1;
					}
					else
					{
						break;
					}
				}
			}
		}//最后是60往左往右最后一列

		for (i = 0; i <= CAMERA_W - 1; i++)
		{
			if (wl[i] == -1)//判断为白
			{
				for (j = bottom; j >= 0; j--)
				{
					if (image_buf[camera_line_S[j]][i] ==bbb)//为什么一列一列扫,应该是后边用到
					{
						wl[i] = j + 1;    // 这个数组存的是边界为1
						break;
					}
				}
			}
		}
		k = 1;
                
		for (i = 0; i < CAMERA_W- 1; i++)       //
		
                {
			k *= -1;
			k1 =CAMERA_W / 2 + k*i / 2;//中点往左
			if (wl[k1] >= 0)
			{
				break;   //识别到黑线
			}
		} 
		wml = k1;
		if (wl[wml] >= 0)
		{
			k = 1;
			for (i = 0; i <= CAMERA_W - 1; i++)
			{
				k *= -1;
				k1 = CAMERA_W / 2 + k*i / 2;
				if (wl[k1] >= 0)       
				if (wl[k1] < wl[wml])
				{
					wml = k1;
				}
			}
			whiteMax = wl[wml];
		}
		else
		{
			whiteMax = bottom;
		}
                
		if (bottom - whiteMax>whiteMax0)
		{
			bottomMax = bottom;
			whiteMax0 = bottom - whiteMax;
		}
		if (last == 1)
		{
			break;
		}
		bottom -= 3;
		if (bottom <= CAMERA_LINE / 2 + 20)
		{
			bottom = bottomMax;
			last++;
		}

	}
	for (i = 0; i <=CAMERA_W - 1; i++)
	{
		WhiteLength[i] = wl[i];
	}
	*Bottom = bottom;
	*Top = wl[wml];
	if (*Top<0 || bottom>CAMERA_LINE - 1)
	{
		*Top = *Bottom;
	}

	return (wml);
}
s16 heisell_sum;s16 heiserr_sum;
u8 jiance=1;
extern u8 shibiesanjiao;
void GetWayBound(int root, int bottom, int top, int L[], int R[], int C[], int D[], int* KIND)
{
	int i, j;
	int l[ CAMERA_LINE], r[ CAMERA_LINE], c[ CAMERA_LINE], d[CAMERA_LINE], x[CAMERA_LINE], dw, zw, dn, wl, wr, xl, xw,ccsum=0,baise;
	char kind;
	dw = 25;        //到黑三角那的距离///////////////////////////////////////////////////////////////////////////////////////////////
	zw = 30;
	xw = 20;
      
	if (bottom - top <= 10)
	{
		return;
	}

	for (i =CAMERA_H - 1; i >= 0; i--)
	{
		d[i] = -1;
	}


	for (i = bottom; i >= top; i--)
	{
		for (j = root; j >= 0; j--)
		{
			if (j == 0)
			{
				l[i] = 0;           //十字偏了可以改这里吧啊
				break;
			}
			if (image_buf[camera_line_S[i]][j] ==www&&image_buf[camera_line_S[i]][j - 1]==bbb)
			{
				l[i] = j;
				break;
			}
		}
		for (j = root; j <= CAMERA_W - 1; j++)
		{
			if (j ==CAMERA_W - 1)
			{
				r[i] = CAMERA_W- 1;//+20
				break;
			}
			if (image_buf[camera_line_S[i]][j] ==www&&image_buf[camera_line_S[i]][j + 1]==bbb)
			{
				r[i] = j;
				break;
			}
		}
	}
	dn = 0;
	for (i = bottom; i >= top; i--)      ///////////////////////////////单线  当成黑三角的一条边
	{
		if (l[i] >= 0 + dw)//////////////////////////////////////////////////
		{
			if (image_buf[camera_line_S[i]][l[i] - dw] ==www)
			{
				d[i] = l[i] ;
				if (image_buf[camera_line_S[i]][l[i] - 1] ==bbb)//&&image_buf[i][l[i] - 2] ==bbb&&image_buf[i][l[i] - 3] ==bbb)
				{
					dn++;
				}
			}
		}
		if (r[i] <= CAMERA_W - 1 - dw )
		{
			if (image_buf[camera_line_S[i]][r[i] + dw] ==www )
			{
				if (d[i] == -1)
				{
					d[i] = r[i] ;
					if (image_buf[camera_line_S[i]][r[i] + 1] ==bbb&&image_buf[camera_line_S[i]][r[i] + 2] ==bbb&&image_buf[camera_line_S[i]][r[i] + 3] ==bbb)
					{
						dn++;
					}
				}
				else
				{
					d[i] = -1;
					if (image_buf[camera_line_S[i]][l[i] - 1] ==bbb&&image_buf[camera_line_S[i]][l[i] - 2] ==bbb&&image_buf[camera_line_S[i]][l[i] - 3] ==bbb)
					{
						dn--;
					}
				}
			}
		}
	} 
        
       u8  heisell[ CAMERA_LINE]={0};
       u8  heiserr[ CAMERA_LINE]={0};
      
       heisell_sum=0;
       heiserr_sum=0;
	//看看dndn>8jiance==1
       this_is_sanjiao=0;
         if(shibiesanjiao==1&&jiance==1&&dn>=3&&this_is_jiakuan!=1&& mode!=7&& mode!=8)  //&& && dn>=7&& mode!=5&& mode!=6jiance==1&& mode!=5&& mode!=6
        { 
		for (i = 35; i >= 20; i--)
		{
			if (d[i] != -1)
			{
                          
                            for (j = d[i]; j >= d[i]-10; j--)
                            {

                                    if(image_buf[camera_line_S[i]][j] ==bbb)
                                    {
                                            heisell[i]++;
                                         
                                    }
                            }
                            for (j = d[i]; j <=d[i]+10; j++)
                            {

                                    if(image_buf[camera_line_S[i]][j] ==bbb)
                                    {
                                            heiserr[i]++;
                                           
                                    }
                            }
			}
		}

       		for (i = 35; i >= 20; i--)
		{
			if (d[i] != -1)
			{
                          heisell_sum=heisell[i]+heisell_sum;
                          heiserr_sum=heiserr[i]+heiserr_sum;
			}
		}
       
       if(heisell_sum>=45&&heisell_sum<=75&&heiserr_sum<=5)//
       {
         uart_putchar   (UART0 , '5');                    //发送字符5到后车 让后面的车左偏过三角
         this_is_sanjiao=-1;
         jiance=0;
         uart_putchar   (UART0 , '5');                    //发送字符5到后车 让后面的车左偏过三角
       }
       else if(heiserr_sum>=45&&heiserr_sum<=75&&heisell_sum<=5)//
       {
         uart_putchar   (UART0 , '6');                    //发送字符6到后车 让后面的车右偏过三角
         this_is_sanjiao=1;
         jiance=0;
         uart_putchar   (UART0 , '6');                    //发送字符6到后车 让后面的车右偏过三角
       }
       else
       {
         this_is_sanjiao=0;
       }
     }  

        
         // LED_P6x8Str(2,7,"heisell_sum");
        // LED_PrintValueF(72,7,   heisell_sum, 1);  
        //  LED_P6x8Str(2,5,"heiserr_sum");
       //LED_PrintValueF(72,5,   heiserr_sum, 1);
      
     /*  u8  heise[ CAMERA_LINE]={0};
       heise_sum=0;
	//看看dndn>8
      if(dn>=7)
      {
        for(i=25 ;i>=15; i--)//58 38
        {
         for(j=90 ;j>=70 ;j--)//130  30
         {
            if(image_buf[camera_line_S[i]][j] ==bbb)
            {
           
                          heise[i]++;                 
      
            }        
         }         
        }
    
      }
      
   
    
    for (i = 25; i >= 15; i--)        //一场图像20-30行黑色点的色个数                
    {

       heise_sum=heise[i]+heise_sum;
    }
        
            
      this_is_sanjiao=0; 
      
      
    if(heise_sum>=40&& heise_sum<=80 )// 180  230 //看看有前车时是什么样子的
    {
    
    this_is_sanjiao=1;
    //if
    uart_putchar   (UART0 , '7');                    //发送字符3到后车 让后面的车加速通过超车区域
    }
 */
 /*       LED_P6x8Str(2,7,"dn");
          LED_PrintValueF(72,7,   dn, 1);  
          LED_P6x8Str(2,5,"heise_sum");
          LED_PrintValueF(72,5,   heise_sum, 1);*/
            
  
    if(this_is_sanjiao!=0)
      {
      Buzzer_Bi();
      }
      else
      {
      Buzzer_Mie();
      }          

     
	if (1)
	{
		kind = 'd';
	}
 
	if (1)   /////////////////////////////////////////////////////////////////////
	{

		for (i = bottom - 1; i >= top; i--)
		{
			if (d[i] >= 0 && d[i + 1] >= 0 )//&& d[i + 2] >= 0 && d[i + 3] >= 0 && d[i + 4] >= 0
			{
				for (j = 0; j <= 10; j++)
				{
					l[i - j] = (d[i] + d[i + 1]) / 2;
					r[i - j] = (d[i] + d[i + 1]) / 2;
				}
			}
		}
	}
        
        if(0)//cc==1
        {
             for (i = bottom; i >= top; i--)
            {
               if( l[i] == 0 && r[i] == CAMERA_W  - 1)
               {
               
               }
               else    //只有单边丢线时！
               {
             //    if(ccsj!=1)
                 {
                    if (l[i] == 0)            ////////////丢线+20+4
                    {
                            l[i]=r[i] - expect[i]*2 ;////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    }  
                    if (r[i] == CAMERA_W  - 1)
                    {
                            r[i]=l[i] + expect[i]*2 ;
                    }
                 }
               }
            }
            
        }
	for (i = bottom; i >= top; i--)
	{
                c[i] = (l[i] + r[i]) / 2;       
	}
        
       
	if (1)/////////////////////////////////////////////////////////////////////////
	{
		for (i = bottom; i >= top; i--)
		{
			if (d[i] != -1)
			{
				c[i] = d[i];
			}
		}
	}

	for (i = bottom; i >= top; i--)
	{
		L[i] = l[i];
		R[i] = r[i];
		C[i] = c[i];
		D[i] = d[i];
	}
	*KIND = kind;
    
}

void GetWayCE(int bottom, int top, int C[], int kind)
{
	int i, j;
	int el, value1, value2, error[CAMERA_H];
	float sum, errorEverage[CAMERA_H], errorMin;
	el = 10;///////////////////////////////////////////////////////////////////////////////////////////////////
	value1 =60;      //需要调整的数字/////////////////////////////////////////////////////////////////////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	value2 =10;/////////////////////////////////////////////////////////////////////////////////////////////////
 
        
	if (bottom - top <= 10)//这种情况是怎么出现的
	{
		camera_error = Error_Last;
		return;
	}
	if (value1 > bottom || value1<top + el)
	{
		value1 = bottom;
	}
	if (value2 < top + el || value2>bottom)
	{
		value2 = top + el;
	}
	for (i = bottom; i >= top; i--)
	{
		error[i] =  CAMERA_W / 2-C[i];
	}
        
        //////这里选择0或1 cc=1&&ccsj!=1 
	if (0)
	{
		for (i = bottom; i >= top + el; i--)
		{
			sum = 0;
			for (j = i; j >= i - el; j--)
			{
				sum += error[i];
			}
			sum /= (float)(el);
			errorEverage[i] = sum;
		}
		errorMin = errorEverage[value1];
		for (i = value1; i >= value2; i--)
		{
			if ((errorEverage[i] >= 0 ? errorEverage[i] : -errorEverage[i])<(errorMin >= 0 ? errorMin : -errorMin))
			{
				errorMin = errorEverage[i];
			}
		}
		 camera_error= errorMin;
		Error_Last = camera_error;
	}

	else
        {
               sum = 0;
		for (i = value1; i >= value2; i--)
		{
			sum += C[i];
		}
		sum /= (float)(value1 - value2 + 1);
                
                camera_error =79.5-sum ;
             /*   if(this_is_sanjiao==1)
                {
                  
                camera_error=camera_error-60;
                
                } */   
                 if(this_is_jiakuan==1)
                {
                  if(wll<wrr)
                camera_error=camera_error-26;
                else if(wll>wrr)
                  camera_error=camera_error+15;
                else
                  camera_error=camera_error;               
                }  
		Error_Last =  camera_error;   
        }       
      /*    LED_P6x8Str(2,4,"error[top + el]");
           LED_PrintValueF(72,4,   error[top + el], 1);
           LED_P6x8Str(2,5,"sum");
           LED_PrintValueF(72,5,   sum, 1);*/
                
   
}



void GetWay(void)
{
	int  whiteLength[ CAMERA_W], bottom, top, root, l[ CAMERA_H], r[ CAMERA_H], c[ CAMERA_H], d[ CAMERA_H], kind;
                                                             //root 为搜索起点的中心线       
	root = GetWayStart(whiteLength, &bottom, &top);
	GetWayBound(root, bottom, top, l, r, c, d, &kind);//传递了各种行数数组；；；；；；然后算出中心，两侧的数组。
    
	if (kind == 'Z')
	{
		Zj = 10;
	}
	if (Zj > 0)
		Zj--;
	GetWayCE(bottom, top, c, kind);                         //角度给舵机
  
 //       LED_P6x8Str(2,3,"root");
//    LED_PrintValueF(72,3,   root, 1);
//    LED_P6x8Str(2,4,"left_speed");
//    LED_PrintValueF(72,4,   left_speed, 1);
//    LED_P6x8Str(2,5,"right_speed");
//    LED_PrintValueF(72,5,   right_speed, 1);
//    LED_P6x8Str(2,6,"pulse_count_l");
//    LED_PrintValueF(72,6,   pulse_count_l, 1);
//    LED_P6x8Str(2,7,"pulse_count_r");
//    LED_PrintValueF(72,7,   pulse_count_r, 1);
        
   

}

extern s16 car_speed;     //DOLPHIN速度期望值，向前跑为负值

extern float camera_position; 
extern u8 tingche;
extern s16 brick_flag;
u8 podao_0=0;
u8 lost_right = 0;
u8 lost_left = 0;
u8 flag_lost_r = 0;
u8 flag_lost_l = 0;

u8 cross_flag = 0;   //十字路标志
 u16 left_count=0,right_cont=0;
s16 camera_row_num;
u8  left_flag = 0;   //必须清零
u8  right_flag = 0;
u8 zhijiao_flag;
u8 sanjiao=0;
u8  see_jiakuan=0;
//图像处理：改为豪哥，涛哥，勇哥的初级方法201402211054;  改栋哥20140314 ^_^ 摄像头正过来
u8 Camera_line(void)
{
    u8 *p_image;   
        
    s16 i = 0;     //有符号数
    s16 j = 0;
    
    s16 left = 0;
    s16 right = 0;
    s16 temp = 0;
    s16 left_o[2] = {0};
    s16 right_o[2] = {0};
    

    u8 row_lost = 0;
    
    u8 lk_flag = 0;
    
    float cross_lost_l = 0;
    float cross_lost_r = 0;
    
    lost_right = 0;
    lost_left = 0;
    flag_lost_r = 0;
    flag_lost_l = 0;
    
    cross_flag = 0;
    
    s16 ii = 0;
    
    
    line_Y[CAMERA_LINE].x = (u8)((line_Y[CAMERA_LINE-1].x + (CAMERA_W-1)/2)/2);//确定起始扫描
    
    cross_flag = 0;
    row_lost = 0;
    
    left = 0; 
    right = CAMERA_W - 1;
    //line_Y[CAMERA_LINE].x表示第61行的中值  
    /************搜线范围限幅**************/
    if (line_Y[CAMERA_LINE].x > CAMERA_W*0.8)  //第59行 搜线起点的限幅 有第零行的
    {
        line_Y[CAMERA_LINE].x = (u8)(CAMERA_W*0.8);
    }
    else if (line_Y[CAMERA_LINE].x < CAMERA_W*0.2)
    {
        line_Y[CAMERA_LINE].x = (u8)(CAMERA_W*0.2);   
    }
    
   /*******************前6行*********************/ 
    for (i=CAMERA_LINE-1; i>CAMERA_LINE-7; i--)    // 
    {
        p_image = p_camera_buffer[i];//存着每行的第一个点
        
        left_flag = 0;   //必须清零
        right_flag = 0;
        
        left = left-6;  //前6行从中间往两边  
        right = right+6;
        
        /***********防止超出一幅图像的边缘*************/
        if (left<1)  left = 1;
        if (right > CAMERA_W-2)  right = CAMERA_W - 2;
        /**********************************************/
        
        
        for (j=line_Y[i+1].x; j>left-1; j--)   //搜左边 ，这一行的搜线起点是 上一行的中点
        {
            if ( *(p_image+j)==bbb )
            {
               // if( *(p_image+j-1)==bbb )
               //{
                    left_flag = 1;
                    left = j;       //搜到跳变就跳出
                    break;
               // }
            }
            else
            {
           //     *(p_image+j) = 8;      //不是黑点就打点
            }       
        }
        
        
        for (j=line_Y[i+1].x; j<right+1; j++)   //中间往右
        {
            if ( *(p_image+j)==bbb )
            {
               // if ( *(p_image+j+1)==bbb )
               // {
                    right_flag = 1;
                    right = j;
                    break;
               // }
            }
            else
            {
            //    *(p_image+j) = 8;  //不是黑点就打点
            }       
        }
//                 int ab=right-left;
//     
//   printf("%d\n\n", (int)ab);
    
        if ( (left_flag+right_flag)==2 )   //两边都有  这种算法我喜欢
        {
            if ( (right-left)<50 )    //可能是起跑线,应该就是起跑线了
            {
                row_lost++;
              /********确定了一个点的X Y坐标、2边线的X坐标**********/  
 //               line_Y[i].x = line_Y[i+1].x+20; //一个点的确定
 //               line_Y[i].y = i;
 //               line_Y[i].lr_flag = 2;
 //               /**** 我表示我现在还是不理解下面这2句的意思 *****/
 //               line_Y[i].m_l = line_Y[i].x - expect[i];
 //               line_Y[i].m_r = line_Y[i].x + expect[i];//好吧  应该是起跑线了2015 3 6
 //               /************ 下一行搜线范围的确定，**************/                       
 //              left = line_Y[i].x - CAMERA_W/4;             
 //              right = right + CAMERA_W/4;
                
 //              
 //               if (left<1)  left = 1;
 //               if (right > CAMERA_W-2)  right = CAMERA_W - 2; 
 //               Buzzer_Bi();
 //              continue;    //放弃此次循环
            }
            else
            {
                line_Y[i].x = (left+right)/2;   //一个点的确定
                line_Y[i].y = i;
                line_Y[i].lr_flag = 2;
                line_Y[i].m_l = left;
                line_Y[i].m_r = right;
            /**********************对line_Y[i].x的值进行修改校准*********************/    
               if ( line_Y[i].x<CAMERA_W/2)  //80
                {
                    //  line_Y[i].x = left+CAMERA_W/3 + 12;
                      line_Y[i].x = (left + right)/2;
                      /**********可有可无吧**********/
                    if (line_Y[i].x>(CAMERA_W-2))
                        line_Y[i].x = CAMERA_W-2;
                    else if (line_Y[i].x<1)
                        line_Y[i].x = 1;
                    /*********************************/
                }
                else
                {
                  //  line_Y[i].x = right-CAMERA_W/3-12;
                    line_Y[i].x = (left + right)/2;
                    
                    if (line_Y[i].x>CAMERA_W-2)
                        line_Y[i].x = CAMERA_W-2;
                    else if (line_Y[i].x<1)
                        line_Y[i].x = 1;
                }
          /*******结束*************对line_Y[i].x的值进行修改校准*********结束**********/
            }
            
            if (row_lost>1)     //检测到之前丢线了
            {
                for (j=i; j<CAMERA_H-1; j++)  //如果在前6行丢线了，则往后 到119行搜索
                  
                 
                  
                  
                {
                    line_Y[i+1].x = line_Y[i].x;   //前一个中点等于这次的中点
                    /*******难懂***********/
               //     *(p_camera_buffer[i+1]+line_Y[i+1].x) = 38;  //^_^ 打点//p_camera_buffer[i+1] 指向 第i+1个点
                }
                row_lost = 0;
            }
            
               left = line_Y[i].m_l-5; //搜线范围的确定
               right = line_Y[i].m_r+5;
        }
        else if ( (left_flag+right_flag)==1 )  //只搜到左边
        {
            if (left_flag == 1)
            {
               // line_Y[i].x = left + expect[i]*0.9;  //一个点的确定
                line_Y[i].y = i;
                line_Y[i].lr_flag = -1;   //左边有为-1
                line_Y[i].m_l = left;
                line_Y[i].m_r = CAMERA_W-2;
               line_Y[i].x=(line_Y[i].m_l+line_Y[i].m_r)/2;
                if ( line_Y[i].x>(CAMERA_W-2) )  //连续两行搜到边界，右边边界为CAMERA_W-2 即为十字路口
                {
                    line_Y[i].x = CAMERA_W-2;
                    if ( line_Y[i+1].x==CAMERA_W-2 ) 
                        return i;   //中点到边界  ,不再往下搜，跳出返回第几行
                }
                
                if ( line_Y[i].x<line_Y[i+1].x )  //折线判断，如果折，就掰回来
                    line_Y[i].x = line_Y[i+1].x;
            left = line_Y[i].m_l-4;  //只搜到一边时，搜线范围的确定
            right = line_Y[i].m_r;
            }
            else if (right_flag == 1)    //只搜到右边
            {
               // line_Y[i].x = right - expect[i]*0.9; //中心点的确定
                line_Y[i].y = i;
                line_Y[i].lr_flag = 1;   //右边有为1
                line_Y[i].m_l = 1;
                line_Y[i].m_r = right;
                line_Y[i].x=(line_Y[i].m_l+line_Y[i].m_r)/2;
                if ( line_Y[i].x<1 )
                {
                    line_Y[i].x = 1;
                    if (line_Y[i+1].x == 1)
                    return i;    //搜到边界，跳出,  这里有点问题
                }
                              
                if ( line_Y[i].x>line_Y[i+1].x)   //折线判断
                    line_Y[i].x = line_Y[i+1].x; 
              left = line_Y[i].m_l;  //只搜到一边时，搜线范围的确定
               right = line_Y[i].m_r+4;
            }
            
            
         
        }   
        else   //两边都没有
        {
             row_lost++;
                
             line_Y[i].x = line_Y[i+1].x;   //这一行的中点值，等于上一行的中点值
             line_Y[i].y = i;
             line_Y[i].lr_flag = 0;   //左边有为-1
             line_Y[i].m_l = line_Y[i+1].x - expect[i];
             line_Y[i].m_r = line_Y[i+1].x + expect[i];
             
          //   *( p_image+line_Y[i].m_l )= bbb;
          //   *( p_image+line_Y[i].m_r )= bbb; //2边丢线，2边补线
             
             left = line_Y[i].m_l; //两边都搜不到线时搜线范围的确定
             right = line_Y[i].m_r;
        }
        
        
        lk_flag = left_flag + right_flag;
        
       *( p_image + (u8)(line_Y[i].x*2/2+1) ) = bbb;   //在找到的中心点出打黑点 
        
    }
    
    
    if (row_lost>4)  //如果丢线大于4行
    {
        //寻到顶了 跳出
        if ( (*(p_camera_buffer[i-1]+line_Y[i].x)==bbb)||(*(p_camera_buffer[i-2]+line_Y[i].x)==bbb)||(*(p_camera_buffer[i-3]+line_Y[i].x)==bbb))
        {
                *(p_camera_buffer[i-1]+line_Y[i].x) = 29;//画蓝 
                 return i;
        }    
        else    //是十字路
        {
            cross_flag = 1;    //十字路
        }
    }

   
      
    
//////////////////////////////////////////////////////////////////////////////////重点，6 行之后
    row_lost = 0;
    u8 ll=0,rr=0;
    for (i=CAMERA_LINE-7; i>1; i=i-1 )  //前6行后 从第7行
  {
         p_image = p_camera_buffer[i];   //关键
        
         left_flag = 0;
         right_flag = 0;

    //搜线范围的确定，各种情况分类讨论
//     left = line_Y[i+1].x - expect[i] - 4;  //搜线范围的确定 *0.7 是想往外搜多一点
//     right = line_Y[i+1].x + expect[i] + 4; //如果图像全白，基本搜遍整个图
         if (left<1) left = 1;                        //非常重要
         if (right>CAMERA_W-2) right = CAMERA_W-2;
        
         
         for (j=line_Y[i+1].x; j>left-1; j--)   //从中间往左搜
         {
             if ( *(p_image+j)==bbb )
             {
                  if( *(p_image+j-1)==bbb )
                  {
                      left_flag = 1;
                      left = j;
                      left_o[0] = left_o[1];
                      left_o[1] = left;
                      break;
                  }
             }
            else
            {
                *(p_image+j) = 8;     //不是黑点就打点
            }          
        }
        
        for (j=line_Y[i+1].x; j<right+1;j++)   //中间往右边搜
        {
            if ( *(p_image+j) == bbb )
            {
                if( *(p_image+j+1) == bbb )
                {
                    right_flag =1;
                    right = j;
                    right_o[0] = right_o[1];
                    right_o[1] = right;
                    break;                  
                }
            }
            else
            {
                *( p_image+j) = 8;  //不是黑点就打点
            }             
        }
            
        if ( (left_flag+right_flag)==2  )  //跳变两边都有&& mode!=3
        {
                         //起跑线  switch_start_flag==1 && image_start_flag_0==0  image_start==0
              if(tingche==1&&distance_0>3500)
              {
            
                    if ( (fabsf(camera_position-CAMERA_W/2)<16) && (i>=19) && (i<=54) 
                   && (fabsf(line_Y[i+1].x-CAMERA_W/2)<29) && (lk_flag==2) )
                 {
                     
                    if ( (line_Y[i+1].x+1+(i*0.3+2)) >= 139 )
                    {
                        line_Y[i+1].x = 149 - (1+(i*0.3+2));
                    }
                    
                    if ( (line_Y[i+1].x-1-(i*0.3+2)) <= 20 )
                    {
                        line_Y[i+1].x = 20 + (1+(i*0.3+2));
                    } 
                    
                    if( line_Y[i+1].x>=119 )
                    {
                        line_Y[i+1].x = 119;
                    }
                    else if ( line_Y[i+1].x<=40 )
                    {
                        line_Y[i+1].x = 40;
                    }
                                                     
                    temp = 0;
                    if ( image_buf[ camera_line[i-1] ][ (u8)(line_Y[i+1].x+1+(i*0.3+2)) ]==bbb )
                    temp++;
                    if ( image_buf[ camera_line[i-2] ][(u8)(line_Y[i+1].x+1+(i*0.3+2))]==bbb )
                    temp++;
                    if ( image_buf[ camera_line[i-3] ][(u8)(line_Y[i+1].x+1+(i*0.3+2))]==bbb )
                    temp++;
                    if ( image_buf[ camera_line[i-1] ][(u8)(line_Y[i+1].x-1-(i*0.3+2))]==bbb )
                    temp++;
                    if ( image_buf[ camera_line[i-2] ][(u8)(line_Y[i+1].x-1-(i*0.3+2))]==bbb )
                    temp++;
                    if ( image_buf[ camera_line[i-3] ][(u8)(line_Y[i+1].x-1-(i*0.3+2))]==bbb )
                    temp++;
                
                
                    if ( (fabsf(line_Y[i].x-line_Y[i+1].x)<3) && (temp>0) )
                    {
                        temp = 0;
                        for (ii=1; ii<8; ii++)
                        {
                             if (image_buf[ camera_line[i-ii] ][(u8)(line_Y[i+1].x+1+((i-ii)*0.3+2))] == bbb)
                             {
                                 temp++;
                             }
                             else
                             {
                                 image_buf[ camera_line[i-ii] ][(u8)(line_Y[i+1].x+1+((i-ii)*0.3+2))] = 88;
                             }
                        }
                    
                        if ( (temp>0) && (temp<3) )
                        {
                            temp = 0;
                            for (ii=1; ii<7; ii++)
                            {
                                if (image_buf[ camera_line[i-ii] ][(u8)(line_Y[i+1].x-1-((i-ii)*0.3+2))] == bbb)
                                {
                                    temp++;
                                }
                                else
                                image_buf[ camera_line[i-ii] ][(u8)(line_Y[i+1].x-1-((i-ii)*0.3+2))] = 88;             
                            }
                         
                            if ( (temp>0) && (temp<3) )
                            {   
                              //  if ( distance_0>=2500) //这里有个距离 24M 6000大概3M  ,拨码
                                {
                                    if( (image_buf[ camera_line[i-5] ][(u8)(line_Y[i+1].x+1+((i-ii)*0.3+2))]) != bbb )
                                    {
                                        if ( (image_buf[ camera_line[i-5] ][(u8)(line_Y[i+1].x-1-((i-ii)*0.3+2))]) != bbb )
                                        {                                                                                                               
                                                                                                                      
                                                  image_start = i;                             
                                                  return i;
                                        }
                                    }
                                }
                            }   
                                              
                        }
                    } 
                }  
            }   //////////////end起跑线 
          
          
            if ( (right-left)<10 )  //可能检测到起跑线，或弯道曲率太大
            {
                //line_Y[i].x = line_Y[i+1].x+20;  //一个点的确定
               // line_Y[i].y = i;
               // line_Y[i].lr_flag = 2;
                //line_Y[i].m_l = left;
                //line_Y[i].m_r = right;
                
               //lk_flag = 2;
              //Buzzer_Bi();
            }
            else
            { 
                line_Y[i].x = (left+right)/2; //确定当前中心点
                line_Y[i].y = i;
                line_Y[i].lr_flag = 2;
                line_Y[i].m_l = left;
                line_Y[i].m_r = right;
                              
                *(p_image+line_Y[i].x) = 64;   //^_^打点
                
                if (row_lost>1)       //检测到之前丢线了  用斜率来补线
                {
                    temp = (line_Y[i].x-line_Y[i+row_lost+1].x)/(row_lost+1);
                    for (j=1;j<row_lost+1;j=j+1)
                    {
                        line_Y[i+j].x = line_Y[i].x - temp*j;
                        *( p_camera_buffer[i+j] + line_Y[i+j].x ) = 38; //打点
                    }               
                    row_lost = 0;                      
                }
                 //Buzzer_Mie();
                
            }
            if(0)//mode==3
            {
              
              if(ll>=6)//如果一场图像中有丢线
              {
               line_Y[i].x = left + expect[i];//-15;
              }
              else if(rr>=6)
              {
               line_Y[i].x = right - expect[i];//+15;
              }
              else
              {
              //不做处理！
              }   
            
            }
                            if(mode==7||mode==8)
            {
              if(mode==7)
              {
                 line_Y[i].x = left + expect[i]-10;//-15;
              }
              else if(mode==8)
              {
                 line_Y[i].x = right - expect[i]+10;//-15;
              }            
            }
            
            left = line_Y[i].m_l - 10;  //搜线范围的确定 *0.7 是想往外搜多一点
            right = line_Y[i].m_r  +10; //如果图像全白，基本搜遍整个图
            
            lk_flag = 2;
            
        }

        else if ( ((left_flag+right_flag) == 1) )  //跳变一个||mode==3
        {
            if (left_flag==1)    //左边有，有待优化
            {
                if ((line_Y[i+1].lr_flag==0) || (line_Y[i+2].lr_flag==0))
                {
                    line_Y[i].x = line_Y[i+1].x; 
                }           
                else if ( (line_Y[i+1].lr_flag==-1) || (line_Y[i+1].lr_flag==2) )
                {
                    line_Y[i].x = left + (line_Y[i+1].x-line_Y[i+1].m_l) - 1;//一个点的确定 ，
                }
                else 
                {
                    line_Y[i].x = left + expect[i]+15;
                }
                                              
                line_Y[i].y = i;
                line_Y[i].lr_flag = -1;   //左边有为-1
                line_Y[i].m_l = left;
                line_Y[i].m_r = line_Y[i].x + (line_Y[i].x - left);
                
                if(line_Y[i].m_r>(CAMERA_W-2))
                {
                   line_Y[i].m_r=CAMERA_W-2;
                }
                else if(line_Y[i].m_r<1)
                {
                   line_Y[i].m_r=1; 
                } 
                *( p_image+line_Y[i].m_r )= bbb;
                
                
                lost_right++;//回弯的回点
                if (lost_right>=9 && lost_left<=1 && flag_lost_l==0 && flag_lost_r==0)
                {
                    flag_lost_r = line_Y[i+7].m_r;
                }
                
                
                if (line_Y[i].x <= line_Y[i+1].x)  //折点判断
                line_Y[i].x = line_Y[i+1].x+(line_Y[i+2].x-line_Y[i+3].x);
            
            
                if (lk_flag ==2)     //上一次是两边都有
                {
                    line_Y[i+1].x = line_Y[i].x;
                    line_Y[i+2].x = line_Y[i].x;
                
                    *(p_camera_buffer[i+1]+line_Y[i+1].x)= 2;	//画点
		    *(p_camera_buffer[i+1]+line_Y[i+1].x)= 2;	//画点
                }
              if(danbian==1)
              {
              line_Y[i].x = left + expect[i];
              }
                            if(mode==7||mode==8)
            {
              if(mode==7)
              {
                 line_Y[i].x = left + expect[i]-10;//-15;
              }
              else if(mode==8)
              {
                 line_Y[i].x = right - expect[i]+10;//-15;
              }            
            }
               
                left = left - 10;  //搜线范围的确定 *0.7 是想往外搜多一点
                right = line_Y[i+1].x + expect[i] + 10; //如果图像全白，基本搜遍整个图
                
                lk_flag = 1;
            }
            else      //右边有 ，有待优化
            {              
                if ((line_Y[i+1].lr_flag==0) || (line_Y[i+2].lr_flag==0))
                {
                    line_Y[i].x = line_Y[i+1].x;
                }                
                else if ( (line_Y[i+1].lr_flag==1) || (line_Y[i+1].lr_flag==2) )
                {
                    line_Y[i].x = right - (line_Y[i+1].m_r-line_Y[i+1].x) + 1;//一个点的确定 ，
                }
                else 
                {
                    line_Y[i].x = right - expect[i]-15;
                }
                                              
                line_Y[i].y = i;
                line_Y[i].lr_flag = 1;   //右边有为1
                line_Y[i].m_l = line_Y[i].x - (right - line_Y[i].x);;
                line_Y[i].m_r = right;
                
                if(line_Y[i].m_l>(CAMERA_W-2))
                {
                   line_Y[i].m_l=CAMERA_W-2;
                }
                else if(line_Y[i].m_l<1)
                {
                   line_Y[i].m_l=1; 
                } 
             *( p_image+line_Y[i].m_l )= bbb;
                
                
                lost_left++;
                if (lost_left>=9 && lost_right<=1 && flag_lost_r==0 && flag_lost_l==0)
                {
                    flag_lost_l = line_Y[i+7].m_l;
                }
                
                         
                if (line_Y[i].x >= line_Y[i+1].x)  //折点判断
                line_Y[i].x = line_Y[i+1].x-(line_Y[i+3].x-line_Y[i+2].x);
           
            
                if (lk_flag ==2)   //上次两边都有
                {
                    line_Y[i+1].x = line_Y[i].x;
                    line_Y[i+2].x = line_Y[i].x;
                
                    *(p_camera_buffer[i+1]+line_Y[i+1].x)= 2;	//画点
		    *(p_camera_buffer[i+2]+line_Y[i+2].x)= 2;	//画点
                }
                if(danbian==1)
                {
                  line_Y[i].x = right - expect[i];
                }

                            if(mode==7||mode==8)
            {
              if(mode==7)
              {
                 line_Y[i].x = left + expect[i]-10;//-15;
              }
              else if(mode==8)
              {
                 line_Y[i].x = right - expect[i]+10;//-15;
              }            
            }
                
                left = line_Y[i+1].x - expect[i] - 10;  //搜线范围的确定 *0.7 是想往外搜多一点
                right = right + 10; //如果图像全白，基本搜遍整个图
                
                lk_flag = 1;
            }
            
            
                      
            //防止出界，并且两次是边界跳出
            if (line_Y[i].x>=(CAMERA_W-2))
            {
                line_Y[i].x = CAMERA_W - 2;
                if( line_Y[i+1].x== CAMERA_W-2)
                return i+3;                  
            }
            else if (line_Y[i].x<=1)  //这里有个严重的BUG，7.12找到，之前是return i
            {
                line_Y[i].x = 1;
                if( line_Y[i+1].x== 1)
                return i+3;    
            }
            
        *(p_image+line_Y[i].x) = 251;   //^_^打点
            
            if(row_lost>1)
            {
                temp = (line_Y[i].x-line_Y[i+row_lost+1].x)/(row_lost+1);
                for (j=1;j<row_lost+1;j=j+1)
                {
                    line_Y[i+j].x = line_Y[i].x - temp*j;
                    *( p_camera_buffer[i+j] + line_Y[i+j].x ) = 38; //打点
                }
                 
                row_lost = 0;                       
            }                 
       }
        else    //跳变两边都没有
       {
           lk_flag = 0;  
           row_lost++;
           if( (row_lost>4) && (cross_flag==1) && (i>CAMERA_H-20) ) //十字路
           {
              
               cross_flag = 2;
           }
          
           cross_lost_l = (line_Y[CAMERA_LINE-6].m_l-line_Y[CAMERA_LINE-1].m_l)/4;
           cross_lost_r = (line_Y[CAMERA_LINE-1].m_r-line_Y[CAMERA_LINE-6].m_r)/4;
           
           if (cross_lost_l<=0)
           {
               cross_lost_l = 0.01;
           }
           if(cross_lost_l>=0.2)
           {
               cross_lost_l = 0.2;
           }
           
           if (cross_lost_r<=0)
           {
               cross_lost_r = 0.01;
           }
           if(cross_lost_r>=0.2)
           {
               cross_lost_r = 0.2;
           }
           
           if (line_Y[i+row_lost+3].m_l>CAMERA_W/2)
           {
               line_Y[i+row_lost+3].m_l = CAMERA_W/2;
           }
           if (line_Y[i+row_lost+3].m_r<CAMERA_W/2)
           {
               line_Y[i+row_lost+3].m_r = CAMERA_W/2;
           }
                  
          // line_Y[i].x = line_Y[i+1].x+((line_Y[i+1].x-line_Y[i+2].x)+(line_Y[i+2].x-line_Y[i+3].x))/2;//这一行的中点值
           line_Y[i].y = i;
           line_Y[i].lr_flag = 0;   //左边有为-1
           line_Y[i].m_l = (u8)(line_Y[i+row_lost+3].m_l + cross_lost_l*(row_lost+3) + 2);
           line_Y[i].m_r = (u8)(line_Y[i+row_lost+3].m_r - cross_lost_r*(row_lost+3) - 2);
           line_Y[i].x = ( line_Y[i].m_l + line_Y[i].m_r)/2;
            if (line_Y[i].m_l <= line_Y[i+1].m_l)  //折点判断
                line_Y[i].m_l = line_Y[i+1].m_l+(line_Y[i+2].m_l-line_Y[i+3].m_l);
            
            if (line_Y[i].m_r >= line_Y[i+1].m_r)  //折点判断
                line_Y[i].m_r = line_Y[i+1].m_r-(line_Y[i+3].m_r-line_Y[i+2].m_r);
            
           
            if (line_Y[i].m_l - line_Y[i].m_r>0)
            {
                line_Y[i].m_l = CAMERA_W/2-expect[i]/2;
            }
            if (line_Y[i].m_r - line_Y[i].m_l<0)
            {
                line_Y[i].m_r = CAMERA_W/2+expect[i]/2;
            }
           
           
           if(line_Y[i].m_l>(CAMERA_W-2))
           {
               line_Y[i].m_l=CAMERA_W-2;
           }
           else if(line_Y[i].m_l<1)
           {
               line_Y[i].m_l=1; 
           } 
  *( p_image+line_Y[i].m_l )= bbb;
           
           if(line_Y[i].m_r>(CAMERA_W-2))
           {
               line_Y[i].m_r=CAMERA_W-2;
           }
           else if(line_Y[i].m_r<1)
           {
               line_Y[i].m_r=1; 
           } 
           *( p_image+line_Y[i].m_r )= bbb;
           
           
           
           if((line_Y[i].x-line_Y[i+2].x)*(line_Y[i].x-line_Y[i+2].x)==16)
           {
                if((line_Y[i+1].x-line_Y[i+3].x)*(line_Y[i+1].x-line_Y[i+3].x)==16)	
                {
                line_Y[i].x = line_Y[i+1].x;
                line_Y[i+2].x = line_Y[i].x;	
                }
           }
           
           
           if(line_Y[i].x>(CAMERA_W-7))
           {
             line_Y[i].x=CAMERA_W-7;
           }
           else if(line_Y[i].x<6)
           {
             line_Y[i].x=6; 
           }
           
           
           left = line_Y[i].m_l - 14;  //搜线范围的确定 *0.7 是想往外搜多一点
           right = line_Y[i].m_r + 14; //如果图像全白，基本搜遍整个图
           
           
           *( p_image+line_Y[i].x )= bbb;  //画黑点
       }
  
       //寻到顶了，跳出
            if((*(p_camera_buffer[i-1]+line_Y[i].x)==bbb)||(*(p_camera_buffer[i-2]+line_Y[i].x)==bbb))
            {
              *(p_camera_buffer[i-1] + line_Y[i].x) = 29;
               break;
            }

       
     	 *( p_image+line_Y[i].x*2/2+1 ) = bbb;   //找到的中心线打点      
   }
    
     if(0)//看得远且上次两边都有(i==1) && lk_flag==2
	{	
             	    	
		if(right_o[0]+right_o[1]+right-left_o[0]-left_o[1]-left>120) 
		{   		
			 if( (fabsf(line_Y[5].x-line_Y[9].x) +fabsf(line_Y[9].x-line_Y[13].x)+fabsf(line_Y[13].x-line_Y[17].x)<3) )//最后1.1个条件 			  			  			   
					{                     
                                           podao_0=1;                                                                                   
                                         
					}			
		}				
	}
  /*   if( (lk_flag==2)&&(mode==3||mode==7||mode==8))//看得远且上次两边都有这次有丢线&&(left_flag!=1 && right_flag==1)||(left_flag==1 && right_flag!=1lk_flag==2)&&&&(mode==3||mode==7||mode==8)
	{	
             	    	
		if(fabsf(right_o[0]+right_o[1]+right-left_o[0]-left_o[1]-left)>190) 
		{   							  			  			   
					 
                                          
                                           see_jiakuan=1;                                                                                   
                                         
								
		}				
	}*/
    /* LED_P6x8Str(2,3,"podao_0 ");
    LED_PrintValueF(72,3,    podao_0, 1);
              LED_P6x8Str(2,4,"11111 ");
    LED_PrintValueF(72,4,    right_o[0]+right_o[1]+right-left_o[0]-left_o[1]-left, 1);
                   LED_P6x8Str(2,5,"22222 ");
    LED_PrintValueF(72,5,    fabsf(line_Y[5].x-line_Y[9].x) +fabsf(line_Y[9].x-line_Y[13].x)+fabsf(line_Y[13].x-line_Y[17].x), 1);
              LED_P6x8Str(2,6,"i ");
    LED_PrintValueF(72,6,    i, 1);    */
    if(fabs(line_Y[i].x-line_Y[i+1].x)>5)
      line_Y[i].x=(line_Y[i].x-line_Y[i+1].x)/2;
    return i+1;//线全部找到了!!!!      
}


#define image_mid_line  (79.5)
float camera_position = 79.5;         //位置
float camera_position_0 = 79.5;
float camera_position_0_old = 79.5;

float camera_weighed_error = 0;
u8 weighed_flag = 0;
extern u8 servo_row;                 //前瞻 ^_^ 关键、、、、、、、、、、、、、、、、、、
extern u8 chaoche_jiakuan_7;
float servo_history[4] = {79.5,79.5,79.5,79.5};  //本来三个数就够了的，历史位置
u8 servo_history_index = 0;
// camera_row_num = 0;         //搜到的行数，关键也
//extern u16 servoRow;
s16 camera_s = 0;
extern u16 servoRow;
u8 s_right = 0;
u8 s_left = 0;
u8 s_straight = 0;
s16 distance_of_podao=0;
s16 distance_of_jiankuan=0;
s16 distance_of_sanjiao=0;
extern s16 pulse_count;
///////////////////////////////////////////////////////////////////////////////////////////////////////
//unsigned int bs ;
//Dest 目的参数   Scr 源参数  
s16 camera_2_num()
{

   s16 i=0;
   s16 sum=0;
   float average=0;
   weighed_flag = 0;
  
  
   camera_row_num = Camera_line();  //搜线算法处理
               
   camera_s = track_judge(camera_row_num); //赛道判断
   
  // servo_row_0 = servo_control(); 
   
   camera_position_0_old = camera_position_0;
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   u16 hang_shu=0;
   u16 shuangbian=0;

    if(podao_0==1)
    {           
      image_start =0;//坡道来了,清楚起跑线标志 
      mode=0;
       distance_of_podao=distance_of_podao+pulse_count;//路程累加!!!//500->100cm
      Buzzer_Bi();
       if(distance_of_podao>=5500)
       {  
          podao_0=0;
          distance_of_podao=0;                             
       } 
             
    }         
    else
    {
         Buzzer_Mie();          
    } 

   if(jiance==0)//通过一段距离后再开始检测有没有黑三角！
   {
    distance_of_sanjiao=distance_of_sanjiao+pulse_count;
     if(distance_of_sanjiao>=8000)//distance_of_sanjiao>=10008000
     {
       jiance=1;
       distance_of_sanjiao=0;
     }
   }
    if(jiance_jiakuan==0)//通过一段距离后再开始检测有没有加宽区域！
   {
    distance_of_sanjiao=distance_of_sanjiao+pulse_count;
     if(distance_of_sanjiao>=2500)//distance_of_sanjiao>=1000
     {
       jiance_jiakuan=1;
       distance_of_sanjiao=0;
     }
   } 
    for(i=CAMERA_LINE-1;i>29;i--) 
    {
       
     if(line_Y[i].lr_flag==2)   //-1 ，左边， 1 右边 2 两边，0 没
       shuangbian++;
     
    }
    if(danbian==1)
    {           
      mode=0;
     Buzzer_Bi();
     if(shuangbian>=27 )  //发送标志位让后车动&&pp==1&& mode==3
      {
     
      danbian=0;
      mode=0;  
      }
             
    }         
    else
    {
         Buzzer_Mie();          
    }
   // if(shuangbian>43)
   // {
   // NtpBrakeCount_1=0;
   // }
    
   if(chaoche_jiakuan_7==1)
    {
        if(mode==7||mode==8)
        {        
        distance_of_jiankuan=distance_of_jiankuan+pulse_count;  //用此距离来使小车离开一定距离在发送让停车区的车走的标志！      
        }
       if(shuangbian>=27 && distance_of_jiankuan>=3100)  //发送标志位让后车动&&pp==1&& mode==3
        {         
        mode=0;
        distance_of_jiankuan=0;
        uart_putchar   (UART0 , '4');                    //发送字符    
        }
    }

    else
    {
       u16 zuobian=0;
       u16 youbian=0;
        for(i=CAMERA_LINE-1;i>camera_row_num;i--) 
        {
           
           if(line_Y[i].lr_flag==-1)   //-1 ，左边， 1 右边 2 两边，0 没
           zuobian++;
           if(line_Y[i].lr_flag==1)   //-1 ，左边， 1 右边 2 两边，0 没
           youbian++;    
        }
         /*  LED_P6x8Str(2,7,"zuobian");
       LED_PrintValueF(72,7,   zuobian, 1);
               LED_P6x8Str(2,6,"youbian");
       LED_PrintValueF(72,6,   youbian, 1);
               LED_P6x8Str(2,5,"camera_row_num");
       LED_PrintValueF(72,5,   camera_row_num, 1);
                      LED_P6x8Str(2,4,"camera_error");
       LED_PrintValueF(72,4,   camera_error, 1);*/
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       if((mode==7||mode==8)&&camera_row_num<=4&&((zuobian>20&&youbian<2)||(youbian>20&&zuobian<2))&&camera_error<20)//
       {
         
       distance_of_jiankuan=distance_of_jiankuan+pulse_count;  //用此距离来使小车离开一定距离在发送让停车区的车走的标志！
       
       }
          if(shuangbian>=27 && distance_of_jiankuan>=500)  //发送标志位让后车动&&pp==1&& mode==3
        {
      
        mode=0;
        distance_of_jiankuan=0;
        uart_putchar   (UART0 , '4');                    //发送字符4
    
       }
    }
 /*  LED_P6x8Str(2,7,"shuangbian");
   LED_PrintValueF(72,7,   shuangbian, 1);
   LED_P6x8Str(2,6,"see_jiakuan ");
   LED_PrintValueF(72,6,    see_jiakuan, 1); */

               if(pulse_count>100) 
        {
servo_row=29;   //越大越切内道21     25  23
        }
        else if(pulse_count>80)
        {
servo_row=27;   //越大越切内道21     25  23
        }
        else if(pulse_count>70)
        {
servo_row=26;   //越大越切内道21     25  23
        }

         else if(pulse_count>60)
        {
servo_row=25;   //越大越切内道21     25  23
        }
         else if(pulse_count>50)
        {
servo_row=24;   //越大越切内道21     25  23
        }
         else if(pulse_count>40)
        {
servo_row=23;   //越大越切内道21     25  23
        }
         else
        {
servo_row=22;   //越大越切内道21     25  23
        }
          
      
      if(0)//camera_row_num<=2
   {
          for(i=CAMERA_LINE-1;i>camera_row_num;i--) 
     {
      sum=expect_gaosu[i]*line_Y[i].x+sum;
      hang_shu+=(expect_gaosu[i]);
     
     }
      average=sum/(hang_shu);
      camera_position = average;
   }
   
   else
   {
   if ( camera_row_num <= (CAMERA_LINE-servo_row) )  //中点
   {
       camera_position_0 = (line_Y[CAMERA_LINE-servo_row+1].x+line_Y[CAMERA_LINE-servo_row].x)/2;
       weighed_flag = 0;  
   }  
   else
   {       
       if ( camera_row_num >= CAMERA_LINE - 21)     //
       {                                
            camera_position_0 = line_Y[camera_row_num+1].x;  
            weighed_flag = -1;
       }
       else if ( camera_row_num > (CAMERA_LINE-servo_row) )
       {
           weighed_flag = 1;
           
           camera_position_0 = image_mid_line + (servo_history[0] + servo_history[1]
               + servo_history[2] - 238.5)/2.6;  
            
            if (camera_position_0<=9)  camera_position_0 = 9;
            if (camera_position_0>=150) camera_position_0 = 150;
       }   
       else 
       {
           camera_position_0 = camera_position; 
       }
   }
      camera_position= camera_position_0;   //更新误差的地方
   }
   
   
   
  // led_turn(LED1);   //灯闪看频率  
   
    //加权算法
/*  camera_weighed_error = 100*80/( sqrt( ( 6400+(CAMERA_LINE-camera_row_num)*(CAMERA_LINE-camera_row_num) ) ) ) - 79.5;
   camera_weighed_error *= 0.5;   
   if( camera_row_num >= 3 && weighed_flag==0 ) //         //&& curve_flag
   {       
       if(camera_position_0>88.5 && camera_position_0<152.5)
        {
            camera_position = camera_position_0 + camera_weighed_error; 
        }
        else if(camera_position_0<70.5 && camera_position_0>6.5)
        {
            camera_position = camera_position_0 - camera_weighed_error;           
        }
        else
        {
           camera_position = camera_position_0; //_0是中点
        }       
   }    */

  //  LED_P6x8Str(2,7,"weighed");
  //  LED_PrintValueF(72,7,   camera_weighed_error, 1);



    
    
    
   
   if (camera_position<=3.0)  {camera_position =3.0;}  //非常很重要
   else if (camera_position>=156.0) {camera_position = 156.0;}
  
  
   servo_history[servo_history_index] = camera_position_0;       
   if (++servo_history_index >= 3)
   {
       servo_history_index = 0;
   }
   
  camera_error = image_mid_line - camera_position;  
   
  return camera_row_num;
}




u8 servo_control()
{
    u8 servo_row_temp = 0;
    

    return servo_row_temp;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//赛道类型判断
u8 track_judge( u8 camera_row_num_temp )
{
    u8 i = 0;
    u8 ss = 0;
    
    s_right = 0;
    s_left = 0;
    s_straight = 0;
    ss = 0;
    
    for (i=CAMERA_LINE-7; i>camera_row_num_temp;i=i-1)
    {
        if (line_Y[i].x-line_Y[i-1].x>=1)
        {
            s_left +=1;
        }
        else if (line_Y[i-1].x-line_Y[i].x>=1)
        {
            s_right +=1;
        }
        else 
        {
            s_straight+=1;
        }
    }
           
    for (i=CAMERA_LINE-7; i>camera_row_num_temp;i=i-1)
    {
        if (fabsf(line_Y[i].x-image_mid_line)<=20)
        {
            ss +=1;
        }
    }   
    return ss;
}
/***发送原始二值化图像***/
void display_bin_img_uart(void)
{
    u8 i = 0;    //typedef unsigned char  u8
    u8 j = 0;
    u8 temp = 0;
    
    u8 *p_img_source = img_bin_buff;
  
    printf("下一场:\n\n\n\n\n\n");
    for(i=0;i<CAMERA_H;i+=1)
    {        
         for(j=0;j<CAMERA_W/8;j++)
         {  
                p_img_source++;
                temp = *p_img_source;
                if (((temp>>7)&0x01) == 1) printf("0"); else printf("_");
                if (((temp>>6)&0x01) == 1) printf("0"); else printf("_");
                if (((temp>>5)&0x01) == 1) printf("0"); else printf("_");
                if (((temp>>4)&0x01) == 1) printf("0"); else printf("_");
                if (((temp>>3)&0x01) == 1) printf("0"); else printf("_");
                if (((temp>>2)&0x01) == 1) printf("0"); else printf("_");
                if (((temp>>1)&0x01) == 1) printf("0"); else printf("_");
                if (((temp>>0)&0x01) == 1) printf("0"); else printf("_");                     
         }
         printf("      %d",i);
         printf(" \n ");
    }   
}


/*
打点:
白点   0    
黑点   1     bbb 中线 
红点   2 这次丢一边，上次两边都有#    64 两边都没丢^   38连续丢线补线
蓝点    29 顶点跳出
黄点    251   搜到边界
搜过的点  8
*/
/***发送解压后的二值化图像***/
// 没用到指针 ， 有用指针的话要传递指针参数
void display_buf_img_uart(void)
{
    s16 i=0,j=0;
    u8 temp_k = 0;   //typedef unsigned char  u8
    
    u8 temp = 0;
    u8 temp_0 = 0;
       
    for(i=0;i<CAMERA_LINE;i+=1)
    {
        printf("     ");
         for(j=0;j<CAMERA_W;j++)
         {                                      
             temp_k = image_buf[ camera_line[i] ][j];
             switch (temp_k)
             {
                 case 8: { printf("*");++temp_0;}break;
                 case 1: { printf("0"); }break;
                 case 0: { printf("_"); }break;
                 case 64: { printf("^"); }break;
                 case 2: { printf("#"); }break;
                 case 38: { printf("%"); }break;
                 case 29: { printf("@"); }break;
                 case 251: { printf("!"); }break;
                 case 88 :  { printf("&");} break;
                 default : { printf("?"); }break;
             }
         }       
         printf("   %d\n ",i);  
         temp = line_Y[i].x;
         printf("    %d",temp);
         temp = 0;
         printf("    %d",temp_0);
         temp_0 = 0;
   }
   printf("这一场,二值化原图像:\n");
}



void display_yy()
{
     u8 cmd[4] = {0, 255, 1, 0 };   //yy_摄像头串口调试 使用的命令   
     uart_putbuff(FIRE_PORT, cmd, sizeof(cmd));    //先发送命令   
     
    u8 i=0; 
    u8 j=0;       
    for (i=0;i<CAMERA_LINE;i++)
    {     
        for (j=0; j<CAMERA_W; j++)
        {
              if (image_buf[ camera_line[i] ][j] == bbb)
              uart_putchar(FIRE_PORT, 0x00);
              else
              uart_putchar(FIRE_PORT, 0xff);    
        }       
    }
}

//发图象该，十字改，三角改

void Camera_data_show(void)
{

  display_yy();
    //   display_bin_img_uart();
  //   display_buf_img_uart();
  
}
