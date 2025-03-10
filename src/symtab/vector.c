#include <james_utils.h>
#include <symtab/vector.h>

vector *create_vector(unsigned int size) {
    vector *v = (vector *) calloc(1, sizeof(vector));
    v->size = size;
    v->a = (ast_node **) calloc(size, sizeof(ast_node));

    return v;
}

// Resizes a vector
//  0 on successful reallocation
//  1 otherwise (we retained the same size, or allocation failed)
int resize_vector(vector *v, unsigned int size) {
    if(size <= v->size) {    // we dont shrink
        return 1;
    }

    ast_node **new_a = (ast_node **) calloc(size, sizeof(ast_node));
    if(new_a == NULL)
    {
        ERROR("resize: Failed to resize from %d to %d elements: %s",
                v->size, size);
        return 1;
    }

    free(v->a);
    v->a = new_a;
    return 0;
}
