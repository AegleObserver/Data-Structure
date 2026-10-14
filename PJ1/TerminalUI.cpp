#include "TerminalUI.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <chrono>

void TerminalUI::setColor(int fg, int bg) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, fg | (bg << 4));
}

void TerminalUI::resetColor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 7);
}

void TerminalUI::clearScreen() {
    system("cls");
}

void TerminalUI::printBall(char c) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    int color = 7;
    switch (c) {
        case 'R': color = 12; break;
        case 'G': color = 10; break;
        case 'B': color = 9; break;
        case 'Y': color = 14; break;
        case 'P': color = 13; break;
    }
    SetConsoleTextAttribute(h, color);
    std::cout << "O";
    SetConsoleTextAttribute(h, 7);
}

void TerminalUI::printHighlightedBall(char c) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 15 | (4 << 4));
    std::cout << c;
    SetConsoleTextAttribute(h, 7);
}

std::string TerminalUI::progressBar(int current, int max, int width) {
    int filled = (max > 0) ? (current * width / max) : 0;
    if (filled > width) filled = width;
    std::string bar = "[";
    for (int i = 0; i < width; i++) {
        if (i < filled) bar += "#";
        else bar += "-";
    }
    bar += "]";
    return bar;
}

void TerminalUI::displayGameState(const GameModule& game, const std::string& timeText,
                                  const std::string& hint, int arrowIndex) {
    clearScreen();
    setColor(11, 0);
    std::cout << "  ZUMA - Command Line Edition\n";
    setColor(15, 0);
    std::cout << "  --------------------------------------\n";

    if (game.isEndless()) {
        setColor(13, 0);
        std::cout << "  [ENDLESS/SURVIVAL FEED]\n";
        setColor(15, 0);
    }

    std::cout << "  Track: ";
    std::string track = game.getBalls();
    for (size_t i = 0; i < track.size(); i++) {
        printBall(track[i]);
        std::cout << " ";
    }
    std::cout << "\n";

    const std::vector<char>& marks = game.getMarks();
    bool marked = false;
    for (size_t i = 0; i < marks.size(); i++) {
        if (marks[i]) { marked = true; break; }
    }
    if (marked) {
        setColor(12, 0);
        std::string markLine(9, ' ');
        for (size_t i = 0; i < track.size(); i++) {
            markLine += (i < marks.size() && marks[i]) ? '^' : ' ';
            markLine += ' ';
        }
        std::cout << markLine << "\n";
        resetColor();
    }

    if (arrowIndex >= 0) {
        setColor(14, 0);
        std::cout << std::string(9 + 2 * arrowIndex, ' ') << "^\n";
        resetColor();
    }
    if (!hint.empty()) {
        setColor(14, 0);
        std::cout << "  Tutorial: " << hint << "\n";
        resetColor();
    }

    std::cout << "  Size: ";
    setColor(14, 0);
    std::cout << game.getSize() << "/" << game.getMaxSize();
    setColor(15, 0);
    std::cout << "  ";
    setColor(10, 0);
    std::cout << progressBar(game.getSize(), game.getMaxSize(), 20);
    resetColor();
    std::cout << "\n";

    std::cout << "  Current: ";
    printBall(game.getCurrentBall());
    std::cout << "  Next: ";
    printBall(game.getNextBall());
    std::cout << "\n";

    if (!game.isEndless()) {
        std::cout << "  Waiting: ";
        std::string feed = game.getFeed();
        for (size_t i = 0; i < feed.size() && i < 10; i++) {
            printBall(feed[i]);
            std::cout << " ";
        }
        if (feed.size() > 10) std::cout << "...";
        std::cout << "\n";
    }

    std::cout << "  Score: ";
    setColor(10, 0);
    std::cout << game.getScore();
    setColor(14, 0);
    std::cout << "   Turn: ";
    setColor(10, 0);
    std::cout << game.getTurnCount();
    setColor(14, 0);
    std::cout << "   Time: ";
    setColor(10, 0);
    std::cout << timeText;
    setColor(15, 0);
    std::cout << "\n  --------------------------------------\n";
    resetColor();
}

