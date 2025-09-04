#include "Player.h"
#include <Windows.h>
#include <algorithm>

Player::Player() : m_x(0), m_y(0), m_speed(0), m_lastMoveX(0), m_lastMoveY(1), m_fireCooldown(0), m_hp(0), m_maxHp(0) {}

void Player::Init(float x, float y, float speed)
{
    m_x = x; m_y = y; m_speed = speed;
    m_maxHp = m_hp = 100;
}

void Player::Update(float deltaTime, const Map* map)
{
    float moveX = 0, moveY = 0;
    if (GetAsyncKeyState('W') & 0x8000) moveY = -1;
    if (GetAsyncKeyState('S') & 0x8000) moveY = 1;
    if (GetAsyncKeyState('A') & 0x8000) moveX = -1;
    if (GetAsyncKeyState('D') & 0x8000) moveX = 1;

    if(moveX != 0 || moveY != 0) {
        m_lastMoveX = moveX;
        m_lastMoveY = moveY;

        AABB playerBox = {m_x + moveX * m_speed * deltaTime, m_y, 50.0f, 50.0f};
        if (!map->CheckCollision(playerBox)) m_x += moveX * m_speed * deltaTime;

        playerBox = {m_x, m_y + moveY * m_speed * deltaTime, 50.0f, 50.0f};
        if (!map->CheckCollision(playerBox)) m_y += moveY * m_speed * deltaTime;
    }

    if (m_fireCooldown > 0) m_fireCooldown -= deltaTime;

    if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && m_fireCooldown <= 0)
    {
        m_fireCooldown = 0.5f;
        Bullet newBullet;
        float bulletSpeed = 300.0f;
        newBullet.Init(m_x + 20, m_y + 20, m_lastMoveX * bulletSpeed, m_lastMoveY * bulletSpeed);
        m_bullets.push_back(newBullet);
    }

    for (auto& bullet : m_bullets) bullet.Update(deltaTime);
}

void Player::HandleCollisions(std::vector<Enemy>& enemies)
{
    for (auto& bullet : m_bullets) {
        if (!bullet.IsActive()) continue;
        AABB bulletBox = bullet.GetAABB();
        for (auto& enemy : enemies) {
            if (!enemy.IsActive()) continue;
            AABB enemyBox = enemy.GetAABB();
            if (bulletBox.x < enemyBox.x + enemyBox.w && bulletBox.x + bulletBox.w > enemyBox.x &&
                bulletBox.y < enemyBox.y + enemyBox.h && bulletBox.y + bulletBox.h > enemyBox.y)
            {
                bullet.Deactivate();
                enemy.TakeDamage(25);
                break;
            }
        }
    }
}

void Player::Draw(Renderer* renderer)
{
    renderer->DrawQuad(m_x, m_y, 50.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    for (auto& bullet : m_bullets) bullet.Draw(renderer);
}
