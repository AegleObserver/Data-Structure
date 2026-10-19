#ifndef RANKBOARD_H
#define RANKBOARD_H

#include <string>
#include <vector>

struct RankEntry {
    std::string name;
    std::string level;
    std::string mode;
    std::string result;
    int score;
    long long timeMs;
    std::string date;
};

class RankBoard {
public:
    RankBoard();
    ~RankBoard();

    void load();
    void save();
    void addEntry(const RankEntry& entry);
    bool nameExists(const std::string& name) const;
    void display() const;
    void setFilename(const std::string& filename);

private:
    std::vector<RankEntry> entries;
    std::string filename;
};

#endif
