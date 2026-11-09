#include "BinaryTree.c"

int main() {
    full_tree_t tree = {};
    tree_node_t *node1 = CreateNode(50);
    tree_node_t *node2 = CreateNode(20);
    tree_node_t *node3 = CreateNode(10);
    tree_node_t *node4 = CreateNode(30);
    tree_node_t *node5 = CreateNode(60);
    NodeLinkLeft(node1, node2);
    NodeLinkLeft(node2, node3);
    NodeLinkRight(node2, node4);
    NodeLinkRight(node1, node5);
    

    tree_node_t *node6 = CreateNode(120);
    tree_node_t *node7 = CreateNode(150);
    tree_node_t *node8 = CreateNode(55);
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
    DestroyBranch(&node2);
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
    system(CrPngGraphLine(DFLT_TXT_NAME, DFLT_PNG_NAME));
    DestroyBranch(&node1);
    return 0;
}