/*----- t3d_slist.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_slist_h_
#define _t3d_slist_h_

#include <t3d_type.h>


struct __listnode {
  union {
    void *data;
    struct __list *slist;
    const char *str;
    int key;
  };
  struct __listnode *next;
};

struct __list {
  size_t size;  /* count of nodes */
  listnode *head;
  listnode *tail;
};


/* a prototype of callbacked function called by:
    - list_destroy()
    - list_traverse()
    - list_node_free()
    NULL for no use */
typedef void( *pfunc_list_callback )( listnode* node ); 


/* traverses a list, applied callback functionn for each node */
void list_traverse( list *in_list, pfunc_list_callback pfcb_traversenode );

/* allocates a empty list from heap, this creates a new list */
list *list_new( void );

/* clears a list and free memory, the list cannot be used later */
void list_del( list *in_list, pfunc_list_callback pfcb_freedata );

/* creates a new node assigned with data, not allocates for data */
listnode *list_node_new( void *data );

/* free a list node and it&apos;s associated nodes, the freed node cannot be used later */
void list_node_del( listnode* node, pfunc_list_callback pfcb_freedata );

/* creates a new node assigned with a key, not allocates for key */
listnode *list_key_new( int key );

/* finds prev node of given node */
listnode *list_find_prev( const list *in_list, const listnode *node );

/* appends a node to a list at back */
void list_push_back( list *in_list, listnode *node );

/* inserts a node in front of head into a list */
void list_push_front( list *in_list, listnode *in_node );

/* inserts a node after pos node into a list */
void list_insert_after( list *in_list, listnode *pos_node, listnode *in_node );

/* inserts a node before pos node into a list */
void list_insert_before( list *in_list, listnode *pos_node, listnode *in_node );

/* removes the first node from a list and returns it */
listnode *list_pop_front( list *in_list );

/* removes the last node from a list and returns it */
listnode *list_pop_back( list *in_list );

/* removes all nodes but for list itself */
void list_clear( list *in_list, pfunc_list_callback pfcb );

/* returns a copy of a list_t from heap */
list *list_copy( list in_list );

/* concatenates two lists into first list */
void list_concat( list *first, list *second );

/* gets count of nodes in the list */
size_t list_size(const list* in_list);

/* gets node by index: 0-based. 0 is head */
listnode *list_node_at( const list* in_list, const size_t index );

/* slices list off from begin to end, returns begin node
   - caller should free returned list nodes
   - begin and end can be same one */
listnode *list_slice( list *in_list, listnode *begin, listnode *end );


#endif   /* _t3d_slist_h_ */