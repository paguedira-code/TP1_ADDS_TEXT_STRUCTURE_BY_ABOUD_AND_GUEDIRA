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

    int sentence_cmp(PosRBTNode *s1, PosRBTNode *s2) {
    while (s1 != &POS_NIL && s2 != &POS_NIL) {
        if (strcmp(s1->word_ref->word, s2->word_ref->word) != 0) {
            return 0;
        }

        s1 = nextinorder(s1);
        s2 = nextinorder(s2);

        int s1_ended = 0;
        if (s1 == &POS_NIL || s1->is_start_of_sentence != 0) {
            s1_ended = 1;
        }

        int s2_ended = 0;
        if (s2 == &POS_NIL || s2->is_start_of_sentence != 0) {
            s2_ended = 1;
        }

        if (s1_ended == 1 && s2_ended == 1) {
            return 1;
        }

        if (s1_ended == 1 || s2_ended == 1) {
            return 0;
        }
    }
    return 0;
}

PetalNode *intersection_Phrase(PetalNode *p1, PetalNode *p2) {
    if (p1->pos_tree->root == &POS_NIL || p2->pos_tree->root == &POS_NIL) {
        return NULL;
    }

    PetalNode *res = allocatePetalNode();
    res->is_pos = 1;

    int n2 = 0;
    unsigned long *arr2 = hash_petal(p2, &n2);
    PosRBTNode *curr1 = p1->pos_tree->root;

    while (curr1 != &POS_NIL && curr1->left != &POS_NIL) {
        curr1 = curr1->left;
    }

    while (curr1 != &POS_NIL) {
        unsigned long h1 = hash(curr1);
        int match_confirmed = 0;

        if (arr2 != NULL && bsearch(&h1, arr2, n2, sizeof(unsigned long), hash_cmp) != NULL) {
            PosRBTNode *search_node = p2->pos_tree->root;
            while (search_node != &POS_NIL && search_node->left != &POS_NIL) {
                search_node = search_node->left;
            }
            while (search_node != &POS_NIL) {
                if (hash(search_node) == h1) {
                    if (sentence_cmp(curr1, search_node) == 1) {
                        match_confirmed = 1;
                        break;
                    }
                }
                search_node = nextinorder(search_node);
                while (search_node != &POS_NIL && search_node->is_start_of_sentence == 0) {
                    search_node = nextinorder(search_node);
                }
            }
        }

        if (match_confirmed == 1) {
            Insert_word(res, curr1->word_ref->word, 1);
            curr1 = nextinorder(curr1);
            while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
                Insert_word(res, curr1->word_ref->word, 1);
                curr1 = nextinorder(curr1);
            }
        } else {
            curr1 = nextinorder(curr1);
            while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
                curr1 = nextinorder(curr1);
            }
        }
    }
    free(arr2);
    return res;
}

PetalNode *union_Phrase(PetalNode *p1, PetalNode *p2) {
    PetalNode *res = allocatePetalNode();
    res->is_pos = 1;
    int n1 = 0;
    unsigned long *arr1 = hash_petal(p1, &n1);

    PosRBTNode *curr1 = p1->pos_tree->root;
    while (curr1 != &POS_NIL && curr1->left != &POS_NIL) {
        curr1 = curr1->left;
    }
    while (curr1 != &POS_NIL) {
        Insert_word(res, curr1->word_ref->word, 1);
        curr1 = nextinorder(curr1);
        while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
            Insert_word(res, curr1->word_ref->word, 1);
            curr1 = nextinorder(curr1);
        }
    }

    PosRBTNode *curr2 = p2->pos_tree->root;
    while (curr2 != &POS_NIL && curr2->left != &POS_NIL) {
        curr2 = curr2->left;
    }
    while (curr2 != &POS_NIL) {
        unsigned long h2 = hash(curr2);
        int already_exists = 0;

        if (arr1 != NULL && bsearch(&h2, arr1, n1, sizeof(unsigned long), hash_cmp) != NULL) {
            PosRBTNode *search_node = p1->pos_tree->root;
            while (search_node != &POS_NIL && search_node->left != &POS_NIL) {
                search_node = search_node->left;
            }
            while (search_node != &POS_NIL) {
                if (hash(search_node) == h2) {
                    if (sentence_cmp(curr2, search_node) == 1) {
                        already_exists = 1;
                        break;
                    }
                }
                search_node = nextinorder(search_node);
                while (search_node != &POS_NIL && search_node->is_start_of_sentence == 0) {
                    search_node = nextinorder(search_node);
                }
            }
        }

        if (already_exists == 0) {
            Insert_word(res, curr2->word_ref->word, 1);
            curr2 = nextinorder(curr2);
            while (curr2 != &POS_NIL && curr2->is_start_of_sentence == 0) {
                Insert_word(res, curr2->word_ref->word, 1);
                curr2 = nextinorder(curr2);
            }
        } else {
            curr2 = nextinorder(curr2);
            while (curr2 != &POS_NIL && curr2->is_start_of_sentence == 0) {
                curr2 = nextinorder(curr2);
            }
        }
    }
    free(arr1);
    return res;
}

