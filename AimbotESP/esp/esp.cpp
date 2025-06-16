#include "esp.h"

bool WorldToScreen(const Vector3& pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight) {
    float clipX = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
    float clipY = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
    float clipW = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

    if (clipW < 0.1f)
        return false;

    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;

    screen.x = (windowWidth / 2.0f) * ndcX + (windowWidth / 2.0f);
    screen.y = -(windowHeight / 2.0f) * ndcY + (windowHeight / 2.0f);

    return true;
}


void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

Vector2 GetNearestFromCenter(const std::vector<Vector2>& screenPositions, int screenWidth, int screenHeight) {
    Vector2 bestTarget = { 0.0f, 0.0f };
    float bestDistance = 99999.0f;

    for (const auto& pos : screenPositions) {
        float dx = pos.x - (screenWidth / 2.0f);
        float dy = pos.y - (screenHeight / 2.0f);
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < bestDistance) {
            bestDistance = dist;
            bestTarget = pos;
        }
    }

    return bestTarget;
}

void MoveMouseToWindowOrigin(HWND hwnd, int x, int y) {
    POINT pt = { x, y };
    if (ClientToScreen(hwnd, &pt)) {
        SetCursorPos(pt.x, pt.y);
    }
}