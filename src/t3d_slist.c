/*----- t3d_slist.c ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <assert.h>
#include <t3d_slist.h>


void
list_traverse( list *in_list, pfunc_list_callback pfcb )
{
  listnode *node;

  if(pfcb) {
    node = in_list->head;
    while(node) {
      (*pfcb)(node);
      node = node->next;
    }
  }
}

list *
list_new( void )
{
  list *out_list = (list *)malloc(sizeof(list));

  out_list->size = 0;
  out_list->head = out_list->tail = NULL;

  return out_list;
}
 
void
list_del( list *in_list, pfunc_list_callback pf )
{
  list_clear(in_list, pf); 
  free(in_list);
}
 
listnode *
list_node_new( void *data )
{
  listnode *node = (listnode *)malloc(sizeof(listnode));

  node->next = NULL;
  node->data = data;

  return node;
}
 
void
list_node_del( listnode* node, pfunc_list_callback pfcb )
{
  listnode *next;

  while(node) {
    next = node->next;

    if(pfcb)  (*pfcb)(node);
    free(node);
    node = next;
  }
}
 
listnode *
list_key_new( int key )
{
  listnode *node = (listnode *)malloc(sizeof(listnode));

  node->next = NULL;
  node->key = key;

  return node;
}
 
listnode *
list_find_prev( const list *in_list, const listnode *node )
{
  listnode* prev;

  assert(node);
  prev = in_list->head;

  if(prev==node)  return NULL; 

  while(prev && prev->next != node) {
    prev = prev->next;
  }
  assert(prev);

  return prev;
}
 
void
list_push_back( list *in_list, listnode *node )
{
  node->next = NULL;

  if(in_list->head) {
    in_list->tail->next = node;
    in_list->tail = node;
  }
  else
    in_list->head = in_list->tail = node;

  in_list->size++;
}
 
void
list_push_front( list *in_list, listnode *in_node )
{
  in_node->next = in_list->head;
  in_list->head = in_node;

  if(!in_node->next)
    in_list->tail = in_node;

  in_list->size++;
}
 
void
list_insert_after( list *in_list, listnode *pos_node, listnode *in_node )
{
  in_node->next = pos_node->next;
  pos_node->next = in_node;

  if(in_list->tail == pos_node)
    in_list->tail = in_node;

  in_list->size++;
}
 
void
list_insert_before( list *in_list, listnode *pos_node, listnode *in_node )
{
  listnode *prev_node;

  prev_node = list_find_prev(in_list, pos_node);

  if(prev_node)
    list_insert_after(in_list, prev_node, in_node);
  else
    list_push_front(in_list, in_node);
}
 
listnode *
list_pop_front( list *in_list )
{
  listnode *pop_node = NULL;

  if(in_list->head) {
    pop_node = in_list->head;
    in_list->head = in_list->head->next;

    if(in_list->head == NULL)
      in_list->tail = NULL;

    pop_node->next = NULL;
    in_list->size--;
  }
  assert(in_list->size >= 0);
 
  return pop_node;
}
 
listnode *
list_pop_back( list *in_list )
{
  listnode *pop_node = in_list->tail;

  if(in_list->head==pop_node) {
    in_list->size = 0;
    in_list->head = in_list->tail = NULL;
  }
  else {
    assert(pop_node && !pop_node->next);
    in_list->tail = list_find_prev(in_list, pop_node);

    assert(in_list->tail);
    in_list->tail->next = NULL;

    in_list->size--;
  }
  assert(in_list->size >= 0);

  return pop_node;
}
 
void
list_clear( list *in_list, pfunc_list_callback pfcb )
{
  listnode *node;

  if(pfcb) {
    while((node = list_pop_front(in_list))) {
      (*pfcb)(node);
      free(node);
    }
  }
  else {
    while((node = list_pop_front(in_list))) {
      free(node);
    }
  }
  assert(in_list->size == 0);
}
 
list *
list_copy( list in_list )
{
  list *newlist = (list *)malloc(sizeof(list));
  *newlist = in_list;

  return newlist;
}
 
void
list_concat( list *first, list *second )
{
  if(first->head) {
    if(second->head) {
      first->tail->next = second->head;
      first->tail = second->tail;
    }
  }
  else
    *first = *second;

  second->head = second->tail = NULL;
  first->size += second->size;
}

size_t
list_size( const list *in_list )
{
  return in_list->size;
}

listnode * 
list_node_at( const list *in_list, const size_t index )
{
  size_t i = 0;

  listnode *node = in_list->head;
  assert(index >= 0 && index < in_list->size);

  while(i < index) {
    node = node->next;
    i++;
  }

  return node;
}
 
listnode *
list_slice( list *in_list, listnode *begin, listnode *end )
{
  listnode* prev;

  assert(end);
  prev = list_find_prev(in_list, begin);

  if(prev == NULL) {
    assert(begin == in_list->head);
    in_list->head = end->next;
    end->next = NULL;

    if(in_list->tail == end)
      in_list->tail = in_list->head;
  }
  else {
    prev->next = end->next;
    end->next = NULL;

    if(in_list->tail == end)
      in_list->tail = prev;
  }

  in_list->size = 0;
  prev = in_list->head;

  while(prev) {
    in_list->size++;
    prev = prev->next;
  }

  return begin;
}