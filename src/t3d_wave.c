/*----- t3d_wave.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <AL/al.h>
#include <t3d_wave.h>


/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */
struct __riffheader {
  char chunk_id[4];
  int chunk_size;    /* size not including chunk_size or chunk_id */
  char format[4];
};

/*
 * Struct to hold fmt sub_chunk data for WAVE files.
 */
struct __waveformat {
  char sub_chunk_id[4];
  int sub_chunk_size;
  short audio_format;
  short n_channels;
  int sample_rate;
  int byte_rate;
  short block_align;
  short bits_per_sample;
};

/*
* Struct to hold the data of the wave file
*/
struct __wavedatainfo {
  char sub_chunk_id[4];  /* should contain the word data */
  int sub_chunk2_size;   /* stores the size of the data block */
};


typedef struct __riffheader   riffheader;
typedef struct __waveformat   waveformat;
typedef struct __wavedatainfo wavedatainfo;



/*
 * Load wave file function. No need for ALUT with this
 */
unsigned char *
wav_load_file( const char *filename, int *format, int *size, int *frequency )
{
  /* Local Declarations */
  FILE *soundFile = NULL;
  waveformat wavfmt;
  riffheader riffhdr;
  wavedatainfo datainfo;
  unsigned char *data;

  soundFile = fopen(filename, "rb");

  /* read in the first chunk into the struct */
  fread(&riffhdr, sizeof(riffheader), 1, soundFile);
 
  /* check for RIFF and WAVE tag in memeory */
  if((riffhdr.chunk_id[0] != 'R' ||
      riffhdr.chunk_id[1] != 'I' ||
      riffhdr.chunk_id[2] != 'F' ||
      riffhdr.chunk_id[3] != 'F') ||
     (riffhdr.format[0] != 'W' ||
      riffhdr.format[1] != 'A' ||
      riffhdr.format[2] != 'V' ||
      riffhdr.format[3] != 'E')) {
    fclose(soundFile);
    return NULL;
  }

  /* read in the 2nd chunk for the wave info */
  fread(&wavfmt, sizeof(waveformat), 1, soundFile);

  /* check for fmt tag in memory */
  if(wavfmt.sub_chunk_id[0] != 'f' ||
     wavfmt.sub_chunk_id[1] != 'm' ||
     wavfmt.sub_chunk_id[2] != 't' ||
     wavfmt.sub_chunk_id[3] != ' ') {
    fclose(soundFile);
    return NULL;
  }

  /* read in the the last byte of data before the sound file */
  fread(&datainfo, sizeof(wavedatainfo), 1, soundFile);

  /* check for data tag in memory */
  if(datainfo.sub_chunk_id[0] != 'd' ||
     datainfo.sub_chunk_id[1] != 'a' ||
     datainfo.sub_chunk_id[2] != 't' ||
     datainfo.sub_chunk_id[3] != 'a') {
    fclose(soundFile);
    return NULL;
  }

  /* allocate memory for data */
  data = (unsigned char *)malloc(sizeof(unsigned char)*datainfo.sub_chunk2_size);

  /* read in the sound data into the soundData variable */
  if(!fread(data, datainfo.sub_chunk2_size, 1, soundFile)) {
    free(data);
    fclose(soundFile);
    return NULL;
  }
  fclose(soundFile);

  /* now we fill the value of the OpenAL needed variables */
  *size = datainfo.sub_chunk2_size;
  *frequency = wavfmt.sample_rate;
  /* the format depends on the number of channels and the bits per sample */
  if(wavfmt.n_channels == 1) {
    if(wavfmt.bits_per_sample == 8)
      *format = AL_FORMAT_MONO8;
    else if(wavfmt.bits_per_sample == 16)
      *format = AL_FORMAT_MONO16;
  }
  else if(wavfmt.n_channels == 2) {
    if(wavfmt.bits_per_sample == 8)
      *format = AL_FORMAT_STEREO8;
    else if(wavfmt.bits_per_sample == 16)
      *format = AL_FORMAT_STEREO16;
  }

  return data;
}