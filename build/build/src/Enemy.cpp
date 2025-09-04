#include "Enemy.h"

Enemy::Enemy() : m_x(0), m_y(0), m_active(false), m_hp(0), m_maxHp(0) {}

void Enemy::Init(float x, float y)
{
    m_x = x; m_y = y;
    m_maxHp = m_hp = 100;
    m_active = true;
}

void Enemy::Update(float deltaTime) {}

void Enemy::TakeDamage(int damage)
{
    m_hp -= damage;
    if (m_hp <= 0) m_active = false;
}

void Enemy::Draw(Renderer* renderer)
{
    if (!m_active) return;
    renderer->DrawQuad(m_x, m_y, 50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    renderer->DrawQuad(m_x, m_y - 15, 50.0f, 10.0f, 0.2f, 0.0f, 0.0f, 1.0f);
    renderer->DrawQuad(m_x, m_y - 15, 50.0f * ((float)m_hp / m_maxHp), 10.0f, 0.0f, 0.8f, 0.0f, 1.0f);
}
