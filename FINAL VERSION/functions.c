#include "structures.h"

/* ============================================================
 *  SENTINELS GLOBAUX
 * ============================================================ */
AlphaRBTNode ALPHA_NIL = { NULL, NULL, BLACK, 0, &ALPHA_NIL, &ALPHA_NIL, &ALPHA_NIL };
PosRBTNode   POS_NIL   = { 0, NULL, BLACK, &POS_NIL, &POS_NIL, &POS_NIL, 0 };

/* ============================================================
 *  LAYER 0 — Liste de positions
 * ============================================================ */
PosNode* poslist_create_node(PosRBTNode* ref) {
    PosNode* nouveau = malloc(sizeof(PosNode));
    if (!nouveau) { printf("erreur malloc PosNode\n"); return NULL; }
    nouveau->pos_ref = ref;
    nouveau->next    = nouveau;
    nouveau->prev    = nouveau;
    return nouveau;
}

void poslist_insert(PosNode** head, PosRBTNode* ref) {
    PosNode* nouveau = poslist_create_node(ref);
    if (*head == NULL) {
        *head = nouveau;
        return;
    }
    PosNode* dernier  = (*head)->prev;
    dernier->next     = nouveau;
    nouveau->prev     = dernier;
    nouveau->next     = *head;
    (*head)->prev     = nouveau;
}

void poslist_free(PosNode* head) {
    if (!head) return;
    PosNode* courant = head;
    do {
        PosNode* temporaire = courant;
        courant = courant->next;
        free(temporaire);
    } while (courant != head);
}

void poslist_print(PosNode* head) {
    if (!head) { printf("[]"); return; }
    printf("[");
    PosNode* courant = head;
    do {
        printf("%d", courant->pos_ref->position);
        if (courant->next != head) printf(" <-> ");
        courant = courant->next;
    } while (courant != head);
    printf("]");
}

/* ============================================================
 *  LAYER 1 — Allocation des arbres
 * ============================================================ */
AlphaRBT* AlocateAlphaRBT() {
    AlphaRBT* arbre = malloc(sizeof(AlphaRBT));
    arbre->root = &ALPHA_NIL;
    return arbre;
}

PosRBT* AlocatePosRBT() {
    PosRBT* arbre = malloc(sizeof(PosRBT));
    arbre->root   = &POS_NIL;
    arbre->max    = &POS_NIL;
    return arbre;
}

AlphaRBTNode* allocateAlphaRBTNode() {
    AlphaRBTNode* noeud = malloc(sizeof(AlphaRBTNode));
    if (!noeud) { printf("malloc raté AlphaRBTNode\n"); return &ALPHA_NIL; }
    noeud->color    = RED;
    noeud->parent   = &ALPHA_NIL;
    noeud->left     = &ALPHA_NIL;
    noeud->right    = &ALPHA_NIL;
    noeud->pos_list = NULL;
    noeud->rep      = 0;
    noeud->word     = NULL;
    return noeud;
}

PosRBTNode* allocatePosRBTNode() {
    PosRBTNode* noeud = malloc(sizeof(PosRBTNode));
    if (!noeud) { printf("malloc raté PosRBTNode\n"); return &POS_NIL; }
    noeud->color     = RED;
    noeud->parent    = &POS_NIL;
    noeud->left      = &POS_NIL;
    noeud->right     = &POS_NIL;
    noeud->word_ref  = NULL;
    noeud->position  = 0;
    noeud->sent_flag = 0;
    return noeud;
}

/* ============================================================
 *  LAYER 1 — Rotations
 * ============================================================ */
void AlphaRBTRightRot(AlphaRBT* arbre, AlphaRBTNode* y) {
    AlphaRBTNode* x = y->left;
    y->left = x->right;
    if (x->right != &ALPHA_NIL) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == &ALPHA_NIL)       arbre->root      = x;
    else if (y == y->parent->left)     y->parent->left  = x;
    else                               y->parent->right = x;
    x->right  = y;
    y->parent = x;
}

void AlphaRBTLeftRot(AlphaRBT* arbre, AlphaRBTNode* y) {
    AlphaRBTNode* x = y->right;
    y->right = x->left;
    if (x->left != &ALPHA_NIL) x->left->parent = y;
    x->parent = y->parent;
    if (y->parent == &ALPHA_NIL)       arbre->root      = x;
    else if (y == y->parent->right)    y->parent->right = x;
    else                               y->parent->left  = x;
    x->left   = y;
    y->parent = x;
}

void PosRBTRightRot(PosRBT* arbre, PosRBTNode* y) {
    PosRBTNode* x = y->left;
    y->left = x->right;
    if (x->right != &POS_NIL) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == &POS_NIL)         arbre->root      = x;
    else if (y == y->parent->left)     y->parent->left  = x;
    else                               y->parent->right = x;
    x->right  = y;
    y->parent = x;
}

