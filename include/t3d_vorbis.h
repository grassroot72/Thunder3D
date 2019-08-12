/*----- t3d_vorbis.h --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_vorbis_h_
#define _t3d_vorbis_h_

#include <stb_vorbis.h>
#include <t3d_type.h>


struct __audiostream {
  ALuint ID;

  stb_vorbis *stream;
  stb_vorbis_info info;

  ALuint buffers[2];
  ALuint source;
  ALenum format;

  size_t bufferSize;

  size_t totalSamplesLeft;

	int shouldLoop;
};


/* initialize the audio stream */
void audiostream_init( audiostream *as );
/* deinitialize the audio stream */
void audiostream_deinit( audiostream *as );
/* audio stream */
int audiostream_stream( audiostream *as, ALuint buffer );
/* audio stream update */
int audiostream_update( audiostream *as );


#endif   /* _t3d_vorbis_h_ */