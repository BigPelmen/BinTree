#include "BinaryTree.c"

int main() {
    // full_tree_t tree = {};
    tree_node_t *node1 = CreateNode((char *)"abobi");
    tree_node_t *node2 = CreateNode((char *)"oeirhu\ng3289 b  23g32");
    tree_node_t *node3 = CreateNode((char *)"10  2b      uuih   iuu u");
    tree_node_t *node4 = CreateNode((char *)"hel\tlo");
    tree_node_t *node5 = CreateNode((char *)"it");
    NodeLinkLeft(node1, node2);
    NodeLinkLeft(node2, node3);
    NodeLinkRight(node2, node4);
    NodeLinkRight(node1, node5);
    

    tree_node_t *node6 = CreateNode((char *)"cat");
    tree_node_t *node7 = CreateNode((char *)"dog");
    tree_node_t *node8 = CreateNode((char *)"10");
    NodeLinkLeft(node5, node8);
    NodeLinkLeft(node6, node7);
    NodeLinkRight(node5, node6);
    printf("%llu\n", CountNodesBranch(node1));
    // DestroyBranch(node5);
    // FILE *fp = GetCheckedFile(DFLT_TXT_NAME, "w");
    // GraphBranch(fp, node1, "");
    // fclose(fp);
    // printf(CrPngGraphLine(DFLT_TXT_NAME, DFLT_PNG_NAME));
    // system(CrPngGraphLine(DFLT_TXT_NAME, DFLT_PNG_NAME));
    // fp = GetCheckedFile("Preorder.txt", "w");
    // PreorderBranchPrint(fp, node1);
    FILE *fp_one = GetCheckedFile("Preorder.txt", "w");
    PreorderBranchPrint(fp_one, node1);
    fclose(fp_one);
    GetLeftBranchFile("Preorder.txt", node8);
    // DestroyBranch(&node2);
    
    GetRightBranchFile("Preorder.txt", node8);
    // fp_one = freopen("Preorder.txt", "r", fp_one);
    // node8->right_node = GetNodesFromFile(fp_one, node8);
    // NodeLinkRight(node8, GetNodesFromFile(fp_one, node8));
    // node8->left_node = GetNodesFromFile(fp_one, node8);
    // node8->right_node = GetNodesFromFile(fp_one, node8);
    // printf("%d\n", node8->right_node->data);
    
    FILE *fp = GetCheckedFile(DFLT_TXT_NAME, "w");
    GraphBranch(fp, node1, "");
    fclose(fp);
    // printf(CrPngGraphLine(DFLT_TXT_NAME, DFLT_PNG_NAME));
    StartGraphvizTxtCmd(DFLT_TXT_NAME, DFLT_PNG_NAME);
    DestroyBranch(&node1);
    return 0;
}