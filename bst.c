#include <stdlib.h>
#include "bst.h"

/*
We creates and initializes a new BST and adding parameters;
- cmp: function pointer to compare two elements
- print: function pointer to print an element
- freeData: function pointer to free an element
Finally, we returns pointer to the newly created BST
*/
BST* createBst(int (*cmp)(void*, void*), void (*print)(void*), void (*freeData)(void*)){
    // Allocate memory for the BST structure
    BST* tree = (BST*)malloc(sizeof(BST));
    // Initialize the tree with NULL root and function pointers
    tree->root = NULL;
    tree->compare = cmp;
    tree->print = print;
    tree->freeData = freeData;
    return tree;
}

/*
We recursively inserts a node into the BST in order by comparing data.
Finally, we returns pointer to the updated subtree root.
*/
static BSTNode* bstInsertNode(BSTNode* root, void* data, int (*cmp)(void*, void*)){
    // Base case: empty spot found, create new node
    if(!root){
        BSTNode* newNode = malloc(sizeof(BSTNode));
        newNode->data = data;
        newNode->left = NULL;
        newNode->right = NULL;
        return newNode;
    }

    // we compare data and insert recursively in left or right subtree.
    if(cmp(data, root->data) < 0){
        // if data is smaller, go left
        root->left = bstInsertNode(root->left, data, cmp);
    } else {
        // if data is greater or equal, go right
        root->right = bstInsertNode(root->right, data, cmp);
    }
    return root;
}

// Main function to insert data into BST
void bstInsert(BST* tree, void* data){
    if(!tree) return;
    // Call recursive insert starting from root
    tree->root = bstInsertNode(tree->root, data, tree->compare);
}


// We recursively searches for data in the BST and returns a pointer to the found data, or NULL if not found.
static void* bstFind(BSTNode* root, void* data, int (*cmp)(void*, void*)){
    // Base case: reached NULL, data not found
    if(!root)
        return NULL;

    // Compare the search data with current node
    int res = cmp(data, root->data);

    if(res == 0)
        // Data found
        return root->data;
    else if(res < 0)
        return bstFind(root->left, data, cmp);
    else
        return bstFind(root->right, data, cmp);
}

// Main function to search in BST
void* bstSearch(BST* tree, void* data){
    if(!tree) return NULL;
    // Call recursive search starting from root
    return bstFind(tree->root, data, tree->compare);
}

// Traverses BST in inorder.
void bstInorder(BSTNode* root, void (*print)(void*)){
    if(!root) return;
    bstInorder(root->left, print);
    print(root->data);
    bstInorder(root->right, print);
}

// Traverses BST in preorder
void bstPreorder(BSTNode* root, void (*print)(void*)){
    if(!root) return;
    print(root->data);
    bstPreorder(root->left, print);
    bstPreorder(root->right, print);
}
// Traverses BST in postorder
void bstPostorder(BSTNode* root, void (*print)(void*)){
    if(!root) return;
    bstPostorder(root->left, print);
    bstPostorder(root->right, print);
    print(root->data);
}

// Print wrapper
void bstPrintInorder(BST* tree){
    if(!tree) return;
    bstInorder(tree->root, tree->print);
}
void bstPrintPreorder(BST* tree){
    if(!tree) return;
    bstPreorder(tree->root, tree->print);
}
void bstPrintPostorder(BST* tree){
    if(!tree) return;
    bstPostorder(tree->root, tree->print);
}

// We recursively frees all nodes in the BST by postorder traversal to free children before parent.
static void bstFreeNodes(BSTNode* root, void (*freeData)(void*)){
    if(!root) return;
    bstFreeNodes(root->left, freeData);
    bstFreeNodes(root->right, freeData);
    freeData(root->data);
    free(root);
}

// Main function to frees the entire BST including all nodes.
void bstFree(BST* tree){
    if(!tree) return;
    // Free all nodes recursively
    bstFreeNodes(tree->root, tree->freeData);
    // Free the BST structure itself
    free(tree);
}
