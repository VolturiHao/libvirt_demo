#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <nlohmann/json.hpp>
#include "virJson.h"

/**
 * 通用函数
 */
// string转json
std::unique_ptr<VirJSONValue> virJSONValueFromString(const std::string& jsonString) {
    try {
        // 使用nlohmann/json解析json字符串
        nlohmann::json parsedJson = nlohmann::json::parse(jsonString);

        // 创建 VirJSONValue 对象
        auto jsonValue = std::make_unique<VirJSONValue>(VirJSONType::NULL_VALUE);

        if (parsedJson.is_null()) {
            // 设置为 null 类型
            jsonValue->setNullData();
        } else if (parsedJson.is_boolean()) {
            // 设置为布尔值类型
            jsonValue->setBooleanData(parsedJson.get<bool>());
        } else if (parsedJson.is_number_integer() || parsedJson.is_number_float()) {
            // 设置为数字类型
            jsonValue->setNumberData(parsedJson.dump());  // 转换为字符串
        } else if (parsedJson.is_string()) {
            // 设置为字符串类型
            jsonValue->setStringData(parsedJson.get<std::string>());
        } else if (parsedJson.is_array()) {
            // 设置为数组类型
            std::vector<std::shared_ptr<VirJSONValue>> arrayData;
            for (const auto& item : parsedJson) {
                arrayData.push_back(virJSONValueFromString(item.dump()));  // 递归调用解析数组元素
            }
            jsonValue->setArrayData(arrayData);
        } else if (parsedJson.is_object()) {
            // 设置为对象类型
            std::vector<std::shared_ptr<VirJSONObjectPair>> objectData;
            for (const auto& item : parsedJson.items()) {
                // 递归调用解析对象值
                objectData.push_back(std::make_shared<VirJSONObjectPair>(item.key(), virJSONValueFromString(item.value().dump())));
            }
            jsonValue->setObjectData(objectData);
        }

        return jsonValue;

    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return nullptr;
    }
}
// json转buffer
int virJSONValueToBuffer(const VirJSONValue& object, VirBuffer& buf, bool pretty) {
    // 将 VirJSONValue 转换为 JSON 字符串
    std::string jsonString = object.toJsonString(pretty);

    // 添加 JSON 字符串到缓冲区
    buf.add(jsonString);

    // 如果需要美化输出，追加换行符
    if (pretty) {
        buf.add("\n");
    }

    return 0;
}
// json转string
std::string virJSONValueToString(VirJSONValue& object, bool pretty) {
    return object.toJsonString(pretty);
}


bool virJSONValueObjectHasKey(const VirJSONValue& object, const std::string& key) {
    return object.objectHasKey(key);
}

int virJSONValueObjectAppendString(VirJSONValue& object, const std::string& key, const std::string &value) {
    object.objectAppendString(key, value);
}

/**
 * VirJSONValue成员函数
 */

// 构造函数
VirJSONValue::VirJSONValue(VirJSONType type) : type(type) {}

/**
 * 工厂函数
 */
std::unique_ptr<VirJSONValue> VirJSONValue::newNull() {
    return std::unique_ptr<VirJSONValue>(new VirJSONValue(VirJSONType::NULL_VALUE));
}

std::unique_ptr<VirJSONValue> VirJSONValue::newString(const std::string& data) {
    if (data.empty()) {
        return newNull();
    }
    auto val = std::unique_ptr<VirJSONValue>(new VirJSONValue(VirJSONType::STRING));
    val->stringData = data;
    return val;
}

std::unique_ptr<VirJSONValue> VirJSONValue::newNumber(const std::string& data) {
    auto val = std::unique_ptr<VirJSONValue>(new VirJSONValue(VirJSONType::NUMBER));
    val->numberData = data;
    return val;
}

std::unique_ptr<VirJSONValue> VirJSONValue::newBoolean(bool data) {
    auto val = std::unique_ptr<VirJSONValue>(new VirJSONValue(VirJSONType::BOOLEAN));
    val->boolData = data;
    return val;
}

std::unique_ptr<VirJSONValue> VirJSONValue::newObject() {
    return std::unique_ptr<VirJSONValue>(new VirJSONValue(VirJSONType::OBJECT));
}

