#pragma once
#include "EntityManager.h"

class Game
{
    EntityManager entityManager;
    sf::RenderWindow m_window;
    std::shared_ptr<Entity> m_player;
    int m_currentFrame = 0;
    int m_lastEnemySpawn = 0;

public:
    void run();
    Game();
    void spawnEnemy();
    void sScore();
    void sMovement();
    void spawnBullet();
    void spawnHomingBullet();
    void sCollision();
    void spawnPlayer();
    void createText();
    Vec2 getMousePos();
    void userInput();
    void sLifeSpan();
    bool playerCollision(std::shared_ptr<Entity> &player, int height, int width);
    // void enemySpawner();
    void sRender(sf::RenderWindow &window);
    void setupImGui();
};
