/*----- t3d_util.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_util_h_
#define _t3d_util_h_

#include <t3d_type.h>


/* read file to a buffer in memory */
char *util_file_to_mem( const char *filename );
/* get the filename, strip off the path */
void util_get_fname( char *fname, const char *path );
/* read list of strings into a vector */
void util_file_to_vector( vector *v, const char *fname );
/* add file basename(strip off the directory name) to a hashtable paired with a value */
void util_add_basename_to_hash( hashtable *ht, const char *str, const void *value, const int value_sz );


#endif   /* _t3d_util_h_ */