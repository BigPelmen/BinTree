#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdarg.h>

#ifdef CUMMING_DUMP
    #define CUM_ON(...) __VA_ARGS__
#else
    #define CUM_ON(...)
#endif

#define VERBOSE_ERRORS

#ifdef VERBOSE_ERRORS
    #define V_ERRS(...) __VA_ARGS__
#else
    #define V_ERRS(...)
#endif

#define DEF_TREE_TYPE char * 

#ifdef DEF_TREE_TYPE
    typedef DEF_TREE_TYPE elem_tree_t;
#else
    #define DEFAULT_STACK_TYPE int
    typedef DEFAULT_STACK_TYPE elem_tree_t;
#endif // DEF_TREE_TYPE

typedef struct tree_node_t {
    tree_node_t *parent;
    tree_node_t *left_node;
    tree_node_t *right_node;
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

typedef enum {
    ENODE_OK,
    ENODE_CALLOC,
    ENODE_BRANCH_FILE_R,
} bin_node_err_t;

#define MAX_STR_SIZE 1023
#define GET_STR(___x___) #___x___
#define FORMAT_STR(___size___) "%" GET_STR(___size___) "[^}]"
// char *
#ifndef DATA_FORMAT_OUT
    #define DATA_FORMAT_OUT "%s"
#endif
// char *
#ifndef DATA_FORMAT_IN
    #define DATA_FORMAT_IN FORMAT_STR(MAX_STR_SIZE)
#endif
#define DATA_DEFAULT_VAL NULL
#define LEFT_BRACE_SYM '{'
#define RIGHT_BRACE_SYM '}'
#define END_NODE_SYM '*'
#define POISON_BYTE 0xAA
#define CMD_EXTRA_SYMBOLS_AMOUNT 128
#define ORIENTATION_VER "TB"
#define ORIENTATION_HOR "LR"
#ifndef NODE_COLOR
    #define NODE_COLOR "#a70000"
#endif
#ifndef NODE_END_FILL_COLOR
    #define NODE_END_FILL_COLOR "#e860a9"
#endif
#ifndef NODE_FILL_COLOR
    #define NODE_FILL_COLOR "#ff5142"
#endif
#ifndef NODE_CONNECTION_COLOR_YES
    #define NODE_CONNECTION_COLOR_YES "#1e8c00"
#endif
#ifndef NODE_CONNECTION_COLOR_NO
    #define NODE_CONNECTION_COLOR_NO "#00538b"
#endif
#ifndef NODE_FONT_SIZE
    #define NODE_FONT_SIZE 25
#endif
#ifndef NODESEP_VAL
    #define NODESEP_VAL 0.6
#endif
#ifndef RANKSEP_VAL
    #define RANKSEP_VAL 0.75
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

#define FREE(___ptr___, ___bytes_amount___) do { \
    if (___ptr___) { \
        memset(___ptr___, POISON_BYTE, ___bytes_amount___); \
        free(___ptr___), ___ptr___ = NULL; \
    } } while(0)

FILE *GetCheckedFile(const char *f_name, const char *f_mode);
void GraphBranch(FILE *fp, tree_node_t *node, const char *commentary);
void BranchPrint(FILE *fp, tree_node_t *node, size_t rank);
void AdjustPrintOneNode(FILE *fp, tree_node_t *node, size_t rank);
const char *CrPngGraphLine(const char *txt_path, const char *png_path);
int StartGraphvizTxtCmd(const char *txt_path, const char *png_path);
void ClearBranchNoUseIt(tree_node_t **node);
int DestroyBranch(tree_node_t **node);
tree_node_t *CreateNode(elem_tree_t val);
int NodeLinkLeft(tree_node_t *parent_node, tree_node_t *new_node);
int NodeLinkRight(tree_node_t *parent_node, tree_node_t *new_node);
int NodeChangeVal(tree_node_t *node, elem_tree_t new_val);
void PreorderBranchPrint(FILE *fp, tree_node_t *node);
void SkipFileSpaces(FILE *fp);
bool CheckIfNullNode(const char *str);
void CopyNodeData(char *data_str, FILE *fp);
read_node_info_t GetNodeVal(FILE *fp, elem_tree_t *arg);
void SkipCloseBrace(FILE *fp);
tree_node_t *GetNodesFromFile(FILE *fp, tree_node_t *par_node);
bin_node_err_t GetLeftBranchFile(const char *f_name, tree_node_t *parent_node);
bin_node_err_t GetRightBranchFile(const char *f_name, tree_node_t *parent_node);
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
}

