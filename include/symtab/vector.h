/* reimplementation of vectors from C++
 * mostly concerned with the resizing and fast accesses
 * tooled for nodes
 */

#include <stdlib.h>

#include <ast/ast.h>

typedef struct vector_t {
    unsigned int size;

    ast_node **a;   // generate
} vector;

vector *create_vector(unsigned int size);
int resize_vector(vector *v, unsigned int size);
