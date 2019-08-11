/*----- t3d_vector.h ---------------------------------------------------------+
    Developped by: Edward LEI
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_vector_h_
#define _t3d_vector_h_

#include <string.h>
#include <t3d_type.h>


/* generic vector structure */
struct __vector {
  void *items;       /* pointer to dynamically allocated items */
  size_t capacity;   /* number of items that can be held in current storage */
  size_t size;       /* number of items */
  size_t item_size;  /* size (in bytes) of a single item */
};


/* returns a pointer to the item located at specified index */
static inline void *vector_at( const vector *v, const size_t index )
{
  return (char *)(v->items) + index*v->item_size;
}

/* returns a pointer to the first item */
static inline const void *vector_front( const vector *v )
{
  return vector_at(v, 0);
}

/* returns a pointer to the last item */
static inline const void *vector_back( const vector *v )
{
  return vector_at(v, v->size - 1);
}

/* checks whether the vector is empty */
static inline int vector_is_empty( const vector *v )
{
  return v->size == 0;
}

/* returns the number of items
static inline size_t vector_size( const vector *v )
{
  return v->size;
}
*/

/* returns current storage capacity
static inline size_t vector_capacity( const vector *v )
{
  return v->capacity;
}
*/

/* removes all items */
static inline void vector_clear( vector *v )
{
  v->size = 0;
}

/* replace an item */
static inline void vector_set( vector *v, const size_t index, const void *item )
{
  memcpy((char *)(v->items) + index*v->item_size, item, v->item_size);
}


/* creates a new empty vector */
vector *vector_new( const size_t item_size );
/* celetes a vector */
void vector_del( vector *v );
/* check if an item is contained within the vector */
int vector_contains( const vector *v, const void *item, int( *cmp)(const void *, const void *) );
/* reserve storage such that it can hold at last size items */
void vector_reserve( vector *v, const size_t size );
/* decrease capacity to fit actual size */
void vector_shrink( vector *v );
/* insert a single item at specified index */
void vector_insert( vector *v, const size_t index, const void *item );
/* erase a range of items */
void vector_erase_range( vector *v, const size_t first, const size_t last );
/* erase an item */
void vector_erase( vector *v, const size_t index );
/* appends given item to the end of the vector */
void vector_push( vector *v, const void *item );
/* removes the last item of the vector */
void vector_pop( vector *v );
/* resizes the vector to contain size items */
void vector_resize( vector *v, const size_t size );
/* append raw data to the end of the vector */
void vector_push_data( vector *v, const void *data, const size_t count );
/* insert raw data at specified index */
void vector_insert_data( vector *v, const size_t index, const void *data, const size_t count );
/* sort vector items according to cmp function */
void vector_sort( vector *v, int (*cmp)(const void *, const void *) );


#endif	/* _t3d_vector_h_ */
