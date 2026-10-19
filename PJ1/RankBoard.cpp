#include "RankBoard.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <windows.h>

static int displayWidth(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = (unsigned char)s[i];
        if (c >= 0x80 && c <= 0xBF) continue;
        w += (c >= 0xE0) ? 2 : 1;
    }
    return w;
}

static std::string padTo(const std::string& s, int width) {
    std::string r = s;
    for (int i = displayWidth(s); i < width; i++) r += ' ';
    return r;
}

static std::string truncateTo(const std::string& s, int maxW) {
    std::string r;
    int w = 0;
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = (unsigned char)s[i];
        if (c >= 0x80 && c <= 0xBF) { r += (char)c; continue; }
        int cw = (c >= 0xE0) ? 2 : 1;
        if (w + cw > maxW) break;
        w += cw;
        r += (char)c;
    }
    return r;
}

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
    RankEntry e = entry;
    e.name = truncateTo(e.name, 12);
    entries.push_back(e);
}

bool RankBoard::nameExists(const std::string& name) const {
    for (size_t i = 0; i < entries.size(); i++) {
        if (entries[i].name == name) return true;
    }
    return false;
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

    const int NAME_W = 12, LEVEL_W = 7, MODE_W = 10, TIME_W = 5, SCORE_W = 6;

    std::cout << "--- CLEARED (by time) ---\n";
    if (cleared.empty()) {
        std::cout << "No cleared records.\n";
    } else {
        std::cout << "  " << padTo("Name", NAME_W) << "  "
                  << padTo("Level", LEVEL_W) << "  "
                  << padTo("Mode", MODE_W) << "  "
                  << padTo("Time", TIME_W) << "  "
                  << padTo("Score", SCORE_W) << "\n";
        for (size_t i = 0; i < cleared.size() && i < 10; i++) {
            const RankEntry& e = cleared[i];
            long long t = e.timeMs;
            int mins = (int)(t / 60000);
            int secs = (int)((t % 60000) / 1000);
            char buf[32];
            sprintf_s(buf, "%02d:%02d", mins, secs);
            std::cout << "  " << padTo(e.name, NAME_W) << "  "
                      << std::setw(LEVEL_W) << std::left << e.level << "  "
                      << std::setw(MODE_W) << std::left << e.mode << "  "
                      << buf << "  "
                      << std::setw(SCORE_W) << std::right << e.score << "\n";
        }
    }

    std::cout << "--- BEST SCORE (not cleared) ---\n";
    if (failed.empty()) {
        std::cout << "No records.\n";
    } else {
        std::cout << "  " << padTo("Name", NAME_W) << "  "
                  << padTo("Level", LEVEL_W) << "  "
                  << padTo("Mode", MODE_W) << "  "
                  << padTo("Time", TIME_W) << "  "
                  << padTo("Score", SCORE_W) << "\n";
        for (size_t i = 0; i < failed.size() && i < 10; i++) {
            const RankEntry& e = failed[i];
            long long t = e.timeMs;
            int mins = (int)(t / 60000);
            int secs = (int)((t % 60000) / 1000);
            char buf[32];
            sprintf_s(buf, "%02d:%02d", mins, secs);
            std::cout << "  " << padTo(e.name, NAME_W) << "  "
                      << std::setw(LEVEL_W) << std::left << e.level << "  "
                      << std::setw(MODE_W) << std::left << e.mode << "  "
                      << buf << "  "
                      << std::setw(SCORE_W) << std::right << e.score << "\n";
        }
    }
    std::cout << "===============================\n";
}
