// #define CUMMING_DUMP
#include "BinaryTree.c"

// TODO EXIT SYMBOL - TEST
// TODO DY LEAVES OF THE TREE IN THE DUMP (END NODES) - DONEs

#define YES_ANSWER_SYM 'y'
#define NO_ANSWER_SYM 'n'
#define UNKNOWN_ANSWER_SYM 'u'
#define STOP_ANSWER_SYM 'x'
// #define BOUNDARY_SYM_OPEN LEFT_BRACE_SYM
// #define BOUNDARY_SYM_CLOSE RIGHT_BRACE_SYM

// typedef struct {
//     FILE *fp_storage_txt;
//     const char *storage_txt_name;
//     const char *png_name;
//     const char *graph_txt_name;
//     FILE *fp_graph_txt;
// } data_files_t;

// typedef struct {
//     data_files_t *files_info;
//     full_tree_t *bin_tree;
// } storage_tree_t;

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
    GO_EXIT_ANSWER = STOP_ANSWER_SYM,
    GO_UNKNOWN_ANSWER = UNKNOWN_ANSWER_SYM,
} user_answer_l_r_t;

typedef struct {
    int sym;
    user_answer_l_r_t akinator_cmd;
} user_cmd_t;

const user_cmd_t users_commands[] = {
    {.sym = UNKNOWN_ANSWER_SYM, .akinator_cmd = GO_UNKNOWN_ANSWER},
    {.sym = YES_ANSWER_SYM, .akinator_cmd = GO_LEFT_ANSWER},
    {.sym = NO_ANSWER_SYM, .akinator_cmd = GO_RIGHT_ANSWER},
    {.sym = STOP_ANSWER_SYM, .akinator_cmd = GO_EXIT_ANSWER}
};

const size_t user_cmds_arr_size = sizeof(users_commands) / sizeof(users_commands[0]);

run_mode_t SwitchModeArgc(int argc, char const **argv);
run_mode_t ParseArgv(const char *argv_str);
void SkipFlagMinuses(const char **argv_str);
run_mode_t CheckModeFlag(const char *argv_str, int flag, run_mode_t m_one, run_mode_t m_two);
tree_node_t *GetArgvRootNode(int argc, char const **argv);
const char *GetRootNodeFileName(int argc, char const **argv);
void SwitchRegimeAkinator(run_mode_t r_mode, tree_node_t *root_node, const char *f_name);
user_answer_l_r_t ChangeNodeAfterCmd(tree_node_t **cur_node);
int RunAkinator(tree_node_t *root_node, const char *f_name);
void PrintAkinatorMenu(void);
tree_node_t *GoCycleByTree(tree_node_t *root_node);
void KeepLine(tree_node_t **cur_node, tree_node_t *change_node);
user_answer_l_r_t AskUserCmd(void);
user_cmd_t GetUserCmd(void);
bool CheckIfCmd(int got_sym);
user_cmd_t GetCmdUserBySym(int got_sym);
user_cmd_t GetCmdUserByCmd(user_answer_l_r_t got_cmd);
void MakeAnswerSafe(char *answer, size_t ans_len);
bool GotStopSymAnswer(char *answer, size_t ans_len);
user_answer_l_r_t AskIfRightAnswer(tree_node_t *cur_node);
// int GetYesOrNo(void);
void SkipLineStdin(void);
tree_node_t *FixTheAnswer(tree_node_t *fix_node);
bool CheckFgetsAnsBad(char *answer);
// int AskUserIfRigth(void);
void TryAllignLeftSon(tree_node_t **fix_node, tree_node_t **new_question_node);
void TryAllignRightSon(tree_node_t **fix_node, tree_node_t **new_question_node);
void SkipFileSpaces(FILE *stream);
int RunShowDataBase(tree_node_t *root_node);

