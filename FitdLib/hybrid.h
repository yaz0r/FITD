#pragma once

#include "sprite.h"

struct sHybrid_EntityPart {
    uint8_t id;
    uint8_t nbpt;
    uint8_t cout;
    uint8_t cin;
    std::vector<std::array<int16_t, 2>> XY;
};

struct sHybrid_Entity {
    uint16_t flags;

    std::vector<sHybrid_EntityPart> parts;
};

struct sHybrid_Anim {
    uint16_t id;
    uint16_t flag;
    int16_t deltaX;
    int16_t deltaY;
};

struct sHybrid_Animation {
    uint8_t flag;
    uint8_t count;
    std::vector<sHybrid_Anim> anims;
};

struct sHybrid {
    sHybrid(uint8_t* buffer, int size);

    std::vector<sHybrid_Entity> entities;
    std::vector<sHybrid_Sprite> sprites;
    std::vector<sHybrid_Animation> animations;

private:
    void readEntites(uint8_t* buffer, int size);
    void readSprites(uint8_t* buffer, int size);
    void readAnimations(uint8_t* buffer, int size);
};

extern hqrEntryStruct<sHybrid>* HQ_Hybrides;

void AffHyb(int index, int X, int Y, sHybrid* pHybrid);