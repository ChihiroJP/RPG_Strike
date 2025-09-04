#include "Bullet.h"

Bullet::Bullet() : m_x(0), m_y(0), m_vx(0), m_vy(0), m_lifetime(0), m_active(false) {}

void Bullet::Init(float x, float y, float vx, float vy)
{
    m_x = x; m_y = y; m_vx = vx; m_vy = vy;
    m_lifetime = 10.0f;
    m_active = true;
}

void Bullet::Update(float deltaTime)
{
    if (!m_active) return;
    m_x += m_vx * deltaTime;
    m_y += m_vy * deltaTime;
    m_lifetime -= deltaTime;
    if (m_lifetime <= 0.0f) m_active = false;
}

void Bullet::Draw(Renderer* renderer)
{
    if (!m_active) return;
    renderer->DrawQuad(m_x, m_y, 10.0f, 10.0f, 1.0f, 1.0f, 0.0f, 1.0f);
}