void TerminalUI::displayMenu() {
    clearScreen();
    setColor(11, 0);
    std::cout << "\n           Z U M A\n";
    setColor(15, 0);
    std::cout << "      Command Line Edition\n";
    std::cout << "  --------------------------------------\n";
    std::cout << "   ";
    setColor(10, 0);
    std::cout << "1. Start Game";
    setColor(15, 0);
    std::cout << "\n   ";
    setColor(10, 0);
    std::cout << "2. Load Save";
    setColor(15, 0);
    std::cout << "\n   ";
    setColor(10, 0);
    std::cout << "3. Rankboard";
    setColor(15, 0);
    std::cout << "\n   ";
    setColor(13, 0);
    std::cout << "M. Settings";
    setColor(15, 0);
    std::cout << "\n   ";
    setColor(12, 0);
    std::cout << "0. Quit";
    setColor(15, 0);
    std::cout << "\n  --------------------------------------\n";
    resetColor();
    std::cout << "  Choose: ";
}

int TerminalUI::getMainChoice() {
    std::string input;
    if (!(std::cin >> input)) {
        std::cin.clear();
        return 0;
    }
    if (input == "M" || input == "m") return 10;
    try {
        return std::stoi(input);
    } catch (...) {
        return -1;
    }
}

void TerminalUI::displayTutorialIntro() {
    setColor(14, 0);
    std::cout << "\n  ================ TUTORIAL ================\n";
    std::cout << "  Learn the basics in 4 guided steps:\n";
    std::cout << "   1. Insert a ball by typing a position number\n";
    std::cout << "   2. The FEED ball joins the track after each turn\n";
    std::cout << "   3. Discard a useless ball by typing 'd'\n";
    std::cout << "   4. Clear the whole track to win\n";
    std::cout << "  ==========================================\n";
    resetColor();
}

void TerminalUI::displayTutorialError(const std::string& expected) {
    setColor(14, 0);
    std::cout << "  [Tutorial] Follow the hint! Expected input: '" << expected << "'\n";
    resetColor();
}

void TerminalUI::displayLevelMenu(const std::vector<std::string>& names) {
    clearScreen();
    setColor(11, 0);
    std::cout << "\n  --- Select Level ---\n";
    resetColor();
    for (size_t i = 0; i < names.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << names[i] << "\n";
    }
    std::cout << "  0. Back\n";
    std::cout << "  Choose: ";
}

int TerminalUI::getLevelChoice() {
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return -1;
    }
    return choice;
}

void TerminalUI::displayModeMenu() {
    clearScreen();
    setColor(11, 0);
    std::cout << "\n  --- Select Mode ---\n";
    resetColor();
    std::cout << "  1. Normal\n";
    std::cout << "  2. Endless\n";
    std::cout << "  3. Timed (2:00)\n";
    std::cout << "  4. Survival\n";
    std::cout << "  5. Chameleon\n";
    std::cout << "  0. Back\n";
    std::cout << "  Choose: ";
}

int TerminalUI::getModeChoice() {
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return -1;
    }
    return choice;
}

std::string TerminalUI::getCommand() {
    setColor(14, 0);
    std::cout << "\n  Enter index to insert, or [d]iscard [u]ndo [r]estart [s]ave [p]ause [q]uit\n  > ";
    resetColor();
    std::string input;
    std::cin >> input;
    return input;
}

std::string TerminalUI::getCommandDeadline(const std::function<long long()>& remainingMs, bool& timedOut) {
    setColor(14, 0);
    std::cout << "\n  Enter index to insert, or [d]iscard [u]ndo [r]estart [s]ave [p]ause [q]uit  [clock running]\n  > ";
    resetColor();
    timedOut = false;
    std::string input;
    while (true) {
        if (_kbhit()) {
            char c = _getch();
            if (c == '\r' || c == '\n') {
                std::cout << "\n";
                return input;
            }
            if (c == '\b') {
                if (!input.empty()) {
                    input.pop_back();
                    std::cout << "\b \b";
                }
            } else if (c >= '0' && c <= '9') {
                input += c;
                std::cout << c;
            } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                std::cout << c << "\n";
                return std::string(1, c);
            }
        }
        if (remainingMs() <= 0) {
            while (_kbhit()) _getch();
            std::cout << "\n";
            timedOut = true;
            return "";
        }
        Sleep(50);
    }
}

int TerminalUI::getInsertIndex(int size) {
    int index;
    setColor(14, 0);
    std::cout << "  Insert at index (0-" << size << "): ";
    resetColor();
    if (!(std::cin >> index)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return -1;
    }
    return index;
}

