#ifndef TUNNEL_H
#define TUNNEL_H

#include "GameModule.h"
#include "RankBoard.h"
#include "LevelPlugin.h"
#include <chrono>

enum class GameMode { NORMAL, ENDLESS, TIMED, SURVIVAL };

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
    long long survivalRewardMs;
    bool soundEnabled;
    bool currentTutorial;
    int tutorialStep;
    std::string currentFilename;
    std::vector<GameModule::Snapshot> history;

    long long currentElapsedMs() const;
    long long survivalRemainingMs() const;
    long long timeRemainingMs() const;
    bool timeExpired() const;
    std::string formatTime(long long ms) const;
    std::string timeLabel() const;
    void addSurvivalReward(int len, int chainLevel);
    void playSound(const std::string& type);

    int selectLevel();
    int selectMode();
    void playLevel(const std::string& filename, GameMode mode, bool loadSave = false);
    bool playTurn();
    void finishTurn();
    bool handlePause();
    void showRankBoard();
    void showSettings();
    void recordResult(const std::string& filename, GameMode mode, bool win);
};

#endif