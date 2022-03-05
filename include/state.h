#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <algorithm>

class State {
public:
    std::unordered_map<std::string, std::string> stateValues;

    explicit State(const std::string& fileName);
    void parseLine(const std::string& line);
    std::string toString() const;
};

#endif // STATE_H
