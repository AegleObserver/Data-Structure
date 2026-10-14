#include "FileReader.h"
#include <fstream>
#include <iostream>

LevelData FileReader::readLevel(const std::string& filename) {
    LevelData data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "ACTIVE") data.active = value;
        else if (key == "FEED") data.feed = value;
        else if (key == "AMMO") data.ammo = value;
        else if (key == "MAX_LENGTH") data.maxSize = std::stoi(value);
    }

    file.close();
    return data;
}
