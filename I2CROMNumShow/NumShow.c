#include <reg52.h>

sbit dula = P2 ^6;        //段选信号的锁存器控制
sbit wela = P2 ^7;        //位选信号的锁存器控制

/* 数码管编码表 */
unsigned char code
LedChar[] = {
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71, 0x00};

/* 动态数码管显示缓冲区*/
unsigned char LedBuff[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char code
table[]={
0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};  //数码管各位的码表

unsigned char i = 0;    // 动态扫描索引

void setDisplayNum(unsigned long Num, unsigned long base) //显示程序
{
    /* 将 Num 按照base进制由低至高，提取为数码管将要显示字符的编码 */
    unsigned long pow2 = base * base;
    unsigned long pow3 = base * base * base;
    unsigned long pow4 = base * base * base * base;
    unsigned long pow5 = base * base * base * base * base;

    LedBuff[0] = LedChar[Num % base];
    if (Num >= base) { LedBuff[1] = LedChar[Num / base % base]; } else { LedBuff[1] = 0x00; }
    if (Num >= pow2) { LedBuff[2] = LedChar[Num / pow2 % base]; } else { LedBuff[2] = 0x00; }
    if (Num >= pow3) { LedBuff[3] = LedChar[Num / pow3 % base]; } else { LedBuff[3] = 0x00; }
    if (Num >= pow4) { LedBuff[4] = LedChar[Num / pow4 % base]; } else { LedBuff[4] = 0x00; }
    if (Num >= pow5) { LedBuff[5] = LedChar[Num / pow5 % base]; } else { LedBuff[5] = 0x00; }
}

void scanDisplayNum() //显示程序
{
    P0 = 0xFF;  // 消除残影

/* 数码管动态扫描刷新 */
    P0 = table[i];
    wela = 1;
    wela = 0;
    P0 = LedBuff[i];
    dula = 1;
    dula = 0;

    switch (i) {
        case 5:
            P0 = table[i];
            wela = 1;
            wela = 0;
            P0 = LedBuff[i];
            dula = 1;
            dula = 0;
            i = 0;
            break;
        default:
            P0 = table[i];
            wela = 1;
            wela = 0;
            P0 = LedBuff[i];
            dula = 1;
            dula = 0;
            i++; break;;
    }
}
