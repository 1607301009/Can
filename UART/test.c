#include <stdio.h>

unsigned char *StrAddStr(unsigned char *str1, unsigned char *str2) {
    unsigned char *pt = str1;
    while (*str1 != '\0') str1++;
    while (*str2 != '\0') *str1++ = *str2++;
    *str1 = '\0';
    return pt;
}

void NumAddStr(unsigned char *str, unsigned int num, unsigned char radix) {
    unsigned char tmp;
    unsigned char i = 0;
    unsigned char NewStr[5]={0};

    while (*str != '\0') str++;

    do      //从各位开始变为字符，直到最高位，最后应该反转
    {
        tmp =  num % radix;
        num = num / radix;
        NewStr[i++] = tmp;
    } while (num > 0);
    do      //从各位开始变为字符，直到最高位，最后应该反转
    {
        tmp = NewStr[--i];
        if (tmp <= 9)              // 转换为 0-9 或 A-F
            *str++ = tmp + '0';
        else
            *str++ = tmp - 10 + 'A';
    } while (i > 0);
    *str = '\0';                 // 添加字符串结束符
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
    *str = '\0';                // 添加字符串结束符
}

void UART_send_buffer(unsigned char *ReceiveIDNote, unsigned char len) {
    printf("UART_send_buffer: >>%s<<, len:>>%d<< \n");
}
unsigned char SendIDNote[] = "\r\nSend ID:";
unsigned char ReceiveIDNote[] = "\r\nReceive ID:";

void Send(unsigned long int ID, unsigned char EXIDE, unsigned char DLC, unsigned char *Send_data) {
    //NumAddStr(SendIDNote, ID);
    //StrAddStr(SendIDNote, ReceiveIDNote);
    unsigned char tmp[4] = "0x";
    NumAddStr(tmp, ID, 16);
    Delay_Nms(1000);
    UART_send_buffer(ReceiveIDNote, sizeof(ReceiveIDNote)); //发送一个字符
    Delay_Nms(1000);
    UART_send_buffer(tmp, sizeof(tmp)); //发送一个字符
    Delay_Nms(1000);
    UART_send_buffer(SendIDNote, sizeof(SendIDNote)); //发送一个字符

    //UART_send_buffer(ID, sizeof(ID)); //发送一个字符
    CAN_Send_buffer(ID, EXIDE, DLC, Send_data);
//    SendIDNote = NumAddStr(*SendIDNote, ID)
//    UART_send_buffer(SendIDNote, sizeof(SendIDNote)); //发送一个字符
}

//unsigned char TXB_Value[]={0x0,0x1,0x2,0x3,0x04,0x05,0x06,0x27,0x08,0x09,0x10,0x11,0x12,0x13};
unsigned char RXB_Value[] = {0x0, 0x1, 0x2, 0x3, 0x04, 0x05, 0x06, 0x27};

unsigned char Read_Value[] = {0x0, 0x1, 0x2, 0x3, 0x04, 0x05, 0x06, 0x27};

//unsigned char ini00[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00,0x00,0x00,0x00,0x00,0x00};

void main(void) {
    unsigned int j;

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

        CAN_Receive_Buffer(TXB0CTRL, Read_Value);
//        UART_send_buffer(Read_Value,14); //发送一个字符

        Delay_Nms(1000);  //移动到下一个字符
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
int main()
{
    /* 我的第一个 C 程序 */
    printf("Hello, World! \n");

    return 0;
}