std::unique_ptr<VirJSONValue> VirJSONValue::newArray() {
    return std::unique_ptr<VirJSONValue>(new VirJSONValue(VirJSONType::ARRAY));
}

/**
 * 对象操作
 */

// 是否有指定key
bool VirJSONValue::objectHasKey(const std::string& key) const {
        if (type != VirJSONType::OBJECT) {
            return false;
        }
        for (const auto& pair : objectData) {
            if (pair->getKey() == key) {
                return true;
            }
        }
        return false;
}

// 插入指定的键值对
int VirJSONValue::objectInsert(const std::string& key, std::unique_ptr<VirJSONValue> value, bool prepend) {
    if (type != VirJSONType::OBJECT) {
        // reportError("Expecting JSON object");
        return -1;
    }

    if (objectHasKey(key)) {
        // reportError("Duplicate key: " + key);
        return -1;
    }
    
    VirJSONObjectPair pair(key, std::move(value));
    if (prepend) {  // 插入到开头
        objectData.insert(objectData.begin(), std::make_shared<VirJSONObjectPair>(pair));
    } else {  // 插入到结尾
        objectData.push_back(std::make_shared<VirJSONObjectPair>(pair));
    }

    return 0;
}

int VirJSONValue::objectInsertString(const std::string& key, const std::string& value, bool prepend) {
    auto jvalue = newString(value);
    return objectInsert(key, std::move(jvalue), prepend);
}

int VirJSONValue::objectAppendString(const std::string& key, const std::string& value) {
    return objectInsertString(key, value, false);
}

// 数组操作实现
int VirJSONValue::arrayAppend(std::unique_ptr<VirJSONValue> value) {
    if (type != VirJSONType::ARRAY) {
        // reportError("Expecting JSON array");
        return -1;
    }
    arrayData.push_back(std::move(value));
    return 0;
}

int VirJSONValue::arrayAppendString(const std::string& value) {
    auto jvalue = newString(value);
    return arrayAppend(std::move(jvalue));
}

/**
 * 其他函数
 */




/**
 * 私有函数实现
 */
// 转换为string
std::string VirJSONValue::toJsonString(bool pretty) const {
    std::ostringstream jsonStream;
    switch (type) {
        case VirJSONType::OBJECT:
            // 先添加左括号
            jsonStream << "{";  
            // 迭代器依次遍历vector<VirJSONObjectPair>键值对添加
            for (auto it = objectData.begin(); it != objectData.end(); ++it) {
                // 非首个元素要加 ","
                if (it != objectData.begin()) {
                    jsonStream << ",";
                }
                // 格式化，换行
                if (pretty) {
                    jsonStream << "\n  ";
                }
                // 解引用 shared_ptr 获取 VirJSONObjectPair 对象
                const auto& pair = **it;
                // 构造 "key": "value"格式的JSON字符串，其中Value要递归调用进行遍历
                jsonStream << "\"" << pair.getKey() << "\": " << pair.getValue()->toJsonString(pretty);
            }
            // 最后换行
            if (pretty && !objectData.empty()) {
                jsonStream << "\n";
            }
            // 添加右括号
            jsonStream << "}";
            break;

        case VirJSONType::ARRAY:
        // 数组的情况下添加[]并依次进行Value的遍历
            jsonStream << "[";
            for (size_t i = 0; i < arrayData.size(); ++i) {
                if (i > 0) {
                    jsonStream << ",";
                }
                if (pretty) {
                    jsonStream << "\n  ";
                }
                jsonStream << arrayData[i]->toJsonString(pretty);
            }
            if (pretty && !arrayData.empty()) {
                jsonStream << "\n";
            }
            jsonStream << "]";
            break;

        case VirJSONType::STRING:
            jsonStream << "\"" << stringData << "\"";
            break;

        case VirJSONType::NUMBER:
            jsonStream << numberData;
            break;

        case VirJSONType::BOOLEAN:
            jsonStream << (boolData ? "true" : "false");
            break;

        case VirJSONType::NULL_VALUE:
            jsonStream << "null";
            break;
    }
    return jsonStream.str();
}


