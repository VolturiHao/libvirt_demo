#include <memory>
#include "qemu_monitor_json.h"



int qemuMonitorJSONArbitraryCommand(QemuMonitor& mon,
                                    const std::string cmd_str,
                                    int fd,
                                    std::string& reply_str) 
{
    std::unique_ptr<VirJSONValue> cmd = virJSONValueFromString(cmd_str);
    if (!cmd) {
        return -1;  // 如果 cmd 为空，返回 -1
    }

    // 定义一个裸指针用于接收 reply
    VirJSONValue* replyRaw = nullptr;

    // 调用 qemuMonitorJSONCommandWithFd
    if (qemuMonitorJSONCommandWithFd(mon, *cmd, fd, &replyRaw) < 0) {
        return -1;  // 调用失败，返回 -1
    }

    // 将裸指针转换为 unique_ptr 进行内存管理
    std::unique_ptr<VirJSONValue> reply(replyRaw);

    // 使用 virJSONValueToString 将 reply 转换为字符串
    reply_str = virJSONValueToString(*reply, false);
    if (reply_str.empty()) {
        return -1;  // 如果生成的字符串为空，返回 -1
    }

    return 0;
}

int
qemuMonitorJSONCommandWithFd(QemuMonitor& mon,
                             VirJSONValue& cmd,
                             int scm_fd,
                             VirJSONValue **reply)
{
    int ret = -1;
    QemuMonitorMessage msg = { 0 };
    VirBuffer cmdbuf;

    *reply = NULL;

    if (virJSONValueObjectHasKey(cmd, "execute")) {
        std::string id = qemuMonitorNextCommandID(mon);

        if (virJSONValueObjectAppendString(cmd, "id", id) < 0) {
            // virReportError(VIR_ERR_INTERNAL_ERROR, "%s",_("Unable to append command 'id' string"));
            return -1;
        }
    }

    if (virJSONValueToBuffer(cmd, cmdbuf, false) < 0)
        return -1;
    
    // virBufferAddLit(&cmdbuf, "\r\n");

    msg.setTxLength(virBufferUse(cmdbuf));
    msg.setTxBuffer(virBufferCurrentContent(cmdbuf));
    msg.setTxFD(scm_fd);

    // todo 发送逻辑太复杂还没写
    ret = qemuMonitorSend(mon, msg);

    if (ret == 0) {
        if (!msg.getRxObject()) {
            // virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
            //                _("Missing monitor reply object"));
            ret = -1;
        } else {
            *reply = (VirJSONValue*) msg.getRxObject();
        }
    }

    return ret;
}