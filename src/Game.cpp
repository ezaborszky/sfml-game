#include "Game.h"
#include "Vec2.h"
#include <iostream>
#include "Utility.h"
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/System/Clock.hpp>


int wheight = 768;
int wwidth = 1024;
int minAngle = 3;
int maxAngle = 8;
int spawnRate = 100;
int minVel = -5;
int maxVel = 5;
int enemySize = 30;
int bulletSpeed = 10;
int bulletLife = 100;



std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));

Game::Game()
{
    m_window.create(sf::VideoMode(wwidth, wheight), "Progi");
    m_window.setFramerateLimit(60);
    ImGui::SFML::Init(m_window);
 
}

// float Game::FPS(){


//     currentTime = clock.getElapsedTime();
//     fps = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds()); // the asSeconds returns a float
//     previousTime = currentTime;

//     return fps;
// }

void Game::setupImGui()
{
    ImGui::Begin("Control Panel");
    for (const auto &entity : entityManager.getEntities("enemy"))
    {
        ImVec4 col((entity->cShape->circle.getOutlineColor().r / 255.f), (entity->cShape->circle.getOutlineColor().g / 255.f), (entity->cShape->circle.getOutlineColor().b / 255.f), 1.0f);
        ImGui::TextColored(col, "%zu %.1f %.1f", entity->id(), entity->cTransform->pos.y, entity->cTransform->pos.x);
    }
    ImGui::End();
}

void Game::run()
{
   
    spawnPlayer();
    entityManager.update();
    createText();
    // entityManager.update();
    float fps;
    sf::Clock clock;
    sf::Time time = clock.getElapsedTime();
    sf::Time current;
    while (m_window.isOpen())
    {

        entityManager.update();
        m_currentFrame++;
        userInput();

        if(!m_player->cInput->pause)
        {
        spawnEnemy();
        ImGui::SFML::Update(m_window, sf::seconds(1.0f / 60.0f));
        setupImGui();
        sCollision();
        sLifeSpan();
        sMovement();
        sScore(fps);
        sRender(m_window);
        current = clock.getElapsedTime();
        fps = 1/(current.asSeconds() - time.asSeconds());
        time = current;
      
        }

    }
}

Vec2 Game::getMousePos()
{
    sf::Vector2i position = sf::Mouse::getPosition(m_window);
    Vec2 mousePos = {(float)position.y, (float)position.x};
    // mousePos.normalize();
    return mousePos;
}

