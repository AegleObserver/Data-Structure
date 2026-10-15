#include "Tunnel.h"
#include "FileReader.h"
#include "TerminalUI.h"
#include <iostream>
#include <cctype>
#include <cstdint>
#include <windows.h>

const std::string SAVE_FILE = "saves/save.txt";
const std::string RANK_FILE = "saves/rank.txt";

const long long TIMED_LIMIT_MS = 120000;
const long long SURVIVAL_DEFAULT_MS = 60000;
const long long SURVIVAL_REWARD_PER_BALL_MS = 500;
const int SURVIVAL_MAX_SIZE = 50;

struct LevelInfo {
    std::string label;
    std::string file;
    bool tutorial;
};

struct TutorialStep {
    std::string input;
    int arrow;
    std::string hint;
};

static const LevelInfo LEVELS[] = {
    {"Tutorial (New Player Guide)", "levels/tutorial.txt", true},
    {"Level 1", "levels/level1.txt", false},
    {"Level 2", "levels/level2.txt", false},
    {"Level 3", "levels/level3.txt", false},
    {"Level 4", "levels/level4.txt", false},
    {"Level 5", "levels/level5.txt", false},
    {"Level 6", "levels/level6.txt", false},
    {"Level 7", "levels/level7.txt", false},
};
const int LEVEL_COUNT = sizeof(LEVELS) / sizeof(LEVELS[0]);

static const TutorialStep TUTORIAL_STEPS[] = {
    {"2", 2, "STEP 1/4: Type 2 to place the R at the arrow. Three same-color balls in a row are cleared!"},
    {"2", 2, "STEP 2/4: A waiting ball joins the track tail each turn (the G). Type 2 to clear the three B's!"},
    {"d", -1, "STEP 3/4: A useless ball can be discarded: type d. Try it!"},
    {"2", 2, "STEP 4/4: Reach the target score to WIN. Type 2 to finish the last two G's!"},
};
const int TUTORIAL_STEP_COUNT = sizeof(TUTORIAL_STEPS) / sizeof(TUTORIAL_STEPS[0]);

Tunnel::Tunnel() : running(true), totalPausedMs(0), isPaused(false),
    currentMode(GameMode::NORMAL), survivalStartMs(SURVIVAL_DEFAULT_MS),
    survivalRewardMs(0), soundEnabled(false),
    currentTutorial(false), tutorialStep(0) {
    rankBoard.setFilename(RANK_FILE);
    rankBoard.load();
}

Tunnel::~Tunnel() {}

long long Tunnel::currentElapsedMs() const {
    auto now = isPaused ? pauseStart : std::chrono::steady_clock::now();
    long long total = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    return total - totalPausedMs;
}

long long Tunnel::survivalRemainingMs() const {
    return survivalStartMs + survivalRewardMs - currentElapsedMs();
}

long long Tunnel::timeRemainingMs() const {
    if (currentMode == GameMode::TIMED) return TIMED_LIMIT_MS - currentElapsedMs();
    if (currentMode == GameMode::SURVIVAL) return survivalRemainingMs();
    return INT64_MAX;
}

bool Tunnel::timeExpired() const {
    if (currentMode == GameMode::TIMED) return timeRemainingMs() <= 0;
    if (currentMode == GameMode::SURVIVAL) return timeRemainingMs() <= 0;
    return false;
}

std::string Tunnel::formatTime(long long ms) const {
    int mins = (int)(ms / 60000);
    int secs = (int)((ms % 60000) / 1000);
    char buf[32];
    sprintf_s(buf, "%02d:%02d", mins, secs);
    return std::string(buf);
}

std::string Tunnel::timeLabel() const {
    if (currentMode == GameMode::TIMED || currentMode == GameMode::SURVIVAL) {
        long long rem = timeRemainingMs();
        if (rem < 0) rem = 0;
        std::string extra = "";
        if (currentMode == GameMode::SURVIVAL) {
            char buf[32];
            sprintf_s(buf, " (+%llds)", survivalRewardMs / 1000);
            extra = buf;
        }
        return formatTime(rem) + " left" + extra;
    }
    return formatTime(currentElapsedMs());
}

void Tunnel::addSurvivalReward(int len, int chainLevel) {
    survivalRewardMs += (long long)len * chainLevel * SURVIVAL_REWARD_PER_BALL_MS;
}

