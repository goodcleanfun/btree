#ifndef BTREE_H
#define BTREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define BTREE_DEFAULT_NODE_MAX_DEGREE 256
#define BTREE_MAX_HEIGHT 128

#endif // BTREE_H

#ifndef BTREE_NAME
#error "Must define BTREE_NAME"
#endif

#ifndef BTREE_KEY_TYPE
#error "Must define BTREE_KEY_TYPE"
#endif

#define BTREE_CONCAT_(a, b) a ## b
#define BTREE_CONCAT(a, b) BTREE_CONCAT_(a, b)
#define BTREE_TYPED(name) BTREE_CONCAT(BTREE_NAME, _##name)
#define BTREE_FUNC(func) BTREE_CONCAT(BTREE_NAME, _##func)

#ifndef BTREE_KEY_LESS_THAN
static inline bool BTREE_FUNC(key_less_than)(BTREE_KEY_TYPE key, BTREE_KEY_TYPE node_key) {
    return key < node_key;
}
#define BTREE_KEY_LESS_THAN BTREE_FUNC(key_less_than)
#endif

#ifndef BTREE_KEY_EQUALS
static inline bool BTREE_FUNC(key_equals)(BTREE_KEY_TYPE key, BTREE_KEY_TYPE node_key) {
    return key == node_key;
}
#define BTREE_KEY_EQUALS BTREE_FUNC(key_equals)
#endif

#ifndef BTREE_NODE_MAX_DEGREE
#define BTREE_NODE_MAX_DEGREE BTREE_DEFAULT_NODE_MAX_DEGREE
#define BTREE_NODE_MAX_DEGREE_DEFINED
#endif

#ifndef BTREE_NODE_MIN_DEGREE
#define BTREE_NODE_MIN_DEGREE ((BTREE_NODE_MAX_DEGREE) / 2)
#define BTREE_NODE_MIN_DEGREE_DEFINED
#endif

/*
The structure of this B-tree node is slightly different than the traditional CS one
due to the way leaves are stored (without needing an additional layer).

Each node stores a height and a degree.
The leaves are all at height 0 nodes, which keeps the tree balanced and logarithmic height.
Internal nodes do not store any leaf data and are used only for traversal/search.

At the inner nodes (height > 0)
    - keys are the internal search keys (only necessary to guide the search from root to leaf)
    - children are the pointers to the next level

Howver, at the leaf level (height = 0)
    - keys are the search keys
    - children are the values

Therefore we use the same number of keys and children throughout all the nodes
in contrast to the traditional B-tree where nodes have one fewer keys than children, forming separators.
*/
typedef struct BTREE_TYPED(node) {
    uint16_t degree;
    uint16_t height;
    BTREE_KEY_TYPE keys[BTREE_NODE_MAX_DEGREE];
    struct BTREE_TYPED(node) *children[BTREE_NODE_MAX_DEGREE];
} BTREE_TYPED(node_t);

#define BTREE_NODE BTREE_TYPED(node_t)

#define BTREE_NODE_MEMORY_POOL_NAME BTREE_TYPED(node_memory_pool)

#define MEMORY_POOL_NAME BTREE_NODE_MEMORY_POOL_NAME
#define MEMORY_POOL_TYPE BTREE_NODE
#include "memory_pool/memory_pool.h"
#undef MEMORY_POOL_NAME
#undef MEMORY_POOL_TYPE

#define BTREE_NODE_MEMORY_POOL_FUNC(name) BTREE_CONCAT(BTREE_NODE_MEMORY_POOL_NAME, _##name)

typedef struct {
    BTREE_TYPED(node_t) *root;
    BTREE_TYPED(node_memory_pool) *pool;
} BTREE_NAME;

BTREE_NAME *BTREE_FUNC(new)(void) {
    BTREE_NAME *tree = calloc(1, sizeof(BTREE_NAME));
    if (tree == NULL) return NULL;
    tree->pool = BTREE_NODE_MEMORY_POOL_FUNC(new)();
    if (tree->pool == NULL) {
        free(tree);
        return NULL;
    }
    BTREE_NODE *root = BTREE_NODE_MEMORY_POOL_FUNC(get)(tree->pool);
    if (root == NULL) {
        BTREE_NODE_MEMORY_POOL_FUNC(destroy)(tree->pool);
        free(tree);
        return NULL;
    }
    root->height = 0;
    root->degree = 0;
    tree->root = root;
    return tree;
}

