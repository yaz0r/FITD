#include <stdlib.h>
#include <assert.h>

#if 1

#else

#include "OpenAL/al.h"
#include "OpenAL/alc.h"

#include "libmad-0.15.1b/mad.h"

#include "common.h"
#include "osystemAL.h"
#include "osystemAL_mp3.h"

struct mad_stream	mad_Stream;
struct mad_frame	mad_Frame;
struct mad_synth	mad_Synth;
mad_timer_t			mad_Timer;

unsigned char* pMp3Data = NULL;
unsigned long int mp3DataSize = 0;

#define NUM_MUSIC_BUFFER 3
#define NUM_SAMPLES_PER_BUFFER (1152 * 40 * 2) // 40 frames per buffer (a bit more than a second of playback), 2 channels
#define BUFFER_SIZE (NUM_SAMPLES_PER_BUFFER * 2) // 2 bytes per sample

ALuint mp3_source = 0;
ALuint mp3_buffers[NUM_MUSIC_BUFFER];

unsigned char decompressionBuffer[BUFFER_SIZE];
int decompressedSize = 0;
bool decompressionFinished = false;

bool bIsMp3Playing = false;

static inline int scale_sample(mad_fixed_t sample)
{
	// round
	sample += (1L << (MAD_F_FRACBITS - 16));

	// clip
	if (sample > MAD_F_ONE - 1)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	// quantize and scale to not saturate when mixing a lot of channels
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}

void decodeMP3()
{
	decompressedSize = 0;
	int16* pCurrentDecompressionBuffer = (int16*)decompressionBuffer;

	while((((unsigned char*)pCurrentDecompressionBuffer) < decompressionBuffer + BUFFER_SIZE) || decompressionFinished)
	{
		if(mad_frame_decode(&mad_Frame,&mad_Stream))
		{
			if(MAD_RECOVERABLE(mad_Stream.error))
			{
				/* Do not print a message if the error is a loss of
				* synchronization and this loss is due to the end of
				* stream guard bytes. (See the comments marked {3}
				* supra for more informations about guard bytes.)
				*/
				if(mad_Stream.error!=MAD_ERROR_LOSTSYNC)
				{
					fflush(stderr);
				}
				continue;
			}
			else
			{
				if(mad_Stream.error==MAD_ERROR_BUFLEN)
				{
					decompressionFinished = true;
					break;
				}
				else
				{
					assert(0);
				}
			}
		}

		mad_synth_frame(&mad_Synth, &mad_Frame);

		assert(mad_Synth.pcm.channels == 2);
		assert(mad_Synth.pcm.length == 1152);

		for(int sampleIdx=0; sampleIdx<mad_Synth.pcm.length; sampleIdx++)
		{
			*pCurrentDecompressionBuffer++ = (int16)scale_sample(mad_Synth.pcm.samples[0][sampleIdx]);
			*pCurrentDecompressionBuffer++ = (int16)scale_sample(mad_Synth.pcm.samples[1][sampleIdx]);
		}

		decompressedSize += mad_Synth.pcm.length * 2 * 2;
	}
}

int osystem_playTrack(int trackId)
{
	osystem_mp3_stop();

	char musicFileName[256];
	sprintf(musicFileName, "%d.mp3", trackId);

	FILE* fHandle = fopen(musicFileName, "rb");
	if(fHandle == NULL)
		return false;

	fseek(fHandle, 0, SEEK_END);
	mp3DataSize = ftell(fHandle);
	fseek(fHandle, 0, SEEK_SET);

	pMp3Data = new unsigned char[mp3DataSize];
	fread(pMp3Data, mp3DataSize, 1, fHandle);
	fclose(fHandle);

	alGenSources(1, &mp3_source);
	alGenBuffers(NUM_MUSIC_BUFFER, mp3_buffers);

	/* First the structures used by libmad must be initialized. */
	mad_stream_init(&mad_Stream);
	mad_frame_init(&mad_Frame);
	mad_synth_init(&mad_Synth);
	mad_timer_reset(&mad_Timer);

	mad_stream_buffer(&mad_Stream,pMp3Data,mp3DataSize);

	decompressionFinished = false;

	for(int bufferIdx=0; bufferIdx<NUM_MUSIC_BUFFER; bufferIdx++)
	{
		decodeMP3();
	
		alBufferData(mp3_buffers[bufferIdx], AL_FORMAT_STEREO16, decompressionBuffer, decompressedSize, 44100);
		assert( decompressionFinished == false );
	}
	alSourceQueueBuffers(mp3_source, NUM_MUSIC_BUFFER, mp3_buffers);
	alSourcePlay(mp3_source);

	checkALError();

	bIsMp3Playing = true;

	return true;
}

void osystem_mp3_stop()
{
	if(bIsMp3Playing)
	{
		alSourceStop(mp3_source);
		alDeleteSources(1, &mp3_source);
		alDeleteBuffers(NUM_MUSIC_BUFFER, mp3_buffers);

		bIsMp3Playing = false;
	}
}

void osystemAL_mp3_Update()
{
	if(bIsMp3Playing)
	{
		ALint numProcessedBuffers;
		alGetSourcei(mp3_source, AL_BUFFERS_PROCESSED, &numProcessedBuffers);

		if(decompressionFinished == false)
		{
			for(int i=0; i<numProcessedBuffers; i++)
			{
				ALuint buffer;
				alSourceUnqueueBuffers(mp3_source, 1, &buffer);

				decodeMP3();

				alBufferData(buffer, AL_FORMAT_STEREO16, decompressionBuffer, decompressedSize, 44100);

				alSourceQueueBuffers(mp3_source, 1, &buffer);
			}

			checkALError();
		}
	}
}
#endif
