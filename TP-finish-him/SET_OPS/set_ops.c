#include "set_ops.h"

PetalNode* petal_union(PetalNode* a, PetalNode* b) {
    PetalNode* res = allocatePetalNode();
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    AlphaRBTNode* new = NULL;
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if (cmp < 0) {
            new = Insert_AlphaRBT(res->alpha_tree, ca->word);
            new->rep = ca->rep;
            ca = alpha_inorder_next(ca);
        } else if (cmp > 0) {
            new = Insert_AlphaRBT(res->alpha_tree, cb->word);
            new->rep = cb->rep;
            cb = alpha_inorder_next(cb);
        } else {
            new = Insert_AlphaRBT(res->alpha_tree, ca->word);
            new->rep = (ca->rep > cb->rep) ? ca->rep : cb->rep;
            ca = alpha_inorder_next(ca);
            cb = alpha_inorder_next(cb);
        }
    }
    while (ca != &ALPHA_NIL) { Insert_AlphaRBT(res->alpha_tree, ca->word); ca = alpha_inorder_next(ca); }
    while (cb != &ALPHA_NIL) { Insert_AlphaRBT(res->alpha_tree, cb->word); cb = alpha_inorder_next(cb); }
    return res;
}

PetalNode* petal_intersection(PetalNode* a, PetalNode* b) {
    PetalNode* res = allocatePetalNode();
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if      (cmp < 0) ca = alpha_inorder_next(ca);
        else if (cmp > 0) cb = alpha_inorder_next(cb);
        else {
            AlphaRBTNode* new = Insert_AlphaRBT(res->alpha_tree, ca->word);
            new->rep = (ca->rep < cb->rep) ? ca->rep : cb->rep;
            ca = alpha_inorder_next(ca);
            cb = alpha_inorder_next(cb);
        }
    }
    return res;
}

PetalNode* petal_difference(PetalNode* a, PetalNode* b) {
    PetalNode* res = allocatePetalNode();
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    AlphaRBTNode* new = NULL;
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if (cmp < 0) {
            new = Insert_AlphaRBT(res->alpha_tree, ca->word);
            new->rep = ca->rep;        
            ca = alpha_inorder_next(ca);
        } else if (cmp > 0) {
            cb = alpha_inorder_next(cb);
        } else {
            if(ca->rep > cb->rep){
                new = Insert_AlphaRBT(res->alpha_tree, ca->word);
                new->rep = ca->rep - cb->rep;
            }
            ca = alpha_inorder_next(ca);
            cb = alpha_inorder_next(cb);
        }
    }
    while (ca != &ALPHA_NIL) { Insert_AlphaRBT(res->alpha_tree, ca->word); ca = alpha_inorder_next(ca); }
    return res;
}

PetalNode* petal_symmetric_difference(PetalNode* a, PetalNode* b) {
    PetalNode* res = allocatePetalNode();
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    AlphaRBTNode* new = NULL;
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if (cmp < 0) {
            new=Insert_AlphaRBT(res->alpha_tree, ca->word);
            new->rep=ca->rep;
            ca = alpha_inorder_next(ca);
        } else if (cmp > 0) {
           new= Insert_AlphaRBT(res->alpha_tree, cb->word);
            new->rep=cb->rep;
            cb = alpha_inorder_next(cb);
        } else {
            ca = alpha_inorder_next(ca);
            cb = alpha_inorder_next(cb);
        }
    }
    while (ca != &ALPHA_NIL) { Insert_AlphaRBT(res->alpha_tree, ca->word); ca = alpha_inorder_next(ca); }
    while (cb != &ALPHA_NIL) { Insert_AlphaRBT(res->alpha_tree, cb->word); cb = alpha_inorder_next(cb); }
    return res;
}

int petal_is_subset_of(PetalNode* a, PetalNode* b) {
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if      (cmp == 0) {
            if(ca->rep<=cb->rep){
            ca = alpha_inorder_next(ca); cb = alpha_inorder_next(cb);
            }else{
                return 0;
            }
        }
        else if (cmp > 0)  cb = alpha_inorder_next(cb);
        else return 0;
    }
    if (ca == &ALPHA_NIL) return 1;
    return 0;
}

PetalNode* petal_sentence_intersection(PetalNode* P1, PetalNode* P2) {
    PetalNode* res = allocatePetalNode();
    SentenceEntry** t = build_sent_table(P2);
    if (!t) return res;
    PosRBTNode* c = pos_inorder_first(P1->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag && sent_in_table(t, c))
            insert_sent(res, c);
        c = pos_inorder_next(c);
    }
    free_sent_table(t);
    return res;
}

PetalNode* petal_sentence_difference(PetalNode* P1, PetalNode* P2) {
    PetalNode* res = allocatePetalNode();
    SentenceEntry** t = build_sent_table(P2);
    if (!t) return res;
    PosRBTNode* c = pos_inorder_first(P1->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag && !sent_in_table(t, c))
            insert_sent(res, c);
        c = pos_inorder_next(c);
    }
    free_sent_table(t);
    return res;
}

PetalNode* petal_sentence_union(PetalNode* P1, PetalNode* P2) {
    PetalNode* res = allocatePetalNode();
    /* d'abord toutes les phrases de P1 */
    PosRBTNode* c = pos_inorder_first(P1->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag) insert_sent(res, c);
        c = pos_inorder_next(c);
    }
    /* puis les phrases de P2 pas encore dans P1 */
    SentenceEntry** t = build_sent_table(P1);
    if (!t) return res;
    c = pos_inorder_first(P2->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag && !sent_in_table(t, c))
            insert_sent(res, c);
        c = pos_inorder_next(c);
    }
    free_sent_table(t);
    return res;
}

