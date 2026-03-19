#include "player.h"

Player::Player(Vector2 startPos)
    : Entity(startPos, 32, 32, 100, GREEN), reloadSfxPtr(nullptr) {
    speed = 200.f;
    damage = 10;
    magazineSize = 8;
    currentAmmo = magazineSize;
    shootCD = 0.15f;
    shootTimer = 0.f;
    reloadDelay = 0.3f;
    reloadTime = 0.8f;
    reloadTimer = 0.f;
    isReloading = false;
}

void Player::update(float deltaTime) {
    if (shootTimer > 0) shootTimer -= deltaTime;

    if (isReloading) {
        if (reloadTimer > 0) {
            reloadTimer -= deltaTime;
        }
        else {
            if (currentAmmo < magazineSize) {
                currentAmmo++;
                if (reloadSfxPtr) {
                    PlaySound(*reloadSfxPtr);
                }
                if (currentAmmo < magazineSize) {
                    reloadTimer = reloadTime;
                }
                else {
                    isReloading = false;
                }
            }
            else {
                isReloading = false;
            }
        }
    }
    else {
        if (currentAmmo <= 0 && shootTimer <= 0) {
            isReloading = true;
            reloadTimer = reloadDelay;
        }
    }

    Vector2 dir = {0, 0};
    if (IsKeyDown(KEY_W)) dir.y -= 1;
    if (IsKeyDown(KEY_S)) dir.y += 1;
    if (IsKeyDown(KEY_A)) dir.x -= 1;
    if (IsKeyDown(KEY_D)) dir.x += 1;

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
}

bool Player::shoot(std::vector<Bullet>& bullets, Camera2D& camera, Sound& shootSfx, Sound& reloadSfx) {
    if (isReloading) return false;

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
        float length = sqrt(toMouse.x * toMouse.x + toMouse.y * toMouse.y);
        if (length > 0) {
            toMouse.x /= length;
            toMouse.y /= length;
        }

        Vector2 bulletPos = {
            pos.x + width / 2 - 2,
            pos.y + height / 2 - 4
        };

        bullets.emplace_back(bulletPos, toMouse, 500.f, damage, 2.f, BulletOwner::PLAYER);
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
