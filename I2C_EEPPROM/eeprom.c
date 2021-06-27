/** eeprom.c */
#include <reg52.h>

extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);

/* EEPROM 读取函数，参数 buf 是数据接收指针，参数 addr 是 EEPROM 中起始地址，参数 len 是待读取的长度 */
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len) {
  /* 采用寻址操作查询当前是否可以进行读写操作 */
  do {
    I2CStart();
    /* 应答就跳出循环，非应答就进行下次查询 */
    if (I2CWrite(0x50 << 1)) {
      break;
    }
    I2CStop();
  } while (1);

  I2CWrite(addr);               // 写入起始地址
  I2CStart();                   // 发送重复启动信号
  I2CWrite((0x50 << 1) | 0x01); // 设备寻址，并设置后续为读操作

  /* 连续读取 len-1 个字节的数据 */
  while (len > 1) {
    *buf++ = I2CReadACK(); // 最后 1 个字节之前为读取操作 + 应答
    len--;
  }

  *buf = I2CReadNAK(); // 最后 1 个字节为读取操作 + 非应答
  I2CStop();
}

/* EEPROM 写入函数，参数 buf 是源数据指针，参数 addr 是 EEPROM 的起始地址，参数 len 是待写入的数据长度 */
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len) {
  /* 等待上次写入操作完成 */
  while (len > 0) {
    /* 采用寻址操作查询当前是否可以进行读写操作 */
    do {
      I2CStart();

      /* 应答就跳出循环，非应答就进行下次查询 */
      if (I2CWrite(0x50 << 1)) {
        break;
      }

      I2CStop();
    } while (1);

    /* 采用页模式连续的写入多个字节 */
    I2CWrite(addr); // 写入起始地址

    while (len > 0) {
      I2CWrite(*buf++); // 写入一个字节数据
      len--;            // 待写入长度计数递减
      addr++;           // EEPROM 地址递增

      /* 通过检测低 3 位是否为零来判断检查地址是否到达了页边界 */
      if ((addr & 0x07) == 0) {
        break; // 如果到达了页边界，就跳出循环结束本次写操作
      }
    }
    I2CStop();
  }
}
