#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdarg.h>

#define VERBOSE_ERRORS

#ifdef VERBOSE_ERRORS
    #define V_ERRS(...) __VA_ARGS__
#else
    #define V_ERRS(...)
#endif

#ifdef DEF_TREE_TYPE
    typedef DEF_TREE_TYPE elem_tree_t;
#else
    #define DEFAULT_STACK_TYPE int
    typedef DEFAULT_STACK_TYPE elem_tree_t;
#endif // DEF_TREE_TYPE

// typedef struct tree_node {
//     struct tree_node *parent;
//     tree_node_t *left_node;
//     tree_node_t *right_node;
//     elem_tree_t data;
// } tree_node_t;

typedef struct zzz_tree_node_struct {
    struct zzz_tree_node_struct *parent;
    struct zzz_tree_node_struct *left_node;
    struct zzz_tree_node_struct *right_node;
    elem_tree_t data;
} tree_node_t;

typedef struct {
    // const char *txt_name;
    // FILE *fp_txt;
    tree_node_t *root_node;
    size_t roots_amount;
    size_t depth;
    tree_node_t **nodes;
} full_tree_t;

typedef enum {
    NODE_HAS_VAL,
    NODE_IS_NULL,
    NODE_LOST_VAL,
} read_node_info_t;

typedef enum {
    ADD_NOTHING = 0,
    ADD_NAME = 1,
    ADD_FILE = 1 << 1,
} stat_tree_t;

#define MAX_STR_SIZE 1024
#define GET_STR(___x___) #___x___
#define FORMAT_STR(___size___) "%" GET_STR(___size___) "[^}]"
#ifndef DATA_FORMAT_OUT
    #define DATA_FORMAT_OUT "%d"
#endif
#ifndef DATA_FORMAT_IN
    #define DATA_FORMAT_IN "%d"
#endif
#define DATA_DEFAULT_VAL 0
#define LEFT_BRACE_SYM '{'
#define RIGHT_BRACE_SYM '}'
#define END_NODE_SYM '*'
#define POISON_BYTE 0xAA
#define CMD_EXTRA_SYMBOLS_AMOUNT 128
#define ORIENTATION_VER "TB"
#define ORIENTATION_HOR "LR"
#ifndef NODESEP_VAL
    #define NODESEP_VAL 0.6
#endif
#ifndef SPLINES_TYPE
    #define SPLINES_TYPE "spline"
#endif
#ifndef FONT_SIZE
    #define FONT_SIZE 20
#endif
#ifndef LABEL_FONT
    #define LABEL_FONT "Verdana"
#endif
#ifndef NODE_SHAPE
    #define NODE_SHAPE "Mrecord"
#endif

static const char DFLT_PNG_NAME[] = "MyPng.png";
static const char DFLT_TXT_NAME[] = "MyTxt.txt";

#define CLOSE_BRACE(fp) fprintf(fp, "%c\n", RIGHT_BRACE_SYM)

FILE *GetCheckedFile(const char *f_name, const char *f_mode);
void GraphBranch(FILE *fp, tree_node_t *node, const char *commentary);
void BranchPrint(FILE *fp, tree_node_t *node, size_t rank);
const char *CrPngGraphLine(const char *txt_path, const char *png_path);
void ClearBranchNoUseIt(tree_node_t **node);
int DestroyBranch(tree_node_t **node);
tree_node_t *CreateNode(elem_tree_t val);
int NodeLinkLeft(tree_node_t *parent_node, tree_node_t *new_node);
int NodeLinkRight(tree_node_t *parent_node, tree_node_t *new_node);
int NodeChangeVal(tree_node_t *node, elem_tree_t new_val);
void PreorderBranchPrint(FILE *fp, tree_node_t *node);
void SkipFileSpaces(FILE *fp);
bool CheckIfNullNode(const char *str);
read_node_info_t GetNodeVal(FILE *fp, elem_tree_t *arg);
void SkipCloseBrace(FILE *fp);
tree_node_t *GetNodesFromFile(FILE *fp, tree_node_t *par_node);
int GetLeftBranchFile(const char *f_name, tree_node_t *parent_node);
int GetRightBranchFile(const char *f_name, tree_node_t *parent_node);
tree_node_t *GetRootBranchFile(const char *f_name);
int TreeStat(full_tree_t *tree_ptr, tree_node_t *root_node);
size_t CountNodesBranch(tree_node_t *node);
size_t GetBranchDepth(tree_node_t *node);