void PosRBTLeftRot(PosRBT* arbre, PosRBTNode* y) {
    PosRBTNode* x = y->right;
    y->right = x->left;
    if (x->left != &POS_NIL) x->left->parent = y;
    x->parent = y->parent;
    if (y->parent == &POS_NIL)         arbre->root      = x;
    else if (y == y->parent->right)    y->parent->right = x;
    else                               y->parent->left  = x;
    x->left   = y;
    y->parent = x;
}

/* ============================================================
 *  LAYER 1 — Fixup après insertion
 * ============================================================ */
void AlphaRBTfix(AlphaRBT* arbre, AlphaRBTNode* noeud) {
    AlphaRBTNode* oncle;
    AlphaRBTNode* grandparent;
    while (noeud->parent != &ALPHA_NIL && noeud->parent->color == RED) {
        grandparent = noeud->parent->parent;
        if (noeud->parent == grandparent->left) {
            oncle = grandparent->right;
            if (oncle->color == RED) {
                oncle->color             = BLACK;
                noeud->parent->color     = BLACK;
                grandparent->color       = RED;
                noeud                    = grandparent;
            } else {
                if (noeud == noeud->parent->right) {
                    noeud = noeud->parent;
                    AlphaRBTLeftRot(arbre, noeud);
                }
                noeud->parent->color = BLACK;
                grandparent->color   = RED;
                AlphaRBTRightRot(arbre, grandparent);
            }
        } else {
            oncle = grandparent->left;
            if (oncle->color == RED) {
                oncle->color             = BLACK;
                noeud->parent->color     = BLACK;
                grandparent->color       = RED;
                noeud                    = grandparent;
            } else {
                if (noeud == noeud->parent->left) {
                    noeud = noeud->parent;
                    AlphaRBTRightRot(arbre, noeud);
                }
                noeud->parent->color = BLACK;
                grandparent->color   = RED;
                AlphaRBTLeftRot(arbre, grandparent);
            }
        }
    }
    arbre->root->color = BLACK;
}

void PosRBTfix(PosRBT* arbre, PosRBTNode* noeud) {
    PosRBTNode* oncle;
    PosRBTNode* grandparent;
    while (noeud->parent != &POS_NIL && noeud->parent->color == RED) {
        grandparent = noeud->parent->parent;
        if (noeud->parent == grandparent->left) {
            oncle = grandparent->right;
            if (oncle->color == RED) {
                oncle->color             = BLACK;
                noeud->parent->color     = BLACK;
                grandparent->color       = RED;
                noeud                    = grandparent;
            } else {
                if (noeud == noeud->parent->right) {
                    noeud = noeud->parent;
                    PosRBTLeftRot(arbre, noeud);
                }
                noeud->parent->color = BLACK;
                grandparent->color   = RED;
                PosRBTRightRot(arbre, grandparent);
            }
        } else {
            oncle = grandparent->left;
            if (oncle->color == RED) {
                oncle->color             = BLACK;
                noeud->parent->color     = BLACK;
                grandparent->color       = RED;
                noeud                    = grandparent;
            } else {
                if (noeud == noeud->parent->left) {
                    noeud = noeud->parent;
                    PosRBTRightRot(arbre, noeud);
                }
                noeud->parent->color = BLACK;
                grandparent->color   = RED;
                PosRBTLeftRot(arbre, grandparent);
            }
        }
    }
    arbre->root->color = BLACK;
}

/* ============================================================
 *  LAYER 1 — Insertion
 * ============================================================ */
AlphaRBTNode* Insert_AlphaRBT(AlphaRBT* arbre, char* mot) {
    AlphaRBTNode* parent = &ALPHA_NIL;
    AlphaRBTNode* noeud  = arbre->root;
    while (noeud != &ALPHA_NIL) {
        int comparaison = strcmp(mot, noeud->word);
        if (comparaison == 0) return noeud;
        parent = noeud;
        noeud  = (comparaison < 0) ? noeud->left : noeud->right;
    }
    AlphaRBTNode* nouveau = allocateAlphaRBTNode();
    nouveau->word         = strdup(mot);
    nouveau->parent       = parent;
    if (parent == &ALPHA_NIL)
        arbre->root = nouveau;
    else if (strcmp(mot, parent->word) < 0)
        parent->left  = nouveau;
    else
        parent->right = nouveau;
    AlphaRBTfix(arbre, nouveau);
    return nouveau;
}

PosRBTNode* Insert_PosRBT(PosRBT* arbre) {
    PosRBTNode* nouveau = allocatePosRBTNode();
    nouveau->parent     = arbre->max;
    if (arbre->max == &POS_NIL) {
        nouveau->position = 1;
        arbre->root       = nouveau;
    } else {
        nouveau->position    = arbre->max->position + 1;
        arbre->max->right    = nouveau;
    }
    PosRBTfix(arbre, nouveau);
    arbre->max = nouveau;
    return nouveau;
}

/* ============================================================
 *  LAYER 1 — Parcours infixe
 * ============================================================ */
