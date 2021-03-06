/**********************51单片机学习例程************************
*  平台:Keil U4 + STC89C52
*  名称:UART口传输一个字符 A
*  编写：起航
*  晶体:11.0592MHZ
******************************************************************/
#include <REG52.H>
#include <STC89C5xRC.H>

#define FOSC   11059200UL        /*单片机使用11.0592M晶体*/
#define BAUD   9600UL            /*波特率定义为9600*/

bit UART_Busy_Flag = 0;                //串口1发送数据忙标志位

unsigned char i, j, k;

/*******************************************************************************
* 函数名  : UART_init
* 描述    : UART初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART_init(void)        //串口初始化
{
    EA = 0;                        //暂时关闭中断
    TMOD &= 0x0F;                //定时器1模式控制在高4位
    TMOD |= 0x20;                //定时器1工作在模式2，自动重装模式
    SCON = 0x50;                //串口工作在模式1
    TH1 = 256 - FOSC / (BAUD * 12 * 16); //计算定时器重装值
    TL1 = 256 - FOSC / (BAUD * 12 * 16);
    PCON |= 0x80;                //串口波特率加倍
    ES = 1;                        //串行中断允许
    TR1 = 1;                        //启动定时器1
    REN = 1;                        //允许接收
    EA = 1;                        //允许中断
}

/*******************************************************************************
* 函数名  : uart
* 描述    : UART中断服务函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void uart(void)

interrupt 4         //串口发送中断
{
if(RI)    //收到数据
{
RI = 0;   //清中断请求
}
else      //发送完一字节数据
{
TI = 0;
UART_Busy_Flag = 0;  //清0正在发送标志
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
void UART_send_str(unsigned char d)          //发送一个字节的数据，形参d即为待发送数据。
{
    SBUF = d; //将数据写入到串口缓冲
    UART_Busy_Flag = 1;     //设置发送标志
    while (UART_Busy_Flag); //等待发送完毕
}

/*******************************************************************************
* 函数名  : UART_send_buffer
* 描述    : UART发送一个缓冲区数据
* 输入    : *buff：待发送缓冲区首地址,len：待发送数据长度
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART_send_buffer(unsigned char *buffer, unsigned int len) {
    for (j = len; j > 0; j--) //发送字符串，直到遇到0才结束
    {
        UART_send_str(*buffer); //发送一个字符
        buffer++;  //移动到下一个字符
    }
}

void delay(unsigned char i) {
    unsigned char j, k;
    for (j = i; j > 0; j--)
        for (k = 90; k > 0; k--);
}


/*******************************************************************************
* 函数名  : StrAddStr
* 描述    : 将str2添加到str1后面
* 输入    : *str：待拼接字符串首地址
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
unsigned char *StrAddStr(unsigned char *str1, unsigned char *str2) {
    unsigned char *pt = str1;
    while (*str1 != '\0') str1++;
    while (*str2 != '\0') *str1++ = *str2++;
    *str1 = '\0';
    return pt;
}

unsigned char zifuchuan[30] = "  A1C9";
unsigned char zifuchuan1[] = " BB";
unsigned char num = 78;


/*******************************************************************************
* 函数名  : NumAddStr
* 描述    : 将数字添加到字符串后面
* 输入    : *str：待拼接字符串首地址, num：待拼接数字
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void NumAddStr(unsigned char *str, unsigned char num) {
    do      //从各位开始变为字符，直到最高位，最后应该反转
    {
        *str++ = num % 10 + '0';
        num = num / 10;
    } while (num > 0);
    *str = '\0';                 // 添加字符串结束符
}


int main() {
    UART_init();
    while (1) {
//        *zifuchuan1 = num + '0';
//        *zifuchuan1 = '\0';
        MemToStr(zifuchuan1, num);
//        my_strcat(zifuchuan1, itoa(num));    //拼接两个字符串，结果保存在第一个字符串当中
        StrAddStr(zifuchuan, zifuchuan1);    //拼接两个字符串，结果保存在第一个字符串当中
        delay(200);
        UART_send_buffer(zifuchuan, sizeof(zifuchuan));

        delay(200);

    }
    return (0);
}