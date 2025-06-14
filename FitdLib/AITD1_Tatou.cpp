#include "common.h"

#include "AITD1.h"

void clearScreenTatou(void)
{
	for(int i=0;i<45120;i++)
	{
		frontBuffer[i] = 0;
	}
}

int make3dTatou(void)
{
    char* tatou2d;
    char* tatou3d;
    int zoom;
    int deltaTime;
    int beta;
    int alpha;
    unsigned int localChrono;
    palette_t tatouPal;
    palette_t paletteBackup;

    tatou2d = CheckLoadMallocPak("ITD_RESS",AITD1_TATOU_MCG);
    tatou3d = CheckLoadMallocPak("ITD_RESS",AITD1_TATOU_3DO);
    char* tatouPalRaw = CheckLoadMallocPak("ITD_RESS",AITD1_TATOU_PAL);
    copyPalette(tatouPalRaw, tatouPal);

    zoom = 8920;
    deltaTime = 50;
    beta = 256;
    alpha = 8;

    SetProjection(160,100,128,500,490);

    copyPalette(currentGamePalette,paletteBackup);

    paletteFill(currentGamePalette,0,0,0);

    setPalette(currentGamePalette);

    copyPalette(tatouPal,currentGamePalette);
    FastCopyScreen(tatou2d+770,frontBuffer);
    FastCopyScreen(frontBuffer,aux2);

    osystem_CopyBlockPhys(frontBuffer,0,0,320,200);

    FadeInPhys(8,0);

    startChrono(&localChrono);

    do
    {
        process_events();

		//timeGlobal++;
		timer = timeGlobal;

        if(evalChrono(&localChrono)<=180) 
        {
            // before lightning strike
            if(key || Click)
            {
                break;
            }
        }
        else
        {
            // lightning strike

            /*  LastSample = -1;
            LastPriority = -1; */

            playSound(CVars[getCVarsIdx(SAMPLE_TONNERRE)]);

            /*     LastSample = -1;
            LastPriority = -1;*/

            paletteFill(currentGamePalette,63,63,63);
            setPalette(currentGamePalette);
            /*  setClipSize(0,0,319,199);*/

            clearScreenTatou();

            setCameraTarget(0,0,0,alpha,beta,0,zoom);

            AffObjet(0,0,0,0,0,0,tatou3d);

            //blitScreenTatou();
            osystem_CopyBlockPhys((unsigned char*)frontBuffer,0,0,320,200);

			process_events();

            copyPalette(tatouPal,currentGamePalette);
            setPalette(currentGamePalette);
			osystem_CopyBlockPhys((unsigned char*)frontBuffer,0,0,320,200);


            while(key==0 && Click == 0 && JoyD == 0)
            {
                // Armadillo rotation loop

                process_events();

                zoom += deltaTime;

                if(zoom>16000)
                    break;

                beta -=8;

                clearScreenTatou();

                setCameraTarget(0,0,0,alpha,beta,0,zoom);

                AffObjet(0,0,0,0,0,0,tatou3d);

                //blitScreenTatou();

                osystem_stopFrame();
            }

            break;
        }
    }while(1);

    free(tatou3d);
    free(tatou2d);

    if(key || Click || JoyD)
    {
        while(key)
        {
          process_events();
        }

        FadeOutPhys(32,0);
        copyPalette(paletteBackup,currentGamePalette);
        return(1);
    }
    else
    {
        FadeOutPhys(16,0);
        copyPalette(paletteBackup,currentGamePalette);
        return(0);
    }

    return(0);
}
