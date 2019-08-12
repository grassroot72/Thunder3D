/*----- t3d_vorbis.c ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <string.h>
#include <AL/al.h>
#include <t3d_vorbis.h>


void
audiostream_init( audiostream *as )
{
  memset(as, 0, sizeof(audiostream));
  alGenSources(1, &as->source);
  alGenBuffers(2, as->buffers);
  as->bufferSize = 4096*8;
  /* we loop by default */
  as->shouldLoop = 1;
}

void
audiostream_deinit( audiostream *as )
{
  alDeleteSources(1, &as->source);
  alDeleteBuffers(2, as->buffers);
  stb_vorbis_close(as->stream);
  memset(as, 0, sizeof(audiostream));
}

int
audiostream_stream( audiostream *as, ALuint buffer )
{
  //Uncomment this to avoid VLAs
  //#define BUFFER_SIZE 4096*32
  #ifndef BUFFER_SIZE  //VLAs ftw
  #define BUFFER_SIZE (as->bufferSize)
  #endif
  ALshort pcm[BUFFER_SIZE];
  int size = 0;
	int result = 0;

  while(size < BUFFER_SIZE) {
    result = stb_vorbis_get_samples_short_interleaved(as->stream, as->info.channels, (pcm + size), (BUFFER_SIZE - size));
    if(result > 0) size += result*as->info.channels;
    else break;
  }

  if(size == 0) return 0;   /* 0 (FALSE) */

  alBufferData(buffer, as->format, pcm, size*sizeof(ALshort), as->info.sample_rate);
  as->totalSamplesLeft -= size;
  #undef BUFFER_SIZE

  return 1;
}

int
audiostream_open( audiostream* as, const char *filename )
{
  as->stream = stb_vorbis_open_filename((char *)filename, NULL, NULL);
  if(!as->stream) return 0;
  /* get file info */
  as->info = stb_vorbis_get_info(as->stream);
  if(as->info.channels == 2)  as->format = AL_FORMAT_STEREO16;
  else as->format = AL_FORMAT_MONO16;

  if(!audiostream_stream(as, as->buffers[0]))  return 0;
  if(!audiostream_stream(as, as->buffers[1]))  return 0;
  alSourceQueueBuffers(as->source, 2, as->buffers);
  alSourcePlay(as->source);

  as->totalSamplesLeft = stb_vorbis_stream_length_in_samples(as->stream)*as->info.channels;

  return 1;
}

int
audiostream_update( audiostream *as )
{
  ALint processed = 0;

  alGetSourcei(as->source, AL_BUFFERS_PROCESSED, &processed);

  while(processed--) {
    ALuint buffer = 0;
    alSourceUnqueueBuffers(as->source, 1, &buffer);

    if(!audiostream_stream(as, buffer)) {
      int shouldExit = 1;

      if(as->shouldLoop) {
        stb_vorbis_seek_start(as->stream);
        as->totalSamplesLeft = stb_vorbis_stream_length_in_samples(as->stream)*as->info.channels;
        shouldExit = !audiostream_stream(as, buffer);
      }

      if(shouldExit) return 0;
    }
    alSourceQueueBuffers(as->source, 1, &buffer);
  }

  return 1;
}

