#include <reg52.h>
#include "MCP2515.H"

void power_on_sig(void) {
    //  �ϵ�Ԥ����
    init_set_config()  // ��ʼ����������
    read_EEPPROM()     // ��ȡ������Ϣ
//    ����CAN_id
}

void main(void) {
//  �ϵ�Ԥ����
    power_on_sig()
    // ����ģʽ���ã�0 �ϵ��ʼ���� 1 ���� 2�� ��дCAN
    // �����Ի㱨����
    work_mode = 0

    while(true):
    swhich work_mode:

}