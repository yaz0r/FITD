#include "common.h"
#include "anim2d.h"
#include "hybrid.h"

std::array<sAnim2d, 20> TabAnim2d;
int NbAnim2D = 0;

sHybrid* PtrAnim2D = nullptr;
void load2dAnims(int cameraIdx) {
    char name[16];

    sprintf(name, "ANIM%02d", g_currentFloor);

    if (PtrAnim2D)
    {
        delete PtrAnim2D;
        PtrAnim2D = nullptr;
    }

    unsigned char* rawAnim2D = (unsigned char*)loadPak(name, cameraIdx);
    PtrAnim2D = new sHybrid(rawAnim2D, getPakSize(name, cameraIdx));
    free(rawAnim2D);
}

void handleAnim2d() {

    std::array<int, NUM_MAX_OBJECT> Index2 = Index;

    for (int i = 0; i < currentCameraZoneList[NumCamera]->hybrids.size(); i++) {
        auto& anim = TabAnim2d[i];
        if (timer - anim.time >= anim.pAnim->flag) {
            anim.frame++;
            if (anim.frame == anim.pAnimation->anims.size()) {
                anim.frame = 0;
                anim.pAnim = anim.pAnimation->anims.begin();
            }
            else {
                anim.pAnim++;
            }
            anim.time = timer;
        }

        int j;
        for (j = NbAffObjets + i - 1; j >= 0; j--) {
            if (!(Index2[j] & 0x8000)) {
                //TODO: test rects
            }
        }

        std::memmove(&Index2[j + 2], &Index2[j + 1], (NbAffObjets + i - j - 1) * sizeof(Index2[0]));
        Index2[j + 1] = 0x8000 + i;
    }

    Index = Index2;
}

void resetAnim2D() {
    NbAnim2D = 0;
}

void startAnim2d(int index) {
    auto& anim = PtrAnim2D->animations[index];

    TabAnim2d[NbAnim2D].pAnimation = &anim;
    TabAnim2d[NbAnim2D].pAnim = anim.anims.begin();
    TabAnim2d[NbAnim2D].frame = 0;
    TabAnim2d[NbAnim2D].time = timer;
    NbAnim2D++;
}