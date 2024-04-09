#include "rbtree.h"

#include <stdlib.h>

node_t *right_rotate(rbtree *t, node_t *y);
node_t *left_rotate(rbtree *t, node_t *x);
void *insert_fixup(rbtree *t, node_t *x);
void rb_transplant(rbtree *t, node_t *u, node_t *v);
// node_t *tree_minimum(const rbtree *t, node_t *sub_root);
void rb_delete_fixup(rbtree *t, node_t *x);

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  p->nil = nil;
  p->root = nil;
  return p;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  node_t *y = p;
  node_t *x;
  int original = y->color;
  // 삭제 될 노드의 왼쪽값이 nil값이면 오른쪽을 그대로 가져옴
  if (p->left == t->nil)
  {
    x = p->right;
    rb_transplant(t, p, p->right);
  }
  // 삭제 될 노드의 오른쪽값이 nil값이면 왼쪽을 그대로 가져옴
  else if (p->right == t->nil)
  {
    x = p->left;
    rb_transplant(t, p, p->left);
  }
  // 노드 p의 left,right모두 nil이 아닌경우 즉 둘 다 있는 경우
  else
  {
    y = p->right;
    while (y->left != t->nil)
    {
      y = y->left;
    }
    original = y->color;
    x = y->right;
    if (y->parent == p)
    {
      x->parent = y;
    }
    else
    {
      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rb_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if (original == RBTREE_BLACK)
  {
    rb_delete_fixup(t, x);
  }
  free(p);
  return 0;
}

void rb_delete_fixup(rbtree *t, node_t *x)
{

  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      node_t *w = x->parent->right;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else
    {
      node_t *w = x->parent->left;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

void delete_rbtree(rbtree *t)
{

  if (t->root != t->nil)
    free_node(t, t->root);
  free(t->nil);
  free(t);
}

void free_node(rbtree *t, node_t *x)
{ // 후위 순회 방식으로 노드와 하위 노드들의 메모리를 반환하는 함수
  if (x->left != t->nil)
    free_node(t, x->left);
  if (x->right != t->nil)
    free_node(t, x->right);

  free(x);
  x = NULL;
}
// RB_delete(T,z)에서 삭제할 노드 z와 z를 대체 할 노드 y의 연결 관계를 바꿔주는 코드임
// 부모의 연결관계만 바꿔주는 역할
// 삭제할 노드 z와 대체 노드 y의 부모, 자식등을 연결 정보를 수정하는 방법을 사용
// 대체노드y의 키값을 원래 삭제할 노드 z에 복사 후 대체 노드 y를 대신 삭제 시키는 방법도 있지만
// 전자를 사용함.
void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
  {
    t->root = v;
  }
  else if (u == u->parent->left)
  {
    u->parent->left = v;
  }
  else
  {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{

  node_t *x;

  x = t->root;
  while (x != t->nil)
  {
    if (x->key == key)
    {
      return x;
    }
    else if (x->key < key)
    {
      x = x->right;
    }
    else if (x->key > key)
    {
      x = x->left;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t)
{
  node_t *temp = t->root;
  if (temp == t->nil)
  {
    return NULL;
  }
  while (temp->left != t->nil)
  {
    temp = temp->left;
  }

  return temp;
}

node_t *rbtree_max(const rbtree *t)
{
  node_t *temp = t->root;
  if (temp == t->nil)
  {
    return NULL;
  }
  while (temp->right != t->nil)
  {
    temp = temp->right;
  }
  return temp;
}

void *insert_fixup(rbtree *t, node_t *x)
{

  while (x->parent->color == RBTREE_RED)
  {
    // 부모랑 왼쪽 삼촌이 같은 즉 1자로 된 그래프의 경우의 수
    if (x->parent == x->parent->parent->left)
    {
      node_t *right_uncle = x->parent->parent->right;
      // 만약 부모의 색깔이 레드인 경우, 색깔을 바꿔줌
      if (right_uncle->color == RBTREE_RED)
      {
        // 맨위가 블랙, 그 왼쪽,오른쪽이 레드 왼쪽의 왼쪽이 레드로 삽입된 케이스
        // 그러면 맨위의 왼쪽오른쪽이 다 레드이고 맨위가 블랙이라서 색깔을 바꿔주면 되는 케이스가 됨
        x->parent->color = RBTREE_BLACK;
        right_uncle->color = RBTREE_BLACK;
        x->parent->parent->color = RBTREE_RED;

        // 바꾼 이후로는 x의값(포인터이기 때문에 주소만 들고있음)을 할아버지 값으로 옮겨서 x에 대해서 겁사를 또 할 수 있도록 만듦
        //  따라서 새로 삽입된 곳에 할아버지를 넣는게 아니라 기존 주소 대신 할아버지 주소를 받아서
        //  while문을 통해서 체크하는 것임
        x = x->parent->parent;
      }
      else
      {
        // 양쪽 왼쪽 오른쪽 값에 누군가 있었다면? 가능한가?
        if (x == x->parent->right)
        {
          // 맨 위 노드가 레드이고 맨위의 왼쪽이 블랙, 오른쪽이 원래 x값일 때의 경우의수
          // 알고리즘책의 319페이지
          // 비교대상을 x의 부모로 바꿔줘서 이상이 있는지를 체크
          // 다만 여기에 들어왔단것은 문제가 있단것이므로 부모를기준으로 왼쪽으로 돌려야함
          x = x->parent;

          left_rotate(t, x);
        }
        x->parent->color = RBTREE_BLACK;
        // grandfather가 안되는 이유는 이 위에서 x값을 바꿨기 때문에 바뀐 상태에서 이리로 오면 parent값이 되어버림
        x->parent->parent->color = RBTREE_RED;
        right_rotate(t, x->parent->parent);
      }
    }
    // 부모랑 오른 삼촌이 같은 즉 1자로 된 그래프의 경우의 수
    // left->right로 변경
    else
    {
      node_t *left_uncle = x->parent->parent->left;
      if (left_uncle->color == RBTREE_RED)
      {
        x->parent->color = RBTREE_BLACK;
        x->parent->parent->color = RBTREE_RED;
        left_uncle->color = RBTREE_BLACK;
        x = x->parent->parent;
      }
      else
      {
        if (x == x->parent->left)
        {
          // 꺾임
          x = x->parent;
          right_rotate(t, x);
        }
        x->parent->color = RBTREE_BLACK;
        x->parent->parent->color = RBTREE_RED;

        left_rotate(t, x->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *left_rotate(rbtree *t, node_t *x)
{
  // 왼쪽으로 회전하는 것
  // x와 y값이 있다고 하면 y의 parent는 x인 상태
  node_t *temp = x->right;
  // x의 오른쪽 자식 노드 값에 y의 왼쪽 노드 값을 대입한다.
  x->right = temp->left;
  // y의 왼쪽 자식 노드가 nil이 아니라면 y의 왼쪽 자식의 부모를 y에서 x로 변경
  if (temp->left != t->nil)
  {
    temp->left->parent = x;
  }
  // y의 부모를 x의 부모로 변경해야함 -> 이게 회전의 주축
  // 아직까지는 y의 부모와 x의 부모가 같으므로 밑에서는 x의 부모를 y로 바꾸는등의 작업을 해야함.
  temp->parent = x->parent;
  // x의 부모가 nil값이란거는 x가 루트 노드인것이라서 t 즉 트리의 루트를 x에서 y로 변경시켜야함
  if (x->parent == t->nil)
  {
    // 여기서 트리의 루트를 y값으로 변경
    t->root = temp;
  }
  // 여기까지 왔다는 것은 x가 루트노드가 아니란 것이니까 더 상위를 봐야함
  // 따라서 x는 x의 부모가 있다는 사실임
  // x값이 부모의 왼쪽 값이라면, x의 부모의 왼쪽을 기존 x에서 y값으로 변경해야함
  else if (x == x->parent->left)
  {
    x->parent->left = temp;
  }
  // 여기는 그 반대로 x값이 부모의 오른쪽 값이라면, x의 부모의 오른쪽을 기존 x에서 y값으로 변경해야함.
  else
  {
    x->parent->right = temp;
  }
  // y의 왼쪽 값을 x로 변경해줘야 y왼쪽으로 연결이 됨.
  temp->left = x;
  // 그리고 나서 x의 부모 값을 y값으로 변경하면 회전이 완료가 됨
  x->parent = temp;
}

node_t *right_rotate(rbtree *t, node_t *y)
{
  // y = temp, x= y
  node_t *temp = y->left;
  y->left = temp->right;
  if (temp->right != t->nil)
  {
    temp->right->parent = y;
  }
  temp->parent = y->parent;
  if (y->parent == t->nil)
  {
    t->root = temp;
  }
  else if (y == y->parent->right)
  {
    y->parent->right = temp;
  }
  else
  {
    y->parent->left = temp;
  }

  temp->right = y;
  y->parent = temp;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // 인서트의 노드가 n개인 레드블랙 트리의 높이는 O(log N) 이므로
  //  인서트의 수행시간은 O(logN)이 된다.
  node_t *y = t->nil;
  node_t *x = t->root;
  // x(t-<root)가 t->nil값이라면 값이 없는거니까 그냥 바로 집어넣으면 됨
  // while문을 통해서 x값을 바꾸면서 값을 찾아가야함

  while (x != t->nil)
  {
    y = x;
    if (key < x->key)
    {
      x = x->left;
    }
    else
    {
      x = x->right;
    }
  }
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->parent = y;
  z->key = key;

  // 아예 부모가 없는 경우 즉 while문을 타지 않았을 경우에 해당
  if (y == t->nil)
  {
    t->root = z;
  }
  else if (z->key < y->key)
  {
    y->left = z;
  }
  else
  {
    y->right = z;
  }
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  insert_fixup(t, z);
  return t->root;
}

int inorder(const rbtree *t, node_t *node, key_t *arr, int i, int n)
{
  if (i >= n)
    return 0;
  if (node != t->nil)
  {
    if (node->left != t->nil)
      i = inorder(t, node->left, arr, i, n);
    arr[i++] = node->key;
    if (node->right != t->nil)
      i = inorder(t, node->right, arr, i, n);
  }
  return i;
}
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  inorder(t, t->root, arr, 0, n);
  return 0;
}
