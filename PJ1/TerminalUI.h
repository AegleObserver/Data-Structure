#ifndef TERMINALUI_H
#define TERMINALUI_H

#include "GameModule.h"
#include <string>
#include <vector>
#include <functional>

class TerminalUI {
public:
    static void displayGameState(const GameModule& game, const std::string& timeText,
                                 const std::string& hint = "", int arrowIndex = -1);
    static void displayAnimation(const std::string& track, const std::string& action,
                                 int start, int len, int chainLevel,
                                 int score, int turn, const std::string& timeText);
    static void displayMenu();
    static int getMainChoice();
    static void displayTutorialIntro();
    static void displayTutorialError(const std::string& expected);
    static void displayLevelMenu(const std::vector<std::string>& names);
    static int getLevelChoice();
    static void displayModeMenu();
    static int getModeChoice();
    static std::string getCommand();
    static std::string getCommandDeadline(const std::function<long long()>& remainingMs, bool& timedOut);
    static int getInsertIndex(int size);
    static void displayWin(int score, int turns, const std::string& elapsed);
    static void displayLose(int score, int turns, const std::string& elapsed, const std::string& reason);
    static void displayInvalidInput();
    static void pauseScreen();
    static void displayPauseMenu();
    static int getPauseChoice();
    static void displaySettingsMenu(bool soundOn, long long survivalLimitMs);
    static int getSettingsChoice();

private:
    static void setColor(int fg, int bg = 0);
    static void resetColor();
    static void clearScreen();
    static void printBall(char c);
    static void printHighlightedBall(char c);
    static std::string progressBar(int current, int max, int width);
};

#endif