#include <stdlib.h>
#include <assert.h>

//#include "OpenAL/al.h"
//#include "OpenAL/alc.h"

#include "soloud.h"
#include "soloud_file.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"

#include "common.h"
#include "osystemAL.h"

void osystemAL_mp3_Update();
void osystemAL_adlib_Update();

SoLoud::Soloud* gSoloud = NULL;

void osystemAL_init()
{
    gSoloud = new SoLoud::Soloud();
    gSoloud->init();
}

class ITD_AudioSource : public SoLoud::AudioSource
{
public:
    char* m_samples;
    int m_size;

    ITD_AudioSource(char* samplePtr, int size);
    virtual ~ITD_AudioSource();
    virtual SoLoud::AudioSourceInstance *createInstance();
};

class ITD_AudioInstance : public SoLoud::AudioSourceInstance
{
    ITD_AudioSource *mParent;
    unsigned int mOffset;
public:
    ITD_AudioInstance(ITD_AudioSource *aParent) : SoLoud::AudioSourceInstance()
    {
        mOffset = 0;
        mParent = aParent;
    }

    virtual unsigned int getAudio(float *aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize) override
    {
        for (int i = 0; i < aSamplesToRead; i++)
        {
            if (mOffset > mParent->m_size)
            {
                aBuffer[i] = 0.f;
            }
            else
            {
                aBuffer[i] = (((float)((unsigned char*)mParent->m_samples)[mOffset]) - 128.f) / 128.f;
                mOffset++;
            }
        }
        return aSamplesToRead;
    }

    virtual bool hasEnded()
    {
        if (mOffset > mParent->m_size)
        {
            return true;
        }

        return false;
    }
};

ITD_AudioSource::ITD_AudioSource(char* samplePtr, int size) : SoLoud::AudioSource()
{
    assert(samplePtr[26] == 1); //assert first block is of sound data type
    int sampleSize = (READ_LE_U32(samplePtr + 26) >> 8) - 2;

    int frequencyDiv = *(unsigned char*)(samplePtr + 30);
    //    int codecId = samplePtr[31];

    char* sampleData = samplePtr + 32;

    int sampleRate = 1000000 / (256 - frequencyDiv);

    m_samples = sampleData;
    m_size = sampleSize-1;

    mBaseSamplerate = sampleRate;
}

ITD_AudioSource::~ITD_AudioSource()
{
}

SoLoud::AudioSourceInstance* ITD_AudioSource::createInstance()
{
   return new ITD_AudioInstance(this);
}

void osystem_playSample(char* samplePtr,int size)
{
    if (g_gameId >= TIMEGATE)
    {
        SoLoud::Wav* pAudioSource = new SoLoud::Wav();
        pAudioSource->loadMem((u8*)samplePtr, size, true);
        gSoloud->play(*pAudioSource);
    }
    else
    {
        ITD_AudioSource* pAudioSource = new ITD_AudioSource(samplePtr, size);
        gSoloud->play(*pAudioSource);
    }
}

extern float gVolume;

void osystemAL_udpate()
{
    gSoloud->setGlobalVolume(gVolume);
}

SoLoud::DiskFile* pFile = NULL;
SoLoud::WavStream* pWavStream = NULL;

int osystem_playTrack(int trackId)
{
    if (pWavStream)
    {
        pWavStream->stop();
    }

    char filename[256];
    sprintf(filename, "%02d.wav", trackId);

    FILE* fHandle = fopen(filename, "rb");
    if (fHandle == NULL)
        return 0;

    pFile = new SoLoud::DiskFile(fHandle);
    pWavStream = new SoLoud::WavStream();
    pWavStream->loadFile(pFile);
    gSoloud->play(*pWavStream);

    return 0;
}