void Game::sMovement()
{
    // PLAYER MOVEMENT
    //----------------------------------------------------------------------

    // LEFT
    if (m_player->cInput->left)
    {
        if ((m_player->cTransform->speed.y) > -5)
            m_player->cTransform->speed.y -= 1;
    }

    // RIGHT
    if (m_player->cInput->right)
    {
        if ((m_player->cTransform->speed.y) < 5)
            m_player->cTransform->speed.y += 1;
    }

    // UP
    if (m_player->cInput->up)
    {
        if ((m_player->cTransform->speed.x) > -5)
            m_player->cTransform->speed.x -= 1;
    }

    // DOWN
    if (m_player->cInput->down)
    {
        if ((m_player->cTransform->speed.x) < 5)
            m_player->cTransform->speed.x += 1;
    }

    // DECELERATE

    if (!m_player->cInput->left && !m_player->cInput->left)
    {
        m_player->cTransform->speed.y *= 0.9;
    }

    if (!m_player->cInput->up && !m_player->cInput->down)
    {
        m_player->cTransform->speed.x *= 0.9;
    }

    // CHECK COLLISION AND MOVE PLAYER
    if (playerCollision(m_player, wheight, wwidth))
    {
        m_player->cTransform->pos.y += m_player->cTransform->speed.y;
        m_player->cTransform->pos.x += m_player->cTransform->speed.x;
        m_player->cShape->circle.setPosition(m_player->cTransform->pos.y, m_player->cTransform->pos.x);
    }

    // ENEMY MOVEMENT
    // ----------------------------------------------------------------------
    for (auto &a : (entityManager.getEntities("enemy")))
    {

        a->cTransform->pos += a->cTransform->speed;
        a->cTransform->rotation += 1;
        if (a->cTransform->rotation >= 360)
            a->cTransform->rotation = 0;

        a->cShape->circle.setRotation(a->cTransform->rotation);
        a->cShape->circle.setPosition((a->cTransform->pos.y), (a->cTransform->pos.x));
    };

    // BULLET MOVEMENT
    //----------------------------------------------------------------------

    for (auto &a : (entityManager.getEntities("bullet")))
    {
        a->cTransform->pos += a->cTransform->speed;
        a->cTransform->rotation += 1;
        a->cShape->circle.setPosition((a->cTransform->pos.y), (a->cTransform->pos.x));
    }

    // HOMING BULLET MOVEMENT
    //----------------------------------------------------------------------

    if (!entityManager.getEntities("enemy").empty())
    {
        for (auto &a : (entityManager.getEntities("hbullet")))
        {

            std::shared_ptr<Entity> closest = findClosest(entityManager.getEntities("enemy"), a);

            Vec2 dir = getDirection(a->cTransform->pos, closest->cTransform->pos);

            dir.y *= bulletSpeed * 0.3;
            dir.x *= bulletSpeed * 0.3;

            a->cTransform->pos += dir;

            a->cShape->circle.setPosition((a->cTransform->pos.y), (a->cTransform->pos.x));
        }
    }
    else
    {
        for (auto &a : (entityManager.getEntities("hbullet")))
        {
            a->cTransform->pos += a->cTransform->speed;
            a->cTransform->rotation += 1;
            a->cShape->circle.setPosition((a->cTransform->pos.y), (a->cTransform->pos.x));
        }
    }
}

void Game::userInput()
{

    sf::Event event;
    while (m_window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(event);
        switch (event.type)
        {
        case sf::Event::KeyPressed:
            switch (event.key.code)
            {
            case sf::Keyboard::A:

                m_player->cInput->left = true;
                break;
            case sf::Keyboard::D:

                m_player->cInput->right = true;
                break;
            case sf::Keyboard::W:

                m_player->cInput->up = true;
                break;
            case sf::Keyboard::S:

                m_player->cInput->down = true;
                break;
            case sf::Keyboard::Space:
                spawnHomingBullet();
                break;
            case sf::Keyboard::Escape:
                if(m_player->cInput->pause == false) 
                {m_player->cInput->pause = true;} else {m_player->cInput->pause = false;};
                break;
            default:
                break;
            }
            break;

        case sf::Event::KeyReleased:
            switch (event.key.code)
            {
            case sf::Keyboard::A:
                m_player->cInput->left = false;

                break;
            case sf::Keyboard::D:
                m_player->cInput->right = false;

                break;
            case sf::Keyboard::W:
                m_player->cInput->up = false;

                break;
            case sf::Keyboard::S:
                m_player->cInput->down = false;

                break;
            default:
                break;
            }
            break;

        case sf::Event::MouseButtonPressed:

            spawnBullet();
            break;

        case sf::Event::Closed:
            m_window.close();
            break;
        default:
            break;
        }
    }
}

// SPAWN
//----------------------------------------------------------------------

// HOMING BULLET
void Game::spawnHomingBullet()
{
    Vec2 playerLoc;

    for (auto &a : entityManager.getEntities("player"))
    {
        playerLoc = a->cTransform->pos;
    }

    Vec2 finDir = getDirection(playerLoc, getMousePos());

    auto bullet = entityManager.addEntity("hbullet");
    bullet->cShape = std::make_shared<CShape>(15, 20);
    bullet->cTransform = std::make_shared<CTransform>(playerLoc, finDir * bulletSpeed);
    bullet->cLifespan = std::make_shared<CLifespan>(300);
    bullet->cShape->circle.setPosition(((bullet->cTransform->pos.y)), (bullet->cTransform->pos.x));
    bullet->cShape->circle.setOrigin(bullet->cShape->circle.getRadius(), bullet->cShape->circle.getRadius());
    bullet->cShape->circle.setFillColor(sf::Color(255, 100, 255));
}

