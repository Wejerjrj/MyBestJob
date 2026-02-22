#ifndef BST_H
#define BST_H

typedef struct BSTNode {
    void* data;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct {
    BSTNode* root;
    int (*compare)(void*, void*);
    void (*print)(void*);
    void (*freeData)(void*);
} BST;

// BST Creation and Initialization
BST* createBst(int (*cmp)(void*, void*), void (*print)(void*), void (*freeData)(void*));

// BST Insertion Functions
void bstInsert(BST* tree, void* data);

// BST Search Functions
void* bstSearch(BST* tree, void* data);

// BST Traversal Functions
void bstInorder(BSTNode* root, void (*print)(void*));
void bstPreorder(BSTNode* root, void (*print)(void*));
void bstPostorder(BSTNode* root, void (*print)(void*));

// BST Print Functions
void bstPrintInorder(BST* tree);
void bstPrintPreorder(BST* tree);
void bstPrintPostorder(BST* tree);

// BST Cleanup Functions
void bstFree(BST* tree);

#endif