int TreeStat(full_tree_t *tree_ptr, tree_node_t *root_node) {
    assert(root_node);

    tree_ptr->root_node = root_node;
    tree_ptr->depth = GetBranchDepth(root_node);
    tree_ptr->roots_amount = CountNodesBranch(root_node);
    if (tree_ptr->nodes != NULL) {
        free(tree_ptr->nodes), tree_ptr->nodes = NULL;
    }
    tree_ptr->nodes = (tree_node_t **)calloc(tree_ptr->roots_amount, sizeof(tree_node_t *));
    if (tree_ptr->nodes == NULL) {
        return errno;
    }
    
    return errno;
    // if (storage_stats == ADD_NOTHING) {
    //     return errno;
    // }
    // va_list ap;
    // va_start(ap, storage_stats);
}

// size_t AddNodesTree(tree_node_t **arr, tree_node_t *root_node) {
//     assert(arr);
//     assert(root_node);

    
// }

#define SIDE_COUNTER(___side___) ((___side___ != NULL) ? CountNodesBranch(___side___) : 0)
size_t CountNodesBranch(tree_node_t *node) {
    assert(node);

    return 1 + SIDE_COUNTER(node->left_node) + SIDE_COUNTER(node->right_node);
}
#undef SIDE_COUNTER

tree_node_t *CreateNode(elem_tree_t val) {  

    tree_node_t *new_node = (tree_node_t *)calloc(1, sizeof(tree_node_t));
    if (new_node == NULL) {
        V_ERRS(fprintf(stderr, "FAILED memory allocation of a new node\n");)
        return NULL;
    }
    new_node->parent = NULL;
    new_node->data = val;
    new_node->left_node = NULL;
    new_node->right_node = NULL;
    return new_node;
}

int NodeLinkLeft(tree_node_t *parent_node, tree_node_t *new_node) {
    assert(parent_node);

    if (new_node != NULL) {
        new_node->parent = parent_node;
        if (parent_node->left_node) {
            return NodeChangeVal(parent_node->left_node, new_node->data);
        }
    }
    parent_node->left_node = new_node;
    return errno;
}

int NodeLinkRight(tree_node_t *parent_node, tree_node_t *new_node) {
    assert(parent_node);

    if (new_node != NULL) {
        new_node->parent = parent_node;
        if (parent_node->right_node) {
            return NodeChangeVal(parent_node->right_node, new_node->data);
        }
    }
    parent_node->right_node = new_node;
    return errno;
}

int NodeChangeVal(tree_node_t *node, elem_tree_t new_val) {
    assert(node);

    node->data = new_val;
    return errno;
}

#define FREE(___ptr___, ___bytes_amount___) do { \
    if (___ptr___) { \
        memset(___ptr___, POISON_BYTE, ___bytes_amount___); \
        free(___ptr___), ___ptr___ = NULL; \
    } } while(0)

void ClearBranchNoUseIt(tree_node_t **node) {
    assert(node);
    assert(*node);

    if ((*node)->left_node) {
        ClearBranchNoUseIt(&((*node)->left_node));
    }
    if ((*node)->right_node) {
        ClearBranchNoUseIt(&((*node)->right_node));
    }
    FREE((*node), sizeof(tree_node_t));
}

int DestroyBranch(tree_node_t **node) {
    assert(node);
    assert(*node);

    if ((*node)->parent) {
        if ((*node)->parent->left_node == *node) {
            (*node)->parent->left_node = NULL;
        }
        if ((*node)->parent->right_node == *node) {
            (*node)->parent->right_node = NULL;
        }
    }
    ClearBranchNoUseIt(node);
    return errno;
}

FILE *GetCheckedFile(const char *f_name, const char *f_mode) {
    assert(f_name);
    assert(f_mode);

    FILE *fp = fopen(f_name, f_mode);
    if (fp == NULL) {
        fprintf(stderr, "File %s in %s mode has not been opened or created\n", f_name, f_mode);
        fprintf(stderr, "%d: %s\n", errno, strerror(errno));
        return NULL;
    }
    return fp;
}

