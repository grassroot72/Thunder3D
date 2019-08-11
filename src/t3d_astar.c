/*----- t3d_astar.c ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_math.h>
#include <t3d_patchmap.h>
#include <t3d_astar.h>


#define ASTAR_MAX_GEN_NODES 4
#define ASTAR_WALK_COST 10


/* calculates the "manhattan distance" between two cells */
static int
astar_manhattan( const ivec2 *n, const ivec2 *dest )
{
  return ASTAR_WALK_COST*(abs(n->x - dest->x) + abs(n->y - dest->y));
}

/* expands nodes from a initial one */
static int
astar_gen_nodes( ivec2 **gen, const ivec2 *center, const patchmap *pm )
{
  int n_nodes = ASTAR_MAX_GEN_NODES;
  int c = 0;
	
  *gen = (ivec2 *)malloc(sizeof(ivec2)*n_nodes);
	
  /* initial node is on first or last row */
  if(!center->x || center->x == pm->w_cells)  n_nodes--;
  /* initial node is on first or last col */
  if(!center->y || center->y == pm->h_cells)  n_nodes--;

  /* x > 0 (left) */
  if(center->x) {
    (*gen)[c].x = center->x - 1;
    (*gen)[c].y = center->y; 
    c++;
  }

  /* y < pm->h_cells - 1 (bottom) */
  if(center->y < pm->h_cells - 1) {
    (*gen)[c].x = center->x;
    (*gen)[c].y = center->y + 1; 
    c++;
  }
	
  /* x < pm->w_cells - 1 (right) */
  if(center->x < pm->w_cells - 1) {
    (*gen)[c].x = center->x + 1;
    (*gen)[c].y = center->y; 
    c++;
  }

  /* y > 0 (up) */
  if(center->y) {
    (*gen)[c].x = center->x;
    (*gen)[c].y = center->y - 1; 
    c++;
  }

  return c;
}

/* inserts node into a list of anode ordered by f, then by g values */
static void
astar_insert_node( anode **list, anode *node )
{
  /* travel node */
  anode *trav = *list;

  /* new first node */
  if(trav == NULL || trav->f > node->f) {
    node->next = trav;
    *list = node;
    return ;
  }

  while(trav->next != NULL) {
    /* middle insertion */
    if(trav->next->f >= node->f) {
      /* move on for equal f value */
      while(trav->next->f == node->f && trav->next->g < node->g) {
        trav = trav->next;
        if(trav->next == NULL) break;
      }

      node->next = trav->next;
      trav->next = node;
      return;
    }
    trav = trav->next;
  }

  /* new last node */
  node->next = NULL;
  trav->next = node;
}

/* insert a node as head of a list of anode */
static void
astar_insert_head_node( anode **list, anode *node )
{
  /* new first node */
  node->next = *list;
  *list = node;
}

/* looks for a node into a list */
static anode *
astar_get_node( anode *list, const ivec2 *n )
{
  /* empty list */
  if(list == NULL)  return NULL;

  /* moving on! */
  while(list->next != NULL) {
    if(list->x == n->x && list->y == n->y)
      return list;

    list = list->next;
  }

  return NULL;
}

/* extract the first node from a list */
static anode *
astar_extract_head( anode **list )
{
  anode *head = *list;

  /* empty list */
  if(list == NULL)  return NULL;

  *list = (*list)->next;

  return head;
}

/* sort a node into a list according f and then g values */
static void
astar_sort_node( anode **list, anode *node )
{
  anode *trav = *list;
	
  /* look for the predecessor of node */
  while(trav->next != node && trav->next != NULL) {
    trav = trav->next;
  }
  /* connect previous and next nodes */
  trav->next = node->next;

  /* reinsert the node to obtain the right order */
  astar_insert_node(list, node);
}

/* deallocate all elements of a list of l_node */
static void
astar_del_list( anode **list )
{
  anode *trav;

  /* move along list and free elements */
  while(*list) {
    trav = *list;
    *list = (*list)->next;
    free(trav);
  }
}

