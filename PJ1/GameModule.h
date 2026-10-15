#ifndef GAMEMODULE_H
#define GAMEMODULE_H

#include <string>
#include <deque>
#include <vector>
#include <functional>

using ElimCallback = std::function<void(const std::string&, const std::string&, int, int, int)>;

class GameModule {
public:
    GameModule();
    ~GameModule();

    void initialize(const std::string& active, const std::string& feed,
                    const std::string& ammo, int maxSize, bool endless = false,
                    int scoreTarget = 0);

    bool insertBall(char ball, int index, ElimCallback cb = nullptr);
    void discardBall();
    int applyChameleon();
    void feedBall();
    void advanceAmmo();
    void incrementTurnCount();

    bool isGameOver() const;
    bool isWin() const;

    std::string getBalls() const;
    const std::vector<char>& getMarks() const;
    int getSize() const;
    int getScore() const;
    int getScoreTarget() const;
    int getWaitingCount() const;
    int getTurnCount() const;
    int getMaxSize() const;

    char getCurrentBall() const;
    char getNextBall() const;
    std::string getFeed() const;
    bool isEndless() const;

    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    struct Snapshot {
        std::string balls;
        std::deque<char> feedQueue;
        std::string ammo;
        int ammoIndex;
        int maxSize;
        int score;
        int scoreTarget;
        int turnCount;
        char currentBall;
        char nextBall;
        bool endlessMode;
        std::vector<char> marks;
    };

    Snapshot getSnapshot() const;
    void applySnapshot(const Snapshot& snap);

private:
    std::string balls;
    std::deque<char> feedQueue;
    std::string ammo;
    int ammoIndex;
    int maxSize;
    int score;
    int scoreTarget;
    int turnCount;
    char currentBall;
    char nextBall;
    bool endlessMode;
    std::vector<char> marks;

    void resolve(int index, ElimCallback cb = nullptr);
    int countScore(int len, int chainLevel);
    char randomBall();
};

#endif
