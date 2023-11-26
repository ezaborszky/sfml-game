#pragma once
#include <SFML/Graphics.hpp>
#include "Vec2.h"

class CTransform
{
public:
    Vec2 pos = {0.0, 0.0};
    Vec2 speed = {0.0, 0.0};
    int rotation = 0;

    CTransform(Vec2 pos, Vec2 speed)
        : pos(pos), speed(speed), rotation(rotation){};
};

class CText
{
public:
    sf::Text text;
    sf::Font font;

    CText(){};
};

class CShape

{
public:
    sf::CircleShape circle;
    CShape(int radius, int points)
        : circle(radius, points)
    {
    }
};

class CCollision
{
    float radius = 0.0;
    CCollision(float radius) : radius(radius){};
};

class CInput
{
public:
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool pause = false;
    CInput(){};
};

class CScore
{
public:
    int score = 0;
    CScore(){};
};

class CLifespan
{
public:
    int lifeSpan = 0;
    int lifeSpanMax = 0;
    CLifespan(int time) : lifeSpan(time), lifeSpanMax(time){};
};