AlphaRBTNode* alpha_inorder_first(AlphaRBT* arbre) {
    AlphaRBTNode* noeud = arbre->root;
    if (noeud == &ALPHA_NIL) return &ALPHA_NIL;
    while (noeud->left != &ALPHA_NIL) noeud = noeud->left;
    return noeud;
}

AlphaRBTNode* alpha_inorder_next(AlphaRBTNode* noeud) {
    if (noeud->right != &ALPHA_NIL) {
        noeud = noeud->right;
        while (noeud->left != &ALPHA_NIL) noeud = noeud->left;
        return noeud;
    }
    AlphaRBTNode* parent = noeud->parent;
    while (parent != &ALPHA_NIL && noeud == parent->right) {
        noeud  = parent;
        parent = parent->parent;
    }
    return parent;
}

PosRBTNode* pos_inorder_first(PosRBT* arbre) {
    PosRBTNode* noeud = arbre->root;
    if (noeud == &POS_NIL) return &POS_NIL;
    while (noeud->left != &POS_NIL) noeud = noeud->left;
    return noeud;
}

PosRBTNode* pos_inorder_next(PosRBTNode* noeud) {
    if (noeud->right != &POS_NIL) {
        noeud = noeud->right;
        while (noeud->left != &POS_NIL) noeud = noeud->left;
        return noeud;
    }
    PosRBTNode* parent = noeud->parent;
    while (parent != &POS_NIL && noeud == parent->right) {
        noeud  = parent;
        parent = parent->parent;
    }
    return parent;
}

/* ============================================================
 *  LAYER 1 — Liberation memoire
 * ============================================================ */
void pos_rbt_free(PosRBTNode* noeud) {
    if (noeud == &POS_NIL) return;
    pos_rbt_free(noeud->left);
    pos_rbt_free(noeud->right);
    free(noeud);
}

void alpha_rbt_free(AlphaRBTNode* noeud) {
    if (noeud == &ALPHA_NIL) return;
    alpha_rbt_free(noeud->left);
    alpha_rbt_free(noeud->right);
    poslist_free(noeud->pos_list);
    free(noeud->word);
    free(noeud);
}

/* ============================================================
 *  LAYER 1 — Affichage
 * ============================================================ */
void Print_PosRBT(PosRBT* arbre, PosRBTNode* noeud) {
    if (noeud == &POS_NIL) return;
    Print_PosRBT(arbre, noeud->left);
    printf("%s ", noeud->word_ref->word);
    Print_PosRBT(arbre, noeud->right);
}

void Print_AlphaRBT(AlphaRBTNode* noeud) {
    if (noeud == &ALPHA_NIL) return;
    Print_AlphaRBT(noeud->left);
    printf("  - %s (x%d | positions: ", noeud->word, noeud->rep);
    poslist_print(noeud->pos_list);
    printf(")\n");
    Print_AlphaRBT(noeud->right);
}

/* ============================================================
 *  LAYER 2 — Petal
 * ============================================================ */
PetalNode* allocatePetalNode() {
    PetalNode* petale = malloc(sizeof(PetalNode));
    if (!petale) { printf("malloc raté petal\n"); return NULL; }
    petale->alpha_tree = AlocateAlphaRBT();
    petale->pos_tree   = AlocatePosRBT();
    petale->next       = petale;
    petale->prev       = petale;
    return petale;
}

void petal_free(PetalNode* petale) {
    if (!petale) return;
    pos_rbt_free(petale->pos_tree->root);
    free(petale->pos_tree);
    alpha_rbt_free(petale->alpha_tree->root);
    free(petale->alpha_tree);
    free(petale);
}

void Insert_word(PetalNode* petale, char* mot, int drapeau_phrase) {
    AlphaRBTNode* noeud_alpha = Insert_AlphaRBT(petale->alpha_tree, mot);
    PosRBTNode*   noeud_pos   = Insert_PosRBT(petale->pos_tree);
    noeud_pos->sent_flag      = drapeau_phrase;
    noeud_alpha->rep++;
    noeud_pos->word_ref       = noeud_alpha;
    poslist_insert(&noeud_alpha->pos_list, noeud_pos);
}

void petal_print_by_position(PetalNode* petale) {
    if (!petale) return;
    Print_PosRBT(petale->pos_tree, petale->pos_tree->root);
    printf("\n");
}

void petal_print_by_alpha(PetalNode* petale) {
    if (!petale) return;
    Print_AlphaRBT(petale->alpha_tree->root);
}

char* petal_word_at_pos(PetalNode* petale, int position) {
    if (petale->pos_tree->root == &POS_NIL) return NULL;
    PosRBTNode* courant = petale->pos_tree->root;
    while (courant != &POS_NIL) {
        if      (position == courant->position) return courant->word_ref->word;
        else if (position  < courant->position) courant = courant->left;
        else                                    courant = courant->right;
    }
    return NULL;
}

int petal_word_count(PetalNode* petale) {
    if (petale->pos_tree->max == &POS_NIL) return 0;
    return petale->pos_tree->max->position;
}

