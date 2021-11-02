#include "common.h"

#include "AITD1.h"

void clearScreenTatou(void)
{
	int i;

	for(i=0;i<45120;i++)
	{
		frontBuffer[i] = 0;
	}
}

int make3dTatou(void)
{
    char* tatou2d;
    char* tatou3d;
    unsigned char* tatouPal;
    int time;
    int deltaTime;
    int rotation;
    int unk1;
    unsigned char paletteBackup[768];
    unsigned int localChrono;

    tatou2d = loadPakSafe("ITD_RESS",AITD1_TATOU_MCG);
    tatou3d = loadPakSafe("ITD_RESS",AITD1_TATOU_3DO);
    tatouPal = (unsigned char*)loadPakSafe("ITD_RESS",AITD1_TATOU_PAL);

    time = 8920;
    deltaTime = 50;
    rotation = 256;
    unk1 = 8;

    setupCameraProjection(160,100,128,500,490);

    copyPalette(currentGamePalette,paletteBackup);

    paletteFill(currentGamePalette,0,0,0);

    setPalette(currentGamePalette);

    copyPalette(tatouPal,currentGamePalette);
    copyToScreen(tatou2d+770,frontBuffer);
    copyToScreen(frontBuffer,aux2);

    osystem_CopyBlockPhys((unsigned char*)frontBuffer,0,0,320,200);

    FadeInPhys(8,0);

    startChrono(&localChrono);

    do
    {
        process_events();

		//timeGlobal++;
		timer = timeGlobal;

        if(evalChrono(&localChrono)<=180) // avant eclair
        {
            if(key || click)
            {
                break;
            }
        }
        else // eclair
        {
            /*  soundVar2 = -1;
            soundVar1 = -1; */

            playSound(CVars[getCVarsIdx(SAMPLE_TONNERRE)]);

            /*     soundVar2 = -1;
            soundVar1 = -1;*/

            paletteFill(currentGamePalette,63,63,63);
            setPalette(currentGamePalette);
            /*  setClipSize(0,0,319,199);*/

            clearScreenTatou();

            setCameraTarget(0,0,0,unk1,rotation,0,time);

            AffObjet(0,0,0,0,0,0,tatou3d);

            //blitScreenTatou();
            osystem_CopyBlockPhys((unsigned char*)frontBuffer,0,0,320,200);

			process_events();

            copyPalette(tatouPal,currentGamePalette);
            setPalette(currentGamePalette);
			osystem_CopyBlockPhys((unsigned char*)frontBuffer,0,0,320,200);


            while(key==0 && click == 0 && JoyD == 0) // boucle de rotation du tatou
            {
                process_events();

                time+=deltaTime-25;

                if(time>16000)
                    break;

                rotation -=8;

                clearScreenTatou();

                setCameraTarget(0,0,0,unk1,rotation,0,time);

                AffObjet(0,0,0,0,0,0,tatou3d);

                //blitScreenTatou();

                osystem_stopFrame();
            }

            break;
        }
    }while(1);

    free(tatouPal);
    free(tatou3d);
    free(tatou2d);

    if(key || click || JoyD)
    {
        while(key)
        {
          process_events();
        }

        fadeOut(32,0);
        copyPalette((unsigned char*)paletteBackup,currentGamePalette);
        return(1);
    }
    else
    {
        fadeOut(16,0);
        copyPalette((unsigned char*)paletteBackup,currentGamePalette);
        return(0);
    }

    return(0);
}