void Tunnel::playSound(const std::string& type) {
    if (!soundEnabled) return;
    if (type == "insert") {
        Beep(440, 80);
    } else if (type == "eliminate") {
        Beep(660, 120);
        Sleep(60);
        Beep(880, 100);
    } else if (type == "chain") {
        Beep(880, 100);
        Sleep(50);
        Beep(1100, 100);
        Sleep(50);
        Beep(1320, 120);
    } else if (type == "win") {
        Beep(523, 150);
        Sleep(100);
        Beep(659, 150);
        Sleep(100);
        Beep(784, 200);
    } else if (type == "lose") {
        Beep(392, 200);
        Sleep(150);
        Beep(330, 300);
    }
}

int Tunnel::selectLevel() {
    while (true) {
        std::vector<std::string> names;
        for (int i = 0; i < LEVEL_COUNT; i++) names.push_back(LEVELS[i].label);
        TerminalUI::displayLevelMenu(names);
        int choice = TerminalUI::getLevelChoice();
        if (choice >= 1 && choice <= LEVEL_COUNT) return choice;
        if (choice == 0) return 0;
        TerminalUI::displayInvalidInput();
    }
}

int Tunnel::selectMode() {
    while (true) {
        TerminalUI::displayModeMenu();
        int choice = TerminalUI::getModeChoice();
        if (choice >= 1 && choice <= 5) return choice;
        if (choice == 0) return 0;
        TerminalUI::displayInvalidInput();
    }
}

void Tunnel::run() {
    while (running) {
        TerminalUI::displayMenu();
        int choice = TerminalUI::getMainChoice();

        if (choice == 0) {
            running = false;
        } else if (choice == 1) {
            int level = selectLevel();
            if (level == 0) continue;

            const LevelInfo& lv = LEVELS[level - 1];
            if (lv.tutorial) {
                playLevel(lv.file, GameMode::NORMAL);
                continue;
            }

            int mode = selectMode();
            if (mode == 0) continue;

            GameMode gm = GameMode::NORMAL;
            if (mode == 2) gm = GameMode::ENDLESS;
            else if (mode == 3) gm = GameMode::TIMED;
            else if (mode == 4) gm = GameMode::SURVIVAL;
            else if (mode == 5) gm = GameMode::CHAMELEON;
            playLevel(lv.file, gm);
        } else if (choice == 10) {
            showSettings();
        } else if (choice == 2) {
            playLevel("", GameMode::NORMAL, true);
        } else if (choice == 3) {
            showRankBoard();
        } else {
            TerminalUI::displayInvalidInput();
        }
    }
}

void Tunnel::playLevel(const std::string& filename, GameMode mode, bool loadSave) {
    currentMode = mode;
    currentFilename = filename;
    survivalRewardMs = 0;
    currentTutorial = false;
    tutorialStep = 0;

    if (loadSave) {
        if (!game.load(SAVE_FILE)) {
            std::cout << "  No save file found.\n";
            TerminalUI::pauseScreen();
            return;
        }
        if (game.isEndless()) currentMode = GameMode::ENDLESS;
    } else {
        LevelData data = FileReader::readLevel(filename);
        if (data.ammo.empty()) {
            std::cerr << "  Failed to load level: " << filename << std::endl;
            TerminalUI::pauseScreen();
            return;
        }
        bool endless = (mode == GameMode::ENDLESS || mode == GameMode::SURVIVAL);
        int maxSize = data.maxSize;
        int target = (mode == GameMode::NORMAL || mode == GameMode::CHAMELEON)
                     ? data.scoreTarget : 0;
        game.initialize(data.active, data.feed, data.ammo, maxSize, endless, target);
        currentTutorial = data.tutorial;
    }

    startTime = std::chrono::steady_clock::now();
    totalPausedMs = 0;
    isPaused = false;

    std::cout << "\n  Game started!";
    const char* modeName = "Normal";
    if (currentMode == GameMode::ENDLESS) modeName = "Endless";
    else if (currentMode == GameMode::TIMED) modeName = "Timed (2:00)";
    else if (currentMode == GameMode::SURVIVAL) modeName = "Survival";
    else if (currentMode == GameMode::CHAMELEON) modeName = "Chameleon";
    std::cout << " [" << modeName << "]\n";
    std::cout << "  Initial track: " << game.getBalls() << "\n";
    std::cout << "  Max length: " << game.getMaxSize() << "\n";

    if (currentTutorial) {
        TerminalUI::displayTutorialIntro();
        TerminalUI::pauseScreen();
    }

    bool win = false;
    while (!game.isGameOver() && !game.isWin() && !timeExpired()) {
        if (!playTurn()) break;
    }

    win = game.isWin();
    long long elapsed = currentElapsedMs();

    if (win) {
        TerminalUI::displayWin(game.getScore(), game.getTurnCount(), formatTime(elapsed));
        playSound("win");
    } else if (game.isGameOver()) {
        std::string reason = "Track exceeded max length!";
        if (game.getScoreTarget() > 0 && game.getSize() == 0
            && game.getScore() < game.getScoreTarget()) {
            reason = "Target score not reached!";
        }
        TerminalUI::displayLose(game.getScore(), game.getTurnCount(), formatTime(elapsed), reason);
        playSound("lose");
    } else if (timeExpired()) {
        TerminalUI::displayLose(game.getScore(), game.getTurnCount(), formatTime(elapsed), "Time's up!");
        playSound("lose");
    }
    TerminalUI::pauseScreen();

    if (!loadSave && !currentTutorial) {
        recordResult(filename, currentMode, win);
    }
}

