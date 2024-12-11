#pragma once
#include "qemu_monitor.h"
#include "../util/virJson.h"

/**
 * 全局函数声明
 */
int qemuMonitorJSONArbitraryCommand(QemuMonitor& mon,
                                    const std::string& cmd_str,
                                    int fd,
                                    std::string& reply_str);
int qemuMonitorJSONCommandWithFd(QemuMonitor& mon,
                             VirJSONValue& cmd,
                             int scm_fd,
                             VirJSONValue **reply);