int petal_sentence_count(PetalNode* petale) {
    if (petale->pos_tree->root == &POS_NIL) return 0;
    int         nombre  = 0;
    PosRBTNode* courant = pos_inorder_first(petale->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag) nombre++;
        courant = pos_inorder_next(courant);
    }
    return nombre;
}

int petal_get_sentence_start(PetalNode* petale, int n) {
    if (petale->pos_tree->root == &POS_NIL) return -1;
    int         nombre  = 0;
    PosRBTNode* courant = pos_inorder_first(petale->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag) {
            nombre++;
            if (nombre == n) return courant->position;
        }
        courant = pos_inorder_next(courant);
    }
    return -1;
}

void petal_print_sentences(PetalNode* petale) {
    if (petale->pos_tree->root == &POS_NIL) { printf("  Petale vide.\n"); return; }
    int         numero  = 0;
    PosRBTNode* courant = pos_inorder_first(petale->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag) {
            if (numero > 0) printf("\n");
            numero++;
            printf("  [Phrase %d] ", numero);
        }
        printf("%s ", courant->word_ref->word);
        courant = pos_inorder_next(courant);
    }
    printf("\n");
}

void petal_print_range(PetalNode* petale, int debut, int fin) {
    if (petale->pos_tree->root == &POS_NIL) { printf("  Petale vide.\n"); return; }
    if (debut < 1 || fin < debut || debut > petal_word_count(petale)) {
        printf("  Intervalle invalide.\n"); return;
    }
    PosRBTNode* courant = petale->pos_tree->root;
    while (courant != &POS_NIL && courant->position != debut)
        courant = (debut < courant->position) ? courant->left : courant->right;
    printf("  [%d..%d] ", debut, fin);
    while (courant != &POS_NIL && courant->position <= fin) {
        printf("%s ", courant->word_ref->word);
        courant = pos_inorder_next(courant);
    }
    printf("\n");
}

int petal_unique_word_count(PetalNode* petale) {
    if (petale->alpha_tree->root == &ALPHA_NIL) return 0;
    int           nombre  = 0;
    AlphaRBTNode* courant = alpha_inorder_first(petale->alpha_tree);
    while (courant != &ALPHA_NIL) { nombre++; courant = alpha_inorder_next(courant); }
    return nombre;
}

char* petal_most_frequent_word(PetalNode* petale) {
    if (petale->alpha_tree->root == &ALPHA_NIL) return NULL;
    AlphaRBTNode* courant = alpha_inorder_first(petale->alpha_tree);
    AlphaRBTNode* meilleur = courant;
    while (courant != &ALPHA_NIL) {
        if (courant->rep > meilleur->rep) meilleur = courant;
        courant = alpha_inorder_next(courant);
    }
    return meilleur->word;
}

char* petal_least_frequent_word(PetalNode* petale) {
    if (petale->alpha_tree->root == &ALPHA_NIL) return NULL;
    AlphaRBTNode* courant  = alpha_inorder_first(petale->alpha_tree);
    AlphaRBTNode* meilleur = courant;
    while (courant != &ALPHA_NIL) {
        if (courant->rep < meilleur->rep) meilleur = courant;
        courant = alpha_inorder_next(courant);
    }
    return meilleur->word;
}

void petal_print_frequency_table(PetalNode* petale) {
    if (petale->alpha_tree->root == &ALPHA_NIL) { printf("  Petale vide.\n"); return; }
    int total = 0;
    AlphaRBTNode* courant = alpha_inorder_first(petale->alpha_tree);
    while (courant != &ALPHA_NIL) { total += courant->rep; courant = alpha_inorder_next(courant); }
    printf("  %-20s %6s %8s\n", "Mot", "Nb", "Freq%%");
    printf("  %-20s %6s %8s\n", "---", "--", "-----");
    courant = alpha_inorder_first(petale->alpha_tree);
    while (courant != &ALPHA_NIL) {
        double frequence = total > 0 ? (100.0 * courant->rep / total) : 0.0;
        printf("  %-20s %6d %7.2f%%\n", courant->word, courant->rep, frequence);
        courant = alpha_inorder_next(courant);
    }
    printf("  %-20s %6d %8s\n", "TOTAL", total, "100.00%%");
}

void petal_print_stats(PetalNode* petale) {
    if (petale->alpha_tree->root == &ALPHA_NIL) { printf("  Petale vide.\n"); return; }
    int    total_mots    = petal_word_count(petale);
    int    mots_uniques  = petal_unique_word_count(petale);
    int    nb_phrases    = petal_sentence_count(petale);
    char*  plus_frequent = petal_most_frequent_word(petale);
    char*  moins_frequent= petal_least_frequent_word(petale);
    double densite       = total_mots > 0 ? (100.0 * mots_uniques / total_mots) : 0.0;
    printf("  %-25s %d\n",    "Mots total:",       total_mots);
    printf("  %-25s %d\n",    "Mots uniques:",      mots_uniques);
    printf("  %-25s %d\n",    "Phrases:",           nb_phrases);
    printf("  %-25s %.2f%%\n","Densite lexicale:",  densite);
    printf("  %-25s %s\n",    "Plus frequent:",     plus_frequent  ? plus_frequent  : "N/A");
    printf("  %-25s %s\n",    "Moins frequent:",    moins_frequent ? moins_frequent : "N/A");
}