bool Tunnel::playTurn() {
    history.push_back(game.getSnapshot());
    if (history.size() > 100) history.erase(history.begin());

    bool tuto = currentTutorial && tutorialStep < TUTORIAL_STEP_COUNT;
    if (tuto) {
        TerminalUI::displayGameState(game, timeLabel(),
                                     TUTORIAL_STEPS[tutorialStep].hint,
                                     TUTORIAL_STEPS[tutorialStep].arrow);
    } else {
        TerminalUI::displayGameState(game, timeLabel());
    }

    bool timedOut = false;
    std::string input;
    if (currentMode == GameMode::TIMED || currentMode == GameMode::SURVIVAL) {
        input = TerminalUI::getCommandDeadline([&]() { return timeRemainingMs(); }, timedOut);
    } else {
        input = TerminalUI::getCommand();
    }

    if (timedOut) {
        return true;
    }

    if (input.empty()) {
        TerminalUI::displayInvalidInput();
        return true;
    }

    if (tuto) {
        std::string got = input;
        for (size_t i = 0; i < got.size(); i++) got[i] = (char)tolower((unsigned char)got[i]);
        char c0 = got[0];
        if (c0 != 'p' && c0 != 'q' && c0 != 'r' && got != TUTORIAL_STEPS[tutorialStep].input) {
            TerminalUI::displayTutorialError(TUTORIAL_STEPS[tutorialStep].input);
            Sleep(900);
            history.pop_back();
            return true;
        }
    }

    bool numeric = true;
    for (char ch : input) {
        if (!isdigit((unsigned char)ch)) { numeric = false; break; }
    }

    int insertIndex = -1;
    if (numeric) {
        insertIndex = std::stoi(input);
    } else {
        char c = (char)tolower((unsigned char)input[0]);

        if (c == 's') {
            if (game.save(SAVE_FILE)) std::cout << "  Game saved!\n";
            else std::cerr << "  Failed to save.\n";
            return true;
        }
        if (c == 'r') {
            std::cout << "  Restarting...\n";
            history.clear();
            playLevel(currentFilename, currentMode);
            return false;
        }
        if (c == 'u') {
            if (history.size() <= 1) {
                std::cout << "  Nothing to undo.\n";
                return true;
            }
            history.pop_back();
            game.applySnapshot(history.back());
            std::cout << "  Undone.\n";
            return true;
        }
        if (c == 'p') {
            return handlePause();
        }
        if (c == 'q') {
            return false;
        }
        if (c == 'd') {
            game.discardBall();
            finishTurn();
            return true;
        }
        if (c == 'i') {
            insertIndex = TerminalUI::getInsertIndex(game.getSize());
        } else {
            TerminalUI::displayInvalidInput();
            return true;
        }
    }

    auto anim = [&](const std::string& state, const std::string& action, int start, int len, int chain) {
        TerminalUI::displayAnimation(state, action, start, len, chain,
                                     game.getScore(), game.getTurnCount(), timeLabel());
        if (currentMode == GameMode::SURVIVAL && len > 0) {
            addSurvivalReward(len, chain);
        }
        if (action == "insert") playSound("insert");
        else if (action == "eliminate") playSound("eliminate");
        else if (action == "chain") playSound("chain");
        Sleep(500);
    };
    if (!game.insertBall(game.getCurrentBall(), insertIndex, anim)) {
        TerminalUI::displayInvalidInput();
        return true;
    }

    finishTurn();
    return true;
}

