#pragma once
#include "Renderer.h"
#include "AABB.h"
#include <vector>

class Map
{
public:
    Map();
    void Load();
    void Draw(Renderer* renderer);
    bool CheckCollision(const AABB& box) const;

private:
    std::vector<AABB> m_walls;
};
