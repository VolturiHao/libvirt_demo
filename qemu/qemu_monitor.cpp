#include <iostream>
#include "qemu_monitor_json.h"
#include "qemu_monitor.h"




/**
 * 全局函数实现
 */
// 获取下一条命令ID
std::string qemuMonitorNextCommandID(QemuMonitor& mon){
    return mon.nextCommandID();
}
// todo 发送qmp指令
int qemuMonitorSend(QemuMonitor& mon, QemuMonitorMessage& msg) {
    std::cout << "current Command ID:" << mon.getNextSerial() << std::endl;
    std::cout << "Message Contend:" << msg.getTxBuffer() << std::endl;
    return 0;

}
int qemuMonitorArbitraryCommand(QemuMonitor& mon,
                            const std::string cmd,
                            int fd,
                            std::string& reply,
                            bool hmp)
{
    // VIR_DEBUG("cmd=%s, fd=%d, reply=%p, hmp=%d", cmd, fd, reply, hmp);
    printf("cmd=%s, fd=%d, reply=%p, hmp=%d", cmd, fd, reply, hmp);

    // 还没写
    // QEMU_CHECK_MONITOR(mon);

    if (hmp)  // 还没写
        // return qemuMonitorJSONHumanCommand(mon, cmd, fd, reply);
        return -1;
    else
        return qemuMonitorJSONArbitraryCommand(mon, cmd, fd, reply);
}


std::string QemuMonitor::nextCommandID() {
    return "libvirt-" + std::to_string(++nextSerial);
}

void QemuMonitor::resetSerial(int value) {
    nextSerial = value;
}
