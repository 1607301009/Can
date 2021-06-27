/** I2C.c */
#include <intrins.h>
#include <reg52.h>

#define I2CDelay() {_nop_();_nop_();_nop_();_nop_();}
sbit I2C_SCL = P3 ^ 7;
sbit I2C_SDA = P3 ^ 6;

/* 产生总线起始信号 */
void I2CStart() {
    I2C_SDA = 1; // 确保 SDA、SCL 都是高电平
    I2C_SCL = 1;
    I2CDelay();
    I2C_SDA = 0; // 拉低 SDA
    I2CDelay();
    I2C_SCL = 0; // 拉低 SCL
}

/* 产生总线停止信号 */
void I2CStop() {
    I2C_SCL = 0; // 确保 SDA、SCL 都是低电平
    I2C_SDA = 0;
    I2CDelay();
    I2C_SCL = 1; // 拉高 SCL
    I2CDelay();
    I2C_SDA = 1; // 拉高 SDA
    I2CDelay();
}

/* I2C总线写操作，dat-待写入字节，返回值-从机应答位的值 */
bit I2CWrite(unsigned char dat) {
    bit ack;            // 用于暂存响应位的值
    unsigned char mask; // 用于检测字节内某一位值的掩码

    /* 从高位至低位依次执行 */
    for (mask = 0x80; mask != 0; mask >>= 1) {
        /* 将该位的值输出至 SDA */
        if ((mask & dat) == 0)
            I2C_SDA = 0;
        else
            I2C_SDA = 1;

        I2CDelay();
        I2C_SCL = 1;      // 拉高 SCL
        I2CDelay();
        I2C_SCL = 0;      // 拉低 SCL，完成一个位周期
    }

    I2C_SDA = 1;        // 8 位数据发送完毕后，主设备释放 SDA 以检测从设备响应
    I2CDelay();
    I2C_SCL = 1;        // 拉高 SCL
    ack = I2C_SDA;      // 读取此时 SDA 的值，即从设备响应值
    I2CDelay();
    I2C_SCL = 0;        // 再次拉低 SCL 完成响应

    return (~ack);      // 应答值取反以符合日常逻辑，0 表示不存在/忙/写入失败，1 表示存在/空闲/写入成功
}

/* I²C 总线读操作，并且发送 NAK 非应答信号，返回读取到的字节 */
unsigned char I2CReadNAK() {
    unsigned char mask;
    unsigned char dat;
    I2C_SDA = 1;        // 确保主设备释放 SDA

    /* 从高位到低位依次进行 */
    for (mask = 0x80; mask != 0; mask >>= 1) {
        I2CDelay();
        I2C_SCL = 1;      // 拉高 SCL

        /* SDA 的值为 0 时，dat 对应位置清零，为 1 时对应位置 1 */
        if (I2C_SDA == 0)
            dat &= ~mask;
        else
            dat |= mask;

        I2CDelay();
        I2C_SCL = 0;      // 拉低 SCL 让从设备发送下一位
    }

    I2C_SDA = 1;        // 8 位数据发送完毕后拉高 SDA，发送非应答信号
    I2CDelay();
    I2C_SCL = 1;        // 拉高 SCL
    I2CDelay();
    I2C_SCL = 0;        // 拉低 SCL 完成非应答位

    return dat;
}

/* I²C 总线读操作，并且发送 ACK 应答信号，返回读取到的字节 */
unsigned char I2CReadACK() {
    unsigned char mask;
    unsigned char dat;
    I2C_SDA = 1;        // 确保主设备释放 SDA

    /* 从高位到低位依次进行 */
    for (mask = 0x80; mask != 0; mask >>= 1) {
        I2CDelay();
        I2C_SCL = 1;      // 拉高 SCL

        /* SDA 的值为 0 时，dat 对应位置清零，为 1 时对应位置 1 */
        if (I2C_SDA == 0)
            dat &= ~mask;
        else
            dat |= mask;

        I2CDelay();
        I2C_SCL = 0;      // 拉低 SCL 让从设备发送下一位
    }

    I2C_SDA = 0;        // 8 位数据发送完毕后拉高 SDA，发送应答信号
    I2CDelay();
    I2C_SCL = 1;        // 拉高 SCL
    I2CDelay();
    I2C_SCL = 0;        // 拉低 SCL 完成应答位

    return dat;
}