// BULLET
void Game::spawnBullet()
{

    Vec2 playerLoc;

    for (auto &a : entityManager.getEntities("player"))
    {
        playerLoc = a->cTransform->pos;
    }

    Vec2 finDir = getDirection(playerLoc, getMousePos());

    auto bullet = entityManager.addEntity("bullet");
    bullet->cShape = std::make_shared<CShape>(10, 20);
    bullet->cTransform = std::make_shared<CTransform>(playerLoc, finDir * bulletSpeed);
    bullet->cLifespan = std::make_shared<CLifespan>(bulletLife);
    bullet->cShape->circle.setPosition(((bullet->cTransform->pos.y)), (bullet->cTransform->pos.x));
    bullet->cShape->circle.setOrigin(bullet->cShape->circle.getRadius(), bullet->cShape->circle.getRadius());
    bullet->cShape->circle.setFillColor(sf::Color(255, 255, 255));
}

// BULLET LIFE SPAN --------------
void Game::sLifeSpan()
{
    for (const auto &a : (entityManager.getEntities()))
    {
        if (a->cLifespan)
        {
            a->cLifespan->lifeSpan--;
            if (a->cLifespan->lifeSpan <= 0)
            {
                a->destroy();
                return;
            }

            float alphaDecrease = (255.f / a->cLifespan->lifeSpan);

            sf::Color currentColor = a->cShape->circle.getFillColor();

            if (alphaDecrease > currentColor.a)
            {
                currentColor.a = 0; // Ensure alpha doesn't go negative
            }
            else
            {
                currentColor.a -= static_cast<sf::Uint8>(alphaDecrease);
            }

            a->cShape->circle.setFillColor(currentColor);

            
            sf::Color currentColorOut = a->cShape->circle.getOutlineColor();

            if (alphaDecrease > currentColorOut.a)
            {
                currentColorOut.a = 0; // Ensure alpha doesn't go negative
            }
            else
            {
                currentColorOut.a -= static_cast<sf::Uint8>(alphaDecrease);
            }

            a->cShape->circle.setOutlineColor(currentColorOut);
        }
    }
}

// SPAWN ENEMY --------------
void Game::spawnEnemy()
{
    if ((m_currentFrame - m_lastEnemySpawn) >= spawnRate)
    {

        auto enemy = entityManager.addEntity("enemy");
        enemy->cShape = std::make_shared<CShape>(enemySize, randomVelocity(minAngle, maxAngle));
        enemy->cTransform = std::make_shared<CTransform>(randomPos(enemySize, wwidth, wheight), Vec2(randomVelocity(minVel, maxVel), randomVelocity(minVel, maxVel)));
        enemy->cShape->circle.setPosition(((enemy->cTransform->pos.x)), (enemy->cTransform->pos.y));
        enemy->cShape->circle.setOrigin(enemy->cShape->circle.getRadius(), enemy->cShape->circle.getRadius());
        enemy->cShape->circle.setFillColor(sf::Color(0, 0, 0, 0));
        enemy->cShape->circle.setOutlineThickness(3);
        enemy->cShape->circle.setOutlineColor(randomColor(rng));
        m_lastEnemySpawn = m_currentFrame;
        
    }
}

