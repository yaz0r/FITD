#ifndef _LIFE_
#define _LIFE_

enum enumLifeMacro
{
    LM_INVALID = -1,
    LM_DO_MOVE = 0,
    LM_ANIM_ONCE,
    LM_ANIM_ALL_ONCE,
    LM_BODY,
    LM_IF_EGAL,
    LM_IF_DIFFERENT,
    LM_IF_SUP_EGAL,
    LM_IF_SUP,
    LM_IF_INF_EGAL,
    LM_IF_INF,
    LM_GOTO,
    LM_RETURN,
    LM_END,
    LM_ANIM_REPEAT,
    LM_ANIM_MOVE,
    LM_MOVE,
    LM_HIT,//0x10
    LM_MESSAGE,
    LM_MESSAGE_VALUE,
    LM_VAR,
    LM_INC,
    LM_DEC,
    LM_ADD,
    LM_SUB,
    LM_LIFE_MODE,
    LM_SWITCH,
    LM_CASE,
    LM_CAMERA,
    LM_START_CHRONO,
    LM_MULTI_CASE,
    LM_FOUND,
    LM_LIFE,
    LM_DELETE, //0x20
    LM_TAKE,
    LM_IN_HAND,
    LM_READ,
    LM_ANIM_SAMPLE,
    LM_SPECIAL,
    LM_DO_REAL_ZV,
    LM_SAMPLE,
    LM_TYPE,
    LM_GAME_OVER,
    LM_MANUAL_ROT,
    LM_RND_FREQ,
    LM_MUSIC,
    LM_SET_BETA,
    LM_DO_ROT_ZV,
    LM_STAGE,
    LM_FOUND_NAME, // 0x30
    LM_FOUND_FLAG,
    LM_FOUND_LIFE,
    LM_CAMERA_TARGET,
    LM_DROP,
    LM_FIRE,
    LM_TEST_COL,
    LM_FOUND_BODY,
    LM_SET_ALPHA,
    LM_STOP_BETA,
    LM_DO_MAX_ZV,
    LM_PUT,
    LM_C_VAR,
    LM_DO_NORMAL_ZV,
    LM_DO_CARRE_ZV,
    LM_SAMPLE_THEN,
    LM_LIGHT,  // 0x40
    LM_SHAKING,
    LM_INVENTORY,
    LM_FOUND_WEIGHT,
    LM_UP_COOR_Y,
    LM_SPEED,
    LM_PUT_AT,
    LM_DEF_ZV,
    LM_HIT_OBJECT,
    LM_GET_HARD_CLIP,
    LM_ANGLE,
    LM_REP_SAMPLE,
    LM_THROW,
    LM_WATER,
    LM_PICTURE,
    LM_STOP_SAMPLE, 
    LM_NEXT_MUSIC, // 0x50
    LM_FADE_MUSIC,
    LM_STOP_HIT_OBJECT,
    LM_COPY_ANGLE,
    LM_END_SEQUENCE,
    LM_SAMPLE_THEN_REPEAT,
    LM_WAIT_GAME_OVER,
    LM_GET_MATRICE,
    LM_STAGE_LIFE,
    LM_CONTINUE_TRACK,
    LM_ANIM_RESET,
    LM_RESET_MOVE_MANUAL,
    LM_PLUIE,
    LM_ANIM_HYBRIDE_ONCE,
    LM_ANIM_HYBRIDE_REPEAT,
    LM_MODIF_C_VAR,
    LM_CALL_INVENTORY, // 0x60
    LM_BODY_RESET,
    LM_DEL_INVENTORY,
    LM_SET_INVENTORY,
    LM_PLAY_SEQUENCE,
    LM_2D_ANIM_SAMPLE,
    LM_SET_GROUND,
    LM_PROTECT,
    LM_DEF_ABS_ZV,
    LM_DEF_SEQUENCE_SAMPLE,
    LM_READ_ON_PICTURE,
    LM_FIRE_UP_DOWN, // AITD3 only

    // TIMEGATE
    LM_DO_ROT_CLUT, // 100
    LM_STOP_CLUT, // 101
    LM_IF_IN, // 102
    LM_IF_OUT, // 103
    LM_SET_VOLUME_SAMPLE, // 105
    LM_FADE_IN_MUSIC, // 108
    LM_SET_MUSIC_VOLUME, // 109
    LM_MUSIC_AND_LOOP, // 110
    LM_MUSIC_THEN, // 111
    LM_MUSIC_THEN_LOOP, // 112
    LM_START_FADE_IN_MUSIC, // 113
    LM_START_FADE_IN_MUSIC_THEN, // 114
    LM_START_FADE_IN_MUSIC_LOOP, // 115
    LM_FADE_OUT_MUSIC_STOP, // 116
    LM_MUSIC_ALTER_TEMPO, // 117
    LM_REP_SAMPLE_N_TIME, // 118
};

typedef enum enumLifeMacro enumLifeMacro;

extern enumLifeMacro AITD1LifeMacroTable[];
extern enumLifeMacro AITD2LifeMacroTable[];

void processLife(int lifeNum, bool callFoundLife);
int createFlow( int mode, int X, int Y, int Z, int stage, int room, int alpha, int beta, int gamma, ZVStruct* zvPtr);
void animMove(int a,int b,int c,int d,int e,int f,int g);
void doRealZv(tObject* actorPtr);
void setStage(int newStage, int newRoomLocal, int X, int Y, int Z);
void fire(int fireAnim, int X, int Y, int Z, int hitForce, int nextAnim);
void put(int x,int y,int z,int room,int stage,int alpha,int beta,int gamma,int idx);
void getHardClip();
void throwObj(int animThrow, int frameThrow, int arg_4, int objToThrowIdx, int throwRotated, int throwForce, int animNext);

#ifdef DEBUG
void appendFormated(const char* format, ...);
#else
#define appendFormated(format, ...) {}
#endif

#endif
