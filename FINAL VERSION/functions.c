#include "structures.h"

AlphaRBTNode ALPHA_NIL = { NULL, NULL, BLACK, 0, &ALPHA_NIL, &ALPHA_NIL, &ALPHA_NIL };
PosRBTNode POS_NIL = { 0, NULL, BLACK, &POS_NIL, &POS_NIL, &POS_NIL };

PosNode* poslist_create_node(PosRBTNode* ref)
{
    PosNode* node = malloc(sizeof(PosNode));
    if (node == NULL) {
        printf("malloc failed PosNode\n");
        return NULL;
    }
    node->pos_ref = ref;
    node->next = node;
    node->prev = node;
    return node;
}

void poslist_insert(PosNode** head, PosRBTNode* ref)
{
    PosNode* node = poslist_create_node(ref);
    if (*head == NULL) {
        *head = node;
        return;
    }
    PosNode* last = (*head)->prev;
    last->next = node;
    node->prev = last;
    node->next = *head;
    (*head)->prev = node;
}

void poslist_free(PosNode* head)
{
    if (head == NULL) {
        return;
    }
    PosNode* cur = head;
    do {
        PosNode* tmp = cur;
        cur = cur->next;
        free(tmp);
    } while (cur != head);
}

void poslist_print(PosNode* head)
{
    if (head == NULL) {
        printf("[]");
        return;
    }
    printf("[");
    PosNode* cur = head;
    do {
        printf("%d", cur->pos_ref->position);
        if (cur->next != head) {
            printf(" <-> ");
        }
        cur = cur->next;
    } while (cur != head);
    printf("]");
}

AlphaRBT* allocate_alpha_rbt()
{
    AlphaRBT* tree = malloc(sizeof(AlphaRBT));
    tree->root = &ALPHA_NIL;
    return tree;
}

PosRBT* allocate_pos_rbt()
{
    PosRBT* tree = malloc(sizeof(PosRBT));
    tree->root = &POS_NIL;
    tree->max = &POS_NIL;
    return tree;
}

AlphaRBTNode* allocate_alpha_node()
{
    AlphaRBTNode* node = malloc(sizeof(AlphaRBTNode));
    if (node == NULL) {
        printf("malloc failed AlphaRBTNode\n");
        return &ALPHA_NIL;
    }
    node->color = RED;
    node->parent = &ALPHA_NIL;
    node->left = &ALPHA_NIL;
    node->right = &ALPHA_NIL;
    node->pos_list = NULL;
    node->rep = 0;
    node->word = NULL;
    return node;
}

PosRBTNode* allocate_pos_node()
{
    PosRBTNode* node = malloc(sizeof(PosRBTNode));
    if (node == NULL) {
        printf("malloc failed PosRBTNode\n");
        return &POS_NIL;
    }
    node->color = RED;
    node->parent = &POS_NIL;
    node->left = &POS_NIL;
    node->right = &POS_NIL;
    node->word_ref = NULL;
    node->position = 0;
    return node;
}

