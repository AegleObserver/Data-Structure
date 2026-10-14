#include "GameModule.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>

GameModule::GameModule() : ammoIndex(0), maxSize(10), score(0), turnCount(0),
    currentBall(' '), nextBall(' '), endlessMode(false) {
    std::srand((unsigned)std::time(0));
}

GameModule::~GameModule() {}

void GameModule::initialize(const std::string& active, const std::string& feed,
                             const std::string& ammo, int maxSize, bool endless) {
    if (ammo.size() < 2) return;
    balls = active;
    marks.assign(active.size(), 0);
    this->maxSize = maxSize;
    score = 0;
    turnCount = 0;
    endlessMode = endless;
    feedQueue.assign(feed.begin(), feed.end());
    this->ammo = ammo;
    ammoIndex = 2;
    currentBall = ammo[0];
    nextBall = ammo[1];
}

bool GameModule::insertBall(char ball, int index, ElimCallback cb) {
    if (index < 0 || index > (int)balls.size()) return false;
    balls.insert(index, 1, ball);
    marks.insert(marks.begin() + index, 1, 0);
    resolve(index, cb);
    return true;
}

void GameModule::discardBall() {}

int GameModule::applyChameleon() {
    if (balls.empty()) return -1;
    int index = std::rand() % (int)balls.size();
    char oldColor = balls[index];
    const char types[5] = {'R', 'G', 'B', 'Y', 'P'};
    char newColor = oldColor;
    while (newColor == oldColor) newColor = types[std::rand() % 5];
    balls[index] = newColor;
    std::fill(marks.begin(), marks.end(), 0);
    marks[index] = 1;
    return index;
}

void GameModule::feedBall() {
    if (!feedQueue.empty()) {
        balls.push_back(feedQueue.front());
        feedQueue.pop_front();
        marks.push_back(0);
    } else if (endlessMode) {
        balls.push_back(randomBall());
        marks.push_back(0);
    }
}

char GameModule::randomBall() {
    const char types[5] = {'R', 'G', 'B', 'Y', 'P'};
    return types[std::rand() % 5];
}

void GameModule::advanceAmmo() {
    currentBall = nextBall;
    nextBall = ammo[ammoIndex % ammo.size()];
    ammoIndex = (ammoIndex + 1) % ammo.size();
}

void GameModule::incrementTurnCount() { turnCount++; }

bool GameModule::isGameOver() const { return (int)balls.size() > maxSize; }

bool GameModule::isWin() const {
    return balls.empty() && feedQueue.empty() && !endlessMode;
}

std::string GameModule::getBalls() const { return balls; }
const std::vector<char>& GameModule::getMarks() const { return marks; }
int GameModule::getSize() const { return (int)balls.size(); }
int GameModule::getScore() const { return score; }
int GameModule::getWaitingCount() const { return (int)feedQueue.size(); }
int GameModule::getTurnCount() const { return turnCount; }
int GameModule::getMaxSize() const { return maxSize; }
char GameModule::getCurrentBall() const { return currentBall; }
char GameModule::getNextBall() const { return nextBall; }
bool GameModule::isEndless() const { return endlessMode; }

std::string GameModule::getFeed() const {
    std::string result;
    for (size_t i = 0; i < feedQueue.size(); i++) result += feedQueue[i];
    return result;
}

bool GameModule::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << "BALLS=" << balls << "\n";
    file << "FEED=" << getFeed() << "\n";
    file << "AMMO=" << ammo << "\n";
    file << "AMMO_INDEX=" << ammoIndex << "\n";
    file << "MAX_LENGTH=" << maxSize << "\n";
    file << "SCORE=" << score << "\n";
    file << "TURN=" << turnCount << "\n";
    file << "CURRENT=" << currentBall << "\n";
    file << "NEXT=" << nextBall << "\n";
    file << "ENDLESS=" << (endlessMode ? 1 : 0) << "\n";
    file.close();
    return true;//存档功能
}

bool GameModule::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        if (key == "BALLS") balls = value;
        else if (key == "FEED") {
            feedQueue.clear();
            for (char c : value) feedQueue.push_back(c);
        }
        else if (key == "AMMO") ammo = value;
        else if (key == "AMMO_INDEX") ammoIndex = std::stoi(value);
        else if (key == "MAX_LENGTH") maxSize = std::stoi(value);
        else if (key == "SCORE") score = std::stoi(value);
        else if (key == "TURN") turnCount = std::stoi(value);
        else if (key == "CURRENT") currentBall = value.empty() ? ' ' : value[0];
        else if (key == "NEXT") nextBall = value.empty() ? ' ' : value[0];
        else if (key == "ENDLESS") endlessMode = (value == "1");
    }
    file.close();
    marks.assign(balls.size(), 0);
    return !ammo.empty();//进行存档读取，同时给出ammo的状态
}

GameModule::Snapshot GameModule::getSnapshot() const {
    Snapshot s;
    s.balls = balls;
    s.feedQueue = feedQueue;
    s.ammo = ammo;
    s.ammoIndex = ammoIndex;
    s.maxSize = maxSize;
    s.score = score;
    s.turnCount = turnCount;
    s.currentBall = currentBall;
    s.nextBall = nextBall;
    s.endlessMode = endlessMode;
    s.marks = marks;
    return s;
}

void GameModule::applySnapshot(const Snapshot& snap) {
    balls = snap.balls;
    feedQueue = snap.feedQueue;
    ammo = snap.ammo;
    ammoIndex = snap.ammoIndex;
    maxSize = snap.maxSize;
    score = snap.score;
    turnCount = snap.turnCount;
    currentBall = snap.currentBall;
    nextBall = snap.nextBall;
    endlessMode = snap.endlessMode;
    marks = snap.marks;
}

void GameModule::resolve(int index, ElimCallback cb) {
    int chainLevel = 1;
    if (cb) cb(balls, "insert", -1, 0, 0);
    while (true) {
        char c = balls[index];
        int l = index, r = index;
        while (l > 0 && balls[l - 1] == c) --l;
        while (r + 1 < (int)balls.size() && balls[r + 1] == c) ++r;
        int len = r - l + 1;
        if (len < 3) break;
        std::string action = (chainLevel == 1) ? "eliminate" : "chain";
        if (cb) cb(balls, action, l, len, chainLevel);
        balls.erase(l, len);
        marks.erase(marks.begin() + l, marks.begin() + l + len);
        score += countScore(len, chainLevel);
        chainLevel++;
        if (l == 0 || l >= (int)balls.size()) break;
        if (balls[l - 1] != balls[l]) break;
        index = l;
    }
}

int GameModule::countScore(int len, int chainLevel) {
    double factor = 1.0 + (len - 3) * 0.2;
    return (int)(len * 20 * factor);
}
