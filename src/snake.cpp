#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#include "Snake.h"
#include "SnakeAssets.h"
#include "HighScore.h"

float directionToRotation(sf::Vector2i dir) {
    if (dir == sf::Vector2i{ 0, 1 })  return 0.f;
    if (dir == sf::Vector2i{ 0, -1 }) return 180.f;
    if (dir == sf::Vector2i{ 1, 0 })  return 270.f;
    if (dir == sf::Vector2i{ -1, 0 }) return 90.f;

    return 0.f;
}

float cornerRotation(
    sf::Vector2i a,
    sf::Vector2i b
) {
    const sf::Vector2i top{ 0, -1 };
    const sf::Vector2i right{ 1, 0 };
    const sf::Vector2i bottom{ 0, 1 };
    const sf::Vector2i left{ -1, 0 };

    if ((a == top && b == right) ||
        (a == right && b == top))
        return 0.f;

    if ((a == right && b == bottom) ||
        (a == bottom && b == right))
        return 90.f;

    if ((a == bottom && b == left) ||
        (a == left && b == bottom))
        return 180.f;

    if ((a == left && b == top) ||
        (a == top && b == left))
        return 270.f;

    return 0.f;
}

Snake::Snake(
    sf::Vector2i startPos,
    sf::Vector2i startDir,
    int cols,
    int rows
)
    : cols(cols),
    rows(rows),
    direction(startDir),
    nextDirection(startDir) {

    reset(startPos, startDir);
    respawnFood();
}

void Snake::reset(
    sf::Vector2i startPos,
    sf::Vector2i startDir
) {
    body.clear();

    direction = startDir;
    nextDirection = startDir;
    growPending = false;

    for (int i = 0; i < 3; ++i) {
        body.push_back({
            startPos.x - startDir.x * i,
            startPos.y - startDir.y * i
            });
    }
}

void Snake::respawn(
    sf::Vector2i pos,
    sf::Vector2i dir
) {
    const std::size_t length = body.size();

    body.clear();

    direction = dir;
    nextDirection = dir;
    growPending = false;

    for (std::size_t i = 0; i < length; ++i) {
        body.push_back({
            pos.x - dir.x * static_cast<int>(i),
            pos.y - dir.y * static_cast<int>(i)
            });
    }
}

void Snake::setDirection(sf::Vector2i dir) {
    if (dir.x == -direction.x &&
        dir.y == -direction.y) {
        return;
    }

    nextDirection = dir;
}

void Snake::move() {
    direction = nextDirection;

    const sf::Vector2i newHead =
        body.front() + direction;

    body.push_front(newHead);

    if (growPending) {
        growPending = false;
    }
    else {
        body.pop_back();
    }
}

void Snake::grow() {
    growPending = true;
}

bool Snake::checkSelfCollision() const {
    const sf::Vector2i head = body.front();

    for (std::size_t i = 1; i < body.size(); ++i) {
        if (body[i] == head) {
            return true;
        }
    }

    return false;
}

bool Snake::checkWallCollision() const {
    const sf::Vector2i head = body.front();

    return head.x < 0 ||
        head.y < 0 ||
        head.x >= cols ||
        head.y >= rows;
}

bool Snake::checkFoodCollision() const {
    return body.front() == foodPos;
}

bool Snake::respawnFood() {
    for (int attempt = 0; attempt < 100; ++attempt) {
        const sf::Vector2i candidate{
            std::rand() % cols,
            std::rand() % rows
        };

        bool occupied = false;

        for (const auto& segment : body) {
            if (segment == candidate) {
                occupied = true;
                break;
            }
        }

        if (!occupied) {
            foodPos = candidate;
            return true;
        }
    }

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            const sf::Vector2i candidate{ x, y };

            bool occupied = false;

            for (const auto& segment : body) {
                if (segment == candidate) {
                    occupied = true;
                    break;
                }
            }

            if (!occupied) {
                foodPos = candidate;
                return true;
            }
        }
    }

    return false;
}

sf::Vector2i Snake::getFoodPosition() const {
    return foodPos;
}

sf::Vector2i Snake::getDirection() const {
    return direction;
}

const std::deque<sf::Vector2i>& Snake::getBody() const {
    return body;
}

