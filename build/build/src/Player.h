#pragma once
#include "Renderer.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Map.h"
#include <vector>

class Player
{
public:
    Player();
    void Init(float x, float y, float speed);
    void Update(float deltaTime, const Map* map);
    void Draw(Renderer* renderer);
    void HandleCollisions(std::vector<Enemy>& enemies);

    std::vector<Bullet> m_bullets;
    int GetHP() const { return m_hp; }
    int GetMaxHP() const { return m_maxHp; }

private:
    float m_x, m_y, m_speed;
    float m_lastMoveX, m_lastMoveY;
    float m_fireCooldown;
    int m_hp, m_maxHp;
};
