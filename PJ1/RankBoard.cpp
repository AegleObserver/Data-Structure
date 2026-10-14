#include "RankBoard.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <windows.h>

RankBoard::RankBoard() {}

RankBoard::~RankBoard() {}

void RankBoard::setFilename(const std::string& filename) {
    this->filename = filename;
}

void RankBoard::load() {
    entries.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        RankEntry e;

        if (!std::getline(ss, token, '|')) continue;
        e.name = token;
        if (!std::getline(ss, token, '|')) continue;
        e.level = token;
        if (!std::getline(ss, token, '|')) continue;
        e.mode = token;
        if (!std::getline(ss, token, '|')) continue;
        e.result = token;
        if (!std::getline(ss, token, '|')) continue;
        e.score = std::stoi(token);
        if (!std::getline(ss, token, '|')) continue;
        e.timeMs = std::stoll(token);
        if (!std::getline(ss, token, '|')) continue;
        e.date = token;

        entries.push_back(e);
    }
    file.close();
}

void RankBoard::save() {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    for (size_t i = 0; i < entries.size(); i++) {
        const RankEntry& e = entries[i];
        file << e.name << "|" << e.level << "|" << e.mode << "|"
             << e.result << "|" << e.score << "|" << e.timeMs << "|"
             << e.date << "\n";
    }
    file.close();
}

void RankBoard::addEntry(const RankEntry& entry) {
    entries.push_back(entry);
}

void RankBoard::display() const {
    std::cout << "\n========== RANKBOARD ==========\n";

    std::vector<RankEntry> cleared;
    std::vector<RankEntry> failed;

    for (size_t i = 0; i < entries.size(); i++) {
        if (entries[i].result == "WIN") cleared.push_back(entries[i]);
        else failed.push_back(entries[i]);
    }

    std::sort(cleared.begin(), cleared.end(), [](const RankEntry& a, const RankEntry& b) {
        if (a.level != b.level) return a.level < b.level;
        if (a.timeMs != b.timeMs) return a.timeMs < b.timeMs;
        return a.score > b.score;
    });

    std::sort(failed.begin(), failed.end(), [](const RankEntry& a, const RankEntry& b) {
        if (a.level != b.level) return a.level < b.level;
        if (a.score != b.score) return a.score > b.score;
        return a.timeMs < b.timeMs;
    });

    std::cout << "--- CLEARED (by time) ---\n";
    if (cleared.empty()) {
        std::cout << "No cleared records.\n";
    } else {
        std::cout << "  Name       Level   Mode      Time      Score\n";
        for (size_t i = 0; i < cleared.size() && i < 10; i++) {
            const RankEntry& e = cleared[i];
            long long t = e.timeMs;
            int mins = (int)(t / 60000);
            int secs = (int)((t % 60000) / 1000);
            char buf[32];
            sprintf_s(buf, "%02d:%02d", mins, secs);
            std::cout << "  " << e.name;
            std::cout << "   " << e.level << "  " << e.mode << "  " << buf << "    " << e.score << "\n";
        }
    }

    std::cout << "--- BEST SCORE (not cleared) ---\n";
    if (failed.empty()) {
        std::cout << "No records.\n";
    } else {
        std::cout << "  Name       Level   Mode      Time      Score\n";
        for (size_t i = 0; i < failed.size() && i < 10; i++) {
            const RankEntry& e = failed[i];
            long long t = e.timeMs;
            int mins = (int)(t / 60000);
            int secs = (int)((t % 60000) / 1000);
            char buf[32];
            sprintf_s(buf, "%02d:%02d", mins, secs);
            std::cout << "  " << e.name;
            std::cout << "   " << e.level << "  " << e.mode << "  " << buf << "    " << e.score << "\n";
        }
    }
    std::cout << "===============================\n";
}
