//
// Created by dshar on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_PROCESSOR_H
#define INC_22S_FINAL_PROJ_PROCESSOR_H

#include <string>
#include <unordered_map>
#include <vector>

class Processor {

private:
    std::unordered_map<std::string, std::vector<std::string>> *authors = nullptr;

public:
    explicit Processor(std::unordered_map<std::string, std::vector<std::string>> *authors);
};


#endif //INC_22S_FINAL_PROJ_PROCESSOR_H
