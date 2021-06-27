#include <reg52.h>

extern void setDisplayNum(unsigned long Num, unsigned long base);
extern void scanDisplayNum();

unsigned int cnt = 0;   // 定时器 T0 中断次数
unsigned char flag = 0; // 定时 1 秒标志


void main() {
    unsigned long sec = 2; // 消耗的秒数

    TMOD = 0x01;            // 设置 T0 为模式 1
    TH0 = 0xFC;
    TL0 = 0x67; // 设置定时值存储寄存器，定时 1ms

    EA = 1;  // 使能总中断
    ET0 = 1; // 使能 T0 中断
    TR0 = 1; // 启动 T0

    while (1) {
        /* 判断 1 秒定时标志 */
        if (flag == 1) {
            flag = 0; // 定时 1 秒标志清零
            sec++;    // 秒计数自增 1

            setDisplayNum(sec, 10);
        }
    }
}

/* 定时器 T0 中断服务函数 */
void InterruptTimer0()

interrupt 1 {
TH0 = 0xFC;
TL0 = 0x67; // 定时值存储寄存器重新赋初值
cnt++;                  // 中断次数计数值自增 1

/* 判断 T0 溢出是否达到 1000 次 */
if (cnt >= 1000) {
cnt = 0;  // 清零计数值
flag = 1; // 定时 1 秒标志置 1
}

scanDisplayNum();
}