void rose_find_petals_with_word(Rose* rose, const char* mot) {
    if (!rose->petals) { printf("  Rose vide.\n"); return; }
    int        trouve  = 0;
    PetalNode* courant = rose->petals;
    int        index   = 0;
    do {
        AlphaRBTNode* noeud = courant->alpha_tree->root;
        while (noeud != &ALPHA_NIL) {
            int comparaison = strcmp(mot, noeud->word);
            if      (comparaison == 0) break;
            else if (comparaison  < 0) noeud = noeud->left;
            else                       noeud = noeud->right;
        }
        if (noeud != &ALPHA_NIL) {
            trouve = 1;
            printf("  Paragraphe %d (x%d) | positions: ", index, noeud->rep);
            poslist_print(noeud->pos_list);
            printf("\n");
        }
        courant = courant->next;
        index++;
    } while (courant != rose->petals);
    if (!trouve) printf("  Mot '%s' introuvable dans cette rose.\n", mot);
}

/* ============================================================
 *  OPERATIONS ENSEMBLISTES — MOTS (multiset)
 * ============================================================ */
PetalNode* petal_union(PetalNode* a, PetalNode* b) {
    PetalNode*    resultat = allocatePetalNode();
    AlphaRBTNode* courant_a = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* courant_b = alpha_inorder_first(b->alpha_tree);
    AlphaRBTNode* nouveau   = NULL;
    while (courant_a != &ALPHA_NIL && courant_b != &ALPHA_NIL) {
        int comparaison = strcmp(courant_a->word, courant_b->word);
        if (comparaison < 0) {
            nouveau      = Insert_AlphaRBT(resultat->alpha_tree, courant_a->word);
            nouveau->rep = courant_a->rep;
            courant_a    = alpha_inorder_next(courant_a);
        } else if (comparaison > 0) {
            nouveau      = Insert_AlphaRBT(resultat->alpha_tree, courant_b->word);
            nouveau->rep = courant_b->rep;
            courant_b    = alpha_inorder_next(courant_b);
        } else {
            nouveau      = Insert_AlphaRBT(resultat->alpha_tree, courant_a->word);
            nouveau->rep = (courant_a->rep > courant_b->rep) ? courant_a->rep : courant_b->rep;
            courant_a    = alpha_inorder_next(courant_a);
            courant_b    = alpha_inorder_next(courant_b);
        }
    }
    while (courant_a != &ALPHA_NIL) {
        nouveau      = Insert_AlphaRBT(resultat->alpha_tree, courant_a->word);
        nouveau->rep = courant_a->rep;
        courant_a    = alpha_inorder_next(courant_a);
    }
    while (courant_b != &ALPHA_NIL) {
        nouveau      = Insert_AlphaRBT(resultat->alpha_tree, courant_b->word);
        nouveau->rep = courant_b->rep;
        courant_b    = alpha_inorder_next(courant_b);
    }
    return resultat;
}

PetalNode* petal_intersection(PetalNode* a, PetalNode* b) {
    PetalNode*    resultat = allocatePetalNode();
    AlphaRBTNode* courant_a = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* courant_b = alpha_inorder_first(b->alpha_tree);
    while (courant_a != &ALPHA_NIL && courant_b != &ALPHA_NIL) {
        int comparaison = strcmp(courant_a->word, courant_b->word);
        if      (comparaison < 0) courant_a = alpha_inorder_next(courant_a);
        else if (comparaison > 0) courant_b = alpha_inorder_next(courant_b);
        else {
            AlphaRBTNode* nouveau = Insert_AlphaRBT(resultat->alpha_tree, courant_a->word);
            nouveau->rep = (courant_a->rep < courant_b->rep) ? courant_a->rep : courant_b->rep;
            courant_a    = alpha_inorder_next(courant_a);
            courant_b    = alpha_inorder_next(courant_b);
        }
    }
    return resultat;
}