PetalNode* petal_sentence_symmetric_difference(PetalNode* P1, PetalNode* P2) {
    PetalNode* res = allocatePetalNode();
    /* phrases de P1 absentes de P2 */
    SentenceEntry** t2 = build_sent_table(P2);
    if (!t2) return res;
    PosRBTNode* c = pos_inorder_first(P1->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag && !sent_in_table(t2, c)) insert_sent(res, c);
        c = pos_inorder_next(c);
    }
    free_sent_table(t2);
    /* phrases de P2 absentes de P1 */
    SentenceEntry** t1 = build_sent_table(P1);
    if (!t1) return res;
    c = pos_inorder_first(P2->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag && !sent_in_table(t1, c)) insert_sent(res, c);
        c = pos_inorder_next(c);
    }
    free_sent_table(t1);
    return res;
}

int petal_sentence_is_subset(PetalNode* P1, PetalNode* P2) {
    SentenceEntry** t = build_sent_table(P2);
    if (!t) return 0;
    PosRBTNode* c = pos_inorder_first(P1->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag && !sent_in_table(t, c)) {
            free_sent_table(t);
            return 0;
        }
        c = pos_inorder_next(c);
    }
    free_sent_table(t);
    return 1;
}



/* recherche de sous-chaine positionnelle */
int is_substring_pos(PetalNode* a, PetalNode* b) {
    if (a->pos_tree->root == &POS_NIL) return 1;
    if (b->pos_tree->root == &POS_NIL) return 0;
    PosRBTNode* sa = pos_inorder_first(a->pos_tree);
    PosRBTNode* cb = pos_inorder_first(b->pos_tree);
    while (cb != &POS_NIL) {
        while (cb != &POS_NIL && strcmp(sa->word_ref->word, cb->word_ref->word) != 0)
            cb = pos_inorder_next(cb);
        if (cb == &POS_NIL) return 0;
        PosRBTNode* ca = pos_inorder_next(sa);
        PosRBTNode* cb2 = pos_inorder_next(cb);
        while (ca != &POS_NIL && cb2 != &POS_NIL &&
               strcmp(ca->word_ref->word, cb2->word_ref->word) == 0) {
            ca  = pos_inorder_next(ca);
            cb2 = pos_inorder_next(cb2);
        }
        if (ca == &POS_NIL) return 1;
        cb = pos_inorder_next(cb);
    }
    return 0;
}


PetalNode* rose_union_petals(Rose* r, int i, int j)        { return petal_union(rose_get_petal(r,i), rose_get_petal(r,j)); }
PetalNode* rose_intersect_petals(Rose* r, int i, int j)    { return petal_intersection(rose_get_petal(r,i), rose_get_petal(r,j)); }
PetalNode* rose_diff_petals(Rose* r, int i, int j)         { return petal_difference(rose_get_petal(r,i), rose_get_petal(r,j)); }
PetalNode* rose_sym_diff_petals(Rose* r, int i, int j)     { return petal_symmetric_difference(rose_get_petal(r,i), rose_get_petal(r,j)); }
int        rose_is_subset_petals(Rose* r, int i, int j)    { return petal_is_subset_of(rose_get_petal(r,i), rose_get_petal(r,j)); }
int        rose_is_identical_petals(Rose* r, int i, int j) { return petal_is_identical(rose_get_petal(r,i), rose_get_petal(r,j)); }

PetalNode* rose_cross_union(Rose* r1, int i, Rose* r2, int j)        { return petal_union(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
PetalNode* rose_cross_intersection(Rose* r1, int i, Rose* r2, int j) { return petal_intersection(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
PetalNode* rose_cross_difference(Rose* r1, int i, Rose* r2, int j)   { return petal_difference(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
PetalNode* rose_cross_sym_diff(Rose* r1, int i, Rose* r2, int j)     { return petal_symmetric_difference(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
int        rose_cross_is_subset(Rose* r1, int i, Rose* r2, int j)    { return petal_is_subset_of(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
int        rose_cross_is_identical(Rose* r1, int i, Rose* r2, int j) { return petal_is_identical(rose_get_petal(r1,i), rose_get_petal(r2,j)); }


PetalNode* rose_sentence_intersection(Rose* r1, int i, Rose* r2, int j)        { return petal_sentence_intersection(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
PetalNode* rose_sentence_difference(Rose* r1, int i, Rose* r2, int j)          { return petal_sentence_difference(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
PetalNode* rose_sentence_union(Rose* r1, int i, Rose* r2, int j)               { return petal_sentence_union(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
PetalNode* rose_sentence_symmetric_difference(Rose* r1, int i, Rose* r2, int j){ return petal_sentence_symmetric_difference(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
int        rose_sentence_is_subset(Rose* r1, int i, Rose* r2, int j)           { return petal_sentence_is_subset(rose_get_petal(r1,i), rose_get_petal(r2,j)); }
int        rose_sentence_is_identical(Rose* r1, int i, Rose* r2, int j)        { return petal_sentence_is_identical(rose_get_petal(r1,i), rose_get_petal(r2,j)); }