void BTREE_FUNC(destroy)(BTREE_NAME *tree) {
    if (tree == NULL) return;
    if (tree->pool != NULL) {
        BTREE_NODE_MEMORY_POOL_FUNC(destroy)(tree->pool);
    }
    free(tree);
}


static inline size_t BTREE_FUNC(binary_search_node)(BTREE_NODE *node, BTREE_KEY_TYPE key) {
    /* Standard binary search, but in the case of a B-tree it tells us
       the index of the node the search key fits between/before/after.

       For node->keys[1] through node->keys[degree - 2], it's sort of like The Price Is Right.
       Return the index that is closest to the search value without going over.

       For the leftmost key i.e. node->keys[0], we know that the search key is less than node->keys[1]
       but it may be less than node->keys[0]. Either way the key goes to the leftmost child.

       For the rightmost key i.e. node->keys[degree - 1], the search key is greater than or equal to
       that rightmost key.

       For the following set of keys:
           [1, 2, 4, 6, 8]

       a search for 0 would return 0 (in the interval < 2)
       a search for 1 would return 0 (in the interval < 2)
       a search for 2 would return 1 (in the interval 2-4)
       a search for 3 would return 1 (in the interval 2-4)
       ...
       a search for 8 would return 4 (in the interval >= 8)
       a search for 10 would return 4 (in the interval >= 8)
    */
    size_t lo = 0;
    size_t hi = (size_t)node->degree;
    while (lo + 1 < hi) {
        size_t mid = (lo + hi) / 2;
        if (BTREE_KEY_LESS_THAN(key, node->keys[mid])) {
            hi = mid;
        } else {
            lo = mid;
        }
    }
    return lo;
}

void *BTREE_FUNC(get)(BTREE_NODE *node, BTREE_KEY_TYPE key) {
    if (node == NULL) return NULL;
    BTREE_NODE *current_node = node;
    void *value;
    if (current_node->degree == 0) return NULL;

    while (current_node->height >= 0) {
        size_t idx = BTREE_FUNC(binary_search_node)(current_node, key);
        if (current_node->height > 0) {
            current_node = current_node->children[idx];
        } else {
            // block of height 0 means we have leaf nodes
            if (BTREE_KEY_EQUALS(key, current_node->keys[idx])) {
                value = (void *)current_node->children[idx];
            } else {
                value = NULL;
            }
            return value;
        }
    }
    return NULL;
}

