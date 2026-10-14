#ifndef LEVELPLUGIN_H
#define LEVELPLUGIN_H

#include "FileReader.h"
#include <string>
#include <vector>

struct ValidationError {
    std::string field;
    std::string message;
};

class LevelPlugin {
public:
    static bool validate(const std::string& filename, LevelData& outData,
                         std::vector<ValidationError>& errors);
    static void reportErrors(const std::vector<ValidationError>& errors);
};

#endif
