#include "SpriteData.h"

GameLib::LoadTexture loadTexture[] = {
    { TEXNO::BACK00, L"./Data/Images/Back00.png",   4U }, 
    { TEXNO::SPR000, L"./Data/Images/SPR000.png",   1024U }, 
    { TEXNO::MISC,   L"./Data/Images/misc.png",     1U }, 
    { -1, nullptr, }, // èIóπÉtÉâÉO
};

SpriteUpperLeft sprBack(TEXNO::BACK00, 0, 0, 1280, 720);
SpriteCenter sprPlayer(TEXNO::SPR000, 256, 0, 128, 64);

SpriteCenter sprShot(TEXNO::SPR000, 64 * 5, 64 * 1, 64 * 1, 64 * 1);
SpriteCenter sprMissile(TEXNO::SPR000, 64 * 0, 64 * 4, 64 * 1, 64 * 1);

SpriteMiddleRight sprOption00(TEXNO::SPR000, 64 * 2, 64 * 1, 64 * 1, 64 * 1);
SpriteMiddleRight sprOption01(TEXNO::SPR000, 64 * 3, 64 * 1, 64 * 1, 64 * 1);
SpriteMiddleRight sprOption02(TEXNO::SPR000, 64 * 4, 64 * 1, 64 * 1, 64 * 1);

SpriteCenter sprEnemy(TEXNO::SPR000, 64 * 3, 64 * 3, 64 * 1, 64 * 1);
SpriteCenter sprEnemyShot00(TEXNO::SPR000, 64 * 4, 64 * 3, 64 * 1, 64 * 1);
SpriteCenter sprEnemyShot01(TEXNO::SPR000, 64 * 5, 64 * 3, 64 * 1, 64 * 1);

SpriteCenter sprBomb00(TEXNO::SPR000, 64 * 0, 64 * 2, 64 * 1, 64 * 1);
SpriteCenter sprBomb01(TEXNO::SPR000, 64 * 1, 64 * 2, 64 * 1, 64 * 1);
SpriteCenter sprBomb02(TEXNO::SPR000, 64 * 2, 64 * 2, 64 * 1, 64 * 1);
SpriteCenter sprBomb03(TEXNO::SPR000, 64 * 3, 64 * 2, 64 * 1, 64 * 1);
SpriteCenter sprBomb04(TEXNO::SPR000, 64 * 4, 64 * 2, 64 * 1, 64 * 1);

SpriteUpperLeft sprMeter(TEXNO::MISC, 0, 32 * 0, 512, 32);
SpriteUpperLeft sprFrame(TEXNO::MISC, 0, 32 * 1, 512, 32);