size_t CountNodesBranch(tree_node_t *node) {
    assert(node);

    return 1 + ((node->left_node != NULL) ? CountNodesBranch(node->left_node) : 0) + 
        ((node->right_node != NULL) ? CountNodesBranch(node->right_node) : 0);
}

tree_node_t *CreateNode(elem_tree_t val) {  

    tree_node_t *new_node = (tree_node_t *)calloc(1, sizeof(tree_node_t));
    if (new_node == NULL) {
        V_ERRS(fprintf(stderr, "FAILED memory allocation of a new node\n");)
        return NULL;
    }
    new_node->parent = NULL;
    // char *
    new_node->data = val ? strdup(val) : NULL;
    new_node->left_node = NULL;
    new_node->right_node = NULL;
    return new_node;
}

int NodeLinkLeft(tree_node_t *parent_node, tree_node_t *new_node) {
    assert(parent_node);

    if (new_node != NULL) {
        new_node->parent = parent_node;
        if (parent_node->left_node != NULL) {
            DestroyBranch(&(parent_node->left_node));
        }
    }
    parent_node->left_node = new_node;
    return errno;
}

int NodeLinkRight(tree_node_t *parent_node, tree_node_t *new_node) {
    assert(parent_node);

    if (new_node != NULL) {
        new_node->parent = parent_node;
        if (parent_node->right_node != NULL) {
            DestroyBranch(&(parent_node->right_node));
        }
    }
    parent_node->right_node = new_node;
    return errno;
}

int NodeChangeVal(tree_node_t *node, elem_tree_t new_val) {
    assert(node);

    // char *
    if (node->data != NULL) {  
        free(node->data);
    }
    node->data = new_val ? strdup(new_val) : DATA_DEFAULT_VAL;
    return errno;
}

void ClearBranchNoUseIt(tree_node_t **node) {
    assert(node);
    assert(*node);

    
    if ((*node)->left_node) {
        ClearBranchNoUseIt(&((*node)->left_node));
    }
    if ((*node)->right_node) {
        ClearBranchNoUseIt(&((*node)->right_node));
    }
    
    // char *
    if ((*node)->data != NULL) {
        free((*node)->data);  
    }
    FREE((*node), sizeof(tree_node_t));
}

