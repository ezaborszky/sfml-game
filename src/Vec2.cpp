#include "Vec2.h"
#include <math.h>
#include <iostream>

Vec2::Vec2(float xin, float yin)
    : x(xin), y(yin)
{
    // std::cout << "Class created \n";
}

bool Vec2::operator==(const Vec2 &rhs) const
{
    return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator!=(const Vec2 &rhs) const
{
    return (x != rhs.x || y != rhs.y);
}

Vec2 Vec2::operator+(const Vec2 &rhs) const
{
    return Vec2(x + rhs.y, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2 &rhs) const
{
    return Vec2(x - rhs.y, y - rhs.y);
}

Vec2 Vec2::operator/(const float val) const
{
    return Vec2(x / val, y / val);
}

Vec2 Vec2::operator*(const float val) const
{
    return Vec2(x * val, y * val);
}

void Vec2::operator+=(const Vec2 &rhs)
{
    x = x + rhs.x;
    y = y + rhs.y;
}

float Vec2::dist(const Vec2 &rhs) const
{
    float dx = x - rhs.x;
    float dy = y - rhs.y;

    return std::sqrt(dx * dx + dy * dy);
}

void Vec2::normalize()
{
    float length = sqrt(x * x + y * y);
    if (length != 0)
    {
        x /= length;
        y /= length;
    }
}