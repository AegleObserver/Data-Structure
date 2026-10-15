#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>

struct LevelData {
    std::string active;
    std::string feed;
    std::string ammo;
    int maxSize;
    int scoreTarget = 0;
    bool tutorial = false;
};

class FileReader {
public:
    static LevelData readLevel(const std::string& filename);
};

#endif
