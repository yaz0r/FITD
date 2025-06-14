#include "common.h"

char* pAITD2InventorySprite = NULL;

// ITD_RESS mapping
#define AITD2_CADRE_SPF						0
#define AITD2_ITDFONT						1
#define AITD2_LETTRE						2
#define AITD2_LIVRE							3
#define AITD2_CARNET						4
#define AITD2_CYM11001						5
#define AITD2_CYM11011						6
#define AITD2_CYM11012						7
#define AITD2_CYM11013						8
#define AITD2_PRO07011						9
#define AITD2_PRO07012						10
#define AITD2_PRO07013						11
#define AITD2_PRO08001						12
#define AITD2_PRO08008						13
#define AITD2_INVENTAIRE_PIRATE				14
#define AITD2_INVENTAIRE_GANG				15
#define AITD2_INVENTAIRE_GRACE				16
#define AITD2_OPTION_SCREEN					17
#define AITD2_SPRITES_INVENTAIRE			18

int AITD2KnownCVars[]=
{
	SAMPLE_PAGE,
	BODY_FLAMME,
	MAX_WEIGHT_LOADABLE,
	SAMPLE_CHOC,
	DEAD_PERSO,
	JET_SARBACANE,
	TIR_CANON,
	JET_SCALPEL,
	POIVRE,
	DORTOIR,
	EXT_JACK,
	NUM_MATRICE_PROTECT_1,
	NUM_MATRICE_PROTECT_2,
	NUM_PERSO,
	TYPE_INVENTAIRE,
	PROLOGUE,
	POIGNARD,
	-1
};

