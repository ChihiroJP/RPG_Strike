#pragma once
#include "Renderer.h"
#include "AABB.h"

class Enemy
{
public:
    Enemy();
    void Init(float x, float y);
    void Update(float deltaTime);
    void Draw(Renderer* renderer);
    bool IsActive() const { return m_active; }
    AABB GetAABB() const { return {m_x, m_y, 50.0f, 50.0f}; }
    void TakeDamage(int damage);

private:
    float m_x, m_y;
    bool m_active;
    int m_hp, m_maxHp;
};
