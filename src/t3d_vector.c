/*----- t3d_vector.c ---------------------------------------------------------+
    Developped by: Edward LEI
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <t3d_vector.h>


/* creates a new empty vector */
vector *
vector_new( const size_t item_size )
{
  vector *v = (vector *)malloc(sizeof(vector));

  if(!v) {
    fprintf(stderr, "line %d: Can not allocate memory\n", __LINE__);
    exit(EXIT_FAILURE);
  }
  v->item_size = item_size;
  v->size = 0;
  v->capacity = 1;
  v->items = malloc(v->item_size*v->capacity);
  return v;
}

/* deletes a vector */
void
vector_del( vector *v )
{
  free(v->items);
  free(v);
}

/* check if an item is contained within the vector */
int
vector_contains( const vector *v, const void *item, int( *cmp)(const void *, const void *) )
{
  size_t i;

  for(i=0; i<v->size; ++i) {
    if((*cmp)(item, vector_at(v, i)) == 0) {
      return 1;
    }
  }
  return 0;
}

/* reserve storage such that it can hold at last size items */
void
vector_reserve( vector *v, const size_t size )
{
  if(v->capacity < size) {
    v->items = realloc(v->items, size*v->item_size);
    v->capacity = size;
  }
}

/* decrease capacity to fit actual size */
void
vector_shrink( vector *v )
{
  if(v->capacity > v->size) {
    v->items = realloc(v->items, v->size*v->item_size);
  }
  v->capacity = v->size;
}

/* insert a single item at specified index */
void
vector_insert( vector *v, const size_t index, const void *item )
{
  if(v->capacity <= v->size) {
    vector_reserve(v, 2*v->capacity);
  }
  if(index < v->size) {
    memmove((char *)(v->items) + (index + 1)*v->item_size,
            (char *)(v->items) + (index + 0)*v->item_size,
            (v->size - index)*v->item_size);
  }
  v->size++;
  vector_set(v, index, item);
}

/* erase a range of items */
void
vector_erase_range( vector *v, const size_t first, const size_t last )
{
  memmove((char *)(v->items) + first*v->item_size,
          (char *)(v->items) + last*v->item_size,
          (v->size - last)*v->item_size);
  v->size -= (last-first);
}


/* erase an item */
void
vector_erase( vector *v, const size_t index )
{
  vector_erase_range(v, index, index + 1);
}

/* appends given item to the end of the vector */
void
vector_push( vector *v, const void *item )
{
  vector_insert(v, v->size, item);
}

/* Removes the last item of the vector */
void vector_pop( vector *v )
{
  v->size--;
}

/* resizes the vector to contain size items */
void
vector_resize( vector *v, const size_t size )
{
  if(size > v->capacity) {
    vector_reserve(v, size);
    v->size = v->capacity;
  }
  else
    v->size = size;
}

/* append raw data to the end of the vector */
void
vector_push_data( vector *v, const void *data, const size_t count )
{
  if(v->capacity < (v->size + count)) {
    vector_reserve(v, v->size + count);
  }
  memmove((char *)(v->items) + v->size*v->item_size,
           data,
           count*v->item_size);
  v->size += count;
}

/* insert raw data at specified index */
void
vector_insert_data( vector *v, const size_t index, const void *data, const size_t count )
{
  if(v->capacity < (v->size + count) ) {
    vector_reserve(v, v->size + count);
  }
  memmove((char *)(v->items) + (index + count )*v->item_size,
          (char *)(v->items) + (index)*v->item_size,
           count*v->item_size);
  memmove((char *)(v->items) + index*v->item_size,
           data,
           count*v->item_size );
  v->size += count;
}

/* sort vector items according to cmp function */
void
vector_sort( vector *v, int (*cmp)(const void *, const void *) )
{
  qsort(v->items, v->size, v->item_size, cmp);
}