void TerminalUI::displayWin(int score, int turns, const std::string& elapsed) {
    clearScreen();
    setColor(10, 0);
    std::cout << "\n";
    std::cout << "            * YOU WIN! *\n";
    std::cout << "  --------------------------------------\n";
    std::cout << "  Score: " << score << "  Turns: " << turns << "  Time: " << elapsed << "\n";
    std::cout << "  --------------------------------------\n";
    resetColor();
}

void TerminalUI::displayLose(int score, int turns, const std::string& elapsed, const std::string& reason) {
    clearScreen();
    setColor(12, 0);
    std::cout << "\n";
    std::cout << "            X GAME OVER X\n";
    std::cout << "  --------------------------------------\n";
    std::cout << "  " << reason << "\n";
    std::cout << "  Score: " << score << "  Turns: " << turns << "  Time: " << elapsed << "\n";
    std::cout << "  --------------------------------------\n";
    resetColor();
}

void TerminalUI::displayInvalidInput() {
    setColor(12, 0);
    std::cout << "  Invalid input! Try again.\n";
    resetColor();
}

void TerminalUI::pauseScreen() {
    setColor(14, 0);
    std::cout << "\n  Press Enter to continue...";
    resetColor();
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TerminalUI::displayPauseMenu() {
    clearScreen();
    setColor(13, 0);
    std::cout << "\n  ---------- PAUSED ----------\n";
    resetColor();
    std::cout << "  1. Continue\n";
    std::cout << "  2. Quit to Main Menu\n";
    std::cout << "  Choose: ";
}

int TerminalUI::getPauseChoice() {
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return -1;
    }
    return choice;
}

void TerminalUI::displaySettingsMenu(bool soundOn, long long survivalLimitMs) {
    clearScreen();
    setColor(13, 0);
    std::cout << "\n  ========== Settings ==========\n";
    resetColor();
    std::string mins = std::to_string(survivalLimitMs / 60000);
    if (mins.size() < 2) mins = "0" + mins;
    std::string secs = std::to_string((survivalLimitMs % 60000) / 1000);
    if (secs.size() < 2) secs = "0" + secs;
    std::cout << "  1. Survival Time Limit: " << mins << ":" << secs << "\n";
    std::cout << "  2. Sound Effect: ";
    if (soundOn) {
        setColor(10, 0);
        std::cout << "ON\n";
    } else {
        setColor(12, 0);
        std::cout << "OFF\n";
    }
    resetColor();
    std::cout << "  0. Back\n";
    std::cout << "  Choose: ";
}

int TerminalUI::getSettingsChoice() {
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return -1;
    }
    return choice;
}

void TerminalUI::displayAnimation(const std::string& track, const std::string& action,
                                    int start, int len, int chainLevel,
                                    int score, int turn, const std::string& timeText) {
    clearScreen();
    setColor(15, 0);
    std::cout << "\n";
    if (action == "insert") {
        setColor(11, 0);
        std::cout << "  [INSERT]";
        setColor(15, 0);
        std::cout << "  ball placed\n";
    } else if (action == "eliminate") {
        setColor(10, 0);
        std::cout << "  [ELIMINATE]";
        setColor(15, 0);
        std::cout << "  " << len << " ball(s) removed\n";
    } else {
        setColor(13, 0);
        std::cout << "  [CHAIN x" << chainLevel << "]";
        setColor(15, 0);
        std::cout << "  " << len << " ball(s) removed\n";
    }
    std::cout << "  Track: ";
    for (size_t i = 0; i < track.size(); i++) {
        if (start >= 0 && i >= (size_t)start && i < (size_t)(start + len)) {
            printHighlightedBall(track[i]);
        } else {
            printBall(track[i]);
        }
        std::cout << " ";
    }
    setColor(15, 0);
    std::cout << "\n  Score: ";
    setColor(10, 0);
    std::cout << score;
    setColor(14, 0);
    std::cout << "  Turn: ";
    setColor(10, 0);
    std::cout << turn;
    setColor(14, 0);
    std::cout << "  Time: ";
    setColor(10, 0);
    std::cout << timeText;
    setColor(15, 0);
    std::cout << "\n  --------------------------------------\n";
    resetColor();
}