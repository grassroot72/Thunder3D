/*----- t3d_hashtable.h ------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_hashtable_h_
#define _t3d_hashtable_h_

#include <t3d_type.h>


struct __hashentry;

struct __hashtable {
  unsigned int nchains;
  struct __hashentry **chain;
};


/*---------------------------------------------------------------------------+
   Hash a string, returning a 32bit value.  The hash algorithm used is
   Fowler Noll Vo - a very fast and simple hash, ideal for short strings.
   See http://en.wikipedia.org/wiki/Fowler_Noll_Vo_hash for more details.

   param  datum   The string to hash.
   param  len	   Pointer to unsigned integer to record datum's length in.
   return The calculated hash value for the datum 
 +---------------------------------------------------------------------------*/
static inline unsigned int hash_string_fnv( const char *datum, unsigned int *len )
{
  unsigned int z = 0x811c9dc5;
  const char *start = datum;
  *len = 0;

  if(datum == NULL)
    return 0;

  while(*datum) {
    z *= 0x01000193;
    z ^= *datum++;
  }
  *len = datum - start;

  return z;
}

/* create a new hash table, and return a context for it */
hashtable *hash_new( const unsigned int chains );
/* destroys a hash table, freeing all memory associated with it */
void hash_del( hashtable *ht );
/* adds a key/value pair to a hash table */
int hash_add( hashtable *ht, const char *key, const void *value, const int v_len );
/* looks up a the value associated with with a key from a specific hash table */
const void  *hash_get( const hashtable *ht, const char *key );
/* iterate through all available hash keys */
const char *hash_iterate( const hashtable *ht, unsigned int *c1, unsigned int **c2 );


#endif   /* _t3d_hashtable_h_ */
