#pragma once

#include <fstream>
#include <string>
#include <filesystem>

// Saves high score as a plain number in:
// config/snake_highscore.sav

inline bool saveHighScore(int score) {

    std::filesystem::create_directories("config");

    std::ofstream file(
        "config/snake_highscore.sav"
    );

    if (!file.is_open()) {
        return false;
    }

    file << score;

    return true;
}


inline int loadHighScore() {

    std::ifstream file(
        "config/snake_highscore.sav"
    );

    if (!file.is_open()) {
        return 0;
    }

    int score = 0;

    file >> score;

    return score;
}