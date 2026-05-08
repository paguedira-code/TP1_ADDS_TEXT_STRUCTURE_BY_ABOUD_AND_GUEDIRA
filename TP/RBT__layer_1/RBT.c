#include "rbt.h"

AlphaRBTNode ALPHA_NIL = { NULL, NULL, BLACK, 0, &ALPHA_NIL, &ALPHA_NIL, &ALPHA_NIL };
PosRBTNode   POS_NIL   = { 0, NULL, BLACK, &POS_NIL, &POS_NIL, &POS_NIL, 0 };

AlphaRBT* AlocateAlphaRBT() {
    AlphaRBT* t = malloc(sizeof(AlphaRBT));
    t->root = &ALPHA_NIL;
    return t;
}

PosRBT* AlocatePosRBT() {
    PosRBT* t = malloc(sizeof(PosRBT));
    t->root = &POS_NIL;
    t->max = &POS_NIL;
    return t;
}

AlphaRBTNode* allocateAlphaRBTNode() {
    AlphaRBTNode* nd = malloc(sizeof(AlphaRBTNode));
    if (nd == NULL){ 
        printf("malloc raté AlphaRBTNode\n");
        return &ALPHA_NIL; 
    }
    nd->color = RED;
    nd->parent = &ALPHA_NIL;
    nd->left = &ALPHA_NIL;
    nd->right = &ALPHA_NIL;
    nd->pos_list = NULL;
    nd->rep = 0;
    nd->word = NULL;
    return nd;
}

PosRBTNode* allocatePosRBTNode() {
    PosRBTNode* nd = malloc(sizeof(PosRBTNode));
    if (nd == NULL){ 
        printf("malloc raté PosRBTNode\n");
        return &POS_NIL; 
    }
    nd->color = RED;
    nd->parent = &POS_NIL;
    nd->left = &POS_NIL;
    nd->right = &POS_NIL;
    nd->word_ref = NULL;
    nd->position = 0;
    nd->sent_flag = 0;
    return nd;
}


