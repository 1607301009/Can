/** 用 连续读 和 分页写 模式访问 EEPROM，并且依次加 1 加 2 加 3...最后将结果回写至原地址，此处省略 Lcd1602.c 和 I2C.c */
#include <reg52.h>

extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);
void MemToStr(unsigned char *str, unsigned char *src, unsigned char len);

extern void setDisplayNum(unsigned long Num, unsigned long base);
extern void scanDisplayNum();

unsigned int cnt = 0;   // 定时器 T0 中断次数
unsigned char flag = 0; // 定时 1 秒标志


void main() {
  unsigned char i;
  unsigned char buf[5];
  unsigned char str[20];
    unsigned long sec = 22; // 消耗的秒数

    TMOD = 0x01;            // 设置 T0 为模式 1
    TH0 = 0xFC;
    TL0 = 0x67; // 设置定时值存储寄存器，定时 1ms

    EA = 1;  // 使能总中断
    ET0 = 1; // 使能 T0 中断
    TR0 = 1; // 启动 T0

//    while (1) {
//        /* 判断 1 秒定时标志 */
//        if (flag == 1) {
//            flag = 0; // 定时 1 秒标志清零
//            sec++;    // 秒计数自增 1
//
//            setDisplayNum(sec, 10);
//        }
//    }
  E2Read(buf, 0x8E, sizeof(buf));  // 从 EEPROM 读取一段数据
  MemToStr(str, buf, sizeof(buf)); // 转换为十六进制字符串

  /* 数据依次累加 1, 2, 3... */
  for (i = 0; i < sizeof(buf); i++) {
    buf[i] = buf[i] + 1;
  }
  E2Write(buf, 0x8E, sizeof(buf)); // 将结果写回 EEPROM

    flag = 0; // 定时 1 秒标志清零
//    sec++;    // 秒计数自增 1

    setDisplayNum(buf[4], 10);

  while (1);
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