int DestroyBranch(tree_node_t **node) {
    assert(node);
    assert(*node);

    tree_node_t *tmp_node = (*node);
    if (tmp_node->parent) {
        if (tmp_node->parent->left_node == *node) {
            tmp_node->parent->left_node = NULL;
        }
        else {
            if (tmp_node->parent->right_node == *node) {
                tmp_node->parent->right_node = NULL;
            }
        }
    }
    ClearBranchNoUseIt(&tmp_node);
    (*node) = NULL;
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

void SetGlobalSettings(FILE *fp, const char *commentary) {
    assert(fp);
    assert(commentary);

    fprintf(fp, "digraph G {\n");
    fprintf(fp, "nodesep=%lg;\n", NODESEP_VAL);
    fprintf(fp, "splines=" SPLINES_TYPE ";\n");
    fprintf(fp, "ranksep=%lg;\n", RANKSEP_VAL);
    if (strlen(commentary)) {
        fprintf(fp, "label=\"%s\"; labelloc=\"t\"; fontsize=%d; fontname=\""
            LABEL_FONT "\";\n", commentary, FONT_SIZE);
    }
    fprintf(fp, "rankdir=%s;\n", ORIENTATION_VER);
    fprintf(fp, "node [shape=" NODE_SHAPE ", color=\"" NODE_COLOR "\", penwidth=5.0, "
        "style=filled, fillcolor=\"" NODE_FILL_COLOR "\", fontsize=%d, fontname=\"Helvetica-Bold\"];\n", NODE_FONT_SIZE);
}

void GraphBranch(FILE *fp, tree_node_t *node, const char *commentary) {
    assert(fp);
    assert(node);

    SetGlobalSettings(fp, commentary);
    BranchPrint(fp, node, 1);
    CLOSE_BRACE(fp);
}

size_t GetBranchDepth(tree_node_t *node) {
    if (node == NULL) {
        return 0;
    }
    size_t left_depth = GetBranchDepth(node->left_node);
    size_t right_depth = GetBranchDepth(node->right_node);
    return 1 + ((left_depth > right_depth) ? left_depth : right_depth);
}

void BranchPrint(FILE *fp, tree_node_t *node, size_t rank) {
    assert(fp);
    assert(node);

    AdjustPrintOneNode(fp, node, rank);
    if (node->left_node) {
        fprintf(fp, "son%p:" CUM_ON("left:") "sw -> son%p:" CUM_ON("parent:") "n [weight=1, penwidth=2.5, color=\"" 
            NODE_CONNECTION_COLOR_YES "\"];\n", node, node->left_node);
        BranchPrint(fp, node->left_node, rank + 1);
    }
    if (node->right_node) {
        fprintf(fp, "son%p:" CUM_ON("right:") "se -> son%p:" CUM_ON("parent:") "n [weight=1, penwidth=2.5, color=\"" 
            NODE_CONNECTION_COLOR_NO "\"];\n", node, node->right_node);
        BranchPrint(fp, node->right_node, rank + 1);
    }
}

void AdjustPrintOneNode(FILE *fp, tree_node_t *node, size_t rank) {
    assert(fp);
    assert(node);

    if (node->left_node == NULL && node->right_node == NULL) {
        fprintf(fp, "son%p [label=\"{" CUM_ON("<parent> parent = %p | data = ") DATA_FORMAT_OUT 
            CUM_ON(" | {<left> left = %p | <right> right = %p}") "}\", fillcolor=\"" NODE_END_FILL_COLOR "\"];\n", 
            node, CUM_ON(node->parent,) node->data CUM_ON(, node->left_node, node->right_node));
    }
    else {
        fprintf(fp, "son%p [label=\"{" CUM_ON("<parent> parent = %p | data = ") 
            DATA_FORMAT_OUT CUM_ON(" | {<left> left = %p | <right> right = %p}") "}\"];\n", 
            node, CUM_ON(node->parent,) node->data CUM_ON(, node->left_node, node->right_node));    
    }
    fprintf(fp, "subgraph level_%llu { { rank=same; son%p; } }\n", rank, node);
}

void PreorderBranchPrint(FILE *fp, tree_node_t *node) {
    assert(fp);
    assert(node);

    fprintf(fp, "%c" DATA_FORMAT_OUT, LEFT_BRACE_SYM, node->data);
    if (node->left_node == NULL) {
        fprintf(fp, "%c %c %c ", LEFT_BRACE_SYM, END_NODE_SYM, RIGHT_BRACE_SYM);
    }
    else {
        PreorderBranchPrint(fp, node->left_node);
    }
    if (node->right_node == NULL) {
        fprintf(fp, "%c %c %c ", LEFT_BRACE_SYM, END_NODE_SYM, RIGHT_BRACE_SYM);
    }
    else {
        PreorderBranchPrint(fp, node->right_node);
    }
    fprintf(fp, "%c ", RIGHT_BRACE_SYM);
}

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

void CopyNodeData(char *data_str, FILE *fp) {
    assert(data_str);
    assert(fp);

    int sym = '\0';
    size_t i = 0;
    while ((sym = fgetc(fp)) != RIGHT_BRACE_SYM && sym != LEFT_BRACE_SYM && i < MAX_STR_SIZE - 1 && sym != EOF) {
        data_str[i++] = sym;
    }
    if (sym != EOF) {
        ungetc(sym, fp);
    }
}

read_node_info_t GetNodeVal(FILE *fp, elem_tree_t *arg) {
    assert(fp);
    assert(arg);

    char str[MAX_STR_SIZE] = {};
    CopyNodeData(str, fp);
    if (CheckIfNullNode(str)) {
        return NODE_IS_NULL;
    }
    // char *
    char *ptr = (char *)calloc(MAX_STR_SIZE + 1, sizeof(char));
    if (ptr == NULL) {
        return NODE_LOST_VAL;
    }
    if (sscanf(str, DATA_FORMAT_IN, ptr) == 1) {
        ptr = (elem_tree_t)realloc(ptr, strlen(ptr) + 1);
        if (ptr == NULL) {
            return NODE_LOST_VAL;
        }
        *arg = ptr;
        return NODE_HAS_VAL;
    }
    free(ptr);
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
    elem_tree_t buf_val = DATA_DEFAULT_VAL;
    read_node_info_t node_info = GetNodeVal(fp, &buf_val);
    tree_node_t *cur_node = CreateNode(buf_val);
    free(buf_val), buf_val = NULL;
    if (node_info == NODE_IS_NULL) {
        SkipCloseBrace(fp);
        return NULL;
    }
    if (cur_node != NULL) {
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

bin_node_err_t GetLeftBranchFile(const char *f_name, tree_node_t *parent_node) {
    assert(f_name);
    assert(parent_node);

    if (parent_node->left_node != NULL) {
        DestroyBranch(&(parent_node->left_node));
    }
    FILE *fp = GetCheckedFile(f_name, "r");
    if (fp == NULL) {
        V_ERRS(fprintf(stderr, "File %s was not opened in \"r\" mode\n", f_name);) \
        return ENODE_BRANCH_FILE_R;
    }
    parent_node->left_node = GetNodesFromFile(fp, parent_node);
    fclose(fp);
    return ENODE_OK;
}

bin_node_err_t GetRightBranchFile(const char *f_name, tree_node_t *parent_node) {
    assert(f_name);
    assert(parent_node);

    if (parent_node->right_node != NULL) {
        DestroyBranch(&(parent_node->right_node));
    }
    FILE *fp = GetCheckedFile(f_name, "r");
    if (fp == NULL) {
        V_ERRS(fprintf(stderr, "File %s was not opened in \"r\" mode\n", f_name);) \
        return ENODE_BRANCH_FILE_R;
    }
    parent_node->right_node = GetNodesFromFile(fp, parent_node);
    fclose(fp);
    return ENODE_OK;
}

#define ROOT_NODE_NO_ASSERT_PTR (tree_node_t *)1
tree_node_t *GetRootBranchFile(const char *f_name) {
    assert(f_name);

    tree_node_t *root_node = NULL;
    FILE *fp = GetCheckedFile(f_name, "r");
    if (fp == NULL) {
        V_ERRS(fprintf(stderr, "File %s was not opened in \"r\" mode\n", f_name);) \
        return NULL;
    }
    root_node = GetNodesFromFile(fp, ROOT_NODE_NO_ASSERT_PTR);
    fclose(fp);
    if (root_node == NULL) {
        V_ERRS(fprintf(stderr, "Bad root node got from %s file\n", f_name);)
        return NULL; // ...
    }
    root_node->parent = NULL;
    return root_node;
}
#undef ROOT_NODE_NO_ASSERT_PTR

const char *CrPngGraphLine(const char *txt_path, const char *png_path) {
    assert(txt_path);
    assert(png_path);

    size_t full_size = CMD_EXTRA_SYMBOLS_AMOUNT + strlen(txt_path) + strlen(png_path);
    char *cmd_line = (char *)calloc(full_size, sizeof(char));
    if (cmd_line == NULL) {
        return NULL;
    }
    snprintf(cmd_line, full_size, "dot \"%s\" -Tpng  -o \"%s\"", txt_path, png_path);
    return cmd_line;
}

int StartGraphvizTxtCmd(const char *txt_path, const char *png_path) {
    assert(txt_path);
    assert(png_path);

    char *tmp_ptr = (char *)CrPngGraphLine(txt_path, png_path);
    if (tmp_ptr == NULL) {
        V_ERRS(fprintf(stderr, "Did not manage to construct .dot file by %s and %s file-pathes\n", txt_path, png_path);)
        return errno;
    }
    int sys_status = 0;
    system(tmp_ptr);
    sys_status = errno;
    free(tmp_ptr);
    return sys_status;
}