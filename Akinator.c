// #define CUMMING_DUMP
#include "BinaryTree.c"

// TODO EXIT SYMBOL
// TODO DY LEAVES OF THE TREE IN THE DUMP (END NODES)

#define YES_ANSWER_SYM 'y'
#define NO_ANSWER_SYM 'n'
#define UNKNOWN_ANSWER_SYM 'u'
#define STOP_ANSWER_SYM 'x'
// #define 

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
    TWO_ARG_MODE,
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

typedef int (*NodeLinker)(tree_node_t *parent_node, tree_node_t *new_node);

run_mode_t SwitchModeArgc(int argc, char const **argv);
tree_node_t *GetArgvRootNode(int argc, char const **argv);
void SwitchRegimeAkinator(run_mode_t r_mode, tree_node_t *root_node);
int RunAkinator(tree_node_t *root_node);
void PrintAkinatorMenu(void);
tree_node_t *AskQuestion(tree_node_t *cur_node);
bool AskIfRightAnswer(tree_node_t *cur_node);
int GetYesOrNo(void);
void SkipLineStdin(void);
tree_node_t *FixTheAnswer(tree_node_t *fix_node);
int AskUserIfRigth(void);
void SkipFileSpaces(FILE *stream);
run_mode_t ParseArgv(const char *argv_str);
int RunShowDataBase(tree_node_t *root_node);

int main(int argc, char const *argv[]) {
    run_mode_t r_mode = SwitchModeArgc(argc, argv);
    tree_node_t *root_n = GetArgvRootNode(argc, argv);
    SwitchRegimeAkinator(r_mode, root_n);
    DestroyBranch(&root_n);
    return 0;
}

void KeepLine(tree_node_t **cur_node, tree_node_t *change_node) {
    assert(cur_node);
    assert(*cur_node);

    if (change_node != NULL) {
        (*cur_node) = change_node;
    }
}

