#include "STC89C5xRC.H"
#include <intrins.h>

#define uint unsigned int
#define uchar unsigned char
unsigned char sec; //定义计数值，每过1 秒，sec 加1
unsigned int tcnt; //定时中断次数
bit write = 0; //写24C08 的标志；
sbit sda = P2 ^0; //IIC 接口SDA 定义
sbit scl = P2 ^1; //IIC 接口SCL 定义
sbit dula = P2 ^6;
sbit wela = P2 ^7;
unsigned char j, k;

void delay(unsigned char i) //延时程序
{
    for (j = i; j > 0; j--)
        for (k = 125; k > 0; k--);
}

uchar code
table[]={
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d, //数码管编码
0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

void display(uchar bai_c, uchar sh_c) //显示程序
{
    dula = 0;
    P0 = table[bai_c]; //显示百位
    dula = 1;
    dula = 0;
    wela = 0;
    P0 = 0x7e;
    wela = 1;
    wela = 0;
    delay(5);
    dula = 0;
    P0 = table[sh_c]; //显示十位
    dula = 1;
    dula = 0;
    wela = 0;
    P0 = 0x7d;
    wela = 1;
    wela = 0;
    delay(5);
}

/////////24C02 读写驱动程序////////////////////
void delay1(unsigned char x) //延时子函数
{
    unsigned int i;
    for (i = 0; i < x; i++);
}

void flash() //延时子函数
{ ;; }

void x24c08_init() //24c02 初始化子程序
{
    scl = 1;
    flash();
    sda = 1;
    flash();
}

void start() //启动I2C 总线
{
    sda = 1;
    flash();
    scl = 1;
    flash();
    sda = 0;
    flash();
    scl = 0;
    flash();
}

void stop() //停止I2C 总线
{
    sda = 0;
    flash();
    scl = 1;
    flash();
    sda = 1;
    flash();
}

void writex(unsigned char j) //写一个字节
{
    unsigned char i, temp;
    temp = j;
    for (i = 0; i < 8; i++) {
        temp = temp << 1;
        scl = 0;
        flash();
        sda = CY;
        flash();
        scl = 1;
        flash();
    }
    scl = 0;
    flash();
    sda = 1;
    flash();
}

unsigned char readx() //读一个字节
{
    unsigned char i, j, k = 0;
    scl = 0;
    flash();
    sda = 1;
    for (i = 0; i < 8; i++) {
        flash();
        scl = 1;
        flash();
        if (sda == 1) j = 1;
        else j = 0;
        k = (k << 1) | j;
        scl = 0;
    }
    flash();
    return (k);
}

void clock() //I2C 总线应答子函数
{
    unsigned char i = 0;
    scl = 1;
    flash();
    while ((sda == 1) && (i < 255))i++;
    scl = 0;
    flash();
}

////////从24c02 的地址address 中读取一个字节数据/////
unsigned char x24c08_read(unsigned char address) {
    unsigned char i;
    start();
    writex(0xa0);
    clock();
    writex(address);
    clock();
    start();
    writex(0xa1);
    clock();
    i = readx();
    stop();
    delay1(10);
    return (i);
}

//////向24c02 的address 地址中写入一字节数据info/////
void x24c08_write(unsigned char address, unsigned char info) {
    EA = 0;
    start();
    writex(0xa0);
    clock();
    writex(address);
    clock();
    writex(info);
    clock();
    stop();
    EA = 1;
    delay1(50);
}

void t0(void)

interrupt 1 using 0 //定时中断服务函数
{
TH0 = (65536 - 50000) / 256; //对TH0 TL0 赋值
TL0 = (65536 - 50000) % 256; //重装计数初值
tcnt++; //每过250ust tcnt 加一
if(tcnt==20) //计满20 次（1 秒）时
{
tcnt = 0; //重新再计
sec++;
write = 1; //1 秒写一次24C02
if(sec==100) //定时100 秒，在从零开始计时
{
sec = 0;
}
}
}

void main() //主函数
{
    unsigned char i;
    TMOD = 0x01; //定时器工作在方式1
    ET0 = 1;
    EA = 1; //开中断
    x24c08_init(); //初始化24C08
    sec = x24c08_read(2); //读出保存的数据赋于sec
    TH0 = (65536 - 50000) / 256; //对TH0 TL0 赋值
    TL0 = (65536 - 50000) % 256; //使定时器0.05 秒中断一次
    TR0 = 1; //启动定时器
    while (1) {
        i = 10;
        while (i--) { display(sec / 10, sec % 10); }
        if (write == 1) //判断计时器是否计时一秒
        {
            write = 0; //清零
            x24c08_write(2, sec); //在24c02 的地址2 中写入数据sec
        }
    }
}
