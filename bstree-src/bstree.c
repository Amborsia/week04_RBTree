#include "bstree.h"

#include <stdlib.h>
#include <stdio.h>

bstree *new_bstree(void)
{
    bstree *bst_t = (bstree *)malloc(sizeof(bstree));
    bst_t->root = NULL;
    return bst_t;
}

void delete_bstree(bstree *t)
{
    // TODO: reclaim the tree nodes' memory
    free(t->nil);
    free(t);
}

node_t *treeInsert(node_t *t, const key_t key)
{
    if (t == NULL)
    {
        node_t *n = (node_t *)calloc(1, sizeof(node_t));
        n->key = key;
        n->left = NULL;
        n->right = NULL;
        return n;
    }
    if (key < t->key)
    {
        t->left = treeInsert(t->left, key);
        return t;
    }
    else
    {
        t->right = treeInsert(t->right, key);
        return t;
    }
}

node_t *bstree_insert(bstree *t, const key_t key)
{
    if (t->root == NULL)
    {
        t->root = treeInsert(t->root, key);
        return t->root;
    }
    else
    {
        return treeInsert(t->root, key);
    }
}

node_t *tree_find(node_t *t, const key_t key)
{
    while (t != NULL)
    {
        if (key == t->key)
        {
            return t;
        }
        else if (key < t->key)
        {
            t = t->left;
        }
        else
        {
            t = t->right;
        }
    }
    return t; // 트리를 탐색하다가 NULL이 되면 찾지 못한 것이므로 NULL 반환
}

node_t *bstree_find(const bstree *t, const key_t key)
{
    if (t == NULL || t->root == NULL)
    {
        return NULL;
    }
    else
    {
        return tree_find(t->root, key);
    }
}

node_t *bstree_min(const bstree *t)
{
}

node_t *bstree_max(const bstree *t)
{
}

int bstree_erase(bstree *t, node_t *p)
{

    // delete는 null만나면 위로 올라가서 free
    node_t *node = t->root;
    node_t *temp = NULL;
    node_t *temp2 = NULL;
    // temp는 노드값을 가지고 있다.
    temp = bstree_find(t, p->key);
    printf("-------------------- %d %d %d\n", temp->key, temp->left, temp->right);
    printf("test %d\n", t->root);
    // 노드 자체가 없다면 리턴 널
    if (t->root == NULL)
    {
        printf("----------------0000\n");
        free(t->root);
        return NULL;
    }
    if (temp == NULL)
    {
        printf("----------------1111111\n");
        return NULL;
    }

    if (temp->left == NULL && temp->right == NULL)
    {
        printf("---------------");
        // 들어온 값이랑 루트값이랑 같으면 루트를 삭제해야함
        if (temp == t->root)
        {
            printf("---------------1");
            p = NULL;
        }
        else if ((temp->parent)->key > p->key)
        {
            printf("---------------2");
            (temp->parent)->left = NULL;
        }
        else
        {
            printf("---------------3");
            (temp->parent)->right = NULL;
        }
        printf("---------------4");
        free(t);
    }
    // 노드값이 한가지의 자식만 가지고 있을 경우
    else if (temp->left == NULL)
    {
        printf("test");
        temp->key = (temp->right)->key;
        bstree_erase(t, temp->left);
    }
    else if (temp->right == NULL)
    {
        printf("1232312231231");
        temp->key = (temp->left)->key;
        bstree_erase(t, temp->right);
    }
    // 노드값이 왼쪽 오른쪽 두 자식 모두 가지고 있을때
    else
    {
        printf("????????????");
        p = temp;
        temp = temp->right;
        while (temp->left != NULL)
        {
            temp = temp->left;
        }
        p->key = temp->key;
        bstree_erase(t, p);
    }
    printf("@@@@@@@@@@@@@@@@");
    return 0;
}

int bstree_to_array(const bstree *t, key_t *arr, const size_t n)
{
    // TODO: implement to_array
    return 0;
}
