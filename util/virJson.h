
#pragma once
#include <string>
#include <vector>
#include <memory>
#include "virBuffer.h"

// JSON类型枚举
enum class VirJSONType {
    OBJECT,
    ARRAY,
    STRING,
    NUMBER,
    BOOLEAN,
    NULL_VALUE
};

class VirJSONObject;
class VirJSONArray;
class VirJSONValue;

/**
 * 全局函数
 */
// string转json
std::unique_ptr<VirJSONValue> virJSONValueFromString(const std::string& jsonString);
// json转buffer
int virJSONValueToBuffer(const VirJSONValue& object, VirBuffer& buf, bool pretty);
// json转string
std::string virJSONValueToString(VirJSONValue& object, bool pretty);
// 判断是否有某key
bool virJSONValueObjectHasKey(const VirJSONValue& object, const std::string& key);
// 添加字符串
int virJSONValueObjectAppendString(VirJSONValue& object, const std::string& key, const std::string &value);





/**
 * 类定义
 */
// JSON键值对
class VirJSONObjectPair {
public:
    VirJSONObjectPair(const std::string& key, std::shared_ptr<VirJSONValue> value)
        : key(key), value(value) {}
    ~VirJSONObjectPair() = default;

    // 获取键
    const std::string& getKey() const { return key; }
    // 获取值
    std::shared_ptr<VirJSONValue> getValue() const { return value; }

private:
    std::string key;
    std::shared_ptr<VirJSONValue> value;
};

// JSON值
class VirJSONValue {
public:
    // 构造函数和工厂函数
    explicit VirJSONValue(VirJSONType type);
    ~VirJSONValue() = default;

    // 工厂函数
    static std::unique_ptr<VirJSONValue> newNull();
    static std::unique_ptr<VirJSONValue> newString(const std::string& data);
    static std::unique_ptr<VirJSONValue> newNumber(const std::string& data);
    static std::unique_ptr<VirJSONValue> newBoolean(bool data);
    static std::unique_ptr<VirJSONValue> newObject();
    static std::unique_ptr<VirJSONValue> newArray();

    // 对象操作
    int objectInsert(const std::string& key, std::unique_ptr<VirJSONValue> value, bool prepend = false);
    int objectInsertString(const std::string& key, const std::string& value, bool prepend = false);
    int objectAppendString(const std::string& key, const std::string& value);
    bool objectHasKey(const std::string& key) const;
    
    // 数组操作
    int arrayAppend(std::unique_ptr<VirJSONValue> value);
    int arrayAppendString(const std::string& value);

    // 将 VirJSONValue 转换为 JSON 字符串
    std::string toJsonString(bool pretty = false) const;
    
    // set和get函数
    void setNullData() {
        type = VirJSONType::NULL_VALUE;
        stringData.clear();  // 清空数据字段
        numberData.clear();
        boolData = false;
        objectData.clear();
        arrayData.clear();
    }
    void setObjectData(const std::vector<std::shared_ptr<VirJSONObjectPair>>& objectData) {
        this->objectData = objectData;
        type = VirJSONType::OBJECT;
    }
    void setArrayData(const std::vector<std::shared_ptr<VirJSONValue>>& arrayData) {
        this->arrayData = arrayData;
        type = VirJSONType::ARRAY;
    }
    void setStringData(const std::string& str) {
        this->stringData = str;
        type = VirJSONType::STRING;
    }
    void setNumberData(const std::string& number) {
        numberData = number;
        type = VirJSONType::NUMBER;
    }
    void setBooleanData(bool booleanValue) {
        boolData = booleanValue;
        type = VirJSONType::BOOLEAN;
    }

    // 获取类型
    VirJSONType getType() const { return type; }

private:
    // 将 VirJSONValue 转换为 JSON 对象（类似于 C 中的 json_object）
    // std::string toJsonC(bool pretty = false) const;
    

    VirJSONType type;
    std::vector<std::shared_ptr<VirJSONObjectPair>> objectData;
    std::vector<std::shared_ptr<VirJSONValue>> arrayData;
    std::string stringData;
    std::string numberData;
    bool boolData = false;
};

// JSON对象
class VirJSONObject {
public:
    void addPair(const std::string& key, std::shared_ptr<VirJSONValue> value) {
        pairs.emplace_back(std::make_shared<VirJSONObjectPair>(key, value));
    }

    const std::vector<std::shared_ptr<VirJSONObjectPair>>& getPairs() const { return pairs; }

private:
    std::vector<std::shared_ptr<VirJSONObjectPair>> pairs;
};

// JSON数组
class VirJSONArray {
public:
    void addValue(std::shared_ptr<VirJSONValue> value) {
        values.push_back(value);
    }

    const std::vector<std::shared_ptr<VirJSONValue>>& getValues() const { return values; }

private:
    std::vector<std::shared_ptr<VirJSONValue>> values;
};