void Tunnel::finishTurn() {
    game.feedBall();
    game.advanceAmmo();
    game.incrementTurnCount();
    if (currentMode == GameMode::CHAMELEON) game.applyChameleon();
    if (currentTutorial && tutorialStep < TUTORIAL_STEP_COUNT) tutorialStep++;
}

bool Tunnel::handlePause() {
    pauseStart = std::chrono::steady_clock::now();
    isPaused = true;

    while (true) {
        TerminalUI::displayPauseMenu();
        int choice = TerminalUI::getPauseChoice();

        if (choice == 1) {
            auto now = std::chrono::steady_clock::now();
            totalPausedMs += std::chrono::duration_cast<std::chrono::milliseconds>(now - pauseStart).count();
            isPaused = false;
            return true;
        } else if (choice == 2) {
            isPaused = false;
            return false;
        } else {
            TerminalUI::displayInvalidInput();
        }
    }
}

void Tunnel::showRankBoard() {
    rankBoard.display();
    TerminalUI::pauseScreen();
}

void Tunnel::showSettings() {
    while (true) {
        TerminalUI::displaySettingsMenu(soundEnabled, survivalStartMs);
        int choice = TerminalUI::getSettingsChoice();

        if (choice == 1) {
            std::cout << "  Enter survival time limit in seconds (10-600): ";
            int secs;
            if (!(std::cin >> secs)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                TerminalUI::displayInvalidInput();
                continue;
            }
            if (secs < 10 || secs > 600) {
                TerminalUI::displayInvalidInput();
                continue;
            }
            survivalStartMs = (long long)secs * 1000;
            std::cout << "  Survival time limit set to " << secs << "s.\n";
            TerminalUI::pauseScreen();
        } else if (choice == 2) {
            soundEnabled = !soundEnabled;
            std::cout << "  Sound Effect: " << (soundEnabled ? "ON" : "OFF") << "\n";
            TerminalUI::pauseScreen();
        } else if (choice == 0) {
            return;
        } else {
            TerminalUI::displayInvalidInput();
        }
    }
}

void Tunnel::recordResult(const std::string& filename, GameMode mode, bool win) {
    std::string level = "Unknown";
    size_t slash = filename.find_last_of("/\\");
    std::string base = (slash == std::string::npos) ? filename : filename.substr(slash + 1);
    size_t dot = base.find('.');
    if (dot != std::string::npos) base = base.substr(0, dot);
    if (base.size() > 5 && base.substr(0, 5) == "level") level = "Level" + base.substr(5);
    else if (!base.empty()) level = base;

    std::string modeName = "Normal";
    if (mode == GameMode::ENDLESS) modeName = "Endless";
    else if (mode == GameMode::TIMED) modeName = "Timed";
    else if (mode == GameMode::SURVIVAL) modeName = "Survival";
    else if (mode == GameMode::CHAMELEON) modeName = "Chameleon";

    std::string name;
    std::cout << "  Enter your name for the rankboard: ";
    std::cin.ignore(10000, '\n');
    std::getline(std::cin, name);
    if (name.empty()) name = "Player";

    SYSTEMTIME st;
    GetLocalTime(&st);
    char dateBuf[32];
    sprintf_s(dateBuf, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);

    RankEntry entry;
    entry.name = name;
    entry.level = level;
    entry.mode = modeName;
    entry.result = win ? "WIN" : "LOSE";
    entry.score = game.getScore();
    entry.timeMs = currentElapsedMs();
    entry.date = dateBuf;

    rankBoard.addEntry(entry);
    rankBoard.save();
    std::cout << "  Result recorded!\n";
}