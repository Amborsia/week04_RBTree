#include <assert.h>
#include "../bstree-src/bstree.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// new_bstree should return bstree struct with null root node
void test_init(void)
{
    bstree *t = new_bstree();
    assert(t != NULL);
#ifdef SENTINEL
    assert(t->nil != NULL);
    assert(t->root == t->nil);
#else
    assert(t->root == NULL);
#endif
    delete_bstree(t);
}

// root node should have proper values and pointers
void test_insert_single(const key_t key)
{
    bstree *t = new_bstree();
    node_t *p = bstree_insert(t, key);
    assert(p != NULL);
    assert(t->root == p);
    assert(p->key == key);
#ifdef SENTINEL
    assert(p->left == t->nil);
    assert(p->right == t->nil);
    assert(p->parent == t->nil);
#else
    assert(p->left == NULL);
    assert(p->right == NULL);
    assert(p->parent == NULL);
#endif
    delete_bstree(t);
}

// find should return the node with the key or NULL if no such node exists
void test_find_single(const key_t key, const key_t wrong_key)
{
    bstree *t = new_bstree();
    node_t *p = bstree_insert(t, key);

    node_t *q = bstree_find(t, key);
    assert(q != NULL);
    assert(q->key == key);
    assert(q == p);

    q = bstree_find(t, wrong_key);

    assert(q == NULL);

    delete_bstree(t);
}

// erase should delete root node
void test_erase_root(const key_t key)
{
    bstree *t = new_bstree();
    node_t *p = bstree_insert(t, key);
    assert(p != NULL);
    assert(t->root == p);
    assert(p->key == key);

    bstree_erase(t, p);
    printf("test code = > %d", t->root);
#ifdef SENTINEL
    assert(t->root == t->nil);
#else
    assert(t->root == NULL);
#endif

    delete_bstree(t);
}

static void insert_arr(bstree *t, const key_t *arr, const size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        bstree_insert(t, arr[i]);
    }
}

