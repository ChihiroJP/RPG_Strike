#include "Map.h"

Map::Map() {}

void Map::Load()
{
    m_walls.push_back({0, 0, 1920, 20});
    m_walls.push_back({0, 1060, 1920, 20});
    m_walls.push_back({0, 0, 20, 1080});
    m_walls.push_back({1900, 0, 20, 1080});
    m_walls.push_back({300, 300, 200, 50});
    m_walls.push_back({800, 600, 50, 200});
}

void Map::Draw(Renderer* renderer)
{
    for (const auto& wall : m_walls) {
        renderer->DrawQuad(wall.x, wall.y, wall.w, wall.h, 0.5f, 0.5f, 0.5f, 1.0f);
    }
}

bool Map::CheckCollision(const AABB& box) const
{
    for (const auto& wall : m_walls) {
        if (box.x < wall.x + wall.w && box.x + box.w > wall.x &&
            box.y < wall.y + wall.h && box.y + box.h > wall.y) {
            return true;
        }
    }
    return false;
}
