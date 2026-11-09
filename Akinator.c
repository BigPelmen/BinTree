#include "BinaryTree.c"

typedef struct {
    FILE *fp_storage_txt;
    const char *storage_txt_name;
    const char *png_name;
    const char *graph_txt_name;
    FILE *fp_graph_txt;
} data_files_t;

typedef struct {
    data_files_t *files_info;
    full_tree_t *bin_tree;
} storage_tree_t;

enum enum_switch_mode {
    NO_ARGS_MODE = 1,
    ONE_ARG_MODE,
};

typedef enum {
    SHOW_BASE_MODE,
    PLAY_AKINATOR_MODE,
    SHOW_THEN_PLAY_MODE,
    PLAY_THEN_SHOW_MODE,
} run_mode_t;

int RunAkinator(tree_node_t *root_node) {
    assert(root_node);
    
    // root_node->data = 12345;
    // printf("data: %d\n", root_node->data);

    return errno;
}

#define RET_GOT_ODD_FLAG(___str___, ...) do { \
        V_ERRS(fprintf(stderr, "The programm will be started in the Akinator mode as \"" \
            ___str___ "\" argument has been given\n" __VA_ARGS__);) \
        return PLAY_AKINATOR_MODE; \
    } while(0)
#define CHECK_MODE(___flag_one___, ___flag_two___, ___m_one___, ___m_two___) do { \
    if (*argv_str == ___flag_one___) { \
        if (*(argv_str + 1) == ___flag_two___) { \
            return ___m_one___; \
        } \
        return ___m_two___; \
    } } while(0)
run_mode_t ParseArgv(const char *argv_str) {
    assert(argv_str);

    while (*argv_str++ == '-') {
        ;
    }
    argv_str--;
    if (*argv_str == '\0') {
        RET_GOT_ODD_FLAG("-----...");
    }
    if (*argv_str != 'a' && *argv_str != 'l') {
        RET_GOT_ODD_FLAG("--...-%c", , *argv_str);
    }
    CHECK_MODE('a', 'l', PLAY_THEN_SHOW_MODE, PLAY_AKINATOR_MODE);
    CHECK_MODE('l', 'a', SHOW_THEN_PLAY_MODE, SHOW_BASE_MODE);
    return PLAY_AKINATOR_MODE;
}
#undef RET_GOT_ODD_FLAG
#undef CHECK_MODE

int RunShowDataBase(tree_node_t *root_node) {
    assert(root_node);

    FILE *fp = GetCheckedFile(DFLT_TXT_NAME, "w");
    GraphBranch(fp, root_node, "");
    fclose(fp);
    system(CrPngGraphLine(DFLT_TXT_NAME, DFLT_PNG_NAME));
    return errno;
}

#define SWITCH_MODE(___r_mode___, ___argc___) do { \
        switch (___argc___) { \
        case NO_ARGS_MODE: \
            ___r_mode___ = PLAY_AKINATOR_MODE; \
            break; \
        case ONE_ARG_MODE: \
            ___r_mode___ = ParseArgv(argv[1]); \
            break; \
        default: \
            ___r_mode___ = PLAY_AKINATOR_MODE; \
            break; \
        }; \
    } while(0)
#define SWITCH_RUN(___r_mode___, ___root_node___) do { \
        switch (___r_mode___) { \
        case PLAY_AKINATOR_MODE: \
            RunAkinator(___root_node___); \
            break; \
        case SHOW_BASE_MODE: \
            RunShowDataBase(___root_node___); \
            break; \
        case SHOW_THEN_PLAY_MODE: \
            RunShowDataBase(___root_node___); \
            RunAkinator(___root_node___); \
            break; \
        case PLAY_THEN_SHOW_MODE: \
            RunAkinator(___root_node___); \
            RunShowDataBase(___root_node___); \
            break; \
        }; \
    } while(0)
int main(int argc, char const *argv[]) {
    run_mode_t r_mode = PLAY_AKINATOR_MODE;
    tree_node_t *root_n = GetRootBranchFile("Preorder.txt");
    SWITCH_MODE(r_mode, argc);
    SWITCH_RUN(r_mode, root_n);
    DestroyBranch(&root_n);
    return 0;
}
#undef SWITCH_MODE
#undef SWITCH_RUN