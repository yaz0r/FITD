#include "common.h"
#include "sprite.h"

void sHybrid_Sprite::read(uint8_t* buffer, int bufferSize) {
    flags = READ_LE_U16(buffer); buffer+=2;
    dx = READ_LE_U8(buffer); buffer++;
    dy = READ_LE_U8(buffer); buffer++;

    lines.reserve(dy);
    for (int i = 0; i < dy; i++) {
        auto& line = lines.emplace_back();
        int numBlocks = READ_LE_U8(buffer); buffer++;
        line.blocks.reserve(numBlocks);
        for (int j = 0; j < numBlocks; j++) {
            auto& block = line.blocks.emplace_back();
            block.unk0 = READ_LE_U8(buffer); buffer++;
            block.numWords = READ_LE_U8(buffer); buffer++;
            block.numBytes = READ_LE_U8(buffer); buffer++;

            int dataSize = block.numWords * 4 + block.numBytes;
            block.data.resize(dataSize);
            for (int k = 0; k < dataSize; k++) {
                block.data[k] = READ_LE_U8(buffer); buffer++;
            }
        }
        line.unk = READ_LE_U8(buffer); buffer++;
    }
}

void AffSpr(int spriteNumber, int X, int Y, char* screen, std::vector<sHybrid_Sprite>& sprites) {
    sHybrid_Sprite& sprite = sprites[spriteNumber];

    Y -= sprite.dy;
    for (int y = 0; y < sprite.dy; y++) {
        auto& lineData = sprite.lines[y];
        
        char* lineStart = screen + ((y + Y) * 320 + X);
        for (int i = 0; i < lineData.blocks.size(); i++) {
            auto& block = lineData.blocks[i];
            lineStart += block.unk0;
            memcpy(lineStart, block.data.data(), block.data.size());
            lineStart += block.data.size();
        }
    }
}
