#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

struct Vector3 {
    float x, y, z;
    Vector3();
    Vector3(float _x, float _y, float _z);
};

struct Vector2 {
    float x, y;
    std::string info;
};

enum matchType
{
    FreeForAll,
    Domination,
    SearchAndDestroy,
    Sabotage,
    TeamDeathMatch,
    HeadQuarters,
};
