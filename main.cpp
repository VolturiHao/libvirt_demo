#include <iostream>
#include "./qemu/qemu_monitor.h"

using namespace std;


int demo() {
    // virJSON命令测试
    QemuMonitor mon;
    std::string cmd = "{ \"execute\": \"query-status\" }";
    std::string reply;

    return qemuMonitorArbitraryCommand(mon, cmd, 0, reply, false);
}

int main() {
    // libvirtTest();
    cout << "demo return: " << demo() << endl;
    
    return 0;
}