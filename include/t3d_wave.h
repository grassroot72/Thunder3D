/*----- t3d_wave.h ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_wave_h_
#define _t3d_wave_h_

#include <t3d_type.h>


/* load wave file function. No need for ALUT with this */
unsigned char *wav_load_file( const char *filename, int *format, int *size, int *frequency );


#endif   /* _t3d_audio_h_ */