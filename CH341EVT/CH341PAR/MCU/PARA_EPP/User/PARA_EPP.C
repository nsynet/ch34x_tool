/********************************** (C) COPYRIGHT *******************************
* File Name          : PARA_EPP.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/10/10
* Description        : CH563 EPP DEMO
*                      (1)、CH563 Examples by KEIL3_V3.53;    
*                      (2)、串口0输出监控信息,115200bps,默认晶振100MHz;
*                      (3)、(CH341)EPP并口模式下数据数据发送，CH563通过并口接收;
                       (4)、CH563发送数据，CH341在EPP并口模式下接收数据; 
                       (5)、使用单片机演示收发操作时可以结合CH341上位机软件CH341PAR.EXE;
                       (6)、单片机时指令周期不能超过20ns，如果高于20ns，不能确保准确接收数据;		   
                       (7)、一次最多接收2^32个字节的数据，可以用RECV_NUM来设定;					   
                       (8)、一次最多发送2^32个字节的数据，可以用SEND_NUM来设定;					   
*******************************************************************************/

/*接口定义*/
/*******************************************************************************
*接口可以根据使用的单片机作相应的修改
*PIN				 :CH563(EPP)    DIR      CH341EPP
*                                   PB0          <-        WR	   							   	  						   
*                                   PA9          ->        WAIT					  							   
*                                   PA10         <-        AS	  					   							   
*                                   PA11         <-        DS					  					   							   
*                                   PD0~PD7      <->       D0~D7                双向数据端口				  						   				   
*******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#define DEBUG                        0                                         /*  调试  */

#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
 
/******************************************************************************/
/* 变量定义 */
#define RECV_NUM                   512                                         /*定义接收缓冲区大小*/
#define SEND_NUM                   4096                                        /*定义发送缓冲区大小*/
#define PIN_PB_0_WR                ( 1 << 0 )                                  /* WR(PB0)  */
#define PIN_PA_9_WAIT              ( 1 << 9 )                                  /* WAIT(PA9)*/
#define PIN_PA_10_AS               ( 1 << 10 )                                 /* AS(PA10) */
#define PIN_PA_11_DS               ( 1 << 11 )                                 /* DS(PA11) */

#define PIN_CH341EPP_WR            (R32_PB_PIN&0x001)                          /*获取PIN_WR引脚电平*/
#define PIN_CH341EPP_DS            (R32_PA_PIN&0x800)                          /*获取PIN_DS引脚电平*/

#define PIN_PWAIT_PD( )          { R32_PA_PD  |= PIN_PA_9_WAIT; }              /*PIN_WAIT设置为下拉输出*/
#define PIN_PWAIT_LOW( )         { R32_PA_CLR |= PIN_PA_9_WAIT; }              /*PIN_WAIT引脚输出低电平*/
#define PIN_PWAIT_HIGH( )        { R32_PA_OUT |= PIN_PA_9_WAIT; }              /*PIN_WAIT引脚输出高电平*/
#define PIN_PWAIT_OUT( )         { R32_PA_DIR |= PIN_PA_9_WAIT; }              /*PIN_WAIT设置为输出*/
#define PIN_PWR_IN( )            { R32_PA_DIR &= ~PIN_PB_0_WR; }               /*PIN_WR设置为输入*/
#define PIN_PDS_IN( )            { R32_PA_DIR &= ~PIN_PA_11_DS;}               /*PIN_DS设置为输入*/
#define PIN_PD_IN( )             { R32_PD_DIR &= ~(0xFFFF);}                   /*PIN_PD设置为输入*/
#define PIN_PD_OUT( )            { R32_PD_DIR |= 0xFFFF;}                      /*PIN_PD设置为输出*/

UINT8 buf[RECV_NUM];
UINT16 r,s;

/*******************************************************************************
* Function Name  : Delay_100ns
* Description    : 延时函数(时钟为100MHZ时,延时精确)
* Input          : int n;延时时间(n*50)ns
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_100ns( int n )
{
    int i;
    
    while( n -- ) 
    {
        for( i = FREQ_SYS / 50000000; i != 0; -- i );
    }
}
	
/*******************************************************************************
* Function Name  : TIM0_INIT
* Description    : 定时器0初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void TIM0_INIT( void ) 
{
    R8_TMR0_CTRL_MOD =  0x00 ;                                                  /*定时器输出模式设置 */
    R32_TMR0_COUNT = 0x0000000;                                                 /*定时器当前值0*/
    R32_TMR0_CNT_END = 0x186A0;                                                 /*定时器终值10^5，总时间1ms */
    R8_TMR0_INTER_EN |= RB_TMR_IE_CYC_END;

    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开启全局中断 */
    R8_TMR0_INT_FLAG = 0x7f;                                                    /* 清中断 */
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;                                          /* 开启定时器中断 */
}

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    if( R8_INT_FLAG_0 & RB_IF_TMR0 )                                           /* 定时器中断 */
    {                                                                          /*接收下一批数据收发计数清零*/
        r = 0;
        s = 0;
    }
        TIM0_INIT( );
        printf("..");
}

