#pragma once
#include <string>
#include <memory>

/**
 * 类声明
 */
class QemuMonitor;
class QemuMonitorMessage;

/**
 * 全局函数
 */
std::string qemuMonitorNextCommandID(QemuMonitor& mon);  // 获取下一条命令ID
int qemuMonitorSend(QemuMonitor& mon, QemuMonitorMessage& msg);  // 发送qmp指令
int qemuMonitorArbitraryCommand(QemuMonitor& mon,
                            const std::string cmd,
                            int fd,
                            std::string& reply,
                            bool hmp);


class QemuMonitor {
public:
    QemuMonitor() : nextSerial(0) {}
    
    std::string nextCommandID();  // 获取下一个命令 ID
    void resetSerial(int value = 0);  // 重置 nextSerial
    int getNextSerial() const{ return nextSerial;}

private:
    int nextSerial; // 用于生成命令 ID 的序列号
};

class QemuMonitorMessage {
public:
    // 构造函数
    QemuMonitorMessage(int txFD = -1, const std::string& txBuffer = "")
        : txFD(txFD),
          txBuffer(txBuffer),
          txOffset(0),
          txLength(static_cast<int>(txBuffer.length())),
          rxObject(nullptr),
          finished(false) {}

    // 获取发送文件描述符
    int getTxFD() const { return txFD; }

    // 设置发送文件描述符
    void setTxFD(int fd) { txFD = fd; }

    // 获取发送缓冲区
    const std::string& getTxBuffer() const { return txBuffer; }

    // 设置发送缓冲区
    void setTxBuffer(const std::string& buffer) {
        txBuffer = buffer;
        txLength = static_cast<int>(buffer.length());
    }

    // 获取发送偏移
    int getTxOffset() const { return txOffset; }

    // 设置发送偏移
    void setTxOffset(int offset) { txOffset = offset; }

    // 获取发送长度
    int getTxLength() const { return txLength; }

    // 设置发送长度
    void setTxLength(int length) { txLength = length; }

    // 设置接收对象
    void setRxObject(void* object) { rxObject = object; }

    // 获取接收对象
    void* getRxObject() const { return rxObject; }

    // 检查是否完成
    bool isFinished() const { return finished; }

    // 设置完成状态
    void setFinished(bool state) { finished = state; }

private:
    int txFD;                  // 文件描述符
    std::string txBuffer;      // 发送缓冲区
    int txOffset;              // 发送偏移
    int txLength;              // 发送长度
    void* rxObject;            // 接收对象
    bool finished;             // 是否完成
};
