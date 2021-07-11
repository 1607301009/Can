#include <reg52.h>
#include "MCP2515.H"

bit CAN_MERRF_Flag = 0;                            //CAN报文错误中断标志位
bit CAN_WAKIF_Flag = 0;                            //CAN唤醒中断标志位
bit CAN_ERRIF_Flag = 0;                            //CAN错误中断标志位（EFLG 寄存器中有多个中断源）
bit CAN_TX2IF_Flag = 0;                            //MCP2515发送缓冲器2 空中断标志位
bit CAN_TX1IF_Flag = 0;                            //MCP2515发送缓冲器1 空中断标志位
bit CAN_TX0IF_Flag = 0;                            //MCP2515发送缓冲器0 空中断标志位
bit CAN_RX1IF_Flag = 0;                            //MCP2515接收缓冲器1 满中断标志位
bit CAN_RX0IF_Flag = 0;                            //MCP2515接收缓冲器0 满中断标志位

unsigned char CAN_R_Buffer[8];                        //CAN接收数据保存缓冲区

//MCP2515波特率预分频	要考虑FOSC=8M BRP=0..64 PHSEG1=3..16 PGSEG2=2..8 SJW=1..4
#define    CAN_5Kbps    0x31    //TQ=12500 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define    CAN_10Kbps    0x13    //TQ=6250 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define CAN_25Kbps    0x09    //TQ=2500 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define CAN_50Kbps    0x04    //TQ=1250 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define CAN_100Kbps    0x01    //TQ=500 sample-point=0.850 PRSEG_8TQ PHSEG1_8TQ PHSEG2_3TQ SJW_1TQ
#define CAN_125Kbps    0x01    //TQ=500 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define CAN_250Kbps    0x00    //TQ=250 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define    CAN_500Kbps    0x00    //TQ=250 sample-point=0.750 PRSEG_2TQ PHSEG1_3TQ PHSEG2_2TQ SJW_1TQ

//MCP2515波特率	要考虑FOSC=8M BRP=0..64 PRSEG=1..8 PHSEG1=3..16 PHSEG2=2..8 SJW=1..4 
unsigned char code
bitrate_5Kbps[]={
CAN_5Kbps,PRSEG_6TQ,PHSEG1_7TQ,PHSEG2_2TQ,SJW_1TQ};
unsigned char code
bitrate_10Kbps[]={
CAN_10Kbps,PRSEG_6TQ,PHSEG1_7TQ,PHSEG2_2TQ,SJW_1TQ};
unsigned char code
bitrate_25Kbps[]={
CAN_25Kbps,PRSEG_6TQ,PHSEG1_7TQ,PHSEG2_2TQ,SJW_1TQ};
unsigned char code
bitrate_50Kbps[]={
CAN_50Kbps,PRSEG_6TQ,PHSEG1_7TQ,PHSEG2_2TQ,SJW_1TQ};
unsigned char code
bitrate_100Kbps[]={
CAN_100Kbps,PRSEG_8TQ,PHSEG1_8TQ,PHSEG2_3TQ,SJW_1TQ};
unsigned char code
bitrate_125Kbps[]={
CAN_125Kbps,PRSEG_6TQ,PHSEG1_7TQ,PHSEG2_2TQ,SJW_1TQ};
unsigned char code
bitrate_250Kbps[]={
CAN_250Kbps,PRSEG_6TQ,PHSEG1_7TQ,PHSEG2_2TQ,SJW_1TQ};
unsigned char code
bitrate_500Kbps[]={
CAN_500Kbps,PRSEG_2TQ,PHSEG1_3TQ,PHSEG2_2TQ,SJW_1TQ};

/*******************************************************************************
* 函数名  : Exint_Init
* 描述    : 外部中断1初始化函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Exint_Init(void) {
    PX1 = 1;        //设置外部中断1的中断优先级为高优先级
    IT1 = 1;    //设置INT1的中断类型 (1:仅下降沿 0:上升沿和下降沿)
    EX1 = 1;    //使能INT1中断
    EA = 1;    //使能总中断
}

/*******************************************************************************
* 函数名  : Exint_ISR
* 描述    : 外部中断1中断服务函数 单片机引脚P3.3接MCP2515 INT引脚
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 用于检测MCP2515中断引脚的中断信号
*******************************************************************************/
sbit P1_0 = P1 ^0;

void Exint_ISR(void)

interrupt 2 using 1
{
unsigned char Flag;                                //CAN接收到数据标志
Flag = MCP2515_ReadByte(CANINTF);
//P1_0=!P1_0;

if (Flag&0x80)
CAN_MERRF_Flag = 1;                            //CAN报文错误中断标志位
if (Flag&0x40)
CAN_WAKIF_Flag = 1;                            //CAN唤醒中断标志位
if (Flag&0x20)
CAN_ERRIF_Flag = 1;                            //CAN错误中断标志位（EFLG 寄存器中有多个中断源）
if (Flag&0x10)
CAN_TX2IF_Flag = 1;                            //MCP2515发送缓冲器2 空中断标志位
if (Flag&0x08)
CAN_TX1IF_Flag = 1;                            //MCP2515发送缓冲器1 空中断标志位
if (Flag&0x04)
CAN_TX0IF_Flag = 1;                            //MCP2515发送缓冲器0 空中断标志位
if (Flag&0x02)
//P1_0=!P1_0;
CAN_RX1IF_Flag = 1;                            //MCP2515接收缓冲器1 满中断标志位
if (Flag&0x01)
P1_0 = !P1_0;
CAN_RX0IF_Flag = 1;                            //MCP2515接收缓冲器0 满中断标志位
}

