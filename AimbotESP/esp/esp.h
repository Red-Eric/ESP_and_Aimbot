#pragma once
#include "../struct/struct.h"
#include <Windows.h>
#include <vector>
#include <cmath>

bool WorldToScreen(const Vector3& pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight);
Vector2 GetNearestFromCenter(const std::vector<Vector2>& screenPositions, int screenWidth, int screenHeight);
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color);
void MoveMouseToWindowOrigin(HWND hwnd, int x, int y);