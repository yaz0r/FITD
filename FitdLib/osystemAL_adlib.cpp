#include <stdlib.h>
#include <assert.h>

#include "common.h"
#include "osystemAL.h"

void osystem_playAdlib()
{
#if 0
	if(adlib_source)
	{
		alSourceStop(adlib_source);
		alDeleteSources(1, &adlib_source);
		alDeleteBuffers(NUM_MUSIC_BUFFER, adlib_buffers);
	}
	
	alGenSources(1, &adlib_source);
	alGenBuffers(NUM_MUSIC_BUFFER, adlib_buffers);	

	for(int bufferIdx=0; bufferIdx<NUM_MUSIC_BUFFER; bufferIdx++)
	{
		musicUpdate(NULL, adlibBuffer, BUFFER_SIZE);
	
		alBufferData(adlib_buffers[bufferIdx], AL_FORMAT_MONO16, adlibBuffer, BUFFER_SIZE, 44100);
	}
	alSourceQueueBuffers(adlib_source, NUM_MUSIC_BUFFER, adlib_buffers);

	alSourcePlay(adlib_source);

	checkALError();
#endif
}

void osystemAL_adlib_Update()
{
#if 0
	if(adlib_source)
	{
		ALint numProcessedBuffers;
		alGetSourcei(adlib_source, AL_BUFFERS_PROCESSED, &numProcessedBuffers);

		for(int i=0; i<numProcessedBuffers; i++)
		{
			ALuint buffer;
			alSourceUnqueueBuffers(adlib_source, 1, &buffer);

			musicUpdate(NULL, adlibBuffer, BUFFER_SIZE);
			alBufferData(buffer, AL_FORMAT_MONO16, adlibBuffer, BUFFER_SIZE, 44100);

			alSourceQueueBuffers(adlib_source, 1, &buffer);
		}

		checkALError();
	}
#endif
}