PetalNode* petal_difference(PetalNode* a, PetalNode* b) {
    PetalNode*    resultat  = allocatePetalNode();
    AlphaRBTNode* courant_a = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* courant_b = alpha_inorder_first(b->alpha_tree);
    AlphaRBTNode* nouveau   = NULL;
    while (courant_a != &ALPHA_NIL && courant_b != &ALPHA_NIL) {
        int comparaison = strcmp(courant_a->word, courant_b->word);
        if (comparaison < 0) {
            nouveau      = Insert_AlphaRBT(resultat->alpha_tree, courant_a->word);
            nouveau->rep = courant_a->rep;
            courant_a    = alpha_inorder_next(courant_a);
        } else if (comparaison > 0) {
            courant_b = alpha_inorder_next(courant_b);
        } else {
            if (courant_a->rep > courant_b->rep) {
                nouveau      = Insert_AlphaRBT(resultat->alpha_tree, courant_a->word);
                nouveau->rep = courant_a->rep - courant_b->rep;
            }
            courant_a = alpha_inorder_next(courant_a);
            courant_b = alpha_inorder_next(courant_b);
        }
    }
    while (courant_a != &ALPHA_NIL) {
        nouveau      = Insert_AlphaRBT(resultat->alpha_tree, courant_a->word);
        nouveau->rep = courant_a->rep;
        courant_a    = alpha_inorder_next(courant_a);
    }
    return resultat;
}

PetalNode* petal_symmetric_difference(PetalNode* a, PetalNode* b) {
    PetalNode*    resultat  = allocatePetalNode();
    AlphaRBTNode* courant_a = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* courant_b = alpha_inorder_first(b->alpha_tree);
    while (courant_a != &ALPHA_NIL && courant_b != &ALPHA_NIL) {
        int comparaison = strcmp(courant_a->word, courant_b->word);
        if (comparaison < 0) {
            Insert_AlphaRBT(resultat->alpha_tree, courant_a->word);
            courant_a = alpha_inorder_next(courant_a);
        } else if (comparaison > 0) {
            Insert_AlphaRBT(resultat->alpha_tree, courant_b->word);
            courant_b = alpha_inorder_next(courant_b);
        } else {
            courant_a = alpha_inorder_next(courant_a);
            courant_b = alpha_inorder_next(courant_b);
        }
    }
    while (courant_a != &ALPHA_NIL) { Insert_AlphaRBT(resultat->alpha_tree, courant_a->word); courant_a = alpha_inorder_next(courant_a); }
    while (courant_b != &ALPHA_NIL) { Insert_AlphaRBT(resultat->alpha_tree, courant_b->word); courant_b = alpha_inorder_next(courant_b); }
    return resultat;
}

int petal_is_subset_of(PetalNode* a, PetalNode* b) {
    AlphaRBTNode* courant_a = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* courant_b = alpha_inorder_first(b->alpha_tree);
    while (courant_a != &ALPHA_NIL && courant_b != &ALPHA_NIL) {
        int comparaison = strcmp(courant_a->word, courant_b->word);
        if      (comparaison == 0) { courant_a = alpha_inorder_next(courant_a); courant_b = alpha_inorder_next(courant_b); }
        else if (comparaison  > 0) courant_b = alpha_inorder_next(courant_b);
        else return 0;
    }
    if (courant_a == &ALPHA_NIL) return 1;
    return 0;
}

int petal_is_identical(PetalNode* a, PetalNode* b) {
    AlphaRBTNode* courant_a = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* courant_b = alpha_inorder_first(b->alpha_tree);
    while (courant_a != &ALPHA_NIL && courant_b != &ALPHA_NIL) {
        if (strcmp(courant_a->word, courant_b->word) != 0) return 0;
        courant_a = alpha_inorder_next(courant_a);
        courant_b = alpha_inorder_next(courant_b);
    }
    if (courant_a == &ALPHA_NIL && courant_b == &ALPHA_NIL) return 1;
    return 0;
}

/* ============================================================
 *  OPERATIONS ENSEMBLISTES — PHRASES (hash table)
 * ============================================================ */
#define TAILLE_TABLE  1024
#define BASE_HASH     31
#define MOD_HASH      1000000007

static unsigned int hash_mot(const char* mot) {
    unsigned int h = 0;
    while (*mot) { h = h * BASE_HASH + (unsigned char)(*mot); mot++; }
    return h;
}

static unsigned int hash_phrase(PosRBTNode* debut) {
    unsigned int h    = 0;
    unsigned int base = 1;
    PosRBTNode*  courant = debut;
    while (courant != &POS_NIL && (courant == debut || !courant->sent_flag)) {
        h    = (h + hash_mot(courant->word_ref->word) * base) % MOD_HASH;
        base = (base * BASE_HASH) % MOD_HASH;
        courant = pos_inorder_next(courant);
    }
    return h;
}

static int comparer_phrases(PosRBTNode* phrase1, PosRBTNode* phrase2) {
    PosRBTNode* courant1 = phrase1;
    PosRBTNode* courant2 = phrase2;
    while (1) {
        int fin1 = (courant1 == &POS_NIL || (courant1 != phrase1 && courant1->sent_flag));
        int fin2 = (courant2 == &POS_NIL || (courant2 != phrase2 && courant2->sent_flag));
        if (fin1 && fin2) return 1;
        if (fin1 || fin2) return 0;
        if (strcmp(courant1->word_ref->word, courant2->word_ref->word) != 0) return 0;
        courant1 = pos_inorder_next(courant1);
        courant2 = pos_inorder_next(courant2);
    }
}

