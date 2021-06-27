/** RS485.c */
#include <intrins.h>
#include <reg52.h>

sbit RS485_DIR = P1 ^ 7;        // RS485����ѡ������

bit flagFrame = 0;              // ����֡������ɱ�־λ����ʾ���յ���һ֡������
bit flagTxd = 0;                // ���ֽڷ�����ɱ�־λ��������� TXD �жϱ�־λ

unsigned char cntRxd = 0;       // �����ֽڼ�����
unsigned char pdata bufRxd[64]; // �����ֽڻ�����

extern void UartAction(unsigned char *buf, unsigned char len);

/* �������ú�����baud-ͨ�Ų����� */
void ConfigUART(unsigned int baud) {
  RS485_DIR = 0;                           // �� RS485 ����Ϊ����

  SCON = 0x50;                             // ���ô���Ϊ������ʽ 1
  TMOD &= 0x0F;                            // ���㶨ʱ�� T1 ����λ
  TMOD |= 0x20;                            // ���ö�ʱ�� T1 Ϊ����ģʽ 2
  TH1 = 256 - (11059200 / 12 / 32) / baud; // ���㶨ʱ�� T1 ��ʼֵ
  TL1 = TH1;                               // ����λ��ʱֵ��Ϊ��λ�ĳ�ֵ
  ET1 = 0;                                 // ���ö�ʱ�� T1 �ж�
  ES = 1;                                  // �����ж�ʹ��
  TR1 = 1;                                 // ������ʱ�� T1
}

/* �����ʱ��������ʱ t * 10 ΢���ʱ�� */
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

/* ��������д�뺯�������� buf �ǵȴ��������ݵ�ָ�룬���� len �Ƿ��͵ĳ��� */
void UartWrite(unsigned char *buf, unsigned char len) {
  RS485_DIR = 1;       // �� RS485 ����Ϊ����

  /* ѭ���������е��ֽ� */
  while (len--) {
    flagTxd = 0;       // ������ɱ�־λ����
    SBUF = *buf++;     // ���� 1 ���ֽڵ�����
    while (!flagTxd) ; // �ȴ����ֽڷ������
  }

  DelayX10us(5);       // �ȴ�����ֹͣλ��ɣ���ʱʱ���ɲ����ʾ���
  RS485_DIR = 0;       // �� RS485 ����Ϊ����
}

/* �������ݶ�ȡ���������� buf ��ʾ����ָ���ָ�룬���� len ��ʾ��ȡ�ĳ��ȣ�����ֵ��ʵ�ʶ�ȡ���ĳ��� */
unsigned char UartRead(unsigned char *buf, unsigned char len) {
  unsigned char i;
  /* ��ָ���Ķ�ȡ���ȴ���ʵ�ʽ��յ������ݳ���ʱ������ȡ��������Ϊʵ�ʽ��յ������ݳ��� */
  if (len > cntRxd) {
    len = cntRxd;
  }

  /* �����յ������ݿ���������ָ�� */
  for (i = 0; i < len; i++) {
    *buf++ = bufRxd[i];
  }

  cntRxd = 0; // ���ռ���������
  return len; // ����ʵ�ʶ�ȡ�ĳ���
}

/* ���ڽ��ռ�غ��������ڿ���ʱ�����ж�֡�������ڶ�ʱ�ж��е��ã����� ms ��ʾ��ʱ��� */
void UartRxMonitor(unsigned char ms) {
  static unsigned char cntbkp = 0;  // ���ռ�������
  static unsigned char idletmr = 0; // ���߿��м�ʱ����

  /* �����ռ�������������ʱ���������ߵĿ���ʱ�� */
  if (cntRxd > 0) {
    /* ������ռ������������ı䣨���ս��յ�����ʱ�������м�ʱ�������� */
    if (cntbkp != cntRxd) {
      cntbkp = cntRxd;
      idletmr = 0;
    }
    /* ������ռ�������û�иı䣨�����߱��ֿ���ʱ����������ʱ������ۼ� */
    else {
      /* ���߿��м�ʱС�� 30ms �����ۼ� */
      if (idletmr < 30) {
        idletmr += ms;
        /* ���߿��м�ʱ�ﵽ 30ms������Ϊһ֡���ݽ������ */
        if (idletmr >= 30) {
          flagFrame = 1;            // ��������֡������ɱ�־λ
        }
      }
    }
  } else {
    cntbkp = 0;
  }
}

/* �����������������ڼ�������֡�Ľ��ղ�������Ӧ���ܣ�������ѭ���н��е��� */
void UartDriver() {
  unsigned char len;
  unsigned char pdata buf[40];
  /* ������֡����ʱ����ȡ����������� */
  if (flagFrame) {
    flagFrame = 0;

    len = UartRead(buf, sizeof(buf) - 2); // �����յ��������ȡ��������
    UartAction(buf, len);                 // ��������֡�����ö���ִ�к���
  }
}

/* �����жϷ����� */
void InterruptUART() interrupt 4 {
  /* ���յ��µ��ֽ����� */
  if (RI) {
    RI = 0;      // �����жϱ�־λ����

    /* ���ջ�������δ����ʱ����������ֽڣ������������� */
    if (cntRxd < sizeof(bufRxd)) {
      bufRxd[cntRxd++] = SBUF;
    }
  }
  /* �ֽڷ������ */
  if (TI) {
    TI = 0;      // �����жϱ�־λ����
    flagTxd = 1; // ������ɱ�־λ�� 1
  }
}
