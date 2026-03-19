#include "enemy.h"
#include "player.h"
#include "game.h"

Enemy::Enemy(Vector2 p, float w, float h, int hp, int dmg, float spd,
             float alertRad, float chaseRad, Behavior beh)
    : Entity(p, w, h, hp, (beh == Behavior::RANGER) ? YELLOW : MAROON) {
    damage = dmg;
    speed = spd;
    attackCD = 1.f;
    attackTimer = 0;
    behavior = beh;
    alertRadius = alertRad;
    chaseRadius = chaseRad;
    state = State::IDLE;

    if (beh == Behavior::RANGER) {
        magazineSize = 16;
        currentAmmo = magazineSize;
        shootCD = 0.5f;
        shootTimer = 0.3f;
        reloadDelay = 0.5f;
        reloadTime = 0.3f;
        reloadTimer = 0.f;
        isReloading = false;
    }
}

bool Enemy::canDetectPlayer(const Player& player) const {
    float distance = Vector2Distance(pos, player.getPosition());
    if (state == State::ALERTED) {
        return distance <= chaseRadius;
    } else {
        return distance <= alertRadius;
    }
}

void Enemy::update(float deltaTime, const Player& player) {
    if (attackTimer > 0) attackTimer -= deltaTime;
    if (shootTimer > 0) shootTimer -= deltaTime;
    if (reloadTimer > 0) reloadTimer -= deltaTime;

    if (canDetectPlayer(player)) {
        state = State::ALERTED;

        if (behavior == Behavior::CHASE) {
            chasePlayer(player, deltaTime);
        }
        else if (behavior == Behavior::RANGER) {
            float distance = Vector2Distance(pos, player.getPosition());
            if (distance > chaseRadius * 0.7f) {
                chasePlayer(player, deltaTime);
            }
            else if (distance < chaseRadius * 0.3f) {
                Vector2 away = {
                    pos.x - player.getPosition().x,
                    pos.y - player.getPosition().y
                };
                float len = sqrt(away.x * away.x + away.y * away.y);
                if (len > 0) {
                    away.x /= len;
                    away.y /= len;
                    pos.x += away.x * speed * deltaTime;
                    pos.y += away.y * speed * deltaTime;
                }
            }
        }
    } else {
        state = State::IDLE;
    }

    if (behavior == Behavior::RANGER && isReloading) {
        if (reloadTimer <= 0 && currentAmmo < magazineSize) {
            currentAmmo++;
            if (currentAmmo < magazineSize) {
                reloadTimer = reloadTime;
            } else {
                isReloading = false;
            }
        }
    }
}

void Enemy::chasePlayer(const Player& player, float deltaTime) {
    Vector2 toPlayer = {
        player.getPosition().x - pos.x,
        player.getPosition().y - pos.y
    };
    float length = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
    if (length > 0) {
        toPlayer.x /= length;
        toPlayer.y /= length;
        pos.x += toPlayer.x * speed * deltaTime;
        pos.y += toPlayer.y * speed * deltaTime;
    }
}

bool Enemy::attack(Player& player) {
    if (behavior == Behavior::CHASE) {
        if (canDetectPlayer(player) && checkCollision(player) && attackTimer <= 0) {
            player.takeDamage(damage);
            attackTimer = attackCD;
            return true;
        }
    }
    return false;
}

bool Enemy::shoot(std::vector<Bullet>& bullets, const Player& player, Sound& shootSfx) {
    if (behavior != Behavior::RANGER) return false;
    if (isReloading) return false;
    if (shootTimer > 0) return false;

    if (currentAmmo <= 0) {
        isReloading = true;
        reloadTimer = reloadDelay;
        return false;
    }

    if (state == State::ALERTED) {
        Vector2 center = {pos.x + width / 2, pos.y + height / 2};
        Vector2 toPlayer = {
            player.getPosition().x + player.getBounds().width / 2 - center.x,
            player.getPosition().y + player.getBounds().height / 2 - center.y
        };
        float length = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
        if (length > 0) {
            toPlayer.x /= length;
            toPlayer.y /= length;
        }

        Vector2 bulletPos = {
            pos.x + width / 2 - 2,
            pos.y + height / 2 - 4
        };

        bullets.emplace_back(bulletPos, toPlayer, 400.f, damage, 3.f, BulletOwner::ENEMY);
        currentAmmo--;
        shootTimer = shootCD;
        PlaySound(shootSfx);

        if (currentAmmo <= 0) {
            isReloading = true;
            reloadTimer = reloadDelay;
        }

        return true;
    }
    return false;
}

void Enemy::draw() const {
    Color drawColor = color;
    if (state == State::ALERTED) {
        drawColor = (behavior == Behavior::RANGER) ? YELLOW : RED;
    }

    DrawRectangle(pos.x, pos.y, width, height, drawColor);

    float hpPercent = (float)health / maxHealth;
    DrawRectangle(pos.x, pos.y - 8, width * hpPercent, 4, GREEN);

    // 🔧 Радиусы только в debug режиме
    if (g_debugMode) {
        DrawCircleLines(pos.x + width/2, pos.y + height/2, alertRadius, WHITE);
        if (state == State::ALERTED) {
            DrawCircleLines(pos.x + width/2, pos.y + height/2, chaseRadius,
                           (behavior == Behavior::RANGER) ? YELLOW : RED);
        }
    }

    if (behavior == Behavior::RANGER && isReloading) {
        int blink = (int)(GetTime() * 5) % 2;
        if (blink) {
            DrawRectangle(pos.x + 4, pos.y + 4, width - 8, height - 8, ORANGE);
        }
    }
}
