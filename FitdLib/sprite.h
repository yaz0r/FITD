#pragma once

struct sHybrid_SpriteBlock {
    uint8_t unk0;
    uint8_t numWords;
    uint8_t numBytes;
    std::vector<uint8_t> data;
};

struct sHybrid_SpriteLine {
    std::vector<sHybrid_SpriteBlock> blocks;
    uint8_t unk;
};

struct sHybrid_Sprite {
    uint16_t flags;
    uint8_t dx;
    uint8_t dy;
    std::vector<sHybrid_SpriteLine> lines;

    void read(uint8_t* buffer, int bufferSize);
};

void AffSpr(int spriteNumber, int X, int Y, char* screen, std::vector<sHybrid_Sprite>& sprites);