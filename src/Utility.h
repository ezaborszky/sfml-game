#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include "Vec2.h"

Vec2 randomPos(int radius, int wHeight, int wWidth)
{

    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));

    // Define the range for the random numbers (window height and window length)
    int windowHeight = wHeight - radius * 2.5; // Replace with your window's height
    int windowLength = wWidth - radius * 2.5;  // Replace with your window's length

    // Create a uniform distribution for each range
    std::uniform_int_distribution<int> heightDistribution(radius, windowHeight);
    std::uniform_int_distribution<int> lengthDistribution(radius, windowLength);

    // Generate random numbers within the specified ranges
    int randomHeight = heightDistribution(rng);
    int randomLength = lengthDistribution(rng);

    return Vec2((float)randomLength, (float)randomHeight);
};

int randomVelocity(int minVel, int maxVel)
{
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));

    // Define the range for the random numbers (-10 to +10)
    int minRange = minVel;
    int maxRange = maxVel;

    // Create a uniform distribution for the specified range
    std::uniform_int_distribution<int> distribution(minRange, maxRange);

    // Generate a random number within the specified range
    int randomNumber = distribution(rng);
    // Check if the number is between -1 and 0
    if (randomNumber > -1 && randomNumber < 0)
    {
        randomNumber = -1;
    }

    // Check if the number is between 0 and 1
    if (randomNumber > 0 && randomNumber < 1)
    {
        randomNumber = 1;
    }

    return randomNumber;
};

sf::Color randomColor(std::mt19937 &rng)
{
    // Create a uniform distribution for the specified range
    std::uniform_int_distribution<int> distribution(0, 255);

    int r = distribution(rng);
    int g = distribution(rng);
    int b = distribution(rng);

    // Ensure that r, g, and b are different
    while (r == g || r == b || g == b)
    {
        r = distribution(rng);
        g = distribution(rng);
        b = distribution(rng);
    }

    return sf::Color(r, g, b);
}

double calculateDistance(const std::shared_ptr<Entity> &entity1, const std::shared_ptr<Entity> &entity2)
{
    if (!entity1 || !entity2)
    {
        return std::numeric_limits<double>::max();
    }

    int x1 = entity1->cTransform->pos.x;
    int y1 = entity1->cTransform->pos.y;
    int x2 = entity2->cTransform->pos.x;
    int y2 = entity2->cTransform->pos.y;

    double dx = x1 - x2;
    double dy = y1 - y2;

    return std::sqrt(dx * dx + dy * dy);
}

std::shared_ptr<Entity> findClosest(const EntityVec &entVec, const std::shared_ptr<Entity> &target)
{
    if (entVec.empty() || !target)
    {
        return nullptr;
    };

    std::shared_ptr<Entity> closest = entVec[0];

    double minDistance = calculateDistance(target, closest);

    for (const auto &entity : entVec)
    {
        double distance = calculateDistance(target, entity);
        if (distance < minDistance)
        {
            minDistance = distance;
            closest = entity;
        }
    }

    return closest;
}

Vec2 getDirection(Vec2 start, Vec2 target)
{

    Vec2 direction = Vec2(target.y - start.y, target.x - start.x);

    float angle = std::atan2(direction.y, direction.x);

    Vec2 finDir;

    finDir.x = std::sin(angle);
    finDir.y = std::cos(angle);
    //  std::cout << "x: " << finDir.x << "  " << "y: " << finDir.y << std::endl;
    return finDir;
}