bool BTREE_FUNC(insert)(BTREE_NAME *tree, BTREE_KEY_TYPE key, void *value) {
    if (tree == NULL || tree->root == NULL) return false;
    BTREE_NODE *root = tree->root;
    BTREE_NODE *current_node = root;
    bool finished = false;
    if (root->height == 0 && root->degree == 0) {
        root->keys[0] = key;
        root->children[0] = (BTREE_NODE *)value;
        root->degree = 1;
        return true;
    }

    size_t i = 0;

    size_t stack_size = 0;
    BTREE_NODE *stack[BTREE_MAX_HEIGHT];

    while (current_node->height > 0) {
        if (stack_size >= BTREE_MAX_HEIGHT) return false;
        stack[stack_size++] = current_node;
        size_t idx = BTREE_FUNC(binary_search_node)(current_node, key);
        current_node = current_node->children[idx];
    }
    // current_node is now a leaf at which we insert
    BTREE_NODE *insert_value = (BTREE_NODE *)value;
    BTREE_KEY_TYPE insert_key = key;
    while (!finished) {
        uint32_t start = 0;
        int32_t i = 0;
        if (current_node->height > 0) {
            // insert in non-leaf starts at 1
            start = 1;
        } else {
            // insert in a leaf starts at 0
            start = 0;
        }

        if (current_node->degree < BTREE_NODE_MAX_DEGREE) {
            /* Node still has room to insert
               move everything up to create the insertion gap

               Example:
               [1, 2, 4, 6, 8]

                insert 5
                [1, 2, 4, 6, 8] -> [1, 2, 4, 5, 6, 8]
            */
            i = BTREE_FUNC(binary_search_node)(current_node, insert_key);
            // ensures that i is at least 1 in non-leaf/internal nodes
            if (i < start) i = start;

            /* Binary search returns the index of the closest key
               that is less than the search key if it (by definition) did not exist.
               
               There is one special case however if the search returns 0.
               Here we have to compare the insert key to the first key in the node.
               If the insert key is greater, we have to increment i by 1 to insert at the correct position.
               If it's less than the first key, we can insert at the first position.
            */
            if (!BTREE_KEY_LESS_THAN(insert_key, current_node->keys[i])) {
                i++;
            }
            // move everything to the right by 1 from the insertion index, insert the new key/child, and bump the degree
            memmove(current_node->keys + i + 1, current_node->keys + i, (current_node->degree - i) * sizeof(BTREE_KEY_TYPE));
            memmove(current_node->children + i + 1, current_node->children + i, (current_node->degree - i) * sizeof(BTREE_NODE *));
            current_node->keys[i] = insert_key;
            current_node->children[i] = insert_value;
            current_node->degree++;
            finished = true;
        } else {
            /* node is full, have to split
               split the node in half and copy the keys/children while also
               inserting the new key/child in the correct position.
            */
            bool insert_done = false;
            BTREE_NODE *new_node = BTREE_NODE_MEMORY_POOL_FUNC(get)(tree->pool);
            i = BTREE_NODE_MAX_DEGREE - 1;
            int32_t j = (BTREE_NODE_MAX_DEGREE - 1) / 2;
            while (j >= 0) {
                if (insert_done || BTREE_KEY_LESS_THAN(insert_key, current_node->keys[i])) {
                    new_node->children[j] = current_node->children[i];
                    new_node->keys[j--] = current_node->keys[i--];
                } else {
                    new_node->children[j] = insert_value;
                    new_node->keys[j--] = insert_key;
                    insert_done = true;
                }
            }
            // upper half done. If key has not been inserted, insert it in the lower half
            while (!insert_done) {
                if (BTREE_KEY_LESS_THAN(insert_key, current_node->keys[i]) && i >= start) {
                    current_node->children[i + 1] = current_node->children[i];
                    current_node->keys[i + 1] = current_node->keys[i];
                    i--;
                } else {
                    current_node->children[i + 1] = insert_value;
                    current_node->keys[i + 1] = insert_key;
                    insert_done = true;
                }
            } // finished insertion
            current_node->degree = (BTREE_NODE_MAX_DEGREE + 1) - ((BTREE_NODE_MAX_DEGREE + 1) / 2);
            new_node->degree = (BTREE_NODE_MAX_DEGREE + 1) / 2;
            new_node->height = current_node->height;
            // split nodes complete, insert the new node above
            insert_value = new_node;
            insert_key = new_node->keys[0];
            if (stack_size > 0) {
                // not at the root, move up one level
                current_node = stack[--stack_size];
            } else {
                // splitting the root, need copy to keep root address
                new_node = BTREE_NODE_MEMORY_POOL_FUNC(get)(tree->pool);
                memcpy(new_node->children, current_node->children, current_node->degree * sizeof(BTREE_NODE *));
                memcpy(new_node->keys, current_node->keys, current_node->degree * sizeof(BTREE_KEY_TYPE));
                new_node->height = current_node->height;
                new_node->degree = current_node->degree;
                current_node->height++;
                current_node->degree = 2;
                current_node->children[0] = new_node;
                current_node->children[1] = insert_value;
                current_node->keys[1] = insert_key;
                finished = true;
            }
        }
    }
    return true;
}


