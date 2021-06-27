/** main.c */
#include <reg52.h>

unsigned char T0RH = 0; // 定时器 T0 重载值的高字节
unsigned char T0RL = 0; // 定时器 T0 重载值的低字节

void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);

void main() {
  EA = 1;           // 总中断使能
  ConfigTimer0(1);  // 配置定时器 T0 定时 1ms
  ConfigUART(9600); // 设置波特率 9600

  while (1) {
    UartDriver();   // 调用串口驱动程序
  }
}

/* 串口动作函数，参数 buf 是接收到的命令帧指针，参数 len 是命令帧的长度 */
void UartAction(unsigned char *buf, unsigned char len) {
  buf[len++] = '\r'; // 在接收到的数据帧后添加回车符
  buf[len++] = '\n'; // 在接收到的数据帧后添加换行符

  UartWrite(buf, len);
}

/* 配置并启动定时器 T0，参数 ms 是定时时间 */
void ConfigTimer0(unsigned int ms) {
  unsigned long tmp; // 临时变量

  tmp = 11059200 / 12;              // 定时器计数频率
  tmp = (tmp * ms) / 1000;          // 计算计数值
  tmp = 65536 - tmp;                // 计算定时器初始值
  tmp = tmp + 33;                   // 补偿由于中断响应延迟造成的误差

  T0RH = (unsigned char)(tmp >> 8); // 拆分定时值为高低位
  T0RL = (unsigned char)tmp;
  TMOD &= 0xF0;                     // 定时器 T0 控制位置 0
  TMOD |= 0x01;                     // 配置定时器 T0 为工作模式 1
  TH0 = T0RH;
  TL0 = T0RL;                       // 定时值存储寄存器赋初值
  ET0 = 1;                          // 定时器 T0 中断使能
  TR0 = 1;                          // 启动定时器 T0
}

/* 定时器 T0 中断服务函数 */
void InterruptTimer0() interrupt 1 {
  TH0 = T0RH;
  TL0 = T0RL;       // 定时值存储寄存器重新赋初值
  UartRxMonitor(1); // 调用串口接收监控函数
}