/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void FIQ_Handler( void )
{
    R8_TMR0_CTRL_MOD &= ~RB_TMR_COUNT_EN;                                      /* 关定时器计数使能*/
    PIN_PD_IN( );
    //Delay_100ns(2);
    if(PIN_CH341EPP_DS == 0)
    {
        //PIN_PWAIT_LOW( );                                                    /*拉低PIN_WAIT，保持CH341数据输出*/
        buf[r++] = (UINT8)R32_PD_PIN;                                          /*读取数据*/
        //PIN_PWAIT_HIGH( );                                                   /*拉高PIN_WAIT，允许CH341输出下个数据*/
    }
    while(PIN_CH341EPP_WR == 0)                                                /*PIN_WR为低电平*/
    {
        if(PIN_CH341EPP_DS == 0)                                               /*PIN_DS为低电平*/
        {
            //Delay_100ns(1);                                                  /*根据单片机指令周期选择延时还是使用WAIT信号*/
            PIN_PWAIT_LOW( );                                                  /*拉低PIN_WAIT，保持CH341数据输出*/
            buf[r++] = (UINT8)R32_PD_PIN;                                      /*读取数据*/
            PIN_PWAIT_HIGH( );                                                 /*拉高PIN_WAIT，允许CH341输出下个数据*/
            //Delay_100ns(1);
            if(r >= RECV_NUM)                                                  /*防止数组溢出*/														
            {
                r = 0;
                s = 0;
            }
        }
    }
    R32_TMR0_COUNT = 0x0000000;                                                /* 定时器计数清零，从新计数*/
    R32_INT_STATUS_PB   = 0xfffff;                                             /* 中断标志写1清零 */
    R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;                                        /*定时器计数使能*/
}

/*******************************************************************************
* Function Name  : mInitUART0
* Description    : 为printf和getkey输入输出初始化串口0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void mInitUART0( void )
{
    UINT32    x, x2;

    x = 10 * FREQ_SYS * 2 / 16 / 115200;                                        /* 115200bps */
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                        /* 四舍五入 */
    R8_UART0_LCR = 0x80;                                                        /* DLAB位置1 */
    R8_UART0_DIV = 1;                                                           /* 预分频 */
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;

    R8_UART0_LCR = RB_LCR_WORD_SZ ;                                             /* 设置字节长度为8 */
    R8_UART0_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |    
                   RB_FCR_FIFO_EN ;                                             /* 设置FIFO触发点为14，清发送和接收FIFO，FIFO使能 */
    R8_UART0_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R32_PB_SMT |= RXD0|TXD0;                                                    /* RXD0 schmitt input, TXD0 slow rate */
    R32_PB_PD &= ~ RXD0;                                                        /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD0;                                                         /* TXD0 output enable */
}

/*******************************************************************************
* Function Name  : fputc
* Description    : 通过串口输出监控信息
* Input          : c-- writes the character specified by c 
*                  *f--the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/

int fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                                                           /* 发送数据 */
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* 等待数据发送 */
    return( c );
}

/*******************************************************************************
* Function Name  : FIQ_InitPB
* Description    : 中断配置初始化 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void FIQ_InitPB( void )
{
    R32_PB_PU  |= 0x00000001;                                                   /* GPIO B上拉设置，置1表示上拉 */ 
    R32_PB_DIR &= ~0x00000001;                                                  /* GPIO B方向设置为输入 , direction: 0=in, 1=out */

    R32_INT_ENABLE_PB |= 0x00000001;                                            /* GPIO B组各位中断使能 ： 1-使能，0-禁止 */
    R32_INT_MODE_PB   |= 0x00000001;                                            /* GPIO B组中断方式 ：1-边沿中断,0-电平中断 */      
    R32_INT_POLAR_PB  &= ~0x00000001;                                           /* GPIO B组中断极性 ：1-上升沿中断/高电平，0-下降沿中断/低电平 */      
    
    R32_INT_STATUS_PB   = 0xfffff;                                              /* 中断标志写1清零 */
    R8_INT_EN_FIQ_1    |= RB_IE_FIQ_PB;                                         /* GPIO B组中断使能 */ 
    R8_INT_EN_FIQ_GLOB |= RB_IE_FIQ_GLOB;                                       /* 全局中断使能 */     
}

