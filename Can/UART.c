#include <REGX52.H>

#define FOSC   11059200UL		/*单片机使用11.0592M晶体*/	 
#define BAUD   9600UL		    /*波特率定义为9600*/

bit UART_Busy_Flag=0;				//串口1发送数据忙标志位

unsigned char i,j,k;

/*******************************************************************************
* 函数名  : UART_init
* 描述    : UART初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART_init(void)		//串口初始化
{
 EA=0; 						//暂时关闭中断
 TMOD&=0x0F;  				//定时器1模式控制在高4位
 TMOD|=0x20;    			//定时器1工作在模式2，自动重装模式
 SCON=0x50;     			//串口工作在模式1
 TH1=256-FOSC/(BAUD*12*16); //计算定时器重装值
 TL1=256-FOSC/(BAUD*12*16);
 PCON|=0x80;    			//串口波特率加倍
 ES=1;         				//串行中断允许
 TR1=1;        				//启动定时器1
 REN=1;        				//允许接收 
 EA=1;         				//允许中断
}

/*******************************************************************************
* 函数名  : uart
* 描述    : UART中断服务函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void uart(void) interrupt 4		 //串口发送中断
{
 if(RI)    //收到数据
 {
  RI=0;   //清中断请求
 }
 else      //发送完一字节数据
 {
  TI=0;
  UART_Busy_Flag=0;  //清0正在发送标志
 }
}

/*******************************************************************************
* 函数名  : UART_send_str
* 描述    : UART发送一个字节
* 输入    : d：待发送数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART_send_str(unsigned char d)		  //发送一个字节的数据，形参d即为待发送数据。
{
 SBUF=d; //将数据写入到串口缓冲
 UART_Busy_Flag=1;	 //设置发送标志
 while(UART_Busy_Flag); //等待发送完毕
}

/*******************************************************************************
* 函数名  : UART_send_buffer
* 描述    : UART发送一个缓冲区数据
* 输入    : *buff：待发送缓冲区首地址,len：待发送数据长度
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART_send_buffer(unsigned char *buffer,unsigned int len)
{
 for(j=len;j>0;j--) //发送字符串，直到遇到0才结束
 {
  UART_send_str(*buffer); //发送一个字符
  buffer++;  //移动到下一个字符
 }
}