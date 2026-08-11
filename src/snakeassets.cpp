#include "SnakeAssets.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

SnakeAssets::SnakeAssets()
    : offsetSprite(offsetTexture),
    grassSprite(grassTexture),
    borderH1Sprite(borderH1Texture),
    borderV1Sprite(borderV1Texture),
    borderH2Sprite(borderH2Texture),
    borderV2Sprite(borderV2Texture),
    headSprite(headTexture),
    bodySprite(bodyTexture),
    tailSprite(tailTexture),
    cornerSprite(cornerTexture),
    obstacleSprite(obstacleTexture),
    foodSprite(foodTextures[0]),
    eatSound(eatSoundBuffer) ,
    crashSound(crashSoundBuffer), brokenWallSprite(brokenWallTexture) {
}

bool SnakeAssets::loadTexture(
    sf::Texture& texture,
    const std::string& path
) {
    if (!texture.loadFromFile(path)) {
        std::cerr << "Failed to load: " << path << '\n';
        return false;
    }

    return true;
}

void SnakeAssets::setupSnakeSprite(
    sf::Sprite& sprite,
    sf::Texture& texture
) {
    sprite.setTexture(texture, true);

    const sf::Vector2u size = texture.getSize();

    if (size.x == 0 || size.y == 0) {
        return;
    }

    sprite.setOrigin({
        size.x / 2.f,
        size.y / 2.f
        });

    const float scale =
        32.f / static_cast<float>(
            std::max(size.x, size.y)
            );

    sprite.setScale({
        scale,
        scale
        });
}

