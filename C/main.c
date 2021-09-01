#include <reg52.h>
#include "MCP2515.H"

void power_on_sig(void) {
    //  上电预处理
    init_set_config()  // 初始化基础设置
    read_EEPPROM()     // 读取配置信息
//    设置CAN_id
}

void main(void) {
//  上电预处理
    power_on_sig()
    // 工作模式设置：0 上电初始化， 1 配置 2， 读写CAN
    // 周期性汇报在线
    work_mode = 0

    while(true):
    swhich work_mode:

}