/*******************************************************************************
* Function Name  : EPP_rev
* Description    : 接收EPP并口模式数据
* Input          : PUINT8 buf;接收数据缓冲区
				   UINT32 i;接收计数
* Output         : None
* Return         : None
*******************************************************************************/
UINT32 EPP_rev(PUINT8 buf,UINT32 i)
{
    while(PIN_CH341EPP_WR == 0)                                                /*PIN_WR为低电平*/
    {
        if(PIN_CH341EPP_DS == 0)                                               /*PIN_DS为低电平*/
        {
            //Delay_100ns(1);                                                  /*根据单片机指令周期选择延时还是使用WAIT信号*/
            PIN_PWAIT_LOW( );                                                  /*拉低PIN_WAIT，保持CH341数据输出*/
            buf[i++] = (UINT8)R32_PD_PIN;                                      /*读取数据*/
            PIN_PWAIT_HIGH( );                                                 /*拉高PIN_WAIT，允许CH341输出下个数据*/
            //Delay_100ns(1);
            if(i >= RECV_NUM)                                                  /*防止数组溢出*/														
            {
                break;
            }
        }
    }
    return i;	
}

/*******************************************************************************
* Function Name  : EPP_send
* Description    : CH563并口发送数据，EPP并口模式接收数据
* Input          : PUINT8 buf;待发送数据缓冲区
				 ：UINT32 i;发送计数
* Output         : None
* Return         : None
*******************************************************************************/
UINT32 EPP_send(PUINT8 buf,UINT32 i)
{                                                                              /*为了保证数据正确有效，如果单片处理速度够快，建议加上下面判断条件*/															
    //while(PIN_CH341EPP_WR)                                                   /*PIN_WR为高电平*/
    //{                                                                        /*CH341WR默认输出高电平*/
        while(PIN_CH341EPP_DS == 0)                                            /*PIN_DS为低电平*/
        {																		
            //Delay_100ns(1);                                                  /*拉低PIN_WAIT，保持CH563数据输出*/
            //PIN_PWAIT_LOW( );                                                /*延时和WAIT信号脚根据单片机的实际情况采用*/
            //Delay_100ns(3);												
            //R32_PD_OUT = buf[i++];
            i++;                                                               /*发送计数*/
            Delay_100ns(1);
            //PIN_PWAIT_HIGH( );                                               /*拉高PIN_WAIT，允许CH563输出下个数据*/
        }
    //}
    return i;
}

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int main( void ) 
{
    UINT8 recv_buf[RECV_NUM];                                                  /*定义演示接收缓冲区*/
    UINT8 send_buf[SEND_NUM];                                                  /*定义演示发送缓冲区*/
    UINT32 i;                                                                  /*接收计数*/
    UINT32 j;                                                                  /*发送计数*/

    Delay_ms( 100 );                                                           /*初始化串口0*/
    mInitUART0( );                                                             /*为了让计算机通过串口监控演示过程*/
    FIQ_InitPB( );                                                             /*中断配置初始化*/
    TIM0_INIT( ); 

    i = 0;
    r = 0;
    s = 0;
    PIN_PWR_IN( );                                                             /*设置PIN_WR为输入*/
    PIN_PD_OUT( );                                                             /*设置PIN_DS为输入*/
    PIN_PD_IN( );                                                              /*设置PIN_PD为输入*/
    PIN_PWAIT_OUT( );                                                          /*设置PIN_WAIT为输出*/
    PIN_PWAIT_HIGH( );                                                         /*初始化WAIT输出高电平*/

    memset(recv_buf,0,RECV_NUM);                                               /*初始化接收缓冲区*/
    memset(buf,0,RECV_NUM);
    for(j = 0;j <SEND_NUM;j++)                                                 /*初始化发送缓冲区*/
    {
        send_buf[j] = j%256;	
    }

    printf("Ready to contact with CH341EPP...\n\n");
    while(1)                                                                   /*通过上位机软件收发小于512字节的数据*/
    {
        PIN_PD_OUT( );
        R32_PD_OUT = buf[s];
        if(PIN_CH341EPP_DS == 0)
        {
            s++;
            s = s%RECV_NUM;
            //Delay_100ns(1);
        }
    }

#if DEBUG
    printf( "Ready to receive 341EPP data...\n" );                             /*通过上位机软件下传RECV_NUM字节的数据*/
    while(1)
    {																			
        i = EPP_rev(recv_buf,i);                                               /*接收数据*/
        if(i >= RECV_NUM)														
        {
            break;
        }
    }
    for(i=0;i<RECV_NUM;i++)	                                                   /*串口输出接收数据*/
    {
        printf("  %X",recv_buf[i]);
    }
#endif

#if DEBUG
    printf("\n");                                                              /*通过上位机软件接收SEND_NUM字节的数据*/
    PIN_PD_OUT( );                                                             /* 设置PIN_PD为输出 */
    printf( "Ready to send data...\n" );
    while(1)
    {                                                                          /*循环发送send_buf中的数据*/
        R32_PD_OUT = j%SEND_NUM;                                               /*发送之前准备好数据*/                                          
        j = EPP_send(send_buf,j);                                              /*发送数据*/
    }
#endif 

}

/*********************************** endfile **********************************/