static SentenceEntry** construire_table_phrases(PetalNode* petale) {
    SentenceEntry** table = calloc(TAILLE_TABLE, sizeof(SentenceEntry*));
    if (!table) return NULL;
    PosRBTNode* courant = pos_inorder_first(petale->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag) {
            unsigned int    h      = hash_phrase(courant);
            int             slot   = h % TAILLE_TABLE;
            SentenceEntry*  entree = malloc(sizeof(SentenceEntry));
            entree->hash  = h;
            entree->start = courant;
            entree->next  = table[slot];
            table[slot]   = entree;
        }
        courant = pos_inorder_next(courant);
    }
    return table;
}

static void liberer_table_phrases(SentenceEntry** table) {
    for (int i = 0; i < TAILLE_TABLE; i++) {
        SentenceEntry* courant = table[i];
        while (courant) {
            SentenceEntry* temporaire = courant;
            courant = courant->next;
            free(temporaire);
        }
    }
    free(table);
}

static int phrase_dans_table(SentenceEntry** table, PosRBTNode* noeud) {
    unsigned int   h      = hash_phrase(noeud);
    SentenceEntry* entree = table[h % TAILLE_TABLE];
    while (entree) {
        if (entree->hash == h && comparer_phrases(noeud, entree->start)) return 1;
        entree = entree->next;
    }
    return 0;
}

static void inserer_phrase_dans_petale(PetalNode* resultat, PosRBTNode* debut) {
    PosRBTNode* courant = debut;
    while (courant != &POS_NIL && (courant == debut || !courant->sent_flag)) {
        Insert_word(resultat, courant->word_ref->word, courant->sent_flag);
        courant = pos_inorder_next(courant);
    }
}

PetalNode* petal_sentence_intersection(PetalNode* P1, PetalNode* P2) {
    PetalNode*      resultat = allocatePetalNode();
    SentenceEntry** table    = construire_table_phrases(P2);
    if (!table) return resultat;
    PosRBTNode* courant = pos_inorder_first(P1->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag && phrase_dans_table(table, courant))
            inserer_phrase_dans_petale(resultat, courant);
        courant = pos_inorder_next(courant);
    }
    liberer_table_phrases(table);
    return resultat;
}

PetalNode* petal_sentence_difference(PetalNode* P1, PetalNode* P2) {
    PetalNode*      resultat = allocatePetalNode();
    SentenceEntry** table    = construire_table_phrases(P2);
    if (!table) return resultat;
    PosRBTNode* courant = pos_inorder_first(P1->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag && !phrase_dans_table(table, courant))
            inserer_phrase_dans_petale(resultat, courant);
        courant = pos_inorder_next(courant);
    }
    liberer_table_phrases(table);
    return resultat;
}

PetalNode* petal_sentence_union(PetalNode* P1, PetalNode* P2) {
    PetalNode* resultat = allocatePetalNode();
    PosRBTNode* courant = pos_inorder_first(P1->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag) inserer_phrase_dans_petale(resultat, courant);
        courant = pos_inorder_next(courant);
    }
    SentenceEntry** table = construire_table_phrases(P1);
    if (!table) return resultat;
    courant = pos_inorder_first(P2->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag && !phrase_dans_table(table, courant))
            inserer_phrase_dans_petale(resultat, courant);
        courant = pos_inorder_next(courant);
    }
    liberer_table_phrases(table);
    return resultat;
}

PetalNode* petal_sentence_symmetric_difference(PetalNode* P1, PetalNode* P2) {
    PetalNode* resultat = allocatePetalNode();
    SentenceEntry** table2 = construire_table_phrases(P2);
    if (!table2) return resultat;
    PosRBTNode* courant = pos_inorder_first(P1->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag && !phrase_dans_table(table2, courant))
            inserer_phrase_dans_petale(resultat, courant);
        courant = pos_inorder_next(courant);
    }
    liberer_table_phrases(table2);
    SentenceEntry** table1 = construire_table_phrases(P1);
    if (!table1) return resultat;
    courant = pos_inorder_first(P2->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag && !phrase_dans_table(table1, courant))
            inserer_phrase_dans_petale(resultat, courant);
        courant = pos_inorder_next(courant);
    }
    liberer_table_phrases(table1);
    return resultat;
}

int petal_sentence_is_subset(PetalNode* P1, PetalNode* P2) {
    SentenceEntry** table = construire_table_phrases(P2);
    if (!table) return 0;
    PosRBTNode* courant = pos_inorder_first(P1->pos_tree);
    while (courant != &POS_NIL) {
        if (courant->sent_flag && !phrase_dans_table(table, courant)) {
            liberer_table_phrases(table);
            return 0;
        }
        courant = pos_inorder_next(courant);
    }
    liberer_table_phrases(table);
    return 1;
}

int petal_sentence_is_identical(PetalNode* P1, PetalNode* P2) {
    if (petal_sentence_count(P1) != petal_sentence_count(P2)) return 0;
    return petal_sentence_is_subset(P1, P2);
}