PetalNode *difference_Phrase(PetalNode *p1, PetalNode *p2) {
    if (p1->pos_tree->root == &POS_NIL) {
        return NULL;
    }
    PetalNode *res = allocatePetalNode();
    res->is_pos = 1;
    int n2 = 0;
    unsigned long *arr2 = hash_petal(p2, &n2);

    PosRBTNode *curr1 = p1->pos_tree->root;
    while (curr1 != &POS_NIL && curr1->left != &POS_NIL) {
        curr1 = curr1->left;
    }

    while (curr1 != &POS_NIL) {
        unsigned long h1 = hash(curr1);
        int in_p2 = 0;

        if (arr2 != NULL && bsearch(&h1, arr2, n2, sizeof(unsigned long), hash_cmp) != NULL) {
            PosRBTNode *search_node = p2->pos_tree->root;
            while (search_node != &POS_NIL && search_node->left != &POS_NIL) {
                search_node = search_node->left;
            }
            while (search_node != &POS_NIL) {
                if (hash(search_node) == h1) {
                    if (sentence_cmp(curr1, search_node) == 1) {
                        in_p2 = 1;
                        break;
                    }
                }
                search_node = nextinorder(search_node);
                while (search_node != &POS_NIL && search_node->is_start_of_sentence == 0) {
                    search_node = nextinorder(search_node);
                }
            }
        }

        if (in_p2 == 0) {
            Insert_word(res, curr1->word_ref->word, 1);
            curr1 = nextinorder(curr1);
            while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
                Insert_word(res, curr1->word_ref->word, 1);
                curr1 = nextinorder(curr1);
            }
        } else {
            curr1 = nextinorder(curr1);
            while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
                curr1 = nextinorder(curr1);
            }
        }
    }
    free(arr2);
    return res;
}

int subset_Phrase(PetalNode *p1, PetalNode *p2) {
    if (p1->pos_tree->root == &POS_NIL) {
        return 1;
    }
    if (p2->pos_tree->root == &POS_NIL) {
        return 0;
    }
    int n2 = 0;
    unsigned long *arr2 = hash_petal(p2, &n2);

    PosRBTNode *curr1 = p1->pos_tree->root;
    while (curr1 != &POS_NIL && curr1->left != &POS_NIL) {
        curr1 = curr1->left;
    }

    while (curr1 != &POS_NIL) {
        unsigned long h1 = hash(curr1);
        int sentence_found = 0;

        if (arr2 != NULL && bsearch(&h1, arr2, n2, sizeof(unsigned long), hash_cmp) != NULL) {
            PosRBTNode *search_node = p2->pos_tree->root;
            while (search_node != &POS_NIL && search_node->left != &POS_NIL) {
                search_node = search_node->left;
            }
            while (search_node != &POS_NIL) {
                if (hash(search_node) == h1) {
                    if (sentence_cmp(curr1, search_node) == 1) {
                        sentence_found = 1;
                        break;
                    }
                }
                search_node = nextinorder(search_node);
                while (search_node != &POS_NIL && search_node->is_start_of_sentence == 0) {
                    search_node = nextinorder(search_node);
                }
            }
        }

        if (sentence_found == 0) {
            free(arr2);
            return 0;
        }

        curr1 = nextinorder(curr1);
        while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
            curr1 = nextinorder(curr1);
        }
    }
    free(arr2);
    return 1;
}

unsigned long hash(PosRBTNode *curr)
{
    unsigned long h = 0;
    int factor = 1;

    if (curr == &POS_NIL)
        return 0;

    do
    {
        int i = 0;
        while (curr->word_ref->word[i] != '\0')
        {
            h += curr->word_ref->word[i] * factor;
            factor++;
            i++;
        }

        curr = nextinorder(curr);
    } while (curr != &POS_NIL && curr->is_start_of_sentence == 0);

    return h;
}

int hash_cmp(const void *a, const void *b)
{
    unsigned long h1 = *(const unsigned long *)a;
    unsigned long h2 = *(const unsigned long *)b;
    if (h1 > h2)
    {
        return 1;
    }
    else
    {
        if (h1 < h2)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

unsigned long *hash_petal(PetalNode *p, int *len)
{
    int n = 0;
    PosRBTNode *curr = p->pos_tree->root;

    while (curr != &POS_NIL && curr->left != &POS_NIL)
        curr = curr->left;

    PosRBTNode *tmp = curr;
    while (tmp != &POS_NIL)
    {
        n++;
        do
        {
            tmp = nextinorder(tmp);
        } while (tmp != &POS_NIL && tmp->is_start_of_sentence == 0);
    }

    *len = n;
    if (n == 0)
        return NULL;

    unsigned long *arr = malloc(n * sizeof(unsigned long));
    int i = 0;

    while (curr != &POS_NIL)
    {
        arr[i] = hash(curr);
        i++;
        do
        {
            curr = nextinorder(curr);
        } while (curr != &POS_NIL && curr->is_start_of_sentence == 0);
    }

    qsort(arr, n, sizeof(unsigned long), hash_cmp);
    return arr;
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

