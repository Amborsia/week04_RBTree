#include "rbtree.h"

#include <stdlib.h>

node_t *right_rotate(rbtree *t, node_t *y);
node_t *left_rotate(rbtree *t, node_t *x);
void *insert_fixup(rbtree *t, node_t *x);
void rb_transplant(rbtree *t, node_t *u, node_t *v);
void rb_delete_fixup(rbtree *t, node_t *x);

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  // 노드 nil값을 하나를 만들어줘야 모든 nil값을 이리로 모을 수 있음
  nil->color = RBTREE_BLACK;
  // 모든 nil노드는 black이기 때문에 색깔을 검정색으로 진행시킴
  p->nil = nil;
  p->root = nil;
  return p;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  // 연산 시간은 O(logN)임
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
  // 경우의 수를 살펴보면
  // case 1 -> case2-> case4
  // case 1 -> case3-> case4
  // case 1-> case4
  // case 4
  // case 2
  // case 3-> case 4
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      node_t *sibling = x->parent->right;
      // 더블리 블랙의 형제가 red인경우
      //  해결법 부모를 기준으로 회전 근데 회전 전에 부모와 형제의 색깔을 바꿔줌
      if (sibling->color == RBTREE_RED)
      {
        // case 1
        sibling->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        sibling = x->parent->right;
        // 이렇게 돌리고 나면 case 2,3,4를 체크해봐야 하므로 case1이 가장 상위단에 있어야함
        // 모든 케이스를 다 돌리고 나서 확인을 할 수 있기 때문.
      }
      // 형제의 두 자녀가 모두 black인지를 체크해야함
      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK)
      {
        // case 2
        // 형제의 색깔을 레드로 바꿔 준 이후 부모에게 전달해야함
        // 그리고서 부모에게 블랙에 대한 책임을 전가하고 비교 대상을 부모로 옮겨서
        // 부모가 처리할 수 있도록 해야함
        // 레드앤블랙인 경우 그냥 색깔만 바꾸고
        // 더블리 블랙일때는 다시 그 노드가 루트냐 아니냐로 따지고
        // 루트인 경우 블랙으로 변경,
        // 루트가 아닌 경우 case1,2,3,4를 다시 적용
        // 다시 적용해야하기에 while문을 통해서 다시 돌아가게끔 만듦
        sibling->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        // 오른쪽 형제가 black이고,
        //  그 형제의 왼쪽 자녀가 레드 이 조건은 위에 if문에서 해결이 될 것임
        //  그리고 오른쪽 자녀가 블랙인 경우
        if (sibling->right->color == RBTREE_BLACK)
        {
          // case3
          // 왼쪽 자식은 검은색으로 변경, 오른쪽은 더블리 블랙이 될 것임
          sibling->left->color = RBTREE_BLACK;
          // 형제의 색깔은 색깔을 내려준것이니까 레드로 바뀌게 됨
          sibling->color = RBTREE_RED;
          // 이후 형제를 기준으로 오른쪽으로 회전시키게 됨
          right_rotate(t, sibling);
          // 형제는 다시 형제로 맞춰줘야하기 때문에 x의 형제의 오른쪽으로 진행하면 자기 자신이 나오게 됨
          sibling = x->parent->right;
          // 이후 케이스 4번을 적용해서 이 문제를 해결하면 됨
        }
        // case 4
        // 형제의 색깔을 부모의 색깔로 바꿔줌
        sibling->color = x->parent->color;
        // 부모의 색깔은 블랙으로 변경해줌
        x->parent->color = RBTREE_BLACK;
        // 형제의 오른쪽 자식을 검은색으로 변경해줘야함
        // 의문? 강의에서는 부모와 형제의 색깔을 변경하고 돌렸는데 여기는 그렇게 하지 않음
        // 아마 결과론적으로 접근한게 코드가 더 짧아지기 때문에 그런 것 같음
        sibling->right->color = RBTREE_BLACK;
        // 이후 왼쪽으로 돌리고
        left_rotate(t, x->parent);
        // x의 값을 루트로 만들어서 while문을 끝마치게 됨
        x = t->root;
      }
    }
    else
    {
      // 이 밑으로는 위에서 얘기한 것의 반대로 진행하면 됨
      node_t *sibling = x->parent->left;
      if (sibling->color == RBTREE_RED)
      {
        sibling->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        sibling = x->parent->left;
      }
      if (sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK)
      {
        sibling->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (sibling->left->color == RBTREE_BLACK)
        {
          sibling->right->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          left_rotate(t, sibling);
          sibling = x->parent->left;
        }
        sibling->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        sibling->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

void delete_rbtree(rbtree *t)
{
  // 루트 노드를 돌면서 nil값이 아니라면 계속 파고 들어가서 메모리를 없애야함
  if (t->root != t->nil)
    free_node(t, t->root);
  // 마지막에는 트리와 nil값만이 존재하다보니 마지막에 삭제를 시켜야함
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
  // x = NULL;
}

void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
  // RB_delete(T,z)에서 삭제할 노드 z와 z를 대체 할 노드 y의 연결 관계를 바꿔주는 코드임
  // 부모의 연결관계만 바꿔주는 역할
  // 삭제할 노드 z와 대체 노드 y의 부모, 자식등을 연결 정보를 수정하는 방법을 사용
  // 대체노드y의 키값을 원래 삭제할 노드 z에 복사 후 대체 노드 y를 대신 삭제 시키는 방법도 있지만
  // 전자를 사용함.

  // 일반적인 BST트리와의 차이점음 1행에서 NIL대신 경계노드인 t->nil값을 참고하고,

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
  // 여기서 조건없이 v->p값을 할당한다.
  //
  v->parent = u->parent;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{

  node_t *x;

  x = t->root;
  // x값 초기를 루트로 잡고 x가 nil이 아닐때까지 계속 반복
  // nil이란 것은 값이 없는것이기 때문
  while (x != t->nil)
  {
    // x->key가 key값이라는 이유는 값을 제대로 찾았다는 것을 의미함
    //  그러면 x값 즉 노드를 반환해줌
    if (x->key == key)
    {
      return x;
    }
    // x->key가 key보다 작다는 것은 그래프의 이진그래프의 특성상
    // 오른쪽에 큰 값이 있기 때문에 오른쪽으로 계속 x를 이동시켜줌
    else if (x->key < key)
    {
      x = x->right;
    }
    else if (x->key > key)
    {
      x = x->left;
    }
  }
  // while문을 타지 않았다는 것 혹은 여기까지 왔다는 의미는
  // 찾지 못했거나, 처음부터 x의 값은 nil값이기 때문에 null을 반환해줘야함
  return NULL;
}

node_t *rbtree_min(const rbtree *t)
{
  node_t *temp = t->root;
  // 노드를 루트로 잡아줌
  if (temp == t->nil)
  {
    // root값이 nil값이라면 값이 아예 없는거니까 NULL값을 반환
    return NULL;
  }
  // temp값의 왼쪽이 nil값이 아니라면 계속 왼쪽으로 이동
  // 가장 작은 값이 왼쪽이기 때문에 최소값을 찾기 위해서는 왼쪽으로 와야함
  while (temp->left != t->nil)
  {
    temp = temp->left;
  }
  // 가장 최소값의 노드를 반환해줌
  return temp;
}

node_t *rbtree_max(const rbtree *t)
{
  node_t *temp = t->root;
  // 노드를 루트로 잡아줌
  if (temp == t->nil)
  {
    // root값이라면 값이 아예 없기때문에 NULL값을 반환
    return NULL;
  }
  // temp값의 왼쪽이 nil값이 아니라면 계속 오른쪽으로 이동
  // 가장 큰 값이 오른쪽이기 때문에 최댓값을 찾기 위해서는 오른쪽으로 계속해서 가야함
  while (temp->right != t->nil)
  {
    temp = temp->right;
  }
  // 가장 최댓값의 노드를 반환해줌
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
        // 맨위가 블랙, 그 왼쪽,오른쪽이 레드 루트 기준으로 왼쪽의 왼쪽이 레드로 삽입된 케이스
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
  // 새로운 노드값에 대해서 삽입을 해야하니까
  // z값을 초기화 시켜줌
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  // 삽입할 때 색깔은 항상 레드이며 삽입 이후에는 fixup을 이용해야함
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
