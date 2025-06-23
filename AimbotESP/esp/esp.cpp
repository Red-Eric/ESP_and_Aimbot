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

bool GetNearestFromCenter(const std::vector<Vector2>& screenPositions, int screenWidth, int screenHeight, float FOV, Vector2& outTarget) {
    float bestDistance = FOV;
    bool found = false;

    for (const auto& pos : screenPositions) {
        float dx = pos.x - (screenWidth / 2.0f);
        float dy = pos.y - (screenHeight / 2.0f);
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < bestDistance) {
            bestDistance = dist;
            outTarget = pos;
            found = true;
        }
    }

    return found;
}

void MoveMouseToWindowOrigin(HWND hwnd, int x, int y) {
    POINT pt = { x, y };
    if (ClientToScreen(hwnd, &pt)) {
        SetCursorPos(pt.x, pt.y);
    }
}

std::string GetDistanceInMetersString(const Vector3& myPos, const Vector3& target) {
    float dx = target.x - myPos.x;
    float dy = target.y - myPos.y;
    float dz = target.z - myPos.z;

    float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
    float meters = dist / 50.0f;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << meters << " m";
    return oss.str();
}