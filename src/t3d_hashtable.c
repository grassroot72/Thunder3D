/*----- t3d_hashtable.h ------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <t3d_hashtable.h>


struct __hashentry {
  char *pairing;             /* block containing 'key\0value' */
  unsigned int key_length;   /* length of key */
  struct __hashentry *next;  /* next entry */
};


/*---------------------------------------------------------------------------+
  create a new hash table, and return a context for it. The memory consumption
  of a hash table is approximately 8 + (nchains * 12) bytes if it is empty.

  chains -  number of chains/buckets this hash table will have. This
            should be a prime number, and ideally a prime number just
            over a power of two, for best performance and distribution.
  return hashtable - containing the context of this hash table or NULL
            if there is insufficent memory to create it and its chains.
 +---------------------------------------------------------------------------*/
hashtable *
hash_new( const unsigned int chains )
{
  hashtable *r = (hashtable *)malloc(sizeof(hashtable));

  if(r == NULL)  return NULL;

  r->nchains = chains;
  r->chain = calloc(chains, sizeof(struct __hashentry *));

  if(r->chain == NULL) {
    free(r);
    return NULL;
  }

  return r;
}

/*---------------------------------------------------------------------------+
  destroys a hash table, freeing all memory associated with it.
 +---------------------------------------------------------------------------*/
void
hash_del( hashtable *ht )
{
  unsigned int i;

  if(ht == NULL)  return;

  for(i = 0; i < ht->nchains; i++) {
    if(ht->chain[i] != NULL) {
      struct __hashentry *e = ht->chain[i];
      while(e) {
        struct __hashentry *n = e->next;
        free(e->pairing);
        free(e);
        e = n;
      }
    }
  }

  free(ht->chain);
  free(ht);
}

/*---------------------------------------------------------------------------+
  adds a key/value pair to a hash table.  if the key you're adding is already
  in the hash table, it does not replace it, but it does take precedent over
  it.  the old key/value pair will be inaccessable but still in memory until
  hash_destroy() is called on the hash table.

  param  ht     the hash table context to add the key/value pair to.
  param  key    the key to associate the value with.  a copy is made.
  param  value  the value to associate the key with.  a copy is made.
  param  v_len  the length of the value item. if value is a string, v_len should
                be strlen(value) + 1.
  return true if the add succeeded, false (0) otherwise.
 +---------------------------------------------------------------------------*/
int
hash_add( hashtable *ht, const char *key, const void *value, const int v_len )
{
  unsigned int h, c;
  struct __hashentry *e;

  if(ht == NULL || key == NULL || value == NULL)
    return 0;

  e = (struct __hashentry *)malloc(sizeof(struct __hashentry));
  if(e == NULL)  return 0;

  h = hash_string_fnv(key, &(e->key_length));
  c = h%ht->nchains;

	e->pairing = malloc(v_len + e->key_length + 1);
  if(e->pairing == NULL) {
    free(e);
    return 0;
  }
  memcpy(e->pairing, key, e->key_length + 1);
  memcpy(e->pairing + e->key_length + 1, value, v_len);

  e->next = ht->chain[c];
  ht->chain[c] = e;

  return 1;
}

/*---------------------------------------------------------------------------+
  looks up a the value associated with with a key from a specific hash table.
 
  param  ht    the hash table context to look up the key in.
  param  key   the key to search for.
  return the value associated with the key, or NULL if it was not found.
 +---------------------------------------------------------------------------*/
const void *
hash_get( const hashtable *ht, const char *key )
{
  unsigned int h, c, key_length;
  struct __hashentry *e;

  if(ht == NULL || key == NULL)
    return NULL;

  h = hash_string_fnv(key, &key_length);
  c = h%ht->nchains;

  for(e = ht->chain[c]; e; e = e->next) {
    if((key_length == e->key_length) && (memcmp(key, e->pairing, key_length) == 0))
      return e->pairing + key_length + 1;
  }

  return NULL;
}

/*---------------------------------------------------------------------------+
  iterate through all available hash keys.

  param  ht  the hash table context to iterate.
  param  c1  pointer to first context
  param  c2  pointer to second context (set to 0 on first call)
  return the next hash key, or NULL for no more keys
 +---------------------------------------------------------------------------*/
const char *
hash_iterate( const hashtable *ht, unsigned int *c1, unsigned int **c2 )
{
  struct __hashentry **he = (struct __hashentry **)c2;

  if(ht == NULL)  return NULL;

  if(!*he)
    *c1 = -1;
  else
    *he = (*he)->next;

  if(*he)
    return (*he)->pairing;

  while(!*he) {
    (*c1)++;
    if(*c1 >= ht->nchains)
      return NULL;
    *he = ht->chain[*c1];
  }
  return (*he)->pairing;
}