#define SET_GLOBAL_SETTINGS(___stream___, ___commentary___) do { \
        fprintf(___stream___, "digraph G {\n"); \
        fprintf(___stream___, "nodesep=%lg;\n", NODESEP_VAL); \
        fprintf(___stream___, "splines=" SPLINES_TYPE ";\n"); \
        if (strlen(___commentary___)) { \
            fprintf(___stream___, "label=\"%s\"; labelloc=\"t\"; fontsize=%d; fontname=\"" \
                LABEL_FONT "\";\n", ___commentary___, FONT_SIZE); \
        } \
        fprintf(___stream___, "rankdir=%s;\n", ORIENTATION_VER); \
        fprintf(fp, "node [shape=" NODE_SHAPE "];\n"); \
    } while(0)
void GraphBranch(FILE *fp, tree_node_t *node, const char *commentary) {
    assert(fp);
    assert(node);

    SET_GLOBAL_SETTINGS(fp, commentary);
    BranchPrint(fp, node, 1);
    CLOSE_BRACE(fp);
}
#undef SET_GLOBAL_SETTINGS

size_t GetBranchDepth(tree_node_t *node) {
    if (node == NULL) {
        return 0;
    }
    size_t left_depth = GetBranchDepth(node->left_node);
    size_t right_depth = GetBranchDepth(node->right_node);
    return 1 + ((left_depth > right_depth) ? left_depth : right_depth);
}

// void PrintFrameWork()

#define RECURSIVE_PRINT(___son_node___, ___parent_side___) do { \
    if (___son_node___) { \
        fprintf(fp, "son%p:" ___parent_side___ " -> son%p:parent:n [weight=1];\n", node, ___son_node___); \
        BranchPrint(fp, ___son_node___, rank + 1); \
    } } while(0)
void BranchPrint(FILE *fp, tree_node_t *node, size_t rank) {
    assert(fp);
    assert(node);

    fprintf(fp, "son%p [label=\"{<parent> parent = %p | data = " DATA_FORMAT_OUT " | {<left> left = %p | <right> right = %p}}\"];\n", 
        node, node->parent, node->data, node->left_node, node->right_node);
    fprintf(fp, "subgraph level_%llu { { rank=same; son%p; } }\n", rank, node);
    RECURSIVE_PRINT(node->left_node, "left:sw");
    RECURSIVE_PRINT(node->right_node, "right:se");
}
#undef RECURSIVE_PRINT

#define IF_HOLLOW_SON(___fp___, ___son___) do { \
    if (___son___ == NULL) { \
        fprintf(___fp___, "%c %c %c ", LEFT_BRACE_SYM, END_NODE_SYM, RIGHT_BRACE_SYM); \
    } \
    else { \
        PreorderBranchPrint(___fp___, ___son___); \
    } } while(0)
void PreorderBranchPrint(FILE *fp, tree_node_t *node) {
    assert(fp);
    assert(node);

    fprintf(fp, "%c " DATA_FORMAT_OUT " ", LEFT_BRACE_SYM, node->data);
    IF_HOLLOW_SON(fp, node->left_node);
    IF_HOLLOW_SON(fp, node->right_node);
    fprintf(fp, "%c ", RIGHT_BRACE_SYM);
}
#undef IF_HOLLOW_SON

void SkipFileSpaces(FILE *fp) {
    assert(fp);

    int sym = '\0';
    while ((sym = fgetc(fp)) != EOF) {
        if (!isspace(sym)) {
            ungetc(sym, fp);
            return ;
        }
    }
}

bool CheckIfNullNode(const char *str) {
    assert(str);

    while (isspace(*str) && *str != '\0') {
        
        str++;
    }
    if (*str == END_NODE_SYM) {
        str++;
        while (isspace(*str) && *str != '\0') {
            str++;
        }

        if (*str == '\0' || *str == RIGHT_BRACE_SYM) {
            return true;
        }
        return false;
    }
    return false;
}