// SPAWN PLAYER ---------------
void Game::spawnPlayer()
{
    m_player = entityManager.addEntity("player");
    m_player->cShape = std::make_shared<CShape>(40, 7);
    m_player->cTransform = std::make_shared<CTransform>(Vec2(wheight / 2, wwidth / 2), Vec2(0.f, 0.f));
    m_player->cInput = std::make_shared<CInput>();
    m_player->cScore = std::make_shared<CScore>();
    m_player->cShape->circle.setPosition(((m_player->cTransform->pos.y)), (m_player->cTransform->pos.x));
    m_player->cShape->circle.setOrigin(m_player->cShape->circle.getRadius(), m_player->cShape->circle.getRadius());
    m_player->cShape->circle.setFillColor(sf::Color(0, 0, 0, 0));
    m_player->cShape->circle.setOutlineThickness(3);
    m_player->cShape->circle.setOutlineColor(sf::Color(255, 0, 0));
}

// SPAWN TEXT ------
void Game::createText()
{

    auto text = entityManager.addEntity("text");
    text->cTransform = std::make_shared<CTransform>(Vec2(wheight / 10, wwidth / 2), Vec2(0, 0));
    text->cText = std::make_shared<CText>();
    text->cText->font.loadFromFile("./font1.ttf");
    text->cText->text.setFont(text->cText->font);
    text->cText->text.setString("ASD");     
    text->cText->text.setCharacterSize(40);
    text->cText->text.setPosition(text->cTransform->pos.y, text->cTransform->pos.x);

    auto text2 = entityManager.addEntity("text");
    text2->cTransform = std::make_shared<CTransform>(Vec2(wheight / 20, wwidth / 2), Vec2(0, 0));
    text2->cText = std::make_shared<CText>();
    text2->cText->font.loadFromFile("./font1.ttf");
    text2->cText->text.setFont(text2->cText->font);
    text2->cText->text.setString("ASD");     
    text2->cText->text.setCharacterSize(40);
    text2->cText->text.setPosition(text2->cTransform->pos.y, text2->cTransform->pos.x);
}

// RENDER
//--------------------------------------------------------------
void Game::sRender(sf::RenderWindow &window)
{
    window.clear();
    for (auto a : entityManager.getEntities("player"))
    {
        window.draw(a->cShape->circle);
    }

    for (auto a : entityManager.getEntities("bullet"))
    {
        window.draw(a->cShape->circle);
    }

    for (auto a : entityManager.getEntities("enemy"))
    {
        window.draw(a->cShape->circle);
    }

       for (auto a : entityManager.getEntities("smallenemy"))
    {
        window.draw(a->cShape->circle);
    }

    for (auto a : entityManager.getEntities("hbullet"))
    {
        window.draw(a->cShape->circle);
    }

    for (auto a : entityManager.getEntities("text"))
    {
        window.draw(a->cText->text);
    }

    ImGui::SFML::Render(m_window);

    window.display();
}

// COLLISION
//----------------------------------------------------

