#include "BinaryTree.c"

#define YES_ANSWER_SYM 'y'
#define NO_ANSWER_SYM 'n'
#define UNKNOWN_ANSWER_SYM 'u'

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

typedef enum {
    GO_LEFT_ANSWER = YES_ANSWER_SYM,
    GO_RIGHT_ANSWER = NO_ANSWER_SYM,
} user_answer_l_r_t;

int RunAkinator(tree_node_t *root_node);
tree_node_t *AskQuestion(tree_node_t *cur_node);
bool AskIfRightAnswer(tree_node_t *cur_node);
int GetYesOrNo(void);
tree_node_t *FixTheAnswer(tree_node_t *fix_node);
int AskUserIfRigth(void);
void SkipFileSpaces(FILE *stream);
run_mode_t ParseArgv(const char *argv_str);
int RunShowDataBase(tree_node_t *root_node);

#define KEEP_LINE(___cur_node___, ___change___) do { \
    if (___change___ != NULL) { \
        ___cur_node___ = ___change___; \
    } } while(0)
int RunAkinator(tree_node_t *root_node) {
    assert(root_node);
    
    tree_node_t *cur_node = root_node;
    size_t cycle_safer = 0;
    while (cycle_safer < 65'536 && (cur_node->left_node != NULL || cur_node->right_node != NULL)) {
        if (cur_node->left_node == NULL || cur_node->right_node == NULL) {
            fprintf(stdout, DATA_FORMAT_OUT "? & ", cur_node->data);
            KEEP_LINE(cur_node, cur_node->left_node);
            KEEP_LINE(cur_node, cur_node->right_node);
            cycle_safer++;
            continue;
        }
        cur_node = AskQuestion(cur_node);
        cycle_safer++;
    }
    if (cycle_safer == 65'536) {
        fprintf(stdout, "The game got cycled. Ending the game on \"" DATA_FORMAT_OUT "\"\n", cur_node->data);
        return errno;
    }
    if (AskIfRightAnswer(cur_node)) {
        return errno;
    }
    if (FixTheAnswer(cur_node) != NULL) {
        
        FILE *dump_strg = GetCheckedFile("Preorder.txt", "w");
        PreorderBranchPrint(dump_strg, root_node);
        
        fclose(dump_strg);
    }
    return errno;
}
#undef KEEP_LINE

#define TRY_ALLIGN_SON(___son___, ___func_name___) do { \
    if (fix_node != NULL && ___son___ == fix_node) { \
        ___func_name___(fix_node->parent, new_question_node); \
        fix_node = NULL; \
    } } while(0)
tree_node_t *FixTheAnswer(tree_node_t *fix_node) {
    assert(fix_node);

    char *answer = (char *)calloc(MAX_STR_SIZE, sizeof(char));
    if (answer == NULL) {
        V_ERRS(fprintf(stderr, "Did not manage to allocate memory for the new node and its data\n");)
        return NULL;
    }
    fprintf(stdout, "Please enter your question so that the game could guess this stuff next time:\n");
    if (fgets(answer, MAX_STR_SIZE - 1, stdin) == NULL) {
        free(answer);
        return NULL;
    }
    tree_node_t *new_question_node = CreateNode(answer);
    NodeLinkRight(new_question_node, CreateNode(fix_node->data));
    fprintf(stdout, "Now please say the game the answer for this question describing it correctly:\n");
    if (fgets(answer, MAX_STR_SIZE - 1, stdin) == NULL) {
        free(answer);
        return NULL;
    }
    NodeLinkLeft(new_question_node, CreateNode(answer));
    
    if (fix_node->parent) {
        TRY_ALLIGN_SON(fix_node->parent->left_node, NodeLinkLeft);
        printf("in %p\n", fix_node);
        // FIXME fixnode->parent было убито 0xAAAAAAAAAAAAAAA
        TRY_ALLIGN_SON(fix_node->parent->right_node, NodeLinkRight);
    }
    
    free(answer);

    return new_question_node;
}
#undef TRY_ALLIGN_SON

tree_node_t *AskQuestion(tree_node_t *cur_node) {
    assert(cur_node);
    
    fprintf(stdout, DATA_FORMAT_OUT "?\n", cur_node->data);
    user_answer_l_r_t ans = (user_answer_l_r_t)AskUserIfRigth();
    switch (ans) {
    case GO_LEFT_ANSWER:
        return cur_node->left_node;
    case GO_RIGHT_ANSWER:
        return cur_node->right_node;
    };
    return NULL;
}

bool AskIfRightAnswer(tree_node_t *cur_node) {
    assert(cur_node);

    fprintf(stdout, "Is it \"%s\"?\n", cur_node->data);
    int ans = AskUserIfRigth();
    if (ans == YES_ANSWER_SYM) {
        fprintf(stdout, "Thanks for the game!\n");
        return true;
    }
    return false;
}

int GetYesOrNo() {
    int sym = '\0';
    while (isspace(sym = getchar()) && sym != '\n') {
        ;
    }
    if (sym == '\n') {
        fprintf(stdout, "Please enter not an empty line\n");
        return UNKNOWN_ANSWER_SYM;
    }
    if (tolower(sym) != NO_ANSWER_SYM && tolower(sym) != YES_ANSWER_SYM) {
        fprintf(stdout, "Please enter \"y\" or \"n\"\n");
        return UNKNOWN_ANSWER_SYM;
    }
    int tmp_res = tolower(sym);
    while (isspace(sym = getchar()) && sym != '\n') {
        ;
    }
    if (sym == '\n') {
        return tmp_res;
    }
    fprintf(stdout, "Please enter \"y\" or \"n\"\n");
    return UNKNOWN_ANSWER_SYM;
}

int AskUserIfRigth() {
    int sym = '\0';
    do {
        sym = GetYesOrNo();
    } while(sym == UNKNOWN_ANSWER_SYM);
    return sym;
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
        RET_GOT_ODD_FLAG("-----...", );
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