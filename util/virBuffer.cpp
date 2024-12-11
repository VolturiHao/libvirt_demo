#include "virBuffer.h"

/**
 * 全局函数实现
 */
// 获取缓冲区当前的内容
std::string virBufferCurrentContent(const VirBuffer& buf) {
    // 如果缓冲区为空或没有内容，则返回空字符串
    if (buf.str().empty()) {
        return "";
    }

    return buf.str();
}

// 获取缓冲区使用的字节数
size_t virBufferUse(const VirBuffer& buf) {
    // 使用缓冲区的长度作为字节数
    return buf.str().length();
}

// 获取当前的缩进
size_t VirBuffer::getEffectiveIndent() const {
    std::string currentBuffer = buffer.str();

    // 如果当前缓冲区为空或最后一个字符是换行符，则返回缩进值
    if (currentBuffer.empty() || currentBuffer.back() == '\n') {
        return static_cast<size_t>(indent);
    }

    return 0; // 否则不需要缩进
}

// 应用缩进
void VirBuffer::applyIndent() {
    static const std::string spaces =
        "                               "; // 31 个空格
    size_t spaceCount = spaces.size();
    size_t toIndent = getEffectiveIndent();

    // 如果无需缩进，直接返回
    if (toIndent == 0) {
        return;
    }

    // 按需添加空格
    while (toIndent > spaceCount) {
        buffer << spaces;
        toIndent -= spaceCount;
    }

    buffer << spaces.substr(0, toIndent);
}

// 添加字符串
void VirBuffer::add(const std::string& str) {
    add(str.c_str(), static_cast<int>(str.size()));
}

void VirBuffer::add(const char* str, int len) {
    if (!str) {
        return;
    }

    // 应用缩进
    applyIndent();

    // 添加字符串内容
    if (len < 0) {
        buffer << str; // 添加完整字符串
    } else {
        buffer.write(str, len); // 添加指定长度的字符串
    }
}