void Game::sCollision()
{
    for (auto &a : entityManager.getEntities("enemy"))
    {
        if (a->cTransform->pos.y >= (wwidth - (a->cShape->circle.getRadius())))
        {
            a->cTransform->speed.y *= -1.f;
        }

        if (a->cTransform->pos.y <= (a->cShape->circle.getRadius()))
        {
            a->cTransform->speed.y *= -1.f;
        }

        if (a->cTransform->pos.x >= (wheight - (a->cShape->circle.getRadius())))
        {
            a->cTransform->speed.x *= -1.f;
        }

        if (a->cTransform->pos.x <= (a->cShape->circle.getRadius()))
        {
            a->cTransform->speed.x *= -1.f;
        }
    }

    for (auto &e : entityManager.getEntities("enemy"))
    {
        for (auto &b : entityManager.getEntities("bullet"))
        {
            if (((e->cTransform->pos.dist(b->cTransform->pos)) < (e->cShape->circle.getRadius() * 1.2)) && e->cTransform->pos != b->cTransform->pos)
            {   

                // std::cout << e->cShape->circle.getRadius() << " " << enemySize << std::endl;
                if((int)e->cShape->circle.getRadius() >= (int)enemySize)
                {
                     spawnSmallEnemy(e);
                }
             
                e->destroy();
                b->destroy();
                m_player->cScore->score += 100;
            }
        }
    }

        for (auto &e : entityManager.getEntities("smallenemy"))
    {
        for (auto &b : entityManager.getEntities("bullet"))
        {
            if (((e->cTransform->pos.dist(b->cTransform->pos)) < (e->cShape->circle.getRadius() * 1.2)) && e->cTransform->pos != b->cTransform->pos)
            {   

                
                e->destroy();
                b->destroy();
                m_player->cScore->score += 100;
            }
        }
    }

    for (auto &e : entityManager.getEntities("enemy"))
    {
        for (auto &b : entityManager.getEntities("hbullet"))
        {
            if (((e->cTransform->pos.dist(b->cTransform->pos)) < (e->cShape->circle.getRadius() * 1.2)) && e->cTransform->pos != b->cTransform->pos)
            {
                e->destroy();
                b->destroy();
                m_player->cScore->score += 100;
            }
        }
    }

 

    for (auto &e : entityManager.getEntities("enemy"))
    {
        if (calculateDistance(e, m_player) < (m_player->cShape->circle.getRadius() + e->cShape->circle.getRadius()))
        {
            e->destroy();
            m_player->destroy();
            spawnPlayer();
        }
    }

     for (auto &e : entityManager.getEntities("smallenemy"))
    {
        if (calculateDistance(e, m_player) < (m_player->cShape->circle.getRadius() + e->cShape->circle.getRadius()))
        {
            e->destroy();
            m_player->destroy();
            spawnPlayer();
        }
    }
}

bool Game::playerCollision(std::shared_ptr<Entity> &player, int height, int width)
{

    int x = player->cTransform->pos.x;
    int y = player->cTransform->pos.y;
    int vely = player->cTransform->speed.y;
    int velx = player->cTransform->speed.x;
    int radius = player->cShape->circle.getRadius();

    if ((x + velx >= radius) && (x + velx <= (wheight - radius)))
    {
        if ((y + vely >= radius) && (y <= (wwidth - radius - vely)))
            return true;
    };
    return false;
}

void Game::sScore(float fps)
{
    std::string score = std::to_string(m_player->cScore->score);
    entityManager.getEntities("text")[0]->cText->text.setString(score);

    std::string fpstext = std::to_string(fps) ;
    entityManager.getEntities("text")[1]->cText->text.setString(fpstext);
}

void Game::spawnSmallEnemy(std::shared_ptr<Entity> parentEnemy)
{
        int spawnNumber = parentEnemy->cShape->circle.getPointCount();
        Vec2 pos = parentEnemy->cTransform->pos;
        float angle = (2*3.14)/spawnNumber;
        float initAngle = 0;
      
        
        for(int i = 0; i < spawnNumber; i++)
        {
        
        Vec2 finDir;
        finDir.x = std::sin(initAngle);
        finDir.y = std::cos(initAngle);

        //   std::cout << "x: " << finDir.x << "  " << "y: " << finDir.y << std::endl;

         auto enemy = entityManager.addEntity("enemy");
        enemy->cShape = std::make_shared<CShape>(enemySize/2, spawnNumber);
        enemy->cTransform = std::make_shared<CTransform>(pos, finDir);
        enemy->cShape->circle.setPosition(((enemy->cTransform->pos.x)), (enemy->cTransform->pos.y));
        enemy->cShape->circle.setOrigin(enemy->cShape->circle.getRadius(), enemy->cShape->circle.getRadius());
        enemy->cShape->circle.setFillColor(sf::Color(0, 0, 0, 0));
        enemy->cShape->circle.setOutlineThickness(3);
        enemy->cShape->circle.setOutlineColor(parentEnemy->cShape->circle.getOutlineColor());
        enemy->cLifespan = std::make_shared<CLifespan>(100);
        
        initAngle += angle;
        };
        
        // std::cout << spawnNumber << std::endl;

      
        // m_lastEnemySpawn = m_currentFrame;
};