sf::Vector2i Snake::getHead() const {
    return body.front();
}

void runSnake(sf::RenderWindow& window) {
    constexpr int CELL_SIZE = 32;
    constexpr float MOVE_INTERVAL = 0.2f;

    const sf::Vector2u windowSize = window.getSize();

    const int target =
        static_cast<int>(
            std::min(
                windowSize.x,
                windowSize.y
            ) * 0.78f
            );

    const int playHeight =
        (target / CELL_SIZE) * CELL_SIZE;

    const int playWidth = playHeight;

    const int cols =
        playWidth / CELL_SIZE;

    const int rows =
        playHeight / CELL_SIZE;

    const int offsetX =
        (windowSize.x - playWidth) / 2;

    const int offsetY =
        (windowSize.y - playHeight) / 2;

    SnakeLayout layout;

    layout.cellSize = CELL_SIZE;
    layout.cols = cols;
    layout.rows = rows;
    layout.playWidth = playWidth;
    layout.playHeight = playHeight;
    layout.offsetX = offsetX;
    layout.offsetY = offsetY;

    SnakeAssets assets;

    if (!assets.loadAll(layout, windowSize)) {
        std::cerr << "Failed to load Snake assets.\n";
        return;
    }

    assets.getMusic().play();

    Snake snake(
        { 5, 5 },
        { 1, 0 },
        cols,
        rows
    );

    int lives = 3;
    int score = 0;
    int highScore = loadHighScore();

    bool gameOver = false;
    bool won = false;
    bool paused = false;

    sf::Clock moveClock;

    sf::Text scoreText(assets.getFont());

    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (event->is<sf::Event::KeyPressed>()) {
                const auto key =
                    event->getIf<
                    sf::Event::KeyPressed
                    >()->code;

                if (key == sf::Keyboard::Key::Escape) {
                    return;
                }

                if (key == sf::Keyboard::Key::P ||
                    key == sf::Keyboard::Key::Space) {

                    if (!gameOver) {
                        paused = !paused;
                        moveClock.restart();
                    }
                }

                if (key == sf::Keyboard::Key::R &&
                    gameOver) {

                    lives = 3;
                    score = 0;

                    gameOver = false;
                    won = false;
                    paused = false;

                    snake.reset(
                        { 5, 5 },
                        { 1, 0 }
                    );

                    snake.respawnFood();

                    moveClock.restart();
                }
            }
        }

        if (!gameOver && !paused) {
            if (sf::Keyboard::isKeyPressed(
                sf::Keyboard::Key::W) ||
                sf::Keyboard::isKeyPressed(
                    sf::Keyboard::Key::Up)) {

                snake.setDirection({ 0, -1 });
            }

            if (sf::Keyboard::isKeyPressed(
                sf::Keyboard::Key::A) ||
                sf::Keyboard::isKeyPressed(
                    sf::Keyboard::Key::Left)) {

                snake.setDirection({ -1, 0 });
            }

            if (sf::Keyboard::isKeyPressed(
                sf::Keyboard::Key::S) ||
                sf::Keyboard::isKeyPressed(
                    sf::Keyboard::Key::Down)) {

                snake.setDirection({ 0, 1 });
            }

            if (sf::Keyboard::isKeyPressed(
                sf::Keyboard::Key::D) ||
                sf::Keyboard::isKeyPressed(
                    sf::Keyboard::Key::Right)) {

                snake.setDirection({ 1, 0 });
            }
        }

        if (!gameOver &&
            !paused &&
            moveClock.getElapsedTime().asSeconds()
            >= MOVE_INTERVAL) {

            snake.move();
            moveClock.restart();

            if (snake.checkWallCollision() ||
                snake.checkSelfCollision()) {

                --lives;

                if (lives <= 0) {
                    gameOver = true;
                }
                else {
                    snake.respawn(
                        { cols / 2, rows / 2 },
                        { 1, 0 }
                    );

                    moveClock.restart();
                }
            }

            if (!gameOver &&
                snake.checkFoodCollision()) {

                snake.grow();

                ++score;

                if (score > highScore) {
                    highScore = score;
                    saveHighScore(highScore);
                }

                assets.getEatSound().play();

                if (!snake.respawnFood()) {
                    won = true;
                    gameOver = true;
                }
                else {
                    assets.randomizeFoodTexture();
                }
            }
        }

        const sf::Vector2i food =
            snake.getFoodPosition();

        assets.getFoodSprite().setPosition({
            static_cast<float>(
                offsetX +
                food.x * CELL_SIZE +
                CELL_SIZE / 2
            ),
            static_cast<float>(
                offsetY +
                food.y * CELL_SIZE +
                CELL_SIZE / 2
            )
            });

        window.clear(sf::Color(10, 10, 10));

        assets.drawBackground(window);

        if (gameOver) {
            scoreText.setCharacterSize(32);

            scoreText.setString(
                (won ? "You Win!" : "Game Over!") +
                std::string(
                    "\n\nScore: "
                ) +
                std::to_string(score) +
                "\nHigh Score: " +
                std::to_string(highScore) +
                "\n\nPress R to Restart"
            );

            const sf::FloatRect bounds =
                scoreText.getLocalBounds();

            scoreText.setPosition({
                static_cast<float>(
                    window.getSize().x / 2
                ) - bounds.size.x / 2,

                static_cast<float>(
                    window.getSize().y / 2
                ) - bounds.size.y / 2
                });

            window.draw(scoreText);
            window.display();

            continue;
        }

        scoreText.setCharacterSize(24);

        scoreText.setString(
            "Score: " +
            std::to_string(score) +
            "  High Score: " +
            std::to_string(highScore) +
            "  Lives: " +
            std::to_string(lives)
        );

        scoreText.setPosition({
            static_cast<float>(offsetX),
            static_cast<float>(
                offsetY - 2 * CELL_SIZE
            )
            });

        window.draw(scoreText);
        window.draw(assets.getFoodSprite());

        const auto& body = snake.getBody();

        for (std::size_t i = 0; i < body.size(); ++i) {
            const sf::Vector2i pos = body[i];

            const float centerX =
                offsetX +
                pos.x * CELL_SIZE +
                CELL_SIZE / 2.f;

            const float centerY =
                offsetY +
                pos.y * CELL_SIZE +
                CELL_SIZE / 2.f;

            if (i == 0) {
                auto& sprite =
                    assets.getHeadSprite();

                sprite.setPosition({
                    centerX,
                    centerY
                    });

                sprite.setRotation(
                    sf::degrees(
                        directionToRotation(
                            snake.getDirection()
                        )
                    )
                );

                window.draw(sprite);
            }
            else if (i == body.size() - 1) {
                auto& sprite =
                    assets.getTailSprite();

                sf::Vector2i segmentDirection =
                    body[i - 1] - pos;

                if (segmentDirection == sf::Vector2i{ 0, 0 }) {
                    segmentDirection =
                        snake.getDirection();
                }

                sprite.setPosition({
                    centerX,
                    centerY
                    });

                sprite.setRotation(
                    sf::degrees(
                        directionToRotation(
                            segmentDirection
                        )
                    )
                );

                window.draw(sprite);
            }
            else {
                const sf::Vector2i toHead =
                    body[i - 1] - pos;

                const sf::Vector2i toTail =
                    body[i + 1] - pos;

                const bool straight =
                    (toHead.x + toTail.x == 0) &&
                    (toHead.y + toTail.y == 0);

                if (straight) {
                    auto& sprite =
                        assets.getBodySprite();

                    sprite.setPosition({
                        centerX,
                        centerY
                        });

                    sprite.setRotation(
                        sf::degrees(
                            directionToRotation(toHead)
                        )
                    );

                    window.draw(sprite);
                }
                else {
                    auto& sprite =
                        assets.getCornerSprite();

                    sprite.setPosition({
                        centerX,
                        centerY
                        });

                    sprite.setRotation(
                        sf::degrees(
                            cornerRotation(
                                toHead,
                                toTail
                            )
                        )
                    );

                    window.draw(sprite);
                }
            }
        }

        window.display();
    }
}

int main() {
    sf::RenderWindow window(
        sf::VideoMode::getDesktopMode(),
        "Snake",
        sf::State::Fullscreen
    );

    runSnake(window);

    return 0;
}