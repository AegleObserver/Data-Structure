#include "LevelPlugin.h"
#include <fstream>
#include <iostream>
#include <set>

bool LevelPlugin::validate(const std::string& filename, LevelData& outData,
                           std::vector<ValidationError>& errors) {
    errors.clear();
    outData = LevelData();

    std::ifstream file(filename);
    if (!file.is_open()) {
        errors.push_back({"FILE", "Cannot open: " + filename});
        return false;
    }

    std::set<std::string> required = {"ACTIVE", "FEED", "AMMO", "MAX_LENGTH"};
    std::set<std::string> found;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            errors.push_back({"FORMAT", "Invalid line (no '='): " + line});
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        found.insert(key);

        if (key == "ACTIVE" || key == "FEED" || key == "AMMO") {
            for (char c : value) {
                if (c != 'R' && c != 'G' && c != 'B' && c != 'Y' && c != 'P') {
                    errors.push_back({key, std::string("Invalid color '") + c + "' in: " + value});
                }
            }
            if (key == "ACTIVE") outData.active = value;
            else if (key == "FEED") outData.feed = value;
            else outData.ammo = value;
        } else if (key == "MAX_LENGTH") {
            try {
                int n = std::stoi(value);
                if (n < 1) {
                    errors.push_back({"MAX_LENGTH", "Must be >= 1, got: " + value});
                } else {
                    outData.maxSize = n;
                }
            } catch (...) {
                errors.push_back({"MAX_LENGTH", "Not a number: " + value});
            }
        } else if (key == "TUTORIAL") {
            if (value == "1") outData.tutorial = true;
            else if (value == "0") outData.tutorial = false;
            else errors.push_back({"TUTORIAL", "Must be 0 or 1, got: " + value});
        } else {
            errors.push_back({"FORMAT", "Unknown field: " + key});
        }
    }
    file.close();

    for (const std::string& r : required) {
        if (found.find(r) == found.end()) {
            errors.push_back({"MISSING", "Required field not found: " + r});
        }
    }

    if (outData.ammo.size() < 2) {
        errors.push_back({"AMMO", "Must have at least 2 characters"});
    }

    return errors.empty();
}

void LevelPlugin::reportErrors(const std::vector<ValidationError>& errors) {
    std::cerr << "\n--- Validation Failed (" << errors.size() << " error(s)) ---\n";
    for (size_t i = 0; i < errors.size(); i++) {
        std::cerr << "  [" << errors[i].field << "] " << errors[i].message << "\n";
    }
    std::cerr << "-----------------------------------------\n";
}