read_node_info_t GetNodeVal(FILE *fp, elem_tree_t *arg) {
    assert(fp);
    assert(arg);

    
    char str[MAX_STR_SIZE] = {};
    int sym = '\0';
    size_t i = 0;
    while ((sym = fgetc(fp)) != RIGHT_BRACE_SYM && sym != LEFT_BRACE_SYM && i < MAX_STR_SIZE - 1 && sym != EOF) {
        str[i++] = sym;
        
    }
    if (sym != EOF) {
        ungetc(sym, fp);
    }
    if (CheckIfNullNode(str)) {
        return NODE_IS_NULL;
    }
    // printf("<%s>\n", str);
    if (arg && sscanf(str, DATA_FORMAT_IN, arg) == 1) {
        return NODE_HAS_VAL;
    }
    V_ERRS(fprintf(stderr, "SSCANF FAILED IN file on %s\n", str);)
    return NODE_LOST_VAL;
}

void SkipCloseBrace(FILE *fp) {
    assert(fp);

    SkipFileSpaces(fp);
    int sym = fgetc(fp);
    if (sym != RIGHT_BRACE_SYM && sym != EOF) {
        ungetc(sym, fp);
    }
}

#define RET_IF_END_NODE(___node_info___, ___fp___) do { \
    if (___node_info___ == NODE_IS_NULL) { \
        SkipCloseBrace(___fp___); \
        return NULL; \
    } } while(0);
tree_node_t *GetNodesFromFile(FILE *fp, tree_node_t *par_node) {
    assert(fp);
    assert(par_node);

    SkipFileSpaces(fp);
    int sym = '\0';
    if ((sym = fgetc(fp)) == EOF) {
        return NULL;
    }
    if (sym != LEFT_BRACE_SYM) {
        ungetc(sym, fp);
        return NULL;
    }
    elem_tree_t val = DATA_DEFAULT_VAL;
    tree_node_t *cur_node = NULL;
    read_node_info_t node_info = GetNodeVal(fp, &val);
    RET_IF_END_NODE(node_info, fp);
    if ((cur_node = CreateNode(val))) {
        cur_node->parent = par_node;
        cur_node->left_node = GetNodesFromFile(fp, cur_node);
        cur_node->right_node = GetNodesFromFile(fp, cur_node);
        SkipCloseBrace(fp);
    }
    else {
        SkipCloseBrace(fp);
        return NULL;
    }
    return cur_node;
}
#undef RET_IF_END_NODE

#define GET_BRANCH(___f_name___, ___son___, ___parent___, ___err_ret___) do { \
        FILE *fp = GetCheckedFile(___f_name___, "r"); \
        if (fp == NULL) { \
            V_ERRS(fprintf(stderr, "File %s was not opened in \"r\" mode\n", ___f_name___);) \
            return ___err_ret___; \
        } \
        ___son___ = GetNodesFromFile(fp, ___parent___); \
        fclose(fp); \
    } while(0)
int GetLeftBranchFile(const char *f_name, tree_node_t *parent_node) {
    assert(f_name);
    assert(parent_node);

    GET_BRANCH(f_name, parent_node->left_node, parent_node, errno);
    return errno;
}

int GetRightBranchFile(const char *f_name, tree_node_t *parent_node) {
    assert(f_name);
    assert(parent_node);

    GET_BRANCH(f_name, parent_node->right_node, parent_node, errno);
    return errno;
}

#define ROOT_NODE_NO_ASSERT_PTR (tree_node_t *)1
tree_node_t *GetRootBranchFile(const char *f_name) {
    assert(f_name);

    tree_node_t *root_node = (tree_node_t *)calloc(1, sizeof(tree_node_t));
    if (root_node == NULL) {
        return NULL;
    }
    GET_BRANCH(f_name, root_node, ROOT_NODE_NO_ASSERT_PTR, NULL);
    root_node->parent = NULL;
    return root_node;
}
#undef ROOT_NODE_NO_ASSERT_PTR
#undef GET_BRANCH

const char *CrPngGraphLine(const char *txt_path, const char *png_path) {
    assert(txt_path);
    assert(png_path);

    size_t full_size = CMD_EXTRA_SYMBOLS_AMOUNT + strlen(txt_path) + strlen(png_path);
    char *cmd_line = (char *)calloc(full_size, sizeof(char));
    if (cmd_line == NULL) {
        return NULL;
    }
    snprintf(cmd_line, CMD_EXTRA_SYMBOLS_AMOUNT, "dot \"%s\" -Tpng  -o \"%s\"", txt_path, png_path);
    return cmd_line;
}