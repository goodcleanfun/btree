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


void check_tree(btree_uint32_node_t *tree, int lower, int upper, bool rec)
{  int i; int seq_error = 0;
   if( tree->height > 0 )
   {  printf("(%d:", tree->height);
      for(i = 1; i< tree->degree; i++ )
        printf(" %d", tree->keys[i]);
      for(i = 1; i< tree->degree; i++ )
      {  if( !( lower <= tree->keys[i] && tree->keys[i]<upper) )
         {  seq_error = 1;
         }
      }
      if( seq_error == 1)
        printf(":?"); 
      printf(")");
      check_tree(tree->children[0], lower, tree->keys[1], true);
      for(i = 1; i< tree->degree-1; i++ )
        check_tree(tree->children[i], tree->keys[i], tree->keys[i+1], true);
      check_tree(tree->children[tree->degree-1], 
              tree->keys[tree->degree-1], upper, true);
   }
   else
   {  printf("[");
      for(i = 0; i< tree->degree; i++ )
        printf(" %d", tree->keys[i]);
      for(i = 0; i< tree->degree; i++ )
      {  if( !( lower <= tree->keys[i] && tree->keys[i]<upper) )
         {  seq_error = 1;
         }
      }
      if( seq_error == 1)
        printf(":?");
      printf("]");
   }
   if (!rec)
     printf("\n");
}

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

    char *a = btree_uint32_search(tree->root, 1);
    ASSERT_STR_EQ(a, "a");

    char *b = btree_uint32_search(tree->root, 3);
    ASSERT_STR_EQ(b, "b");

    char *c = btree_uint32_search(tree->root, 5);
    ASSERT_STR_EQ(c, "c");

    char *d = btree_uint32_search(tree->root, 7);
    ASSERT_STR_EQ(d, "d");

    char *e = btree_uint32_search(tree->root, 9);
    ASSERT_STR_EQ(e, "e");

    a = btree_uint32_delete(tree, 1);
    ASSERT_STR_EQ(a, "a");

    a = btree_uint32_search(tree->root, 1);
    ASSERT(a == NULL);

    b = btree_uint32_delete(tree, 3);
    ASSERT_STR_EQ(b, "b");

    e = btree_uint32_delete(tree, 9);
    ASSERT_STR_EQ(e, "e");

    c = btree_uint32_search(tree->root, 5);
    ASSERT_STR_EQ(c, "c");

    c = btree_uint32_delete(tree, 5);
    ASSERT_STR_EQ(c, "c");

    d = btree_uint32_delete(tree, 7);
    ASSERT_STR_EQ(d, "d");

    d = btree_uint32_search(tree->root, 7);
    ASSERT(d == NULL);

    // Tree should be empty
    ASSERT_EQ(tree->root->height, 0);
    ASSERT_EQ(tree->root->degree, 0);

    btree_uint32_insert(tree, 7, "d");
    d = btree_uint32_search(tree->root, 7);

    // Root should have one child
    ASSERT_EQ(tree->root->height, 0);
    ASSERT_EQ(tree->root->degree, 1);

    d = btree_uint32_delete(tree, 7);
    ASSERT_STR_EQ(d, "d");
    ASSERT_EQ(tree->root->height, 0);
    ASSERT_EQ(tree->root->degree, 0);

    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 47, "x");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 33, "q");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 15, "h");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 35, "r");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 45, "w");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 17, "i");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 19, "j");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 37, "s");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 21, "k");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 11, "f");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 41, "u");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 23, "l");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 25, "m");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 27, "n");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 29, "o");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 13, "g");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 31, "p");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 39, "t");
    check_tree(tree->root, 0, 50, false);
    btree_uint32_insert(tree, 43, "v");
    check_tree(tree->root, 0, 50, false);

    check_tree(tree->root, 0, 50, false);

    char *f = btree_uint32_search(tree->root, 11);
    ASSERT_STR_EQ(f, "f");
    char *g = btree_uint32_search(tree->root, 13);
    ASSERT_STR_EQ(g, "g");
    char *h = btree_uint32_search(tree->root, 15);
    ASSERT_STR_EQ(h, "h");
    char *i = btree_uint32_search(tree->root, 17);
    ASSERT_STR_EQ(i, "i");
    char *j = btree_uint32_search(tree->root, 19);
    ASSERT_STR_EQ(j, "j");
    char *k = btree_uint32_search(tree->root, 21);
    ASSERT_STR_EQ(k, "k");
    char *l = btree_uint32_search(tree->root, 23);
    ASSERT_STR_EQ(l, "l");
    char *m = btree_uint32_search(tree->root, 25);
    ASSERT_STR_EQ(m, "m");
    char *n = btree_uint32_search(tree->root, 27);
    ASSERT_STR_EQ(n, "n");
    char *o = btree_uint32_search(tree->root, 29);
    ASSERT_STR_EQ(o, "o");
    char *p = btree_uint32_search(tree->root, 31);
    ASSERT_STR_EQ(p, "p");
    char *q = btree_uint32_search(tree->root, 33);
    ASSERT_STR_EQ(q, "q");
    char *r = btree_uint32_search(tree->root, 35);
    ASSERT_STR_EQ(r, "r");
    char *s = btree_uint32_search(tree->root, 37);
    ASSERT_STR_EQ(s, "s");
    char *t = btree_uint32_search(tree->root, 39);
    ASSERT_STR_EQ(t, "t");
    char *u = btree_uint32_search(tree->root, 41);
    ASSERT_STR_EQ(u, "u");
    char *v = btree_uint32_search(tree->root, 43);
    ASSERT_STR_EQ(v, "v");
    char *w = btree_uint32_search(tree->root, 45);
    ASSERT_STR_EQ(w, "w");
    char *x = btree_uint32_search(tree->root, 47);
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