/* 将一段内存数据转换为十六进制字符串，参数 str 是字符串指针，参数 src 是源数据地址，参数 len 是数据长度 */
void MemToStr(unsigned char *str, unsigned char *src, unsigned char len) {
  unsigned char tmp;
  while (len--) {
    tmp = *src >> 4;           // 取出高 4 位
    if (tmp <= 9)              // 转换为 0-9 或 A-F
      *str++ = tmp + '0';
    else
      *str++ = tmp - 10 + 'A';
    tmp = *src & 0x0F;         // 取出低 4 位
    if (tmp <= 9)              // 转换为 0-9 或 A-F
      *str++ = tmp + '0';
    else
      *str++ = tmp - 10 + 'A';
    *str++ = ' ';              // 转换完 1 个字节就添加 1 个空格
    src++;
  }
  *str = '\0';                 // 添加字符串结束符
}


unsigned char NoteSend[] = "Send:";
unsigned char NoteReceive[] = "Receive:";
unsigned char NoteID[] = " ID:";
unsigned char NoteData[] = " Data:";
unsigned char Enter[3] = "\r\n";

void Send(unsigned int ID, unsigned char EXIDE, unsigned char DLC, unsigned char *Send_data) {

//    UART_send_buffer(Enter, sizeof(Enter));
//
//    UART_send_buffer(NoteSend, sizeof(NoteSend));
//    UART_send_buffer(NoteID, sizeof(NoteID));
//    UART_send_str("F");
//    UART_send_buffer(Enter, sizeof(Enter));
//
//    UART_send_buffer(NoteSend, strlen(NoteSend));
//    UART_send_buffer(NoteData, sizeof(NoteData));
    unsigned char str[5];
    UART_send_buffer(str, 23);
    UART_send_buffer(Enter, sizeof(Enter));
    Delay_Nms(1000);
//    UART_send_buffer(SendIDNote, sizeof(SendIDNote)); //发送一个字符

    //UART_send_buffer(ID, sizeof(ID)); //发送一个字符
    CAN_Send_buffer(ID, EXIDE, DLC, Send_data);
//    SendIDNote = NumAddStr(*SendIDNote, ID)
//    UART_send_buffer(SendIDNote, sizeof(SendIDNote)); //发送一个字符
}

/*******************************************************************************
* 函数名  : main
* 描述    : 主函数，用户程序从main函数开始运行
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
//unsigned char TXB_Value[]={0x0,0x1,0x2,0x3,0x04,0x05,0x06,0x27,0x08,0x09,0x10,0x11,0x12,0x13};
unsigned char RXB_Value[] = {0x0, 0x1, 0x2, 0x3, 0x04, 0x05, 0x06, 0x27};

unsigned char Read_Value[] = {0x0, 0x1, 0x2, 0x3, 0x04, 0x05, 0x06, 0x27};

//unsigned char ini00[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00,0x00,0x00,0x00,0x00,0x00};

void main(void) {
    unsigned int j;
//
    unsigned long int ID = 0x7FD;
    unsigned char EXIDE = 0;
//	unsigned char DLC=8;
    unsigned char Send_data[] = {0x20, 0xF1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
//    unsigned char zifuchuan1[] = " aaaB22ss21d";

    //unsigned char rd[] = "\\r\\d";
    UART_init();    //UART1初始化配置
    Exint_Init();            //外部中断1初始化函数


    MCP2515_Init(bitrate_100Kbps);

    for (j = 0; j < 2; j++) //发送字符串，直到遇到0才结束
    {
//        UART_send_buffer(SendIDNote, sizeof(SendIDNote)); //发送一个字符
        //UART_send_buffer(rd, sizeof(rd)); //发送一个字符
        Send(ID, EXIDE, sizeof(Send_data), Send_data);
//        CAN_Send_buffer(ID,EXIDE,DLC,Send_data);
        ID++;
        EXIDE = !EXIDE;
//        DLC--;
        Delay_Nms(1000);

//        CAN_Receive_Buffer(TXB0CTRL, Read_Value);
//        UART_send_buffer(Read_Value,14); //发送一个字符

//        Delay_Nms(1000);  //移动到下一个字符
    }

    Delay_Nms(2000);
//	ini00[0]=CAN_Flag;
//	ini00[1]=P3;
    while (1) {

        if (CAN_RX0IF_Flag == 1)                            //接收缓冲器0 满中断标志位
        {
            CAN_RX0IF_Flag = 0;//CAN接收到数据标志
            if (MCP2515_ReadByte(MCP2515_ReadByte) & 0x05)//RXB0滚存到了RXB1
                CAN_Receive_Buffer(RXB1CTRL, RXB_Value);//CAN接收一帧数据
            else
                CAN_Receive_Buffer(RXB0CTRL, RXB_Value);//CAN接收一帧数据
            UART_send_buffer(RXB_Value, 14); //发送一个字符
            Delay_Nms(2000);  //移动到下一个字符
//			UART_send_buffer(RXB_Value,14); //发送一个字符

//			CAN_Receive_Buffer(RXB1CTRL,RXB_Value);//CAN接收一帧数据
//			UART_send_buffer(RXB_Value,14); //发送一个字符
//			Delay_Nms(2000);  //移动到下一个字符
//			UART_send_buffer(RXB_Value,14); //发送一个字符
        }
        if (CAN_RX1IF_Flag == 1)                            //接收缓冲器1 满中断标志位
        {
            CAN_RX1IF_Flag = 0;//CAN接收到数据标志
            CAN_Receive_Buffer(RXB1CTRL, RXB_Value);//CAN接收一帧数据
            UART_send_buffer(RXB_Value, 14); //发送一个字符
            Delay_Nms(2000);  //移动到下一个字符
//			UART_send_buffer(RXB_Value,14); //发送一个字符
        }

        Delay_Nms(2000);
    }

}