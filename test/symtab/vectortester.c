#include <stdio.h>
#include <string.h>

#include <ast/ast.h>
#include <james_utils.h>
#include <parser/grammar.tab.h>
#include <symtab/vector.h>

int main() {
    STDERR("Test 1: Vector size 12");
    vector *v = create_vector(12);

    for(int i = 0; i < v->size; i++){
        v->a[i] = create_node(IDENT);
        char string[10];
        sprintf(string, "%d", i);
        v->a[i]->ident.value = strdup(string);
    }

    for(int i = 0; i < v->size; i++){
        astprint(v->a[i]);
    }
    STDERR("Test 1: PASS");
    STDERR("Test 2: Vector resize");
    STDERR("Test 2: Step 1: resize to size * 2");
    if(resize_vector(v, 12*2) != 0){
        STDERR("FAILED TO REALLOC!!!");
        return 1;
    }

    return 0;
}
