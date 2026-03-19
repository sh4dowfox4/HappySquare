#ifndef PLAYER_H
#define PLAYER_H
#include "entity.h"
#include "bullet.h"

class Player : public Entity {
private:
    float speed;
    int damage;
    int magazineSize;
    int currentAmmo;
    float shootCD;
    float shootTimer;
    float reloadDelay;
    float reloadTime;
    float reloadTimer;
    bool isReloading;
    Sound* reloadSfxPtr;

public:
    Player(Vector2 startPos);
    void update(float deltaTime) override;
    void draw() const override;
    bool shoot(std::vector<Bullet>& bullets, Camera2D& camera, Sound& shootSfx, Sound& reloadSfx);
    void setReloadSound(Sound& sfx) { reloadSfxPtr = &sfx; }
    int getCurrentAmmo() const { return currentAmmo; }
    int getMagazineSize() const { return magazineSize; }
    bool isReloadingNow() const { return isReloading; }
};
#endif // PLAYER_H
