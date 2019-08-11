/*----- t3d_util.c -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <t3d_vector.h>
#include <t3d_hashtable.h>


/* read file to a buffer in memory */
char *
util_file_to_mem( const char *filename )
{
  FILE *file;
  char *buffer;
  size_t size;

  file = fopen(filename, "rb");
  if(!file) {
    fprintf(stderr, "Unable to open file \"%s\".\n", filename);
    return 0;
  }

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);
  buffer = (char *)malloc((size + 1)*sizeof(char));
  fread(buffer, sizeof(char), size, file);
  buffer[size] = 0;
  fclose(file);

  return buffer;
}

/* get the filename, strip off the path */
void
util_get_fname( char *fname, const char *path )
{
  if(path[0] == '\0') {
    fname[0] = '\0';
	  return;
  }

  int len = strlen(path);
  int slen = len;
  const char *fromlast = path + len;

  while(slen--) {
    if(*fromlast == '\\') {
      strcpy(fname, fromlast + 1);  
      return;
    }
    fromlast--;
  }

  slen = len;
  fromlast = path + len;
  while(slen--) {
    if(*fromlast == '/') {
      strcpy(fname, fromlast + 1);  
      return;
    }
    fromlast--;
  }

  strcpy(fname, path);
}

/* read list of strings into a vector */
void
util_file_to_vector( vector *v, const char *fname )
{
  char line[512];
  FILE *fp;


  fp = fopen(fname, "rt");

  while(fgets(line, 512, fp) != NULL) {
    vector_push(v, line);
  }
  fclose(fp);
}

/* add file basename(strip off the directory name) to a hashtable paired with a value */
void
util_add_basename_to_hash( hashtable *ht, const char *str, const void *value, const int value_sz )
{
  char char_array[128];
  char *key_str;

  util_get_fname(char_array, str);
  key_str = strdup(char_array);

  hash_add(ht, key_str, value, value_sz);
  free(key_str);
}