/** 用 连续读 和 分页写 模式访问 EEPROM，并且依次加 1 加 2 加 3...最后将结果回写至原地址，此处省略 Lcd1602.c 和 I2C.c */
#include <reg52.h>

extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);
void MemToStr(unsigned char *str, unsigned char *src, unsigned char len);

void main() {
  unsigned char i;
  unsigned char buf[5];
  unsigned char str[20];

  E2Read(buf, 0x8E, sizeof(buf));  // 从 EEPROM 读取一段数据
  MemToStr(str, buf, sizeof(buf)); // 转换为十六进制字符串

  /* 数据依次累加 1, 2, 3... */
  for (i = 0; i < sizeof(buf); i++) {
    buf[i] = buf[i] + 1 + i;
  }
  E2Write(buf, 0x8E, sizeof(buf)); // 将结果写回 EEPROM
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
