#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef int key_t;

typedef struct node_t
{
  //  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct
{
  node_t *root;
  node_t *nil; // for sentinel
} bstree;

bstree *new_bstree(void);

void delete_bstree(bstree *);

node_t *bstree_insert(bstree *, const key_t);

node_t *bstree_find(const bstree *, const key_t);

node_t *bstree_min(const bstree *);

node_t *bstree_max(const bstree *);

int bstree_erase(bstree *, node_t *);

int bstree_to_array(const bstree *, key_t *, const size_t);

#endif // _RBTREE_H_
