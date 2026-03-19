#include "player.h"

Player::Player(Vector2 startPos)
    : Entity(startPos, 32, 32, 100, GREEN),
      reloadSfxPtr(nullptr) {
    baseSpeed = 200.0f;
    speed = baseSpeed;
    damage = 10;
    magazineSize = 8;
    currentAmmo = magazineSize;
    shootCD = 0.15f;
    shootTimer = 0.0f;
    reloadDelay = 0.3f;
    reloadTime = 0.8f;
    reloadTimer = 0.0f;
    isReloading = false;
    maxStamina = 100.0f;
    currentStamina = maxStamina;
    staminaDrainRate = maxStamina / 8.0f;
    staminaRegenRate = maxStamina / 8.0f;
    isSprinting = false;
}

void Player::update(float deltaTime) {
    if (shootTimer > 0) {
        shootTimer -= deltaTime;
    }

    if (isReloading) {
        if (reloadTimer > 0) {
            reloadTimer -= deltaTime;
        } else {
            if (currentAmmo < magazineSize) {
                currentAmmo++;
                if (reloadSfxPtr) {
                    PlaySound(*reloadSfxPtr);
                }
                if (currentAmmo < magazineSize) {
                    reloadTimer = reloadTime;
                } else {
                    isReloading = false;
                }
            } else {
                isReloading = false;
            }
        }
    } else {
        if (currentAmmo <= 0 && shootTimer <= 0) {
            isReloading = true;
            reloadTimer = reloadDelay;
        }
    }

    bool shiftHeld = IsKeyDown(KEY_LEFT_SHIFT);
    if (shiftHeld && currentStamina > 0) {
        speed = baseSpeed * 2.0f;
        currentStamina -= staminaDrainRate * deltaTime;
        isSprinting = true;
        if (currentStamina < 0) {
            currentStamina = 0;
        }
    } else {
        speed = baseSpeed;
        isSprinting = false;
        if (!shiftHeld && currentStamina < maxStamina) {
            currentStamina += staminaRegenRate * deltaTime;
            if (currentStamina > maxStamina) {
                currentStamina = maxStamina;
            }
        }
    }

    Vector2 dir = {0, 0};
    if (IsKeyDown(KEY_W)) dir.y -= 1;
    if (IsKeyDown(KEY_S)) dir.y += 1;
    if (IsKeyDown(KEY_A)) dir.x -= 1;
    if (IsKeyDown(KEY_D)) dir.x += 1;

    if (dir.x != 0 || dir.y != 0) {
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        dir.x /= length;
        dir.y /= length;
    }

    pos.x += dir.x * speed * deltaTime;
    pos.y += dir.y * speed * deltaTime;
    pos.x = Clamp(pos.x, 0, WORLD_WIDTH - width);
    pos.y = Clamp(pos.y, 0, WORLD_HEIGHT - height);
}

void Player::draw() const {
    DrawRectangle(pos.x, pos.y, width, height, color);

    if (isReloading) {
        int blink = (int)(GetTime() * 10) % 2;
        if (blink) {
            DrawRectangle(pos.x + 4, pos.y + 4, width - 8, height - 8, RED);
        }
    }

    DrawRectangle(pos.x, pos.y - 12, width, 6, DARKGRAY);
    DrawRectangle(pos.x, pos.y - 12, width * (currentStamina / maxStamina), 6, ORANGE);
}

bool Player::shoot(std::vector<Bullet>& bullets, Camera2D& camera, Sound& shootSfx, Sound& reloadSfx) {
    if (isReloading) {
        return false;
    }

    if (currentAmmo <= 0) {
        isReloading = true;
        reloadTimer = reloadDelay;
        return false;
    }

    if (shootTimer <= 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        Vector2 center = {pos.x + width / 2, pos.y + height / 2};
        Vector2 toMouse = {
            mouseWorldPos.x - center.x,
            mouseWorldPos.y - center.y
        };

        float length = std::sqrt(toMouse.x * toMouse.x + toMouse.y * toMouse.y);
        if (length > 0) {
            toMouse.x /= length;
            toMouse.y /= length;
        }

        Vector2 bulletPos = {
            pos.x + width / 2 - 2,
            pos.y + height / 2 - 4
        };

        bullets.emplace_back(bulletPos, toMouse, 500.0f, damage, 2.0f, BulletOwner::PLAYER);
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
