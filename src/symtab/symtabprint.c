#include <string.h>

#include <james_utils.h>
#include <parser/op.h>
#include <symtab/symtab.h>

#define STPRINT_F(fmt, ...) \
    fprintf(stderr, fmt "\n", __VA_ARGS__);

#define STPRINT(fmt) \
    fprintf(stderr, fmt "\n");

#define STPRINT_NNL(fmt) \
    fprintf(stderr, fmt);

#define JUSTIFY \
    fprintf(stderr, "%*c", depth+1, ' ');

void declaratorprinter(ast_node *declarator)
{
    static int fresh_entry = 1;
    static int depth;
    
    if(fresh_entry == 1)
    {
        depth = 0;
        fresh_entry = 0;
    }

    if(declarator == NULL || declarator->list.value->op_type == IDENT) 
    {
         fresh_entry = 1;
         return;
    }
    
    declaratorprinter(declarator->list.next);
    
    depth++;

    switch(declarator->list.value->op_type)
    {
        case POINTER:
            STPRINT("pointer to");
            break;
        case ARRAY:
            STPRINT("array of");    // size?
            break;
        case FUNCTION:
            STPRINT("function of"); // with??
            break;
        default:
            STDERR_F("encoundered bad vardable %d", 
                     declarator->list.value->op_type);
            astprint(declarator->list.value);
            break;
    }
    JUSTIFY;
}

void declspecsprinter(ast_node *decl_specs)
{
    ast_node *i = decl_specs;
    while(i != NULL)
    {
        switch(i->list.value->op_type)
        {
            case VOID:
                STPRINT_NNL("void");
                break;
            case CHAR:
                STPRINT_NNL("char");
                break;
            case SHORT:
                STPRINT_NNL("short");
                break;
            case INT:
                STPRINT_NNL("int");
                break;
            case LONG:
                STPRINT_NNL("long");
                break;
            case FLOAT:
                STPRINT_NNL("float");
                break;
            case DOUBLE:
                STPRINT_NNL("double");
                break;
            case SIGNED:
                STPRINT_NNL("signed");
                break;
            case UNSIGNED:
                STPRINT_NNL("unsigned");
                break;
            case BOOL:
                STPRINT_NNL("bool");    // char under the hood ... 
                break;
            case STRUCT:
                STPRINT_NNL("struct");
                break;
            case UNION:
                STPRINT_NNL("union");
                break;
            case CONST:
                STPRINT_NNL("const");
                break;
            case RESTRICT:
                STPRINT_NNL("restrict");
                break;
            case VOLATILE:
                STPRINT_NNL("volatile");
                break;
            default:
                STPRINT_NNL("undefined");
                break;
        }
        STPRINT_NNL(" ");
        i = i->list.next;
    }
    STPRINT("");
    return;
}

void symtabprint(symbol_scope *scope, enum namespaces ns, char *l)
{
    symtab_elem *e = symtab_lookup(scope, l, ns);

    if(scope == NULL)
    {
        STDERR_F("Given scope invalid when looking up %s, ns %s!", 
                l, nsdecode(ns));
    }

    if(e == NULL)
    {
        STDERR_F("%s is not found in %s, ns %s",
                l, scopedecode(scope->scope), nsdecode(ns));
    }

    STPRINT_F("%s is defined at %s:%u [in %s scope starting at %s:%u] as a",
              l, e->file_origin, e->line_num_origin, 
              scopedecode(scope->scope), scope->origin_file, scope->origin_lineno);
              // TODO: maybe refactor symtab_scope to match symtab_elem? please?

    ast_node *dvar = e->d;
    STPRINT_F("%s with stgclass %s of type: ",
              nodetypedecode(dvar), 
              stgclassdecode(dvar->d.stgclass));
    declaratorprinter(dvar->d.declarator);
    declspecsprinter(dvar->d.decl_specs);
    return;
}

char *stgclassdecode(ast_node *n)
{
    static char name[32];
    name[0] = '\0';

    if(n == NULL)
    {
        strcpy(name, "NULL?");
        return name;
    }
       
    switch(n->op_type)
    {
        case TYPEDEF:
            strcpy(name, "typedef");
            return name; 
        case EXTERN:
            strcpy(name, "extern");
            return name;
        case STATIC:
            strcpy(name, "static");
            return name;
        case AUTO:
            strcpy(name, "auto");
            return name;
        case REGISTER:
            strcpy(name, "register");
            return name;
        default:
            strcpy(name, "<invalid>");
            astprint(n);
            return name;
    }

}

char *scopedecode(enum scopes s)
{
    static char name[32];
    name[0] = '\0';

    switch(s)
    {
        case SCOPE_GLOBAL:
            strcpy(name, "global");
            return name;
        case SCOPE_FUNCTION:
            strcpy(name, "function");
            return name;
        case SCOPE_SUE:
            strcpy(name, "struct/union member");
            return name;
        default:
            strcpy(name, "UNDEFINED");
            return name;
    }
}

char *nsdecode(enum namespaces ns)
{
    static char name[32];
    name[0] = '\0';

    switch(ns)
    {
        case NS_LABELS:
            strcpy(name, "label");
            return name;
        case NS_SUE:
            strcpy(name, "struct/union");
            return name;
        case NS_IDENTS:
            strcpy(name, "ident");
            return name;
        case NS_MEMBERS:
            strcpy(name, "member");
            return name;
        default:
            strcpy(name, "UNDEFINED");
            return name;
    }
}

char *nodetypedecode(ast_node *n)
{
    static char name[32];
    name[0] = '\0';

    switch(n->op_type)
    {
        case DECLARATION: // variable
            strcpy(name, "variable");
            return name;
        case FNDEF:
            strcpy(name, "function");
            return name;
        default:
            strcpy(name, "UNDEFINED");
            astprint(n);
            return name;
    }
}
