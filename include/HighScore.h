#pragma once
#include <fstream>
#include <string>

// Saves high score as a plain number in config/snake_highscore.sav

inline bool saveHighScore(int score) {
    // create config folder if it doesn't exist
    std::filesystem::create_directories("config");
    std::ofstream file("config/snake_highscore.sav");
    if (!file.is_open()) return false;
    file << score;
    return true;
}

inline int loadHighScore() {
    std::ifstream file("config/snake_highscore.sav");
    if (!file.is_open()) return 0; // no file yet = 0
    int score = 0;
    file >> score;
    return score;
}