int RunAkinator(tree_node_t *root_node) {
    assert(root_node);
    
    PrintAkinatorMenu();
    tree_node_t *cur_node = root_node;
    size_t cycle_safer = 0;
    while (cycle_safer < 65'536 && (cur_node->left_node != NULL || cur_node->right_node != NULL)) {
        if (cur_node->left_node == NULL || cur_node->right_node == NULL) {
            fprintf(stdout, DATA_FORMAT_OUT "? & ", cur_node->data);
            KeepLine(&cur_node, cur_node->left_node);
            KeepLine(&cur_node, cur_node->right_node);
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

void PrintAkinatorMenu(void) {
    fprintf(stdout, "Hello! It is Akinator game! Here is a menu:\n");
    fprintf(stdout, "Enter it if you want to...\n");
    fprintf(stdout, "          \"%c\" - answer \"Yes\"\n", YES_ANSWER_SYM);
    fprintf(stdout, "          \"%c\" - answer \"No\"\n", NO_ANSWER_SYM);
    fprintf(stdout, "          \"%c\" - answer \"No\"\n", STOP_ANSWER_SYM);
    fprintf(stdout, "Have a good game!\n");
}

void TryAllignLeftSon(tree_node_t **fix_node, tree_node_t **new_question_node) {
    assert(fix_node);
    assert(new_question_node);
    assert(*new_question_node);

    // printf("%p and %p\n", *fix_parent_son_node, *fix_node);
    if (*fix_node != NULL && (*fix_node)->parent->left_node == *fix_node) {
        NodeLinkLeft((*fix_node)->parent, *new_question_node);
        *fix_node = NULL;
    }   
}

void TryAllignRightSon(tree_node_t **fix_node, tree_node_t **new_question_node) {
    assert(fix_node);
    assert(new_question_node);
    assert(*new_question_node);

    // printf("%p and %p\n", *fix_parent_son_node, *fix_node);
    if (*fix_node != NULL && (*fix_node)->parent->right_node == *fix_node) {
        NodeLinkRight((*fix_node)->parent, *new_question_node);
        *fix_node = NULL;
    }   
}

bool CheckFgetsAnsBad(char *answer) {
    assert(answer);

    if (fgets(answer, MAX_STR_SIZE - 1, stdin) == NULL) {
        free(answer);
        return true;
    }
    answer[strlen(answer) - 1] = '\0';
    return false;
}

tree_node_t *FixTheAnswer(tree_node_t *fix_node) {
    assert(fix_node);

    char *answer = (char *)calloc(MAX_STR_SIZE, sizeof(char));
    if (answer == NULL) {
        V_ERRS(fprintf(stderr, "Did not manage to allocate memory for the new node and its data\n");)
        return NULL;
    }
    fprintf(stdout, "Please enter your question so that the game could guess this stuff next time:\n");
    if (CheckFgetsAnsBad(answer)) {
        return NULL;
    }
    tree_node_t *new_question_node = CreateNode(answer);
    NodeLinkRight(new_question_node, CreateNode(fix_node->data));
    fprintf(stdout, "Now please say the game the answer for this question describing it correctly:\n");
    if (CheckFgetsAnsBad(answer)) {
        return NULL;
    }
    NodeLinkLeft(new_question_node, CreateNode(answer));
    if (fix_node->parent) {
        TryAllignLeftSon(&fix_node, &new_question_node);
        TryAllignRightSon(&fix_node, &new_question_node);
    }
    free(answer);
    return new_question_node;
}
#undef CHECK_FGETS

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

int GetYesOrNo(void) {
    int sym = '\0';
    while (isspace(sym = getchar()) && sym != '\n') {
        ;
    }
    if (sym == '\n') {
        fprintf(stdout, "Please enter not an empty line\n");
        SkipLineStdin();
        return UNKNOWN_ANSWER_SYM;
    }
    if (tolower(sym) != NO_ANSWER_SYM && tolower(sym) != YES_ANSWER_SYM) {
        fprintf(stdout, "Please enter \"y\" or \"n\"\n");
        SkipLineStdin();
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
    SkipLineStdin();
    return UNKNOWN_ANSWER_SYM;
}

int AskUserIfRigth(void) {
    int sym = '\0';
    do {
        sym = GetYesOrNo();
    } while(sym == UNKNOWN_ANSWER_SYM);
    return sym;
}

void SkipLineStdin() {
    while (getchar() != '\n') {
        ;
    }
}

// ...
run_mode_t CheckModeFlag(const char *argv_str, int flag, run_mode_t m_one, run_mode_t m_two) {
    assert(argv_str);

    if (*(argv_str + 1) == flag) {
        return m_one;
    }
    return m_two;
}

void SkipFlagMinuses(const char **argv_str) {
    assert(argv_str);

    while (*((*argv_str)++) == '-') {
        ;
    }
    (*argv_str)--;
}

run_mode_t ParseArgv(const char *argv_str) {
    assert(argv_str);

    SkipFlagMinuses(&argv_str);
    if (*argv_str == 'a') {
        return CheckModeFlag(argv_str, 'l', PLAY_THEN_SHOW_MODE, PLAY_AKINATOR_MODE);
    }
    if (*argv_str == 'l') {
        return CheckModeFlag(argv_str, 'a', SHOW_THEN_PLAY_MODE, SHOW_BASE_MODE);
    }
    if (*argv_str == '\0') {
        V_ERRS(fprintf(stderr, "The programm will be started in the Akinator mode as \"" 
            "-----..." "\" argument has been given\n");)
    }
    if (*argv_str != 'a' && *argv_str != 'l') {
        V_ERRS(fprintf(stderr, "The programm will be started in the Akinator mode as \"" 
            "--...-%c" "\" argument has been given\n", *argv_str);)
    }
    return PLAY_AKINATOR_MODE;
}

int RunShowDataBase(tree_node_t *root_node) {
    assert(root_node);

    FILE *fp = GetCheckedFile(DFLT_TXT_NAME, "w");
    GraphBranch(fp, root_node, "");
    fclose(fp);
    system(CrPngGraphLine(DFLT_TXT_NAME, DFLT_PNG_NAME));
    return errno;
}


run_mode_t SwitchModeArgc(int argc, char const **argv) {
    assert(argv);

    switch (argc) {
    case NO_ARGS_MODE:
        return PLAY_AKINATOR_MODE;
    case ONE_ARG_MODE:
        return ParseArgv(argv[1]);
    case TWO_ARG_MODE:
        if (argv[1][0] == '-') {
            return ParseArgv(argv[1]);
        }
        return ParseArgv(argv[2]);
    default:
        return PLAY_AKINATOR_MODE;
    };
}

tree_node_t *GetArgvRootNode(int argc, char const **argv) {
    assert(argv);

    switch (argc) {
    case NO_ARGS_MODE:
        return GetRootBranchFile("Preorder.txt");
    case ONE_ARG_MODE:
        if (argv[1][0] == '-') {
            return GetRootBranchFile("Preorder.txt");
        }
        return GetRootBranchFile(argv[1]);
    case TWO_ARG_MODE:
        if (argv[1][0] == '-') {
            return GetRootBranchFile(argv[2]);
        }
        return GetRootBranchFile(argv[1]);
    default:
        return GetRootBranchFile("Preorder.txt");
    };
}

void SwitchRegimeAkinator(run_mode_t r_mode, tree_node_t *root_node) {
    assert(root_node);

    switch (r_mode) {
    case PLAY_AKINATOR_MODE:
        RunAkinator(root_node);
        break;
    case SHOW_BASE_MODE:
        RunShowDataBase(root_node);
        break;
    case SHOW_THEN_PLAY_MODE:
        RunShowDataBase(root_node);
        RunAkinator(root_node);
        break;
    case PLAY_THEN_SHOW_MODE:
        RunAkinator(root_node);
        RunShowDataBase(root_node);
        break;
    default:
        RunAkinator(root_node);
        break;
    };
}