void AlphaRBTRightRot(AlphaRBT* T, AlphaRBTNode* y) {
    AlphaRBTNode* x = y->left;
    y->left = x->right;
    if (x->right != &ALPHA_NIL){
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &ALPHA_NIL){
        T->root = x;
    }
    else if (y == y->parent->left){
        y->parent->left = x;
    }
    else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

void AlphaRBTLeftRot(AlphaRBT* T, AlphaRBTNode* y) {
    AlphaRBTNode* x = y->right;
    y->right = x->left;
    if (x->left != &ALPHA_NIL){
        x->left->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &ALPHA_NIL){
        T->root = x;
    }
    else if (y == y->parent->right){
        y->parent->right = x;
    }
    else{
        y->parent->left = x;
    }
    x->left = y;
    y->parent = x;
}

void PosRBTRightRot(PosRBT* T, PosRBTNode* y) {
    PosRBTNode* x = y->left;
    y->left = x->right;
    if (x->right != &POS_NIL){
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &POS_NIL){
        T->root = x;
    }
    else if (y == y->parent->left){
        y->parent->left = x;
    }
    else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

void PosRBTLeftRot(PosRBT* T, PosRBTNode* y) {
    PosRBTNode* x = y->right;
    y->right = x->left;
    if (x->left != &POS_NIL){
        x->left->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &POS_NIL){
        T->root = x;
    }
    else if (y == y->parent->right){
        y->parent->right = x;
    }
    else {
        y->parent->left = x;
    }
    x->left = y;
    y->parent = x;
}

void AlphaRBTfix(AlphaRBT* T, AlphaRBTNode* nd) {
    AlphaRBTNode* unc, *gp;
    while (nd->parent != &ALPHA_NIL && nd->parent->color == RED){
        gp = nd->parent->parent;
        if (nd->parent == gp->left) {
            unc = gp->right;
            if (unc->color == RED) {
                unc->color = BLACK;
                nd->parent->color = BLACK;
                gp->color = RED;
                nd = gp;
            } else {
                if (nd == nd->parent->right) {
                    nd = nd->parent;
                    AlphaRBTLeftRot(T, nd);
                }
                nd->parent->color = BLACK;
                gp->color = RED;
                AlphaRBTRightRot(T, gp);
            }
        } else {
            unc = gp->left;
            if (unc->color == RED) {
                unc->color = BLACK;
                nd->parent->color = BLACK;
                gp->color = RED;
                nd = gp;
            } else {
                if (nd == nd->parent->left) {
                    nd = nd->parent;
                    AlphaRBTRightRot(T, nd);
                }
                nd->parent->color = BLACK;
                gp->color = RED;
                AlphaRBTLeftRot(T, gp);
            }
        }
    }
    T->root->color = BLACK;
}

void PosRBTfix(PosRBT* T, PosRBTNode* nd) {
    PosRBTNode* unc, *gp;
    while (nd->parent != &POS_NIL && nd->parent->color == RED) {
        gp = nd->parent->parent;
        if (nd->parent == gp->left) {
            unc = gp->right;
            if (unc->color == RED) {
                unc->color = BLACK;
                nd->parent->color = BLACK;
                gp->color = RED;
                nd = gp;
            } else {
                if (nd == nd->parent->right) {
                    nd = nd->parent;
                    PosRBTLeftRot(T, nd);
                }
                nd->parent->color = BLACK;
                gp->color = RED;
                PosRBTRightRot(T, gp);
            }
        } else {
            unc = gp->left;
            if (unc->color == RED) {
                unc->color = BLACK;
                nd->parent->color = BLACK;
                gp->color = RED;
                nd = gp;
            } else {
                if (nd == nd->parent->left) {
                    nd = nd->parent;
                    PosRBTRightRot(T, nd);
                }
                nd->parent->color = BLACK;
                gp->color = RED;
                PosRBTLeftRot(T, gp);
            }
        }
    }
    T->root->color = BLACK;
}

AlphaRBTNode* Insert_AlphaRBT(AlphaRBT* T, char* w) {
    AlphaRBTNode* par = &ALPHA_NIL;
    AlphaRBTNode* cur = T->root;
    while (cur != &ALPHA_NIL) {
        int c = strcmp(w, cur->word);
        if (c == 0) return cur; //dija kayna
        par = cur;
        cur = (c < 0) ? cur->left : cur->right;
    }
    AlphaRBTNode* nd = allocateAlphaRBTNode();
    nd->word   = strdup(w);
    nd->parent = par;
    if (par == &ALPHA_NIL)
        T->root = nd;
    else if (strcmp(w, par->word) < 0)
        par->left  = nd;
    else
        par->right = nd;
    AlphaRBTfix(T, nd);
    return nd;
}

PosRBTNode* Insert_PosRBT(PosRBT* T) {
    PosRBTNode* nd = allocatePosRBTNode();
    nd->parent = T->max;
    if (T->max == &POS_NIL) {
        nd->position = 1;
        T->root = nd;
    } else {
        nd->position    = T->max->position + 1;
        T->max->right   = nd;
    }
    PosRBTfix(T, nd);
    T->max = nd;
    return nd;
}

AlphaRBTNode* alpha_inorder_first(AlphaRBT* T) {
    AlphaRBTNode* nd = T->root;
    if (nd == &ALPHA_NIL) return &ALPHA_NIL;
    while (nd->left != &ALPHA_NIL) nd = nd->left;
    return nd;
}

AlphaRBTNode* alpha_inorder_next(AlphaRBTNode* nd) {
    if (nd->right != &ALPHA_NIL) {
        nd = nd->right;
        while (nd->left != &ALPHA_NIL) nd = nd->left;
        return nd;
    }
    AlphaRBTNode* p = nd->parent;
    while (p != &ALPHA_NIL && nd == p->right) { nd = p; p = p->parent; }
    return p;
}

PosRBTNode* pos_inorder_first(PosRBT* T) {
    PosRBTNode* nd = T->root;
    if (nd == &POS_NIL) return &POS_NIL;
    while (nd->left != &POS_NIL) nd = nd->left;
    return nd;
}

PosRBTNode* pos_inorder_next(PosRBTNode* nd) {
    if (nd->right != &POS_NIL) {
        nd = nd->right;
        while (nd->left != &POS_NIL) nd = nd->left;
        return nd;
    }
    PosRBTNode* p = nd->parent;
    while (p != &POS_NIL && nd == p->right) { nd = p; p = p->parent; }
    return p;
}

void pos_rbt_free(PosRBTNode* nd) {
    if (nd == &POS_NIL) return;
    pos_rbt_free(nd->left);
    pos_rbt_free(nd->right);
    free(nd);
}

void alpha_rbt_free(AlphaRBTNode* nd) {
    if (nd == &ALPHA_NIL) return;
    alpha_rbt_free(nd->left);
    alpha_rbt_free(nd->right);
    poslist_free(nd->pos_list);
    free(nd->word);
    free(nd);
}

void Print_PosRBT(PosRBT* T, PosRBTNode* nd) {
    if (nd == &POS_NIL) return;
    Print_PosRBT(T, nd->left);
    printf("%s ", nd->word_ref->word);
    Print_PosRBT(T, nd->right);
}

void Print_AlphaRBT(AlphaRBTNode* nd) {
    if (nd == &ALPHA_NIL) return;
    Print_AlphaRBT(nd->left);
    printf("  - %s (x%d | pos: ", nd->word, nd->rep);
    poslist_print(nd->pos_list);
    printf(")\n");
    Print_AlphaRBT(nd->right);
}
