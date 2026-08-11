#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <string>

#include "Snake.h"

class SnakeAssets {
public:
    SnakeAssets();

    bool loadTexture(
        sf::Texture& texture,
        const std::string& path
    );

    void setupSnakeSprite(
        sf::Sprite& sprite,
        sf::Texture& texture
    );

    bool loadAll(
        const SnakeLayout& layout,
        sf::Vector2u windowSize
    );

    void randomizeFoodTexture();

    void drawBackground(
        sf::RenderWindow& window
    ) const;

    // Getters
    sf::Music& getMusic() {
        return bgMusic;
    }

    sf::Font& getFont() {
        return font;
    }

    sf::Sound& getEatSound() {
        return eatSound;
    }

    sf::Sound& getCrashSound() {
        return crashSound;
    }

    sf::Sprite& getFoodSprite() {
        return foodSprite;
    }

    sf::Sprite& getHeadSprite() {
        return headSprite;
    }

    sf::Sprite& getBodySprite() {
        return bodySprite;
    }

    sf::Sprite& getTailSprite() {
        return tailSprite;
    }

    sf::Sprite& getCornerSprite() {
        return cornerSprite;
    }

    sf::Sprite& getObstacleSprite() {
        return obstacleSprite;
    }
    sf::Sprite& getBrokenWallSprite() {
        return brokenWallSprite;
    }

private:
    // Textures
    sf::Texture offsetTexture;
    sf::Texture grassTexture;

    sf::Texture borderH1Texture;
    sf::Texture borderV1Texture;
    sf::Texture borderH2Texture;
    sf::Texture borderV2Texture;

    sf::Texture headTexture;
    sf::Texture bodyTexture;
    sf::Texture tailTexture;
    sf::Texture cornerTexture;
    sf::Texture obstacleTexture;
    sf::Texture brokenWallTexture;
    sf::Sprite brokenWallSprite;

    sf::Texture foodTextures[4];

    // Sprites
    sf::Sprite offsetSprite;
    sf::Sprite grassSprite;

    sf::Sprite borderH1Sprite;
    sf::Sprite borderV1Sprite;
    sf::Sprite borderH2Sprite;
    sf::Sprite borderV2Sprite;

    sf::Sprite headSprite;
    sf::Sprite bodySprite;
    sf::Sprite tailSprite;
    sf::Sprite cornerSprite;
    sf::Sprite obstacleSprite;

    sf::Sprite foodSprite;

    // Audio
    sf::Music bgMusic;
    sf::SoundBuffer eatSoundBuffer;
    sf::Sound eatSound;
    sf::SoundBuffer crashSoundBuffer;
    sf::Sound crashSound;

    // Font
    sf::Font font;

    int currentFood = 0;
};