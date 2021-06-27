/** RS485.c */
#include <intrins.h>
#include <reg52.h>

sbit RS485_DIR = P1 ^ 7;        // RS485方向选择引脚

bit flagFrame = 0;              // 数据帧接收完成标志位，表示接收到了一帧新数据
bit flagTxd = 0;                // 单字节发送完成标志位，用来替代 TXD 中断标志位

unsigned char cntRxd = 0;       // 接收字节计数器
unsigned char pdata bufRxd[64]; // 接收字节缓冲区

extern void UartAction(unsigned char *buf, unsigned char len);

/* 串口配置函数，baud-通信波特率 */
void ConfigUART(unsigned int baud) {
  RS485_DIR = 0;                           // 将 RS485 设置为接收

  SCON = 0x50;                             // 配置串口为工作方式 1
  TMOD &= 0x0F;                            // 清零定时器 T1 控制位
  TMOD |= 0x20;                            // 配置定时器 T1 为工作模式 2
  TH1 = 256 - (11059200 / 12 / 32) / baud; // 计算定时器 T1 初始值
  TL1 = TH1;                               // 将高位定时值作为低位的初值
  ET1 = 0;                                 // 禁用定时器 T1 中断
  ES = 1;                                  // 串口中断使能
  TR1 = 1;                                 // 启动定时器 T1
}

/* 软件延时函数，延时 t * 10 微秒的时间 */
void DelayX10us(unsigned char t) {
  do {
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
  } while (--t);
}

/* 串口数据写入函数，参数 buf 是等待发送数据的指针，参数 len 是发送的长度 */
void UartWrite(unsigned char *buf, unsigned char len) {
  RS485_DIR = 1;       // 将 RS485 设置为发送

  /* 循环发送所有的字节 */
  while (len--) {
    flagTxd = 0;       // 发送完成标志位清零
    SBUF = *buf++;     // 发送 1 个字节的数据
    while (!flagTxd) ; // 等待该字节发送完成
  }

  DelayX10us(5);       // 等待最后的停止位完成，延时时间由波特率决定
  RS485_DIR = 0;       // 将 RS485 设置为接收
}

/* 串口数据读取函数，参数 buf 表示接收指针的指针，参数 len 表示读取的长度，返回值是实际读取到的长度 */
unsigned char UartRead(unsigned char *buf, unsigned char len) {
  unsigned char i;
  /* 当指定的读取长度大于实际接收到的数据长度时，将读取长度设置为实际接收到的数据长度 */
  if (len > cntRxd) {
    len = cntRxd;
  }

  /* 将接收到的数据拷贝至接收指针 */
  for (i = 0; i < len; i++) {
    *buf++ = bufRxd[i];
  }

  cntRxd = 0; // 接收计数器清零
  return len; // 返回实际读取的长度
}

/* 串口接收监控函数，基于空闲时间来判定帧结束，在定时中断中调用，参数 ms 表示定时间隔 */
void UartRxMonitor(unsigned char ms) {
  static unsigned char cntbkp = 0;  // 接收计数变量
  static unsigned char idletmr = 0; // 总线空闲计时变量

  /* 当接收计数变量大于零时，监听总线的空闲时间 */
  if (cntRxd > 0) {
    /* 如果接收计数变量发生改变（即刚接收到数据时），空闲计时变量清零 */
    if (cntbkp != cntRxd) {
      cntbkp = cntRxd;
      idletmr = 0;
    }
    /* 如果接收计数变量没有改变（即总线保持空闲时），将空闲时间进行累加 */
    else {
      /* 总线空闲计时小于 30ms 持续累加 */
      if (idletmr < 30) {
        idletmr += ms;
        /* 总线空闲计时达到 30ms，就认为一帧数据接收完毕 */
        if (idletmr >= 30) {
          flagFrame = 1;            // 设置数据帧接收完成标志位
        }
      }
    }
  } else {
    cntbkp = 0;
  }
}

/* 串口驱动函数，用于监听数据帧的接收并调用相应功能，主函数循环中进行调用 */
void UartDriver() {
  unsigned char len;
  unsigned char pdata buf[40];
  /* 当数据帧到达时，读取并处理该命令 */
  if (flagFrame) {
    flagFrame = 0;

    len = UartRead(buf, sizeof(buf) - 2); // 将接收到的命令读取至缓冲区
    UartAction(buf, len);                 // 传递数据帧，调用动作执行函数
  }
}

/* 串口中断服务函数 */
void InterruptUART() interrupt 4 {
  /* 接收到新的字节数据 */
  if (RI) {
    RI = 0;      // 接收中断标志位清零

    /* 接收缓冲区尚未用完时，保存接收字节，并递增计数器 */
    if (cntRxd < sizeof(bufRxd)) {
      bufRxd[cntRxd++] = SBUF;
    }
  }
  /* 字节发送完毕 */
  if (TI) {
    TI = 0;      // 发送中断标志位清零
    flagTxd = 1; // 发送完成标志位置 1
  }
}