/* ============================================================
 *  LAYER 3 — Traitement du texte
 * ============================================================ */
int est_delimiteur(char caractere) {
    return (caractere == ' '  || caractere == '\t' || caractere == '\n' ||
            caractere == '.'  || caractere == ','  || caractere == '?'  ||
            caractere == '!'  || caractere == '"'  || caractere == '\'' ||
            caractere == ';'  || caractere == ':'  || caractere == '-');
}

int est_fin_de_phrase(char caractere) {
    return (caractere == '.' || caractere == '!' || caractere == '?');
}

int est_saut_de_paragraphe(char* chaine, int i) {
    return (chaine[i] == '\n' && chaine[i + 1] == '\n');
}

int est_fin_de_chaine(char* chaine, int i) {
    return (chaine[i] == '\0');
}

int sauter_delimiteurs(char* chaine, int i, int* nouveau_para, int* debut_phrase) {
    while (chaine[i] != '\0' && est_delimiteur(chaine[i])) {
        if (est_fin_de_phrase(chaine[i]))      *debut_phrase  = 1;
        if (est_saut_de_paragraphe(chaine, i)) *nouveau_para  = 1;
        i++;
    }
    return i;
}

int collecter_mot(char* chaine, int i) {
    while (chaine[i] != '\0' && !est_delimiteur(chaine[i])) i++;
    return i;
}

void detecter_nouveau_para(char* chaine, int i, char** suivant, int* nouveau_para) {
    if (est_fin_de_chaine(chaine, i)) {
        *nouveau_para = 1;
        *suivant      = NULL;
    } else {
        *nouveau_para = est_saut_de_paragraphe(chaine, i) ? 1 : 0;
        chaine[i]     = '\0';
        *suivant      = chaine + i + 1;
    }
}

char* mot_suivant(char** chaine, int* nouveau_para, int* debut_phrase) {
    if (!chaine || !*chaine || !**chaine) return NULL;
    char* courant = *chaine;
    int   i       = 0;
    *debut_phrase  = 0;
    i = sauter_delimiteurs(courant, i, nouveau_para, debut_phrase);
    if (est_fin_de_chaine(courant, i)) { *chaine = NULL; return NULL; }
    char* debut = courant + i;
    i = collecter_mot(courant, i);
    detecter_nouveau_para(courant, i, chaine, nouveau_para);
    return debut;
}

/* ============================================================
 *  LAYER 4 — Rose
 * ============================================================ */
Rose* AllocateRose() {
    Rose* rose    = malloc(sizeof(Rose));
    rose->petals  = NULL;
    rose->size    = 0;
    rose->nom[0]  = '\0';
    return rose;
}

void ajouter_petale(Rose* rose) {
    PetalNode* petale = allocatePetalNode();
    if (rose->petals == NULL) {
        rose->petals  = petale;
        petale->next  = petale;
        petale->prev  = petale;
    } else {
        PetalNode* queue    = rose->petals->prev;
        queue->next         = petale;
        petale->prev        = queue;
        petale->next        = rose->petals;
        rose->petals->prev  = petale;
    }
    rose->size++;
}

void rose_free(Rose* rose) {
    if (!rose) return;
    if (!rose->petals) { free(rose); return; }
    PetalNode* courant  = rose->petals;
    PetalNode* suivant  = NULL;
    do {
        suivant = courant->next;
        petal_free(courant);
        courant = suivant;
    } while (courant != rose->petals);
    free(rose);
}

PetalNode* rose_get_petal(Rose* rose, int index) {
    if (!rose->petals || index < 0 || index >= rose->size) return NULL;
    PetalNode* courant = rose->petals;
    for (int j = 0; j < index; j++) courant = courant->next;
    return courant;
}

Rose* charger_fichier(const char* nom_fichier) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) { printf("Impossible d'ouvrir %s\n", nom_fichier); return NULL; }
    Rose* rose          = AllocateRose();
    strncpy(rose->nom, nom_fichier, 255);
    char  ligne[1024];
    int   nouveau_para  = 0;
    int   ligne_vide    = 0;
    int   debut_phrase  = 1;
    ajouter_petale(rose);
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '\n' || ligne[0] == '\r') { ligne_vide = 1; continue; }
        if (ligne_vide) {
            ajouter_petale(rose);
            ligne_vide   = 0;
            debut_phrase = 1;
        }
        char* pointeur = ligne;
        char* mot      = mot_suivant(&pointeur, &nouveau_para, &debut_phrase);
        while (mot) {
            Insert_word(rose->petals->prev, mot, debut_phrase);
            debut_phrase = 0;
            mot          = mot_suivant(&pointeur, &nouveau_para, &debut_phrase);
        }
    }
    fclose(fichier);
    return rose;
}

int rose_total_word_count(Rose* rose) {
    if (!rose->petals) return 0;
    PetalNode* courant = rose->petals;
    int        total   = 0;
    do { total += petal_word_count(courant); courant = courant->next; } while (courant != rose->petals);
    return total;
}