void *BTREE_FUNC(delete)(BTREE_NAME *tree, BTREE_KEY_TYPE key) {
    if (tree == NULL || tree->root == NULL) return NULL;
    BTREE_NODE *current = tree->root;
    BTREE_NODE *tmp = NULL;
    bool finished = false;

    size_t stack_size = 0;
    BTREE_NODE *stack[BTREE_MAX_HEIGHT];
    size_t index_stack[BTREE_MAX_HEIGHT];

    while (current->height > 0) {
        // not at the leaf level
        if (stack_size >= BTREE_MAX_HEIGHT) return NULL;

        size_t idx = BTREE_FUNC(binary_search_node(current, key));
        index_stack[stack_size] = idx;
        stack[stack_size++] = current;
        current = current->children[idx];
    }
    // current is now a leaf node where we can delete
    size_t i = BTREE_FUNC(binary_search_node)(current, key);
    if (!BTREE_KEY_EQUALS(key, current->keys[i])) {
        // key was not found
        return NULL;
    } else {
        // key exists, delete from leaf node
        void *deleted_value = (void *)current->children[i];
        current->degree--;
        memmove(current->children + i, current->children + i + 1, (current->degree - i) * sizeof(BTREE_NODE *));
        memmove(current->keys + i, current->keys + i + 1, (current->degree - i) * sizeof(BTREE_KEY_TYPE));
        // deleted from node, rebalance
        finished = false;
        while (!finished) {
            if (current->degree >= BTREE_NODE_MIN_DEGREE) {
                // node is still full enough, we're done
                finished = true;
            } else {
                // node became underfull
                if (stack_size == 0) {
                    // current node is the root
                    if (current->degree >= 2) {
                        // root has at least 2 children, keep it
                        finished = true;
                    } else if (current->height == 0) {
                        // deleted last key from the root, tree is empty, finished
                        finished = true;
                    } else {
                        // delete the root, copy to keep address
                        tmp = current->children[0];
                        memcpy(current->keys, tmp->keys, tmp->degree * sizeof(BTREE_KEY_TYPE));
                        memcpy(current->children, tmp->children, tmp->degree * sizeof(BTREE_NODE *));
                        current->degree = tmp->degree;
                        current->height = tmp->height;
                        BTREE_NODE_MEMORY_POOL_FUNC(release)(tree->pool, tmp);
                        finished = true;
                    }
                } else {
                    // delete from a non-root node, stack_size > 0
                    BTREE_NODE *parent = stack[--stack_size];
                    // current node's index in its parent's children array
                    size_t current_idx = index_stack[stack_size];
                    BTREE_NODE *neighbor = NULL;
                    if (current_idx < (size_t)parent->degree - 1) {
                        // current is not parent's last child, check right sibling
                        neighbor = parent->children[current_idx + 1];
                        if (neighbor->degree > BTREE_NODE_MIN_DEGREE) {
                            // neighbor has at least A + 1 keys, borrow one
                            i = (size_t)current->degree;
                            if (current->height > 0) {
                                current->keys[i] = parent->keys[current_idx + 1];
                            } else {
                                // on leaf level, take leaf key
                                current->keys[i] = neighbor->keys[0];
                                neighbor->keys[0] = neighbor->keys[1];
                            }
                            current->children[i] = neighbor->children[0];
                            parent->keys[current_idx + 1] = neighbor->keys[1];
                            neighbor->children[0] = neighbor->children[1];
                            memmove(neighbor->keys + 1, neighbor->keys + 2, (neighbor->degree - 2) * sizeof(BTREE_KEY_TYPE));
                            memmove(neighbor->children + 1, neighbor->children + 2, (neighbor->degree - 2) * sizeof(BTREE_NODE *));
                            neighbor->degree--;
                            current->degree++;
                            finished = true;
                        } else {
                            // sibling only has A keys, cannot borrow, have to merge
                            i = (size_t)current->degree;
                            if (current->height > 0) {
                                // non-leaf, take neighbor's key from parent, copy all other keys from neighbor
                                current->keys[i] = parent->keys[current_idx + 1];
                                memcpy(current->keys + i + 1, neighbor->keys + 1, (neighbor->degree - 1) * sizeof(BTREE_KEY_TYPE));
                            } else {
                                // on leaf level, copy all leaf keys from neighbor
                                memcpy(current->keys + i, neighbor->keys, neighbor->degree * sizeof(BTREE_KEY_TYPE));
                            }
                            // copy all children from neighbor
                            memcpy(current->children + i, neighbor->children, neighbor->degree * sizeof(BTREE_NODE *));
                            // add neighbor's degree to current
                            current->degree += neighbor->degree;
                            // release neighbor node to memory pool
                            BTREE_NODE_MEMORY_POOL_FUNC(release)(tree->pool, neighbor);
                            // remove neighbor from parent
                            parent->degree--;
                            i = current_idx + 1;
                            // shift parent's keys and children to the left
                            memmove(parent->keys + i, parent->keys + i + 1, (parent->degree - i) * sizeof(BTREE_KEY_TYPE));
                            memmove(parent->children + i, parent->children + i + 1, (parent->degree - i) * sizeof(BTREE_NODE *));
                            // deleted from parent, propagate up
                            current = parent;
                        }
                    } else {
                        // current is the last child, try left sibling
                        neighbor = parent->children[current_idx - 1];
                        if (neighbor->degree > BTREE_NODE_MIN_DEGREE) {
                            /* Left sibling has at least A + 1 keys, take its last key/child
                               and move it current's position 0.

                               Diagram:

                                      parent
                                     /      \
                                 neighbor   current
                               0 1 2 3 4 5 | 6 7 8 9
                            
                               After sharing:

                                     parent
                                    /      \
                                neighbor   current
                               0 1 2 3 4 | 5 6 7 8 9
                            */
                            // move current node's keys and children to the right by 1
                            memmove(current->children + 2, current->children + 1, (current->degree - 1) * sizeof(BTREE_NODE *));
                            memmove(current->keys + 2, current->keys + 1, (current->degree - 1) * sizeof(BTREE_KEY_TYPE));
                            // move neighbor's last child to current's first child
                            current->children[1] = current->children[0];
                            i = neighbor->degree;
                            current->children[0] = neighbor->children[i - 1];
                            if (current->height > 0) {
                                // if not a leaf, put parent's key in the second position of current
                                current->keys[1] = parent->keys[current_idx];
                                current->keys[0] = neighbor->keys[i - 1];
                            } else {
                                // if a leaf, put neighbor's last key in the first position
                                current->keys[1] = current->keys[0];
                                current->keys[0] = neighbor->keys[i - 1];
                            }
                            parent->keys[current_idx] = neighbor->keys[i - 1];
                            neighbor->degree--;
                            current->degree++;
                            finished = true;
                        } else {
                            // left sibling only has A keys, cannot borrow, have to merge
                            i = neighbor->degree;
                            if (current->height > 0) {
                                //non-leaf, take parent's key
                                neighbor->keys[i] = parent->keys[current_idx];
                            } else {
                                // on leaf level, take leaf key
                                neighbor->keys[i] = current->keys[0];
                            }
                            // copy all keys except the first from current to neighbor
                            // (since first key is set above)
                            memcpy(neighbor->keys + i + 1, current->keys + 1, (current->degree - 1) * sizeof(BTREE_KEY_TYPE));
                            // copy over all children from sibling
                            memcpy(neighbor->children + i, current->children, current->degree * sizeof(BTREE_NODE *));
                            // add current's degree to neighbor's
                            neighbor->degree += current->degree;
                            // release current node to memory pool
                            BTREE_NODE_MEMORY_POOL_FUNC(release)(tree->pool, current);
                            // decrease parent's degree. Since current is the last child,
                            // it's like popping from a stack. No need to shift keys/children
                            parent->degree--;
                            // deleted from parent, propagate up
                            current = parent;
                        } // end of share/join for left sibling
                    } // end of current is last child in parent
                } // end of delete root/non-root
            } // end of full/underfull check
        } // end of rebalance
        return deleted_value;
    }
}

#ifdef BTREE_NODE_MAX_DEGREE_DEFINED
#undef BTREE_NODE_MAX_DEGREE
#undef BTREE_NODE_MAX_DEGREE_DEFINED
#endif

#ifdef BTREE_NODE_MIN_DEGREE_DEFINED
#undef BTREE_NODE_MIN_DEGREE
#undef BTREE_NODE_MIN_DEGREE_DEFINED
#endif
