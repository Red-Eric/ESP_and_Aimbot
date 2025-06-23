#pragma once
#include "../struct/struct.h"
#include <Windows.h>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sstream>
bool WorldToScreen(const Vector3& pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight);
bool GetNearestFromCenter(const std::vector<Vector2>& screenPositions, int screenWidth, int screenHeight, float FOV, Vector2& outTarget);
void MoveMouseToWindowOrigin(HWND hwnd, int x, int y);
std::string GetDistanceInMetersString(const Vector3& myPos, const Vector3& target);