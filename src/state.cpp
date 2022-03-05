#include "state.h"
#include <algorithm> 
#include <vector>  

// 从文件读取并解析状态
State::State(const std::string& fileName) {
    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) {
        parseLine(line);
    }
}

// 用于解析文件中的每一行并更新状态变量
void State::parseLine(const std::string& line) {
    std::istringstream iss(line);
    std::string temp, key, value;
    if (std::getline(iss, temp, ' ') && std::getline(iss, key, ' ') && std::getline(iss, value)) {
        // 删除括号
        key.erase(std::remove(key.begin(), key.end(), '('), key.end());
        key.erase(std::remove(key.begin(), key.end(), ')'), key.end());
        value.erase(std::remove(value.begin(), value.end(), '('), value.end());
        value.erase(std::remove(value.begin(), value.end(), ')'), value.end());
        stateValues[key] = value;
    }
}

// 用于将状态转换为字符串的函数，以便在Q表中使用
std::string State::toString() const {
    std::string stateStr;
    for (const auto& pair : stateValues) {
        stateStr += pair.first + "=" + pair.second + ";";
    }
    return stateStr;
}
