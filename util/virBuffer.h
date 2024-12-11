#ifndef VIRBUFFER_H
#define VIRBUFFER_H

#include <string>
#include <sstream>

class VirBuffer;

std::string virBufferCurrentContent(const VirBuffer& buf);
size_t virBufferUse(const VirBuffer& buf);


class VirBuffer {
public:
    VirBuffer() : indent(0) {}

    // 添加字符串到缓冲区
    void add(const std::string& str);
    void add(const char* str, int len = -1);

    // 获取当前的缩进长度
    size_t getEffectiveIndent() const;

    // 获取最终的字符串
    std::string str() const { return buffer.str(); }

    // 设置缩进值
    void setIndent(int newIndent) { indent = newIndent; }

private:
    // 应用自动缩进
    void applyIndent();

    std::ostringstream buffer; // 用于存储最终字符串的缓冲区
    int indent;                // 缩进大小
};

#endif // VIRBUFFER_H