/* calculates a pathfind from start to end cells using A* algorithm */
int
astar_search( ivec2 **path, const ivec2 *start, const ivec2 *end, const patchmap *pm )
{
  /* start or end not walkable or start == end */
  int start_index = start->y*pm->step + start->x;
  int end_index = end->y*pm->step + end->x;
  if(!pm->cells[start_index].status || !pm->cells[end_index].status ||
     (start->x == end->x && start->y == end->y))
    return 0;


  /* A* lists */
  anode *open = NULL;
  anode *closed = NULL;
  /* current node */
  anode *cur = NULL;
  /* generated node */
  anode *gen = NULL;
  /* node already present into OPEN list */
  anode *already_ins = NULL;
  /* travel node */
  anode *trav = NULL;

  /* node extracted from OPEN */
  ivec2 cur_cell;
  /* expanded nodes */
  ivec2 *exp_cells;
  /* management ivec2 */
  ivec2 *cell;
	
  /* init first node */
  anode *first = (anode *)malloc(sizeof(anode));
  first->x = start->x; 
  first->y = start->y;
  first->f = 0;
  first->g = 0;
  first->h = 0;
  first->next = NULL;
  first->father = NULL;

  /* add first node to OPEN */
  astar_insert_node(&open, first);

  /* local counter */
  int i;
  /* nodes expanded at every cycle */
  int n_exp_nodes = 0;

  /* found a path - 0 (FALSE) */
  int found = 0;

  /* go on until found a path or open is empty */
  while(!found && open) {
    /* extract the node with lowest f value */
    cur = astar_extract_head(&open);
    cur_cell.x = cur->x;
    cur_cell.y = cur->y;

    /* expand nodes from the current obtaining cell indexes */
    n_exp_nodes = astar_gen_nodes(&exp_cells, &cur_cell, pm);

    /* insert current into CLOSED list */
    astar_insert_head_node(&closed, cur);

    /* manage expanded nodes */
    for(i = 0; i < n_exp_nodes; i++) {
      cell = &exp_cells[i];

      /* walkable node & not yet into CLOSED (not yet analized) */
      int index = cell->y*pm->step + cell->x;
      if(pm->cells[index].status && astar_get_node(closed, cell) == NULL) {
        /* check if exp_cell is already into OPEN */
        already_ins = astar_get_node(open, cell);

        /* generated node is not yet into OPEN */
        if(already_ins == NULL) {
          gen = (anode *)malloc(sizeof(anode));
          gen->x = cell->x;
          gen->y = cell->y;
          gen->g = cur->g + ASTAR_WALK_COST; 
          gen->h = astar_manhattan(cell, end);
          gen->f = gen->g + gen->h;
          gen->father = cur;

          /* insert generated node into OPEN */
          astar_insert_node(&open, gen);

          /* h(n)= 0 ==> END NODE!!! get out! */
          if(!gen->h) {
            found = 1;
            break;
          }
        }
        /* found a better path */
        else if(already_ins->g > (cur->g + ASTAR_WALK_COST)) {
          /* recalculate node fields */
          already_ins->x = cell->x;
          already_ins->y = cell->y;
          already_ins->g = cur->g + ASTAR_WALK_COST;
          already_ins->h = astar_manhattan(cell, end);
          already_ins->f = already_ins->g + already_ins->h;
          /* resort updated node */
          astar_sort_node(&open, already_ins);
        }
      }
    }
    free(exp_cells);
  }

  /* no path found... it's a defeat :( */
  if(!found) {
    /* free open & closed lists */
    astar_del_list(&open);
    astar_del_list(&closed);
    return 0;
  }

  /* calculate path length */
  int len_path = gen->g/ASTAR_WALK_COST + 1;
  int len_bk = len_path;

  /* allocate memory for path */
  *path = (ivec2 *)malloc(sizeof(ivec2)*len_path);

  /* fill path array with data from nodes */
  trav = gen;
  while(trav != NULL) {
    len_path--;
    (*path)[len_path].x = trav->x;
    (*path)[len_path].y = trav->y;
    trav = trav->father;
  }

  /* free open & closed lists */
  astar_del_list(&open);
  astar_del_list(&closed);

  return len_bk;
}