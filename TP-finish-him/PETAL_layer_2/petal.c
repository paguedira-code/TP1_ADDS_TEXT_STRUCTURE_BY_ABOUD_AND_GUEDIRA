#include "petal.h"

PetalNode* allocatePetalNode() {
    PetalNode* p = malloc(sizeof(PetalNode));
    if (p == NULL){ 
        printf("malloc raté petal\n");
        return NULL;
    }
    p->alpha_tree = AlocateAlphaRBT();
    p->pos_tree = AlocatePosRBT();
    p->next = p;
    p->prev = p;
    return p;
}

void petal_free(PetalNode* p) {
    if (p == NULL){
        return;
    }
    pos_rbt_free(p->pos_tree->root);
    free(p->pos_tree);
    alpha_rbt_free(p->alpha_tree->root);
    free(p->alpha_tree);
    free(p);
}

void Insert_word(PetalNode* p, char* w, int sflag) {
    AlphaRBTNode* an = Insert_AlphaRBT(p->alpha_tree, w);
    PosRBTNode*   pn = Insert_PosRBT(p->pos_tree);
    pn->sent_flag = sflag;
    an->rep++;
    pn->word_ref  = an;
    poslist_insert(&an->pos_list, pn);
}

void petal_print_by_position(PetalNode* p) {
    if (p == NULL) {
        return;
    }
    Print_PosRBT(p->pos_tree, p->pos_tree->root);
    printf("\n");
}

void petal_print_by_alpha(PetalNode* p) {
    if (!p) return;
    Print_AlphaRBT(p->alpha_tree->root);
}

char* petal_word_at_pos(PetalNode* p, int pos) {
    if (p->pos_tree->root == &POS_NIL) return NULL;
    PosRBTNode* c = p->pos_tree->root;
    while (c != &POS_NIL) {
        if      (pos == c->position) return c->word_ref->word;
        else if (pos  < c->position) c = c->left;
        else                         c = c->right;
    }
    return NULL;
}

int petal_pos_exists(PetalNode* p, int pos) {
    if (p->pos_tree->root == &POS_NIL) return 0;
    PosRBTNode* c = p->pos_tree->root;
    while (c != &POS_NIL) {
        if      (pos == c->position) return 1;
        else if (pos  < c->position) c = c->left;
        else                         c = c->right;
    }
    return 0;
}

int petal_word_count(PetalNode* p) {
    if (p->pos_tree->max == &POS_NIL) return 0;
    return p->pos_tree->max->position;
}

int petal_sentence_count(PetalNode* p) {
    if (p->pos_tree->root == &POS_NIL) return 0;
    int nb = 0;
    PosRBTNode* c = pos_inorder_first(p->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag) nb++;
        c = pos_inorder_next(c);
    }
    return nb;
}

int petal_get_sentence_start(PetalNode* p, int n) {
    if (p->pos_tree->root == &POS_NIL) return -1;
    int nb = 0;
    PosRBTNode* c = pos_inorder_first(p->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag) {
            nb++;
            if (nb == n) return c->position;
        }
        c = pos_inorder_next(c);
    }
    return -1;
}

PetalNode* petal_extract_sentence(PetalNode* p, int n) {
    if (p->pos_tree->root == &POS_NIL) return NULL;
    int deb = petal_get_sentence_start(p, n);
    if (deb == -1) return NULL;
    int fin = petal_get_sentence_start(p, n + 1);

    PetalNode*  res = allocatePetalNode();
    PosRBTNode* c   = p->pos_tree->root;

    while (c != &POS_NIL && c->position != deb)
        c = (deb < c->position) ? c->left : c->right;

    while (c != &POS_NIL) {
        if (fin != -1 && c->position >= fin) break;
        Insert_word(res, c->word_ref->word, c->sent_flag);
        c = pos_inorder_next(c);
    }
    return res;
}

void petal_print_sentences(PetalNode* p) {
    if (p->pos_tree->root == &POS_NIL) { printf("  petal vide\n"); return; }
    int nb = 0;
    PosRBTNode* c = pos_inorder_first(p->pos_tree);
    while (c != &POS_NIL) {
        if (c->sent_flag) {
            if (nb > 0) printf("\n");
            nb++;
            printf("  [S%d] ", nb);
        }
        printf("%s ", c->word_ref->word);
        c = pos_inorder_next(c);
    }
    printf("\n");
}

PetalNode* petal_extract_range(PetalNode* p, int i, int j) {
    if (p->pos_tree->root == &POS_NIL) return NULL;
    if (i < 1 || j < i || i > petal_word_count(p)) return NULL;

    PetalNode*  res = allocatePetalNode();
    PosRBTNode* c   = p->pos_tree->root;

    while (c != &POS_NIL && c->position != i)
        c = (i < c->position) ? c->left : c->right;

    while (c != &POS_NIL && c->position <= j) {
        Insert_word(res, c->word_ref->word, c->sent_flag);
        c = pos_inorder_next(c);
    }
    return res;
}

void petal_print_range(PetalNode* p, int i, int j) {
    if (p->pos_tree->root == &POS_NIL) { printf("  petal vide\n"); return; }
    if (i < 1 || j < i || i > petal_word_count(p)) {
        printf("  intervalle invalide\n");
        return;
    }
    PosRBTNode* c = p->pos_tree->root;
    while (c != &POS_NIL && c->position != i)
        c = (i < c->position) ? c->left : c->right;

    printf("  [%d..%d] ", i, j);
    while (c != &POS_NIL && c->position <= j) {
        printf("%s ", c->word_ref->word);
        c = pos_inorder_next(c);
    }
    printf("\n");
}

void petal_context_of_word(PetalNode* p, const char* w, int k) {
    AlphaRBTNode* a = p->alpha_tree->root;
    while (a != &ALPHA_NIL) {
        int cmp = strcmp(w, a->word);
        if      (cmp == 0) break;
        else if (cmp  < 0) a = a->left;
        else               a = a->right;
    }
    if (a == &ALPHA_NIL) { printf("  mot '%s' introuvable\n", w); return; }

    int total = petal_word_count(p);
    PosNode* c = a->pos_list;
    if (!c) return;
    do {
        int pos = c->pos_ref->position;
        int deb = (pos - k < 1)     ? 1     : pos - k;
        int fin = (pos + k > total) ? total : pos + k;
        printf("  ...");
        petal_print_range(p, deb, fin);
        c = c->next;
    } while (c != a->pos_list);
}
