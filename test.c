#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "greatest/greatest.h"

#define BTREE_NAME btree_uint32
#define BTREE_KEY_TYPE uint32_t
#define BTREE_NODE_MAX_DEGREE 4
#include "btree.h"
#undef BTREE_NAME
#undef BTREE_KEY_TYPE
#undef BTREE_NODE_MAX_DEGREE

TEST test_btree(void) {
    btree_uint32 *tree = btree_uint32_new();

    char *val1 = "a";
    char *val2 = "b";
    char *val3 = "c";

    btree_uint32_insert(tree, 1, "a");
    btree_uint32_insert(tree, 3, "b");
    btree_uint32_insert(tree, 5, "c");
    btree_uint32_insert(tree, 7, "d");
    btree_uint32_insert(tree, 9, "e");

    ASSERT_EQ(tree->root->height, 1);
    ASSERT_EQ(tree->root->degree, 2);

    char *a = btree_uint32_get(tree->root, 1);
    ASSERT_STR_EQ(a, "a");

    char *b = btree_uint32_get(tree->root, 3);
    ASSERT_STR_EQ(b, "b");

    char *c = btree_uint32_get(tree->root, 5);
    ASSERT_STR_EQ(c, "c");

    char *d = btree_uint32_get(tree->root, 7);
    ASSERT_STR_EQ(d, "d");

    char *e = btree_uint32_get(tree->root, 9);
    ASSERT_STR_EQ(e, "e");

    a = btree_uint32_delete(tree, 1);
    ASSERT_STR_EQ(a, "a");

    a = btree_uint32_get(tree->root, 1);
    ASSERT(a == NULL);

    b = btree_uint32_delete(tree, 3);
    ASSERT_STR_EQ(b, "b");

    e = btree_uint32_delete(tree, 9);
    ASSERT_STR_EQ(e, "e");

    c = btree_uint32_get(tree->root, 5);
    ASSERT_STR_EQ(c, "c");

    c = btree_uint32_delete(tree, 5);
    ASSERT_STR_EQ(c, "c");

    d = btree_uint32_delete(tree, 7);
    ASSERT_STR_EQ(d, "d");

    d = btree_uint32_get(tree->root, 7);
    ASSERT(d == NULL);

    // Tree should be empty
    ASSERT_EQ(tree->root->height, 0);
    ASSERT_EQ(tree->root->degree, 0);

    btree_uint32_insert(tree, 7, "d");
    d = btree_uint32_get(tree->root, 7);

    // Root should have one child
    ASSERT_EQ(tree->root->height, 0);
    ASSERT_EQ(tree->root->degree, 1);

    d = btree_uint32_delete(tree, 7);
    ASSERT_STR_EQ(d, "d");
    ASSERT_EQ(tree->root->height, 0);
    ASSERT_EQ(tree->root->degree, 0);

    btree_uint32_insert(tree, 47, "x");
    btree_uint32_insert(tree, 33, "q");
    btree_uint32_insert(tree, 15, "h");
    btree_uint32_insert(tree, 35, "r");
    btree_uint32_insert(tree, 45, "w");
    btree_uint32_insert(tree, 17, "i");
    btree_uint32_insert(tree, 19, "j");
    btree_uint32_insert(tree, 37, "s");
    btree_uint32_insert(tree, 21, "k");
    btree_uint32_insert(tree, 11, "f");
    btree_uint32_insert(tree, 41, "u");
    btree_uint32_insert(tree, 23, "l");
    btree_uint32_insert(tree, 25, "m");
    btree_uint32_insert(tree, 27, "n");
    btree_uint32_insert(tree, 29, "o");
    btree_uint32_insert(tree, 13, "g");
    btree_uint32_insert(tree, 31, "p");
    btree_uint32_insert(tree, 39, "t");
    btree_uint32_insert(tree, 43, "v");

    char *f = btree_uint32_get(tree->root, 11);
    ASSERT_STR_EQ(f, "f");
    char *g = btree_uint32_get(tree->root, 13);
    ASSERT_STR_EQ(g, "g");
    char *h = btree_uint32_get(tree->root, 15);
    ASSERT_STR_EQ(h, "h");
    char *i = btree_uint32_get(tree->root, 17);
    ASSERT_STR_EQ(i, "i");
    char *j = btree_uint32_get(tree->root, 19);
    ASSERT_STR_EQ(j, "j");
    char *k = btree_uint32_get(tree->root, 21);
    ASSERT_STR_EQ(k, "k");
    char *l = btree_uint32_get(tree->root, 23);
    ASSERT_STR_EQ(l, "l");
    char *m = btree_uint32_get(tree->root, 25);
    ASSERT_STR_EQ(m, "m");
    char *n = btree_uint32_get(tree->root, 27);
    ASSERT_STR_EQ(n, "n");
    char *o = btree_uint32_get(tree->root, 29);
    ASSERT_STR_EQ(o, "o");
    char *p = btree_uint32_get(tree->root, 31);
    ASSERT_STR_EQ(p, "p");
    char *q = btree_uint32_get(tree->root, 33);
    ASSERT_STR_EQ(q, "q");
    char *r = btree_uint32_get(tree->root, 35);
    ASSERT_STR_EQ(r, "r");
    char *s = btree_uint32_get(tree->root, 37);
    ASSERT_STR_EQ(s, "s");
    char *t = btree_uint32_get(tree->root, 39);
    ASSERT_STR_EQ(t, "t");
    char *u = btree_uint32_get(tree->root, 41);
    ASSERT_STR_EQ(u, "u");
    char *v = btree_uint32_get(tree->root, 43);
    ASSERT_STR_EQ(v, "v");
    char *w = btree_uint32_get(tree->root, 45);
    ASSERT_STR_EQ(w, "w");
    char *x = btree_uint32_get(tree->root, 47);
    ASSERT_STR_EQ(x, "x");

    btree_uint32_destroy(tree);
    PASS();
}



/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

    RUN_TEST(test_btree);

    GREATEST_MAIN_END();        /* display results */
}