int main(int argc, char const *argv[]) {
    run_mode_t r_mode = SwitchModeArgc(argc, argv);
    tree_node_t *root_n = GetArgvRootNode(argc, argv);
    const char *f_name = GetRootNodeFileName(argc, argv);
    if (root_n == NULL) {
        fprintf(stderr, "Did not manage to get the root node from file\n");
        return 1;
    }
    SwitchRegimeAkinator(r_mode, root_n, f_name);
    DestroyBranch(&root_n);
    return 0;
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

void SkipFlagMinuses(const char **argv_str) {
    assert(argv_str);

    while (*((*argv_str)++) == '-') {
        ;
    }
    (*argv_str)--;
}

run_mode_t CheckModeFlag(const char *argv_str, int flag, run_mode_t m_one, run_mode_t m_two) {
    assert(argv_str);

    if (*(argv_str + 1) == flag) {
        return m_one;
    }
    return m_two;
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

const char *GetRootNodeFileName(int argc, char const **argv) {
    assert(argv);

    switch (argc) {
    case NO_ARGS_MODE:
        return "Preorder.txt";
    case ONE_ARG_MODE:
        if (argv[1][0] == '-') {
            return "Preorder.txt";
        }
        return argv[1];
    case TWO_ARG_MODE:
        if (argv[1][0] == '-') {
            return argv[2];
        }
        return argv[1];
    default:
        return "Preorder.txt";
    };
}

void SwitchRegimeAkinator(run_mode_t r_mode, tree_node_t *root_node, const char *f_name) {
    assert(root_node);

    switch (r_mode) {
    case PLAY_AKINATOR_MODE:
        RunAkinator(root_node, f_name);
        break;
    case SHOW_BASE_MODE:
        RunShowDataBase(root_node);
        break;
    case SHOW_THEN_PLAY_MODE:
        RunShowDataBase(root_node);
        RunAkinator(root_node, f_name);
        break;
    case PLAY_THEN_SHOW_MODE:
        RunAkinator(root_node, f_name);
        RunShowDataBase(root_node);
        break;
    default:
        RunAkinator(root_node, f_name);
        break;
    };
}

int RunAkinator(tree_node_t *root_node, const char *f_name) {
    assert(root_node);
    assert(f_name);
    
    PrintAkinatorMenu();
    tree_node_t *cur_node = NULL;
    if ((cur_node = GoCycleByTree(root_node)) == NULL) {
        return errno;
    }
    user_answer_l_r_t ans = GO_UNKNOWN_ANSWER;
    if ((ans = AskIfRightAnswer(cur_node)) == GO_EXIT_ANSWER) {
        fprintf(stdout, "Exiting the game... Thank you for the game!\n");
        return errno;
    }
    else if (GetCmdUserByCmd(ans).sym == YES_ANSWER_SYM) {
        fprintf(stdout, "Thank you for the game!\n");
        return errno;
    }
    if (FixTheAnswer(cur_node) != NULL) {
        FILE *dump_strg = GetCheckedFile(f_name, "w");
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
    fprintf(stdout, "          \"%c\" - answer \"Exit\"\n", STOP_ANSWER_SYM);
    fprintf(stdout, "Have a good game!\n");
}

tree_node_t *GoCycleByTree(tree_node_t *root_node) {
    assert(root_node);

    tree_node_t *cur_node = root_node;
    size_t cycle_safer = 0;
    while (cycle_safer < 65536 && (cur_node->left_node != NULL || cur_node->right_node != NULL)) {
        if (cur_node->left_node == NULL || cur_node->right_node == NULL) {
            fprintf(stdout, DATA_FORMAT_OUT "? & ", cur_node->data);
            KeepLine(&cur_node, cur_node->left_node);
            KeepLine(&cur_node, cur_node->right_node);
            cycle_safer++;
            continue;
        }
        if (ChangeNodeAfterCmd(&cur_node) == GO_EXIT_ANSWER) {
            return NULL;
        }
        cycle_safer++;
    }
    if (cycle_safer == 65536) {
        fprintf(stdout, "The game got cycled. Ending the game on \"" DATA_FORMAT_OUT "\"\n", cur_node->data);
        return NULL;
    }
    return cur_node;
}

void KeepLine(tree_node_t **cur_node, tree_node_t *change_node) {
    assert(cur_node);
    assert(*cur_node);

    if (change_node != NULL) {
        (*cur_node) = change_node;
    }
}

user_answer_l_r_t ChangeNodeAfterCmd(tree_node_t **cur_node) {
    assert(cur_node);
    assert(*cur_node);

    fprintf(stdout, DATA_FORMAT_OUT "?\n", (*cur_node)->data);
    user_answer_l_r_t ans = AskUserCmd();
    if (ans == GO_LEFT_ANSWER) {
        *cur_node = (*cur_node)->left_node;
    }
    if (ans == GO_RIGHT_ANSWER) {
        *cur_node = (*cur_node)->right_node;
    }
    if (ans == GO_UNKNOWN_ANSWER) {
        do {
            fprintf(stdout, "Please, enter a valid command (\"u\" is for service)\n");
            ans = AskUserCmd();
        } while (ans == GO_UNKNOWN_ANSWER);     
    }
    return ans;
}

user_answer_l_r_t AskUserCmd(void) {
    user_cmd_t res_cmd = {.sym = UNKNOWN_ANSWER_SYM, .akinator_cmd = GO_UNKNOWN_ANSWER};
    do {
        res_cmd = GetUserCmd();
    } while(res_cmd.sym == UNKNOWN_ANSWER_SYM);
    return res_cmd.akinator_cmd;
}

user_cmd_t GetUserCmd(void) {
    user_cmd_t res_cmd = {.sym = UNKNOWN_ANSWER_SYM, .akinator_cmd = GO_UNKNOWN_ANSWER};
    int sym = '\0';
    while (isspace(sym = getchar()) && sym != '\n' && sym != EOF) {
        ;
    }
    if (sym == EOF) {
        fprintf(stdout, "EOF has been reached. Exiting the game...\n");
        return GetCmdUserBySym(STOP_ANSWER_SYM);
    }
    if (sym == '\n') {
        fprintf(stdout, "Please enter not an empty line\n");
        return res_cmd;
    }
    if (!CheckIfCmd(tolower(sym))) {
        fprintf(stdout, "Please enter \"y\" or \"n\"\n"), SkipLineStdin();
        return res_cmd;
    }
    int tmp_res = tolower(sym);
    while (isspace(sym = getchar()) && sym != '\n') {
        ;
    }
    if (sym == '\n' || sym == EOF) {
        return GetCmdUserBySym(tmp_res);
    }
    fprintf(stdout, "Please enter \"y\" or \"n\"\n");
    SkipLineStdin();
    return res_cmd;
}

bool CheckIfCmd(int got_sym) {
    for (size_t i = 0; i < user_cmds_arr_size; i++) {
        if (users_commands[i].sym == got_sym) {
            return true;
        }
    }
    return false;
}

user_cmd_t GetCmdUserBySym(int got_sym) {
    for (size_t i = 0; i < user_cmds_arr_size; i++) {
        if (users_commands[i].sym == got_sym) {
            return users_commands[i];
        }
    }
    for (size_t i = 0; i < user_cmds_arr_size; i++) {
        if (users_commands[i].sym == UNKNOWN_ANSWER_SYM) {
            return users_commands[i];
        }
    }
    user_cmd_t unknow = {.sym = UNKNOWN_ANSWER_SYM, .akinator_cmd = GO_UNKNOWN_ANSWER};
    return unknow;
}

user_cmd_t GetCmdUserByCmd(user_answer_l_r_t got_cmd) {
    for (size_t i = 0; i < user_cmds_arr_size; i++) {
        if (users_commands[i].akinator_cmd == got_cmd) {
            return users_commands[i];
        }
    }
    for (size_t i = 0; i < user_cmds_arr_size; i++) {
        if (users_commands[i].sym == UNKNOWN_ANSWER_SYM) {
            return users_commands[i];
        }
    }
    user_cmd_t unknow = {.sym = UNKNOWN_ANSWER_SYM, .akinator_cmd = GO_UNKNOWN_ANSWER};
    return unknow;
}

user_answer_l_r_t AskIfRightAnswer(tree_node_t *cur_node) {
    assert(cur_node);

    fprintf(stdout, "Is it \"%s\"?\n", cur_node->data);
    return AskUserCmd();
}

tree_node_t *FixTheAnswer(tree_node_t *fix_node) {
    assert(fix_node);

    fprintf(stdout, "Could you please add some more information for the game?\n");
    fprintf(stdout, "If you want to exit enter \"%c\"\n", STOP_ANSWER_SYM);
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
        DestroyBranch(&new_question_node);
        return NULL;
    }
    NodeLinkLeft(new_question_node, CreateNode(answer));
    if (fix_node->parent != NULL) {
        TryAllignLeftSon(&fix_node, &new_question_node);
        TryAllignRightSon(&fix_node, &new_question_node);
    }
    free(answer);
    return new_question_node;
}

bool CheckFgetsAnsBad(char *answer) {
    assert(answer);

    if (fgets(answer, MAX_STR_SIZE - 1, stdin) == NULL) {
        free(answer);
        return true;
    }

    size_t ans_len = strlen(answer);
    if (ans_len > 0) {
        answer[--ans_len] = '\0';
        MakeAnswerSafe(answer, ans_len);
        return GotStopSymAnswer(answer, ans_len);    
    }
    return false;
}

void MakeAnswerSafe(char *answer, size_t ans_len) {
    assert(answer);

    for (size_t i = 0; i < ans_len; i++) {
        if (answer[i] == LEFT_BRACE_SYM || answer[i] == RIGHT_BRACE_SYM) {
            answer[i] = '_';
        }
    }
}

bool GotStopSymAnswer(char *answer, size_t ans_len) {
    assert(answer);

    for (size_t i = 0; i < ans_len; i++) {
        if (!isspace(answer[i])) {
            if (tolower(answer[i]) != STOP_ANSWER_SYM) {
                return false;
            }
            size_t j = i + 1;
            for ( ; j < ans_len; j++) {
                if (!isspace(answer[j])) {
                    return false;
                }
            }
            if (j == ans_len) {
                fprintf(stdout, "Closing the game...\n");
                free(answer);
                return true;
            }
        }
    }
    return false;
}

void TryAllignLeftSon(tree_node_t **fix_node, tree_node_t **new_question_node) {
    assert(fix_node);
    assert(new_question_node);
    assert(*new_question_node);

    // printf("%p and %p\n", *fix_parent_son_node, *fix_node);
    if (*fix_node != NULL && (*fix_node)->parent->left_node == *fix_node) {
        // NodeLinkLeft((*fix_node)->parent, *new_question_node);
        (*fix_node)->parent->left_node = *new_question_node;
        (*new_question_node)->parent = (*fix_node)->parent;
        DestroyBranch(fix_node);
        // *fix_node = NULL;
    }
}

void TryAllignRightSon(tree_node_t **fix_node, tree_node_t **new_question_node) {
    assert(fix_node);
    assert(new_question_node);
    assert(*new_question_node);

    // printf("%p and %p\n", *fix_parent_son_node, *fix_node);
    if (*fix_node != NULL && (*fix_node)->parent->right_node == *fix_node) {
        // NodeLinkRight((*fix_node)->parent, *new_question_node);
        // *fix_node = NULL;
        (*fix_node)->parent->right_node = *new_question_node;
        (*new_question_node)->parent = (*fix_node)->parent;
        DestroyBranch(fix_node);
    }   
}

// int GetYesOrNo(void) {
//     int sym = '\0';
//     while (isspace(sym = getchar()) && sym != '\n') {
//         ;
//     }
//     if (sym == '\n') {
//         fprintf(stdout, "Please enter not an empty line\n");
//         SkipLineStdin();
//         return UNKNOWN_ANSWER_SYM;
//     }
//     if (tolower(sym) != NO_ANSWER_SYM && tolower(sym) != YES_ANSWER_SYM) {
//         fprintf(stdout, "Please enter \"y\" or \"n\"\n");
//         SkipLineStdin();
//         return UNKNOWN_ANSWER_SYM;
//     }
//     int tmp_res = tolower(sym);
//     while (isspace(sym = getchar()) && sym != '\n') {
//         ;
//     }
//     if (sym == '\n') {
//         return tmp_res;
//     }
//     fprintf(stdout, "Please enter \"y\" or \"n\"\n");
//     SkipLineStdin();
//     return UNKNOWN_ANSWER_SYM;
// }

void SkipLineStdin() {
    int sym = '\0';
    while ((sym = getchar()) != '\n' && sym != EOF) {
        ;
    }
}

// int AskUserIfRigth(void) {
//     int sym = '\0';
//     do {
//         sym = GetYesOrNo();
//     } while(sym == UNKNOWN_ANSWER_SYM);
//     return sym;
// }

int RunShowDataBase(tree_node_t *root_node) {
    assert(root_node);

    FILE *fp = GetCheckedFile(DFLT_TXT_NAME, "w");
    GraphBranch(fp, root_node, "");
    fclose(fp);
    system(CrPngGraphLine(DFLT_TXT_NAME, DFLT_PNG_NAME));
    return errno;
}