void alpha_right_rotate(AlphaRBT* tree, AlphaRBTNode* y)
{
    AlphaRBTNode* x = y->left;
    y->left = x->right;
    if (x->right != &ALPHA_NIL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &ALPHA_NIL) {
        tree->root = x;
    }
    else if (y == y->parent->left) {
        y->parent->left = x;
    }
    else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

void alpha_left_rotate(AlphaRBT* tree, AlphaRBTNode* y)
{
    AlphaRBTNode* x = y->right;
    y->right = x->left;
    if (x->left != &ALPHA_NIL) {
        x->left->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &ALPHA_NIL) {
        tree->root = x;
    }
    else if (y == y->parent->right) {
        y->parent->right = x;
    }
    else {
        y->parent->left = x;
    }
    x->left = y;
    y->parent = x;
}

void pos_right_rotate(PosRBT* tree, PosRBTNode* y)
{
    PosRBTNode* x = y->left;
    y->left = x->right;
    if (x->right != &POS_NIL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &POS_NIL) {
        tree->root = x;
    }
    else if (y == y->parent->left) {
        y->parent->left = x;
    }
    else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

void pos_left_rotate(PosRBT* tree, PosRBTNode* y)
{
    PosRBTNode* x = y->right;
    y->right = x->left;
    if (x->left != &POS_NIL) {
        x->left->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &POS_NIL) {
        tree->root = x;
    }
    else if (y == y->parent->right) {
        y->parent->right = x;
    }
    else {
        y->parent->left = x;
    }
    x->left = y;
    y->parent = x;
}

void alpha_fixup(AlphaRBT* tree, AlphaRBTNode* node)
{
    AlphaRBTNode* uncle;
    AlphaRBTNode* gp;
    while (node->parent != &ALPHA_NIL && node->parent->color == RED) {
        gp = node->parent->parent;
        if (node->parent == gp->left) {
            uncle = gp->right;
            if (uncle->color == RED) {
                uncle->color = BLACK;
                node->parent->color = BLACK;
                gp->color = RED;
                node = gp;
            }
            else {
                if (node == node->parent->right) {
                    node = node->parent;
                    alpha_left_rotate(tree, node);
                }
                node->parent->color = BLACK;
                gp->color = RED;
                alpha_right_rotate(tree, gp);
            }
        }
        else {
            uncle = gp->left;
            if (uncle->color == RED) {
                uncle->color = BLACK;
                node->parent->color = BLACK;
                gp->color = RED;
                node = gp;
            }
            else {
                if (node == node->parent->left) {
                    node = node->parent;
                    alpha_right_rotate(tree, node);
                }
                node->parent->color = BLACK;
                gp->color = RED;
                alpha_left_rotate(tree, gp);
            }
        }
    }
    tree->root->color = BLACK;
}

void pos_fixup(PosRBT* tree, PosRBTNode* node)
{
    PosRBTNode* uncle;
    PosRBTNode* gp;
    while (node->parent != &POS_NIL && node->parent->color == RED) {
        gp = node->parent->parent;
        if (node->parent == gp->left) {
            uncle = gp->right;
            if (uncle->color == RED) {
                uncle->color = BLACK;
                node->parent->color = BLACK;
                gp->color = RED;
                node = gp;
            }
            else {
                if (node == node->parent->right) {
                    node = node->parent;
                    pos_left_rotate(tree, node);
                }
                node->parent->color = BLACK;
                gp->color = RED;
                pos_right_rotate(tree, gp);
            }
        }
        else {
            uncle = gp->left;
            if (uncle->color == RED) {
                uncle->color = BLACK;
                node->parent->color = BLACK;
                gp->color = RED;
                node = gp;
            }
            else {
                if (node == node->parent->left) {
                    node = node->parent;
                    pos_right_rotate(tree, node);
                }
                node->parent->color = BLACK;
                gp->color = RED;
                pos_left_rotate(tree, gp);
            }
        }
    }
    tree->root->color = BLACK;
}

AlphaRBTNode* insert_alpha(AlphaRBT* tree, char* word)
{
    AlphaRBTNode* parent = &ALPHA_NIL;
    AlphaRBTNode* node = tree->root;
    while (node != &ALPHA_NIL) {
        int cmp = strcmp(word, node->word);
        if (cmp == 0) {
            return node;
        }
        parent = node;
        if (cmp < 0) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }
    AlphaRBTNode* new_node = allocate_alpha_node();
    new_node->word = strdup(word);
    new_node->parent = parent;
    if (parent == &ALPHA_NIL) {
        tree->root = new_node;
    }
    else if (strcmp(word, parent->word) < 0) {
        parent->left = new_node;
    }
    else {
        parent->right = new_node;
    }
    alpha_fixup(tree, new_node);
    return new_node;
}

PosRBTNode* insert_pos(PosRBT* tree)
{
    PosRBTNode* new_node = allocate_pos_node();
    new_node->parent = tree->max;
    if (tree->max == &POS_NIL) {
        new_node->position = 1;
        tree->root = new_node;
    }
    else {
        new_node->position = tree->max->position + 1;
        tree->max->right = new_node;
    }
    pos_fixup(tree, new_node);
    tree->max = new_node;
    return new_node;
}

AlphaRBTNode* alpha_inorder_first(AlphaRBT* tree)
{
    AlphaRBTNode* node = tree->root;
    if (node == &ALPHA_NIL) {
        return &ALPHA_NIL;
    }
    while (node->left != &ALPHA_NIL) {
        node = node->left;
    }
    return node;
}

AlphaRBTNode* alpha_inorder_next(AlphaRBTNode* node)
{
    if (node->right != &ALPHA_NIL) {
        node = node->right;
        while (node->left != &ALPHA_NIL) {
            node = node->left;
        }
        return node;
    }
    AlphaRBTNode* parent = node->parent;
    while (parent != &ALPHA_NIL && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

PosRBTNode* pos_inorder_first(PosRBT* tree)
{
    PosRBTNode* node = tree->root;
    if (node == &POS_NIL) {
        return &POS_NIL;
    }
    while (node->left != &POS_NIL) {
        node = node->left;
    }
    return node;
}

PosRBTNode* pos_inorder_next(PosRBTNode* node)
{
    if (node->right != &POS_NIL) {
        node = node->right;
        while (node->left != &POS_NIL) {
            node = node->left;
        }
        return node;
    }
    PosRBTNode* parent = node->parent;
    while (parent != &POS_NIL && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

void pos_rbt_free(PosRBTNode* node)
{
    if (node == &POS_NIL) {
        return;
    }
    pos_rbt_free(node->left);
    pos_rbt_free(node->right);
    free(node);
}

void alpha_rbt_free(AlphaRBTNode* node)
{
    if (node == &ALPHA_NIL) {
        return;
    }
    alpha_rbt_free(node->left);
    alpha_rbt_free(node->right);
    poslist_free(node->pos_list);
    free(node->word);
    free(node);
}

void print_pos_tree(PosRBT* tree, PosRBTNode* node)
{
    if (node == &POS_NIL) {
        return;
    }
    print_pos_tree(tree, node->left);
    printf("%s ", node->word_ref->word);
    print_pos_tree(tree, node->right);
}

void print_alpha_tree(AlphaRBTNode* node)
{
    if (node == &ALPHA_NIL) {
        return;
    }
    print_alpha_tree(node->left);
    printf("  - %s (x%d | positions: ", node->word, node->rep);
    poslist_print(node->pos_list);
    printf(")\n");
    print_alpha_tree(node->right);
}

PetalNode* allocate_petal()
{
    PetalNode* p = malloc(sizeof(PetalNode));
    if (p == NULL) {
        printf("malloc failed petal\n");
        return NULL;
    }
    p->alpha_tree = allocate_alpha_rbt();
    p->pos_tree = allocate_pos_rbt();
    p->next = p;
    p->prev = p;
    return p;
}

void petal_free(PetalNode* p)
{
    if (p == NULL) {
        return;
    }
    pos_rbt_free(p->pos_tree->root);
    free(p->pos_tree);
    alpha_rbt_free(p->alpha_tree->root);
    free(p->alpha_tree);
    free(p);
}

void insert_word(PetalNode* p, char* word)
{
    AlphaRBTNode* a = insert_alpha(p->alpha_tree, word);
    PosRBTNode* pn = insert_pos(p->pos_tree);
    a->rep = a->rep + 1;
    pn->word_ref = a;
    poslist_insert(&a->pos_list, pn);
}

void petal_print_by_position(PetalNode* p)
{
    if (p == NULL) {
        return;
    }
    print_pos_tree(p->pos_tree, p->pos_tree->root);
    printf("\n");
}

void petal_print_by_alpha(PetalNode* p)
{
    if (p == NULL) {
        return;
    }
    print_alpha_tree(p->alpha_tree->root);
}

char* petal_word_at_pos(PetalNode* p, int position)
{
    if (p->pos_tree->root == &POS_NIL) {
        return NULL;
    }
    PosRBTNode* cur = p->pos_tree->root;
    while (cur != &POS_NIL) {
        if (position == cur->position) {
            return cur->word_ref->word;
        }
        if (position < cur->position) {
            cur = cur->left;
        }
        else {
            cur = cur->right;
        }
    }
    return NULL;
}

int petal_word_count(PetalNode* p)
{
    if (p->pos_tree->max == &POS_NIL) {
        return 0;
    }
    return p->pos_tree->max->position;
}

void petal_print_range(PetalNode* p, int start, int end)
{
    if (p->pos_tree->root == &POS_NIL) {
        printf("  Empty petal.\n");
        return;
    }
    int total = petal_word_count(p);
    if (start < 1 || end < start || start > total) {
        printf("  Invalid range.\n");
        return;
    }
    PosRBTNode* cur = p->pos_tree->root;
    while (cur != &POS_NIL && cur->position != start) {
        if (start < cur->position) {
            cur = cur->left;
        }
        else {
            cur = cur->right;
        }
    }
    printf("  [%d..%d] ", start, end);
    while (cur != &POS_NIL && cur->position <= end) {
        printf("%s ", cur->word_ref->word);
        cur = pos_inorder_next(cur);
    }
    printf("\n");
}

int petal_unique_word_count(PetalNode* p)
{
    if (p->alpha_tree->root == &ALPHA_NIL) {
        return 0;
    }
    int n = 0;
    AlphaRBTNode* cur = alpha_inorder_first(p->alpha_tree);
    while (cur != &ALPHA_NIL) {
        n = n + 1;
        cur = alpha_inorder_next(cur);
    }
    return n;
}

char* petal_most_frequent_word(PetalNode* p)
{
    if (p->alpha_tree->root == &ALPHA_NIL) {
        return NULL;
    }
    AlphaRBTNode* cur = alpha_inorder_first(p->alpha_tree);
    AlphaRBTNode* best = cur;
    while (cur != &ALPHA_NIL) {
        if (cur->rep > best->rep) {
            best = cur;
        }
        cur = alpha_inorder_next(cur);
    }
    return best->word;
}

char* petal_least_frequent_word(PetalNode* p)
{
    if (p->alpha_tree->root == &ALPHA_NIL) {
        return NULL;
    }
    AlphaRBTNode* cur = alpha_inorder_first(p->alpha_tree);
    AlphaRBTNode* best = cur;
    while (cur != &ALPHA_NIL) {
        if (cur->rep < best->rep) {
            best = cur;
        }
        cur = alpha_inorder_next(cur);
    }
    return best->word;
}

void petal_print_frequency_table(PetalNode* p)
{
    if (p->alpha_tree->root == &ALPHA_NIL) {
        printf("  Empty petal.\n");
        return;
    }
    int total = 0;
    AlphaRBTNode* cur = alpha_inorder_first(p->alpha_tree);
    while (cur != &ALPHA_NIL) {
        total = total + cur->rep;
        cur = alpha_inorder_next(cur);
    }
    printf("  %-20s %6s %8s\n", "Word", "Count", "Freq%%");
    printf("  %-20s %6s %8s\n", "----", "-----", "-----");
    cur = alpha_inorder_first(p->alpha_tree);
    while (cur != &ALPHA_NIL) {
        double freq = 0.0;
        if (total > 0) {
            freq = 100.0 * cur->rep / total;
        }
        printf("  %-20s %6d %7.2f%%\n", cur->word, cur->rep, freq);
        cur = alpha_inorder_next(cur);
    }
    printf("  %-20s %6d %8s\n", "TOTAL", total, "100.00%%");
}

void petal_print_stats(PetalNode* p)
{
    if (p->alpha_tree->root == &ALPHA_NIL) {
        printf("  Empty petal.\n");
        return;
    }
    int total_words = petal_word_count(p);
    int unique_words = petal_unique_word_count(p);
    char* most = petal_most_frequent_word(p);
    char* least = petal_least_frequent_word(p);
    double density = 0.0;
    if (total_words > 0) {
        density = 100.0 * unique_words / total_words;
    }
    printf("  %-25s %d\n", "Total words:", total_words);
    printf("  %-25s %d\n", "Unique words:", unique_words);
    printf("  %-25s %.2f%%\n", "Lexical density:", density);
    if (most == NULL) {
        printf("  %-25s %s\n", "Most frequent:", "N/A");
    }
    else {
        printf("  %-25s %s\n", "Most frequent:", most);
    }
    if (least == NULL) {
        printf("  %-25s %s\n", "Least frequent:", "N/A");
    }
    else {
        printf("  %-25s %s\n", "Least frequent:", least);
    }
}

void rose_find_petals_with_word(Rose* rose, char* word)
{
    if (rose->petals == NULL) {
        printf("  Empty rose.\n");
        return;
    }
    int found = 0;
    PetalNode* cur = rose->petals;
    int idx = 0;
    do {
        AlphaRBTNode* node = cur->alpha_tree->root;
        while (node != &ALPHA_NIL) {
            int cmp = strcmp(word, node->word);
            if (cmp == 0) {
                break;
            }
            if (cmp < 0) {
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        if (node != &ALPHA_NIL) {
            found = 1;
            printf("  Paragraph %d (x%d) | positions: ", idx, node->rep);
            poslist_print(node->pos_list);
            printf("\n");
        }
        cur = cur->next;
        idx = idx + 1;
    } while (cur != rose->petals);
    if (found == 0) {
        printf("  Word '%s' not found in this rose.\n", word);
    }
}

PetalNode* petal_union(PetalNode* a, PetalNode* b)
{
    PetalNode* res = allocate_petal();
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    AlphaRBTNode* nn = NULL;
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if (cmp < 0) {
            nn = insert_alpha(res->alpha_tree, ca->word);
            nn->rep = ca->rep;
            ca = alpha_inorder_next(ca);
        }
        else if (cmp > 0) {
            nn = insert_alpha(res->alpha_tree, cb->word);
            nn->rep = cb->rep;
            cb = alpha_inorder_next(cb);
        }
        else {
            nn = insert_alpha(res->alpha_tree, ca->word);
            if (ca->rep > cb->rep) {
                nn->rep = ca->rep;
            }
            else {
                nn->rep = cb->rep;
            }
            ca = alpha_inorder_next(ca);
            cb = alpha_inorder_next(cb);
        }
    }
    while (ca != &ALPHA_NIL) {
        nn = insert_alpha(res->alpha_tree, ca->word);
        nn->rep = ca->rep;
        ca = alpha_inorder_next(ca);
    }
    while (cb != &ALPHA_NIL) {
        nn = insert_alpha(res->alpha_tree, cb->word);
        nn->rep = cb->rep;
        cb = alpha_inorder_next(cb);
    }
    return res;
}

PetalNode* petal_intersection(PetalNode* a, PetalNode* b)
{
    PetalNode* res = allocate_petal();
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if (cmp < 0) {
            ca = alpha_inorder_next(ca);
        }
        else if (cmp > 0) {
            cb = alpha_inorder_next(cb);
        }
        else {
            AlphaRBTNode* nn = insert_alpha(res->alpha_tree, ca->word);
            if (ca->rep < cb->rep) {
                nn->rep = ca->rep;
            }
            else {
                nn->rep = cb->rep;
            }
            ca = alpha_inorder_next(ca);
            cb = alpha_inorder_next(cb);
        }
    }
    return res;
}

PetalNode* petal_difference(PetalNode* a, PetalNode* b)
{
    PetalNode* res = allocate_petal();
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    AlphaRBTNode* nn = NULL;
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if (cmp < 0) {
            nn = insert_alpha(res->alpha_tree, ca->word);
            nn->rep = ca->rep;
            ca = alpha_inorder_next(ca);
        }
        else if (cmp > 0) {
            cb = alpha_inorder_next(cb);
        }
        else {
            if (ca->rep > cb->rep) {
                nn = insert_alpha(res->alpha_tree, ca->word);
                nn->rep = ca->rep - cb->rep;
            }
            ca = alpha_inorder_next(ca);
            cb = alpha_inorder_next(cb);
        }
    }
    while (ca != &ALPHA_NIL) {
        nn = insert_alpha(res->alpha_tree, ca->word);
        nn->rep = ca->rep;
        ca = alpha_inorder_next(ca);
    }
    return res;
}

PetalNode* petal_symmetric_difference(PetalNode* a, PetalNode* b)
{
    PetalNode* res = allocate_petal();
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if (cmp < 0) {
            insert_alpha(res->alpha_tree, ca->word);
            ca = alpha_inorder_next(ca);
        }
        else if (cmp > 0) {
            insert_alpha(res->alpha_tree, cb->word);
            cb = alpha_inorder_next(cb);
        }
        else {
            ca = alpha_inorder_next(ca);
            cb = alpha_inorder_next(cb);
        }
    }
    while (ca != &ALPHA_NIL) {
        insert_alpha(res->alpha_tree, ca->word);
        ca = alpha_inorder_next(ca);
    }
    while (cb != &ALPHA_NIL) {
        insert_alpha(res->alpha_tree, cb->word);
        cb = alpha_inorder_next(cb);
    }
    return res;
}

int petal_is_subset_of(PetalNode* a, PetalNode* b)
{
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        int cmp = strcmp(ca->word, cb->word);
        if (cmp == 0) {
            ca = alpha_inorder_next(ca);
            cb = alpha_inorder_next(cb);
        }
        else if (cmp > 0) {
            cb = alpha_inorder_next(cb);
        }
        else {
            return 0;
        }
    }
    if (ca == &ALPHA_NIL) {
        return 1;
    }
    return 0;
}

int petal_is_identical(PetalNode* a, PetalNode* b)
{
    AlphaRBTNode* ca = alpha_inorder_first(a->alpha_tree);
    AlphaRBTNode* cb = alpha_inorder_first(b->alpha_tree);
    while (ca != &ALPHA_NIL && cb != &ALPHA_NIL) {
        if (strcmp(ca->word, cb->word) != 0) {
            return 0;
        }
        ca = alpha_inorder_next(ca);
        cb = alpha_inorder_next(cb);
    }
    if (ca == &ALPHA_NIL && cb == &ALPHA_NIL) {
        return 1;
    }
    return 0;
}

int is_delimiter(char c)
{
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
        return 1;
    }
    if (c == '.' || c == ',' || c == '?' || c == '!') {
        return 1;
    }
    if (c == '"' || c == '\'' || c == ';' || c == ':' || c == '-') {
        return 1;
    }
    return 0;
}

int is_paragraph_break(char* s, int i)
{
    if (s[i] == '\n' && s[i + 1] == '\n') {
        return 1;
    }
    return 0;
}

int is_end_of_string(char* s, int i)
{
    if (s[i] == '\0') {
        return 1;
    }
    return 0;
}

int skip_delimiters(char* s, int i, int* new_para)
{
    while (s[i] != '\0' && is_delimiter(s[i]) == 1) {
        if (is_paragraph_break(s, i) == 1) {
            *new_para = 1;
        }
        i = i + 1;
    }
    return i;
}

int collect_word(char* s, int i)
{
    while (s[i] != '\0' && is_delimiter(s[i]) == 0) {
        i = i + 1;
    }
    return i;
}

void detect_paragraph_end(char* s, int i, char** next, int* new_para)
{
    if (is_end_of_string(s, i) == 1) {
        *new_para = 1;
        *next = NULL;
        return;
    }
    if (is_paragraph_break(s, i) == 1) {
        *new_para = 1;
    }
    s[i] = '\0';
    *next = s + i + 1;
}

char* next_word(char** str, int* new_para)
{
    if (str == NULL || *str == NULL) {
        return NULL;
    }
    if (**str == '\0') {
        return NULL;
    }
    char* s = *str;
    int i = 0;
    i = skip_delimiters(s, i, new_para);
    if (is_end_of_string(s, i) == 1) {
        *str = NULL;
        return NULL;
    }
    char* start = s + i;
    i = collect_word(s, i);
    detect_paragraph_end(s, i, str, new_para);
    return start;
}

Rose* allocate_rose()
{
    Rose* r = malloc(sizeof(Rose));
    r->petals = NULL;
    r->size = 0;
    r->name[0] = '\0';
    return r;
}

void append_petal(Rose* r)
{
    PetalNode* p = allocate_petal();
    if (r->petals == NULL) {
        r->petals = p;
        p->next = p;
        p->prev = p;
    }
    else {
        PetalNode* tail = r->petals->prev;
        tail->next = p;
        p->prev = tail;
        p->next = r->petals;
        r->petals->prev = p;
    }
    r->size = r->size + 1;
}

void rose_free(Rose* r)
{
    if (r == NULL) {
        return;
    }
    if (r->petals == NULL) {
        free(r);
        return;
    }
    PetalNode* cur = r->petals;
    PetalNode* nx;
    do {
        nx = cur->next;
        petal_free(cur);
        cur = nx;
    } while (cur != r->petals);
    free(r);
}

PetalNode* rose_get_petal(Rose* r, int index)
{
    if (r->petals == NULL) {
        return NULL;
    }
    if (index < 0 || index >= r->size) {
        return NULL;
    }
    PetalNode* cur = r->petals;
    int j = 0;
    while (j < index) {
        cur = cur->next;
        j = j + 1;
    }
    return cur;
}

int line_is_blank(char* line)
{
    int i = 0;
    while (line[i] != '\0') {
        char c = line[i];
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}

Rose* load_file(char* filename)
{
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        printf("Cannot open %s\n", filename);
        return NULL;
    }
    Rose* rose = allocate_rose();
    strncpy(rose->name, filename, 255);
    rose->name[255] = '\0';

    char line[1024];
    int blank_seen = 1;

    while (fgets(line, sizeof(line), f) != NULL) {
        if (line_is_blank(line) == 1) {
            blank_seen = 1;
            continue;
        }
        if (blank_seen == 1) {
            append_petal(rose);
            blank_seen = 0;
        }
        char* ptr = line;
        int dummy_np = 0;
        char* w = next_word(&ptr, &dummy_np);
        while (w != NULL) {
            int j = 0;
            while (w[j] != '\0') {
                if (w[j] >= 'A' && w[j] <= 'Z') {
                    w[j] = w[j] + ('a' - 'A');
                }
                j = j + 1;
            }
            insert_word(rose->petals->prev, w);
            w = next_word(&ptr, &dummy_np);
        }
    }
    fclose(f);
    return rose;
}

int rose_total_word_count(Rose* r)
{
    if (r->petals == NULL) {
        return 0;
    }
    PetalNode* cur = r->petals;
    int total = 0;
    do {
        total = total + petal_word_count(cur);
        cur = cur->next;
    } while (cur != r->petals);
    return total;
}
