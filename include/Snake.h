#pragma once

#include <SFML/System/Vector2.hpp>
#include <deque>
#include <vector>

struct SnakeLayout {
    int cellSize;
    int cols;
    int rows;
    int playWidth;
    int playHeight;
    int offsetX;
    int offsetY;
};

class Snake {
public:
    Snake(
        sf::Vector2i startPos,
        sf::Vector2i startDir,
        int cols,
        int rows
    );

    // Movement
    void setDirection(sf::Vector2i dir);
    void move();
    void grow();

    // Collisions
    bool checkSelfCollision() const;
    bool checkWallCollision() const;
    bool checkFoodCollision() const;

    // Food
    bool respawnFood();
    sf::Vector2i getFoodPosition() const;

    // Obstacles
    void spawnObstacles(int count);
    bool checkObstacleCollision() const;
    const std::vector<sf::Vector2i>& getObstacles() const;

    // Getters
    sf::Vector2i getDirection() const;
    const std::deque<sf::Vector2i>& getBody() const;
    sf::Vector2i getHead() const;

    // Restart / respawn
    void reset(sf::Vector2i startPos, sf::Vector2i startDir);
    void respawn(sf::Vector2i pos, sf::Vector2i dir);

private:
    std::deque<sf::Vector2i> body;

    sf::Vector2i direction;
    sf::Vector2i nextDirection;
    sf::Vector2i foodPos;

    std::vector<sf::Vector2i> obstacles;

    int cols;
    int rows;

    bool growPending = false;
};