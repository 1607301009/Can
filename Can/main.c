#include <reg52.h>
#include "MCP2515.H"

bit CAN_MERRF_Flag = 0;                            //CAN���Ĵ����жϱ�־λ
bit CAN_WAKIF_Flag = 0;                            //CAN�����жϱ�־λ
bit CAN_ERRIF_Flag = 0;                            //CAN�����жϱ�־λ��EFLG �Ĵ������ж���ж�Դ��
bit CAN_TX2IF_Flag = 0;                            //MCP2515���ͻ�����2 ���жϱ�־λ
bit CAN_TX1IF_Flag = 0;                            //MCP2515���ͻ�����1 ���жϱ�־λ
bit CAN_TX0IF_Flag = 0;                            //MCP2515���ͻ�����0 ���жϱ�־λ
bit CAN_RX1IF_Flag = 0;                            //MCP2515���ջ�����1 ���жϱ�־λ
bit CAN_RX0IF_Flag = 0;                            //MCP2515���ջ�����0 ���жϱ�־λ

unsigned char CAN_R_Buffer[8];                        //CAN�������ݱ��滺����

//MCP2515������Ԥ��Ƶ	Ҫ����FOSC=8M BRP=0..64 PHSEG1=3..16 PGSEG2=2..8 SJW=1..4
#define    CAN_5Kbps    0x31    //TQ=12500 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define    CAN_10Kbps    0x13    //TQ=6250 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define CAN_25Kbps    0x09    //TQ=2500 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define CAN_50Kbps    0x04    //TQ=1250 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define CAN_100Kbps    0x01    //TQ=500 sample-point=0.850 PRSEG_8TQ PHSEG1_8TQ PHSEG2_3TQ SJW_1TQ
#define CAN_125Kbps    0x01    //TQ=500 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define CAN_250Kbps    0x00    //TQ=250 sample-point=0.875 PRSEG_6TQ PHSEG1_7TQ PHSEG2_2TQ SJW_1TQ
#define    CAN_500Kbps    0x00    //TQ=250 sample-point=0.750 PRSEG_2TQ PHSEG1_3TQ PHSEG2_2TQ SJW_1TQ

//MCP2515������	Ҫ����FOSC=8M BRP=0..64 PRSEG=1..8 PHSEG1=3..16 PHSEG2=2..8 SJW=1..4 
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
* ������  : Exint_Init
* ����    : �ⲿ�ж�1��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Exint_Init(void) {
    PX1 = 1;        //�����ⲿ�ж�1���ж����ȼ�Ϊ�����ȼ�
    IT1 = 1;    //����INT1���ж����� (1:���½��� 0:�����غ��½���)
    EX1 = 1;    //ʹ��INT1�ж�
    EA = 1;    //ʹ�����ж�
}

/*******************************************************************************
* ������  : Exint_ISR
* ����    : �ⲿ�ж�1�жϷ����� ��Ƭ������P3.3��MCP2515 INT����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ڼ��MCP2515�ж����ŵ��ж��ź�
*******************************************************************************/
sbit P1_0 = P1 ^0;

void Exint_ISR(void)

interrupt 2 using 1
{
unsigned char Flag;                                //CAN���յ����ݱ�־
Flag = MCP2515_ReadByte(CANINTF);
//P1_0=!P1_0;

if (Flag&0x80)
CAN_MERRF_Flag = 1;                            //CAN���Ĵ����жϱ�־λ
if (Flag&0x40)
CAN_WAKIF_Flag = 1;                            //CAN�����жϱ�־λ
if (Flag&0x20)
CAN_ERRIF_Flag = 1;                            //CAN�����жϱ�־λ��EFLG �Ĵ������ж���ж�Դ��
if (Flag&0x10)
CAN_TX2IF_Flag = 1;                            //MCP2515���ͻ�����2 ���жϱ�־λ
if (Flag&0x08)
CAN_TX1IF_Flag = 1;                            //MCP2515���ͻ�����1 ���жϱ�־λ
if (Flag&0x04)
CAN_TX0IF_Flag = 1;                            //MCP2515���ͻ�����0 ���жϱ�־λ
if (Flag&0x02)
//P1_0=!P1_0;
CAN_RX1IF_Flag = 1;                            //MCP2515���ջ�����1 ���жϱ�־λ
if (Flag&0x01)
P1_0 = !P1_0;
CAN_RX0IF_Flag = 1;                            //MCP2515���ջ�����0 ���жϱ�־λ
}