bool SnakeAssets::loadAll(
    const SnakeLayout& layout,
    sf::Vector2u windowSize
) {
    bool ok = true;

    // Snake textures
    ok = loadTexture(
        headTexture,
        "assets/snake_head1.png"
    ) && ok;

    ok = loadTexture(
        bodyTexture,
        "assets/snake_body1.png"
    ) && ok;

    ok = loadTexture(
        tailTexture,
        "assets/snake_tail1.png"
    ) && ok;

    ok = loadTexture(
        cornerTexture,
        "assets/body_corner.png"
    ) && ok;

    setupSnakeSprite(headSprite, headTexture);
    setupSnakeSprite(bodySprite, bodyTexture);
    setupSnakeSprite(tailSprite, tailTexture);
    setupSnakeSprite(cornerSprite, cornerTexture);

    // Obstacle texture
    ok = loadTexture(
        obstacleTexture,
        "assets/obstacle.png"
    ) && ok;

    setupSnakeSprite(obstacleSprite, obstacleTexture);

    ok = loadTexture(
        brokenWallTexture,
        "assets/broken_wall.png"
    ) && ok;

    setupSnakeSprite(brokenWallSprite, brokenWallTexture);

    // Food textures
    static const char* foodPaths[4] = {
        "assets/brain.png",
        "assets/heart.png",
        "assets/lungs.png",
        "assets/stomach.png"
    };

    for (int i = 0; i < 4; ++i) {
        ok = loadTexture(
            foodTextures[i],
            foodPaths[i]
        ) && ok;
    }

    randomizeFoodTexture();

    // Background
    ok = loadTexture(
        offsetTexture,
        "assets/offset.png"
    ) && ok;

    offsetTexture.setRepeated(true);

    offsetSprite.setTextureRect(
        sf::IntRect(
            { 0, 0 },
            {
                static_cast<int>(windowSize.x),
                static_cast<int>(windowSize.y)
            }
        )
    );

    offsetSprite.setPosition({ 0.f, 0.f });

    // Grass
    ok = loadTexture(
        grassTexture,
        "assets/grass.png"
    ) && ok;

    grassTexture.setRepeated(true);

    grassSprite.setTextureRect(
        sf::IntRect(
            { 0, 0 },
            {
                layout.playWidth,
                layout.playHeight
            }
        )
    );

    grassSprite.setPosition({
        static_cast<float>(layout.offsetX),
        static_cast<float>(layout.offsetY)
        });

    // Top border
    ok = loadTexture(
        borderH1Texture,
        "assets/barrel.png"
    ) && ok;

    borderH1Texture.setRepeated(true);

    borderH1Sprite.setTextureRect(
        sf::IntRect(
            { 0, 0 },
            {
                layout.playWidth,
                layout.cellSize
            }
        )
    );

    borderH1Sprite.setPosition({
        static_cast<float>(layout.offsetX),
        static_cast<float>(
            layout.offsetY - layout.cellSize
        )
        });

    // Left border
    ok = loadTexture(
        borderV1Texture,
        "assets/barrel.png"
    ) && ok;

    borderV1Texture.setRepeated(true);

    borderV1Sprite.setTextureRect(
        sf::IntRect(
            { 0, 0 },
            {
                layout.cellSize,
                layout.playHeight
            }
        )
    );

    borderV1Sprite.setPosition({
        static_cast<float>(
            layout.offsetX - layout.cellSize
        ),
        static_cast<float>(layout.offsetY)
        });

    // Bottom border
    ok = loadTexture(
        borderH2Texture,
        "assets/barrel.png"
    ) && ok;

    borderH2Texture.setRepeated(true);

    borderH2Sprite.setTextureRect(
        sf::IntRect(
            { 0, 0 },
            {
                layout.playWidth,
                layout.cellSize
            }
        )
    );

    borderH2Sprite.setPosition({
        static_cast<float>(layout.offsetX),
        static_cast<float>(
            layout.offsetY + layout.playHeight
        )
        });

    // Right border
    ok = loadTexture(
        borderV2Texture,
        "assets/barrel.png"
    ) && ok;

    borderV2Texture.setRepeated(true);

    borderV2Sprite.setTextureRect(
        sf::IntRect(
            { 0, 0 },
            {
                layout.cellSize,
                layout.playHeight
            }
        )
    );

    borderV2Sprite.setPosition({
        static_cast<float>(
            layout.offsetX + layout.playWidth
        ),
        static_cast<float>(layout.offsetY)
        });

    // Font
    if (!font.openFromFile("fonts/regular.ttf")) {
        std::cerr << "Failed to load: fonts/regular.ttf\n";
        ok = false;
    }

    // Background music
    if (bgMusic.openFromFile(
        "audio/background_snake.wav"
    )) {
        bgMusic.setLooping(true);
        bgMusic.setVolume(30.f);
    }
    else {
        std::cerr
            << "Failed to load: "
            << "audio/background_snake.wav\n";
    }

    // Eat sound
    if (eatSoundBuffer.loadFromFile(
        "audio/food_crunch.mp3"
    )) {
        eatSound.setVolume(100.f);
    }
    else {
        std::cerr
            << "Failed to load: "
            << "audio/food_crunch.mp3\n";
    }
    // Crash sound
    if (crashSoundBuffer.loadFromFile(
        "audio/crash.wav"
    )) {
        crashSound.setVolume(100.f);
    }
    else {
        std::cerr
            << "Failed to load: "
            << "audio/crash.wav\n";
    }

    return ok;
}

void SnakeAssets::randomizeFoodTexture() {
    currentFood = std::rand() % 4;

    foodSprite.setTexture(
        foodTextures[currentFood],
        true
    );

    const sf::Vector2u foodSize =
        foodTextures[currentFood].getSize();

    if (foodSize.x > 0 && foodSize.y > 0) {
        // Keep the food centered inside its 32x32 cell.
        foodSprite.setOrigin({
            foodSize.x / 2.f,
            foodSize.y / 2.f
            });

        // Slightly smaller than the cell so it has some spacing.
        const float foodScale =
            26.f / static_cast<float>(
                std::max(foodSize.x, foodSize.y)
                );

        foodSprite.setScale({
            foodScale,
            foodScale
            });
    }
}

void SnakeAssets::drawBackground(
    sf::RenderWindow& window
) const {
    window.draw(offsetSprite);
    window.draw(grassSprite);

    window.draw(borderH1Sprite);
    window.draw(borderV1Sprite);
    window.draw(borderH2Sprite);
    window.draw(borderV2Sprite);
}