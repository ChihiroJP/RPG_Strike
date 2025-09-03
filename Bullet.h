#pragma once
#include "Renderer.h"
#include "AABB.h"

class Bullet
{
public:
    Bullet();
    void Init(float x, float y, float vx, float vy);
    void Update(float deltaTime);
    void Draw(Renderer* renderer);
    bool IsActive() const { return m_active; }
    void Deactivate() { m_active = false; }
    AABB GetAABB() const { return {m_x, m_y, 10.0f, 10.0f}; }

private:
    float m_x, m_y;
    float m_vx, m_vy;
    float m_lifetime;
    bool m_active;
};