/* ��һ���ڴ�����ת��Ϊʮ�������ַ��������� str ���ַ���ָ�룬���� src ��Դ���ݵ�ַ������ len �����ݳ��� */
void MemToStr(unsigned char *str, unsigned char *src, unsigned char len) {
  unsigned char tmp;
  while (len--) {
    tmp = *src >> 4;           // ȡ���� 4 λ
    if (tmp <= 9)              // ת��Ϊ 0-9 �� A-F
      *str++ = tmp + '0';
    else
      *str++ = tmp - 10 + 'A';
    tmp = *src & 0x0F;         // ȡ���� 4 λ
    if (tmp <= 9)              // ת��Ϊ 0-9 �� A-F
      *str++ = tmp + '0';
    else
      *str++ = tmp - 10 + 'A';
    *str++ = ' ';              // ת���� 1 ���ֽھ���� 1 ���ո�
    src++;
  }
  *str = '\0';                 // ����ַ���������
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
//    UART_send_buffer(SendIDNote, sizeof(SendIDNote)); //����һ���ַ�

    //UART_send_buffer(ID, sizeof(ID)); //����һ���ַ�
    CAN_Send_buffer(ID, EXIDE, DLC, Send_data);
//    SendIDNote = NumAddStr(*SendIDNote, ID)
//    UART_send_buffer(SendIDNote, sizeof(SendIDNote)); //����һ���ַ�
}

/*******************************************************************************
* ������  : main
* ����    : ���������û������main������ʼ����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
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
    UART_init();    //UART1��ʼ������
    Exint_Init();            //�ⲿ�ж�1��ʼ������


    MCP2515_Init(bitrate_100Kbps);

    for (j = 0; j < 2; j++) //�����ַ�����ֱ������0�Ž���
    {
//        UART_send_buffer(SendIDNote, sizeof(SendIDNote)); //����һ���ַ�
        //UART_send_buffer(rd, sizeof(rd)); //����һ���ַ�
        Send(ID, EXIDE, sizeof(Send_data), Send_data);
//        CAN_Send_buffer(ID,EXIDE,DLC,Send_data);
        ID++;
        EXIDE = !EXIDE;
//        DLC--;
        Delay_Nms(1000);

//        CAN_Receive_Buffer(TXB0CTRL, Read_Value);
//        UART_send_buffer(Read_Value,14); //����һ���ַ�

//        Delay_Nms(1000);  //�ƶ�����һ���ַ�
    }

    Delay_Nms(2000);
//	ini00[0]=CAN_Flag;
//	ini00[1]=P3;
    while (1) {

        if (CAN_RX0IF_Flag == 1)                            //���ջ�����0 ���жϱ�־λ
        {
            CAN_RX0IF_Flag = 0;//CAN���յ����ݱ�־
            if (MCP2515_ReadByte(MCP2515_ReadByte) & 0x05)//RXB0���浽��RXB1
                CAN_Receive_Buffer(RXB1CTRL, RXB_Value);//CAN����һ֡����
            else
                CAN_Receive_Buffer(RXB0CTRL, RXB_Value);//CAN����һ֡����
            UART_send_buffer(RXB_Value, 14); //����һ���ַ�
            Delay_Nms(2000);  //�ƶ�����һ���ַ�
//			UART_send_buffer(RXB_Value,14); //����һ���ַ�

//			CAN_Receive_Buffer(RXB1CTRL,RXB_Value);//CAN����һ֡����
//			UART_send_buffer(RXB_Value,14); //����һ���ַ�
//			Delay_Nms(2000);  //�ƶ�����һ���ַ�
//			UART_send_buffer(RXB_Value,14); //����һ���ַ�
        }
        if (CAN_RX1IF_Flag == 1)                            //���ջ�����1 ���жϱ�־λ
        {
            CAN_RX1IF_Flag = 0;//CAN���յ����ݱ�־
            CAN_Receive_Buffer(RXB1CTRL, RXB_Value);//CAN����һ֡����
            UART_send_buffer(RXB_Value, 14); //����һ���ַ�
            Delay_Nms(2000);  //�ƶ�����һ���ַ�
//			UART_send_buffer(RXB_Value,14); //����һ���ַ�
        }

        Delay_Nms(2000);
    }

}