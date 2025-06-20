#include "common.h"
#include "hybrid.h"

hqrEntryStruct<sHybrid>* HQ_Hybrides = nullptr;

sHybrid::sHybrid(uint8_t* buffer, int size) {
    assert(size == READ_LE_U32(buffer + 20)); // EOF offset

    for (int i = 0; i < 5; i++) {
        if (int startOffset = READ_LE_U32(buffer + 4 * i)) {
            int blockSize = 0;
            for (int j = i + 1; j < 6; j++) {
                if (int nextOffset = READ_LE_U32(buffer + 4 * j)) {
                    blockSize = nextOffset - startOffset;
                    break;
                }
            }
            assert(blockSize);

            switch (i) {
            case 0: // Entities
                readEntites(buffer + startOffset, blockSize);
                break;
            case 2: // Animation
                readAnimations(buffer + startOffset, blockSize);
                break;
            default:
                break;
            }
        }
    }
}

void sHybrid::readEntites(uint8_t* buffer, int size) {
    int count = READ_LE_U32(buffer) / 4;
    entities.reserve(count);
    for (int i = 0; i < count; i++) {
        uint8_t* data = buffer + READ_LE_U32(buffer + i * 4);
        
        auto& entity = entities.emplace_back();
        entity.flags = READ_LE_U16(data); data+=2;

        int numEntities = (entity.flags >> 8) & 0xFF;

        entity.parts.reserve(numEntities);
        for (int j = 0; j < numEntities; j++) {
            auto& part = entity.parts.emplace_back();

            part.id = READ_LE_U8(data); data++;
            part.nbpt = READ_LE_U8(data); data++;
            part.cout = READ_LE_U8(data); data++;
            part.cin = READ_LE_U8(data); data++;

            part.XY.resize(part.nbpt);
            for (int k = 0; k < part.nbpt; k++) {
                part.XY[k][0] = READ_LE_S16(data); data += 2;
                part.XY[k][1] = READ_LE_S16(data); data += 2;
            }
        }
    }
}

void sHybrid::readAnimations(uint8_t* buffer, int size) {
    int count = READ_LE_U32(buffer) / 4;
    anims.reserve(count);
    for (int i = 0; i < count; i++) {
        uint8_t* animStart = buffer + READ_LE_U32(buffer + i * 4);
        auto& anim = anims.emplace_back();
        anim.flag = READ_LE_U8(animStart); animStart++;
        anim.count = READ_LE_U8(animStart); animStart++;
        anim.animations.reserve(anim.count);
        for (int j = 0; j < anim.count; j++) {
            auto& animation = anim.animations.emplace_back();
            animation.id = READ_LE_U16(animStart); animStart += 2;
            animation.flag = READ_LE_U16(animStart); animStart += 2;
            animation.deltaX = READ_LE_S16(animStart); animStart += 2;
            animation.deltaY = READ_LE_S16(animStart); animStart += 2;
        }
    }
}

void AffHyb(int index, int X, int Y, sHybrid* pHybrid) {
    auto& entity = pHybrid->entities[index];
    for (int i = 0; i < entity.parts.size(); i++) {
        auto& part = entity.parts[i];
        switch (part.id) {
        case 6:
            for (int i = 0; i < part.XY.size()-1; i++) {
                osystem_draw3dLine(part.XY[i][0], part.XY[i][1], 0, part.XY[i+1][0], part.XY[i+1][1], 0, part.cin);
            }
            break;
        }
    }
}