static int comp(const void *p1, const void *p2)
{
    const key_t *e1 = (const key_t *)p1;
    const key_t *e2 = (const key_t *)p2;
    if (*e1 < *e2)
    {
        return -1;
    }
    else if (*e1 > *e2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};

// min/max should return the min/max value of the tree
void test_minmax(key_t *arr, const size_t n)
{
    // null array is not allowed
    assert(n > 0 && arr != NULL);

    bstree *t = new_bstree();
    assert(t != NULL);

    insert_arr(t, arr, n);
    assert(t->root != NULL);
#ifdef SENTINEL
    assert(t->root != t->nil);
#endif

    qsort((void *)arr, n, sizeof(key_t), comp);
    node_t *p = bstree_min(t);
    assert(p != NULL);
    assert(p->key == arr[0]);

    node_t *q = bstree_max(t);
    assert(q != NULL);
    assert(q->key == arr[n - 1]);

    bstree_erase(t, p);
    p = bstree_min(t);
    assert(p != NULL);
    assert(p->key == arr[1]);

    if (n >= 2)
    {
        bstree_erase(t, q);
        q = bstree_max(t);
        assert(q != NULL);
        assert(q->key == arr[n - 2]);
    }

    delete_bstree(t);
}

void test_to_array(bstree *t, const key_t *arr, const size_t n)
{
    assert(t != NULL);

    insert_arr(t, arr, n);
    qsort((void *)arr, n, sizeof(key_t), comp);

    key_t *res = calloc(n, sizeof(key_t));
    bstree_to_array(t, res, n);
    for (int i = 0; i < n; i++)
    {
        assert(arr[i] == res[i]);
    }
    free(res);
}

void test_multi_instance()
{
    bstree *t1 = new_bstree();
    assert(t1 != NULL);
    bstree *t2 = new_bstree();
    assert(t2 != NULL);

    key_t arr1[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
    const size_t n1 = sizeof(arr1) / sizeof(arr1[0]);
    insert_arr(t1, arr1, n1);
    qsort((void *)arr1, n1, sizeof(key_t), comp);

    key_t arr2[] = {4, 8, 10, 5, 3};
    const size_t n2 = sizeof(arr2) / sizeof(arr2[0]);
    insert_arr(t2, arr2, n2);
    qsort((void *)arr2, n2, sizeof(key_t), comp);

    key_t *res1 = calloc(n1, sizeof(key_t));
    bstree_to_array(t1, res1, n1);
    for (int i = 0; i < n1; i++)
    {
        assert(arr1[i] == res1[i]);
    }

    key_t *res2 = calloc(n2, sizeof(key_t));
    bstree_to_array(t2, res2, n2);
    for (int i = 0; i < n2; i++)
    {
        assert(arr2[i] == res2[i]);
    }

    free(res2);
    free(res1);
    delete_bstree(t2);
    delete_bstree(t1);
}

// Search tree constraint
// The values of left subtree should be less than or equal to the current node
// The values of right subtree should be greater than or equal to the current
// node

static bool search_traverse(const node_t *p, key_t *min, key_t *max,
                            node_t *nil)
{
    if (p == nil)
    {
        return true;
    }

    *min = *max = p->key;

    key_t l_min, l_max, r_min, r_max;
    l_min = l_max = r_min = r_max = p->key;

    const bool lr = search_traverse(p->left, &l_min, &l_max, nil);
    if (!lr || l_max > p->key)
    {
        return false;
    }
    const bool rr = search_traverse(p->right, &r_min, &r_max, nil);
    if (!rr || r_min < p->key)
    {
        return false;
    }

    *min = l_min;
    *max = r_max;
    return true;
}

void test_search_constraint(const bstree *t)
{
    assert(t != NULL);
    node_t *p = t->root;
    key_t min, max;
#ifdef SENTINEL
    node_t *nil = t->nil;
#else
    node_t *nil = NULL;
#endif
    assert(search_traverse(p, &min, &max, nil));
}

void test_minmax_suite()
{
    key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
    const size_t n = sizeof(entries) / sizeof(entries[0]);
    test_minmax(entries, n);
}

void test_to_array_suite()
{
    bstree *t = new_bstree();
    assert(t != NULL);

    key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
    const size_t n = sizeof(entries) / sizeof(entries[0]);
    test_to_array(t, entries, n);

    delete_bstree(t);
}

void test_find_erase(bstree *t, const key_t *arr, const size_t n)
{
    for (int i = 0; i < n; i++)
    {
        node_t *p = bstree_insert(t, arr[i]);
        printf("insert => arr[%d] = %d\n", i, arr[i]);
        assert(p != NULL);
    }

    for (int i = 0; i < n; i++)
    {
        node_t *p = bstree_find(t, arr[i]);
        // printf("arr[%d] = %d\n", i, arr[i]);
        assert(p != NULL);
        assert(p->key == arr[i]);
        bstree_erase(t, p);
    }

    for (int i = 0; i < n; i++)
    {
        node_t *p = bstree_find(t, arr[i]);
        // printf("-------------- %d %d\n", p->key, t->root->left);
        // printf("arr[%d] = %d\n", i, arr[i]);

        assert(p == NULL);
    }

    for (int i = 0; i < n; i++)
    {
        node_t *p = bstree_insert(t, arr[i]);
        assert(p != NULL);
        node_t *q = bstree_find(t, arr[i]);
        assert(q != NULL);
        assert(q->key == arr[i]);
        assert(p == q);
        bstree_erase(t, p);
        q = bstree_find(t, arr[i]);
        assert(q == NULL);
    }
}

void test_find_erase_fixed()
{
    const key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
    const size_t n = sizeof(arr) / sizeof(arr[0]);
    bstree *t = new_bstree();
    assert(t != NULL);

    test_find_erase(t, arr, n);

    delete_bstree(t);
}

void test_find_erase_rand(const size_t n, const unsigned int seed)
{
    srand(seed);
    bstree *t = new_bstree();
    key_t *arr = calloc(n, sizeof(key_t));
    for (int i = 0; i < n; i++)
    {
        arr[i] = rand();
    }

    test_find_erase(t, arr, n);

    free(arr);
    delete_bstree(t);
}

int main(void)
{
    test_init();
    test_insert_single(1024);
    test_find_single(512, 1029);
    test_erase_root(128);
    // -----------
    test_find_erase_fixed();
    test_minmax_suite();
    test_to_array_suite();
    test_multi_instance();
    test_find_erase_rand(10000, 17);
    printf("Passed all tests!\n");
}
