#ifndef TUNNEL_H
#define TUNNEL_H

#include "GameModule.h"
#include "RankBoard.h"
#include <chrono>

enum class GameMode { NORMAL, ENDLESS, SURVIVAL, CHAMELEON, COLLECTION };

class Tunnel {
public:
    Tunnel();
    ~Tunnel();
    void run();
private:
    GameModule game;
    RankBoard rankBoard;
    bool running;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point pauseStart;
    long long totalPausedMs;
    bool isPaused;
    GameMode currentMode;
    long long survivalStartMs;
    bool soundEnabled;
    bool currentTutorial;
    int tutorialStep;
    std::string currentFilename;
    std::string currentSaveFile;
    std::vector<GameModule::Snapshot> history;

    long long currentElapsedMs() const;
    long long survivalRemainingMs() const;
    long long timeRemainingMs() const;
    bool timeExpired() const;
    std::string formatTime(long long ms) const;
    std::string timeLabel() const;
    void playSound(const std::string& type);

    int selectLevel();
    int selectMode();
    void playLevel(const std::string& filename, GameMode mode, bool loadSave = false);
    bool playTurn();
    void finishTurn();
    bool saveToSlot();
    bool handlePause();
    void showRankBoard();
    void showSettings();
    void recordResult(const std::string& filename, GameMode mode, bool win);
};

#endif