enumLifeMacro AITD2LifeMacroTable[] = 
{
	LM_DO_MOVE,         //0        
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
	LM_HIT,             //10
	LM_MESSAGE,         
	LM_VAR,                     
	LM_INC,                     
	LM_DEC,                     
	LM_ADD,                     
	LM_SUB,                     
	LM_LIFE_MODE,               
	LM_SWITCH,                  
	LM_CASE,                    
	LM_START_CHRONO,            
	LM_MULTI_CASE,              
	LM_FOUND,                   
	LM_LIFE,                    
	LM_DELETE,                  
	LM_TAKE,                    
	LM_IN_HAND,         //20        
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
	LM_FOUND_NAME,              
	LM_FOUND_FLAG,              
	LM_FOUND_LIFE,      //30        
	LM_CAMERA_TARGET,           
	LM_DROP,                    
	LM_FIRE,                    
	LM_TEST_COL,                
	LM_FOUND_BODY,              
	LM_SET_ALPHA,               
	LM_DO_MAX_ZV,               
	LM_PUT,                     
	LM_DO_NORMAL_ZV,            
	LM_DO_CARRE_ZV,             
	LM_SAMPLE_THEN,             
	LM_LIGHT,                   
	LM_SHAKING,                 
	LM_INVENTORY,               
	LM_FOUND_WEIGHT,            
	LM_PUT_AT,          //40         
	LM_DEF_ZV,                  
	LM_HIT_OBJECT,              
	LM_ANGLE,                   
	LM_REP_SAMPLE,              
	LM_THROW,                   
	LM_WATER,                   
	LM_PICTURE,                 
	LM_STOP_SAMPLE,             
	LM_NEXT_MUSIC,              
	LM_FADE_MUSIC,              
	LM_STOP_HIT_OBJECT,         
	LM_COPY_ANGLE,              
	LM_SAMPLE_THEN_REPEAT,      
	LM_WAIT_GAME_OVER,          
	LM_GET_MATRICE,             
	LM_STAGE_LIFE,              //50
	LM_CONTINUE_TRACK,          
	LM_ANIM_RESET,              
	LM_RESET_MOVE_MANUAL,       
	LM_PLUIE,                   
	LM_ANIM_HYBRIDE_ONCE,       
	LM_ANIM_HYBRIDE_REPEAT,     
	LM_MODIF_C_VAR,             
	LM_CALL_INVENTORY,          
	LM_BODY_RESET,              
	LM_DEL_INVENTORY,           
	LM_SET_INVENTORY,           
	LM_PLAY_SEQUENCE,           
	LM_2D_ANIM_SAMPLE,          
	LM_SET_GROUND,              
	LM_PROTECT,                 
	LM_DEF_ABS_ZV,              //60
	LM_DEF_SEQUENCE_SAMPLE,
	LM_READ_ON_PICTURE,
	LM_FIRE_UP_DOWN, // AITD3 only

    // TIMEGATE
    LM_DO_ROT_CLUT, // 100
    LM_STOP_CLUT, // 101
    LM_IF_IN, // 102
    LM_IF_OUT, // 103
    LM_INVALID,
    LM_SET_VOLUME_SAMPLE, // 105
    LM_INVALID,
    LM_INVALID,
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

int AITD2MusicToTrackMapping[21] = {
	21,
	9,
	7,
	5,
	4,
	18,
	8,
	10,
	14,
	11,
	12,
	6,
	13,
	20,
	15,
	2,
	3,
	16,
	17,
	19,
	22
};

void startAITD2()
{
	fontHeight = 14;
	g_gameUseCDA = true;
	pAITD2InventorySprite = loadPak("ITD_RESS", AITD2_SPRITES_INVENTAIRE);
	assert(pAITD2InventorySprite);
	
	startGame(8,0,0); // intro

	while(1)
	{
		int startupMenuResult = MainMenu();

		switch(startupMenuResult)
		{
		case -1: // timeout
			{
				break;
			}
		case 0: // new game
			{
				startGame(8,7,1);

				break;
			}
		case 1: // continue
			{
				if(restoreSave(12,0))
				{
					//          updateShaking();

					FlagInitView = 2;

					InitView();

					PlayWorld(1, 1);

					//          freeScene();

					FadeOutPhys(8,0);
				}

				break;
			}
		case 2: // exit
			{
				freeAll();
				exit(-1);

				break;
			}
		}
	}
}

void drawInventoryAITD2()
{
	switch (CVars[getCVarsIdx(TYPE_INVENTAIRE)])
	{
	case 0:
		LoadPak("ITD_RESS", AITD2_INVENTAIRE_PIRATE, logicalScreen);
		break;
	case 1:
		LoadPak("ITD_RESS", AITD2_INVENTAIRE_GANG, logicalScreen);
		break;
	case 2:
		LoadPak("ITD_RESS", AITD2_INVENTAIRE_GRACE, logicalScreen);
		break;
	default:
		assert(0);
	}

	statusLeft = 27;
	statusTop = 100;
	statusRight = 159;
	statusBottom = 174;

	SetProjection(((statusRight-statusLeft)/2)+statusLeft,((statusBottom-statusTop)/2) + statusTop,128,400,390);
}

int	TabXSprite [3] = {127,118,124};
int	TabYSprite [3] = {136,104,131};

void redrawInventorySpriteAITD2()
{
	int inventoryType = CVars[getCVarsIdx(TYPE_INVENTAIRE)];

	AffSpfI(TabXSprite[inventoryType], TabYSprite[inventoryType], inventoryType, pAITD2InventorySprite);
}

void AITD2_ReadBook(int index, int type)
{
	switch(type)
	{
	case 0: // READ_MESSAGE
		{
			LoadPak("ITD_RESS", AITD2_LETTRE, aux);
			palette_t lpalette;
			copyPalette(aux+64000,lpalette);
			convertPaletteIfRequired(lpalette);
			copyPalette(lpalette,currentGamePalette);
			setPalette(lpalette);
			osystem_CopyBlockPhys((unsigned char*)aux,0,0,320,200);
			turnPageFlag = 0;
			Lire(index, 60, 10, 245, 190, 0, 124, 124);
			break;
		}
	case 1: // READ_BOOK
		{
			LoadPak("ITD_RESS", AITD2_LIVRE, aux);
            palette_t lpalette;
			copyPalette(aux+64000,lpalette);
			convertPaletteIfRequired(lpalette);
			copyPalette(lpalette,currentGamePalette);
			setPalette(lpalette);
			osystem_CopyBlockPhys((unsigned char*)aux,0,0,320,200);
			turnPageFlag = 1;
			Lire(index, 60, 10, 245, 190, 0, 124, 124);
			break;
		}
	case 2: // READ_CARNET
		{
			LoadPak("ITD_RESS", AITD2_CARNET, aux);
            palette_t lpalette;
			copyPalette(aux+64000,lpalette);
			convertPaletteIfRequired(lpalette);
			copyPalette(lpalette,currentGamePalette);
			setPalette(lpalette);
			osystem_CopyBlockPhys((unsigned char*)aux,0,0,320,200);
			turnPageFlag = 0;
			Lire(index, 60, 10, 245, 190, 0, 124, 124);
			break;
		}
	default:
		assert(0);
	}
}
