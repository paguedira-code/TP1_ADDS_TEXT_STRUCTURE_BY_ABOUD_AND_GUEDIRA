#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
//NIL definition for RBTs:
AlphaRBTNode ALPHA_NIL = { NULL, NULL, 0, &ALPHA_NIL, &ALPHA_NIL, &ALPHA_NIL };
PosRBTNode   POS_NIL   = { NULL,    NULL, 0, &POS_NIL,   &POS_NIL,   &POS_NIL   };
//pos_list_functions:
PosNode* poslist_create_node(PosRBTNode* pos_ref) {
    PosNode* new = malloc(sizeof(PosNode));
    if (new == NULL) {
        printf("ERROR allocation of PosNode\n");
        return NULL;
    }
    new->pos_ref = pos_ref;
    new->next    = new;
    new->prev    = new;
    return new;
}
void poslist_insert(PosNode** head, PosRBTNode* pos_ref) {
    PosNode* new = poslist_create_node(pos_ref);
    if (*head == NULL) {
        *head = new;
    } else {
        PosNode* last = (*head)->prev;
        last->next    = new;
        new->prev     = last;
        new->next     = *head;
        (*head)->prev = new;
    }
}
void poslist_free(PosNode* head) {
    if (head == NULL){
        return;
    }
    PosNode* cur = head;
    do {
        PosNode* tmp = cur;
        cur          = cur->next;
        free(tmp);
    } while (cur != head);
}
PosNode* poslist_copy(PosNode* head) {
    if (head == NULL) return NULL;
    PosNode* result = NULL;
    PosNode* cur    = head;
    do {
        poslist_insert(&result, cur->pos_ref);
        cur = cur->next;
    } while (cur != head);
    return result;
}
void poslist_print(PosNode* head) {
    if (head == NULL) { printf("[]"); return; }
    printf("[");
    PosNode* cur = head;
    do {
        printf("%d", cur->pos_ref->position);
        if (cur->next != head) printf(" <-> ");
        cur = cur->next;
    } while (cur != head);
    printf("]");
}

char *seperate_words(char **str, int *newpara){
    if ((str == NULL) || (*str == NULL) || (**str == '\0'))
    {
        return NULL;
    }
    char *currpos = *str;
    int i = 0;
    while (currpos[i] != '\0' && (currpos[i] == '.' || currpos[i] == ',' || currpos[i] == '?' || currpos[i] == '"' || currpos[i] == ' ' || currpos[i] == '\t' || currpos[i] == '\n'))
    {
        i++;
        if (currpos[i] == '\n' && newpara != NULL)
        {
            *newpara = 1;
        }
    }
    if (currpos[i] == '\0')
    {
        *newpara = 0;
        *str = NULL;
        return NULL;
    }
    char *start = currpos + i;
    while (currpos[i] != '\0' && currpos[i] != '.' && currpos[i] != ',' && currpos[i] != '?' && currpos[i] != '"' && currpos[i] != ' ' && currpos[i] != '\t' && currpos[i] != '\n')
    {
        i++;
    }
    if (currpos[i] == '\0')
    {
        *newpara = 1;
        *str = NULL;
    }
    else
    {
        if (currpos[i] == '\n' && (currpos[i + 1] == '\0' || currpos[i + 1] == '\t'))
        {
            *newpara = 1;
        }
        else
        {
            *newpara = 0;
        }
        currpos[i] = '\0';
        *str = currpos + i + 1;
    }
    return start;
}

//RBTs_functions:
AlphaRBT *AlocateAlphaRBT(){
    AlphaRBT *tree = malloc(sizeof(AlphaRBT));
    tree->root = &ALPHA_NIL;
    return tree;
}

PosRBT *AlocatePosRBT(){
    PosRBT *tree = malloc(sizeof(PosRBT));
    tree->root = &POS_NIL;
    tree->max = &POS_NIL;
    return tree;
}

AlphaRBTNode *allocateAlphaRBTNode(AlphaRBT *RBT){
    AlphaRBTNode *P = malloc(sizeof(AlphaRBTNode));
    if (P == NULL){
        printf("failed alocation");
        return &ALPHA_NIL;
    }
    P->color = RED;
    P->parent = &ALPHA_NIL;
    P->left = &ALPHA_NIL;
    P->right = &ALPHA_NIL;
    P->pos_list = NULL;
    return P;
}

PosRBTNode *allocatePosRBTNode(PosRBT *RBT){
    PosRBTNode *P = malloc(sizeof(PosRBTNode));
    if (P == NULL){
        printf("failed alocation");
        return &POS_NIL;
    }
    P->color = RED;
    P->parent = &POS_NIL;
    P->left = &POS_NIL;
    P->right = &POS_NIL;
    return P;
}

void AlphaRBTRightRot(AlphaRBT *RBT, AlphaRBTNode *y)
{
    AlphaRBTNode *x = y->left;
    y->left = x->right;
    if (x->right != &ALPHA_NIL)
    {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &ALPHA_NIL){
        RBT->root = x;
    }
    else
    {
        if (y == y->parent->left)
        {
            y->parent->left = x;
        }
        else
        {
            y->parent->right = x;
        }
    }
    x->right = y;
    y->parent = x;
}

void AlphaRBTLeftRot(AlphaRBT *RBT, AlphaRBTNode *y)
{
    AlphaRBTNode *x = y->right;
    y->right = x->left;
    if (x->left != &ALPHA_NIL)
    {
        x->left->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &ALPHA_NIL)
    {
        RBT->root = x;
    }
    else
    {
        if (y == y->parent->right)
        {
            y->parent->right = x;
        }
        else
        {
            y->parent->left = x;
        }
    }
    x->left = y;
    y->parent = x;
}

void PosRBTRightRot(PosRBT *RBT, PosRBTNode *y)
{
    PosRBTNode *x = y->left;
    y->left = x->right;
    if (x->right != &POS_NIL)
    {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &POS_NIL)
    {
        RBT->root = x;
    }
    else
    {
        if (y == y->parent->left)
        {
            y->parent->left = x;
        }
        else
        {
            y->parent->right = x;
        }
    }
    x->right = y;
    y->parent = x;
}

void PosRBTLeftRot(PosRBT *RBT, PosRBTNode *y)
{
    PosRBTNode *x = y->right;
    y->right = x->left;
    if (x->left != &POS_NIL)
    {
        x->left->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == &POS_NIL)
    {
        RBT->root = x;
    }
    else
    {
        if (y == y->parent->right)
        {
            y->parent->right = x;
        }
        else
        {
            y->parent->left = x;
        }
    }
    x->left = y;
    y->parent = x;
}

void AlphaRBTfix(AlphaRBT *RBT, AlphaRBTNode *P)
{
    AlphaRBTNode *unc = &ALPHA_NIL;
    AlphaRBTNode *gp = &ALPHA_NIL;
    while ((P->parent != &ALPHA_NIL) && (P->parent->color == RED))
    {
        gp = P->parent->parent;
        if (P->parent == gp->left)
        {
            unc = gp->right;
            if (unc->color == RED)
            {
                unc->color = BLACK;
                P->parent->color = BLACK;
                gp->color = RED;
                P = gp;
            }
            else
            {
                if (P == P->parent->right)
                {
                    P = P->parent;
                    AlphaRBTLeftRot(RBT, P);
                }
                P->parent->color = BLACK;
                gp->color = RED;
                AlphaRBTRightRot(RBT, gp);
            }
        }
        else
        {
            unc = gp->left;
            if (unc->color == RED)
            {
                unc->color = BLACK;
                P->parent->color = BLACK;
                gp->color = RED;
                P = gp;
            }
            else
            {
                if (P == P->parent->left)
                {
                    P = P->parent;
                    AlphaRBTRightRot(RBT, P);
                }
                P->parent->color = BLACK;
                gp->color = RED;
                AlphaRBTLeftRot(RBT, gp);
            }
        }
    }
    RBT->root->color = BLACK;
}

void PosRBTfix(PosRBT *RBT, PosRBTNode *P){
    PosRBTNode *unc;
    PosRBTNode *gp;
    while ((P->parent != &POS_NIL) && (P->parent->color == RED))
    {
        gp = P->parent->parent;
        if (P->parent == gp->left)
        {
            unc = gp->right;
            if (unc->color == RED)
            {
                unc->color = BLACK;
                P->parent->color = BLACK;
                gp->color = RED;
                P = gp;
            }
            else
            {
                if (P == P->parent->right)
                {
                    P = P->parent;
                    PosRBTLeftRot(RBT, P);
                }
                P->parent->color = BLACK;
                gp->color = RED;
                PosRBTRightRot(RBT, gp);
            }
        }
        else
        {
            unc = gp->left;
            if (unc->color == RED)
            {
                unc->color = BLACK;
                P->parent->color = BLACK;
                gp->color = RED;
                P = gp;
            }
            else
            {
                if (P == P->parent->left)
                {
                    P = P->parent;
                    PosRBTRightRot(RBT, P);
                }
                P->parent->color = BLACK;
                gp->color = RED;
                PosRBTLeftRot(RBT, gp);
            }
        }
    }
    RBT->root->color = BLACK;
}

AlphaRBTNode *Insert_AlphaRBT(AlphaRBT *RBT, char *word){
    AlphaRBTNode *parent = &ALPHA_NIL;
    AlphaRBTNode *node = RBT->root;
    while (node != &ALPHA_NIL && strcmp(word, node->word) != 0)
    {
        parent = node;
        if (strcmp(word, node->word) < 0)
        {
            node = node->left;
        }
        else
        {
            node = node->right;
        }
    }

    if (node != &ALPHA_NIL)
    {
        return node;
    }

    AlphaRBTNode *P = allocateAlphaRBTNode(RBT);
    P->word = strdup(word);
    P->parent = parent;

    if (parent == &ALPHA_NIL)
    {
        RBT->root = P;
    }
    else
    {
        if (strcmp(word, parent->word) < 0)
        {
            parent->left = P;
        }
        else
        {
            parent->right = P;
        }
    }
    AlphaRBTfix(RBT, P);
    return P;
}

PosRBTNode* Insert_PosRBT(PosRBT* RBT) {
    PosRBTNode* P = allocatePosRBTNode(RBT);
    P->parent     = RBT->max; 

    if (RBT->max == &POS_NIL) {
        P->position = 1;
        RBT->root   = P;
    } else {
        P->position      = RBT->max->position + 1;
        RBT->max->right  = P;
    }

    PosRBTfix(RBT, P);
    RBT->max = P;  
    return P;
}

void Print_PosRBT(PosRBT *RBT, PosRBTNode *node){
    // Base case: Stop when we hit the sentinel 'nil' node
    if (node != &POS_NIL)
    {
        Print_PosRBT(RBT, node->left);

        // Print the word by following the pointer to the Alpha tree!
        printf("%s ", node->word_ref->word);

        Print_PosRBT(RBT, node->right);
    }
}

void Print_AlphaRBT(AlphaRBTNode* node) {
    if (node == &ALPHA_NIL){
        return;
    }
    Print_AlphaRBT(node->left);

    printf("  - %s (Appears at positions: ", node->word);
    PosNode* cur = node->pos_list;
    if (cur != NULL) {
        do {
            printf("%d", cur->pos_ref->position);
            cur = cur->next;
            if (cur != node->pos_list) printf(", ");
        } while (cur != node->pos_list);
    }
    printf(")\n");

    Print_AlphaRBT(node->right);
}

//petal_functions:
PetalNode *allocatePetalNode()
{
    PetalNode *Petal = malloc(sizeof(PetalNode));
    Petal->alpha_tree = NULL;
    Petal->pos_tree = NULL;
    Petal->next = Petal;
    Petal->prev = Petal;
    return Petal;
}

void Insert_word(PetalNode* P, char* word) {
    AlphaRBTNode* alphanode  = Insert_AlphaRBT(P->alpha_tree, word);
    PosRBTNode*   posnode    = Insert_PosRBT(P->pos_tree);
    posnode->word_ref        = alphanode;
    poslist_insert(&alphanode->pos_list, posnode);
}
//text_processing_functions:
int is_delimiter(char c) {
    return (c == ' '  || c == '\t' || c == '\n' ||
            c == '.'  || c == ','  || c == '?'  ||
            c == '!'  || c == '"'  || c == '\'' ||
            c == ';'  || c == ':'  || c == '-');
}

int is_paragraph_break(char* str, int i) {
    return (str[i] == '\n' && str[i + 1] == '\n');
}

int is_end_of_string(char* str, int i) {
    return (str[i] == '\0');
}

int skip_delimiters(char* str, int i, int* newpara) {
    while (str[i] != '\0' && is_delimiter(str[i])) {
        if (is_paragraph_break(str, i)) *newpara = 1;
        i++;
    }
    return i;
}

int collect_word(char* str, int i) {
    while (str[i] != '\0' && !is_delimiter(str[i])) {
        i++;
    }
    return i;
}

void detect_newpara(char* str, int i, char** next, int* newpara) {
    if (is_end_of_string(str, i)) {
        /* last word in the entire text */
        *newpara = 1;
        *next    = NULL;
    } else {
        *newpara = is_paragraph_break(str, i) ? 1 : 0;
        str[i]   = '\0';
        *next    = str + i + 1;
    }
}

char* separate_words(char** str, int* newpara) {
    if (str == NULL || *str == NULL || **str == '\0') return NULL;

    char* cur = *str;
    int   i   = 0;

    /* Step 1 — skip leading delimiters */
    i = skip_delimiters(cur, i, newpara);

    /* Step 2 — nothing left after delimiters */
    if (is_end_of_string(cur, i)) {
        *str = NULL;
        return NULL;
    }

    /* Step 3 — mark start of word */
    char* start = cur + i;

    /* Step 4 — collect word characters */
    i = collect_word(cur, i);

    /* Step 5 — detect paragraph break and advance str */
    detect_newpara(cur, i, str, newpara);

    return start;
}

//Rose_fucntions:
Rose *AllocateRose(){
    Rose *R = malloc(sizeof(Rose));
    R->petals = NULL;
    R->size = 0;
    return R;
}

void append_petal(Rose* rose){
    PetalNode* P = allocatePetalNode();
    if (rose->petals == NULL) {
        rose->petals = P;
        P->next      = P;
        P->prev      = P;
    } else {
        PetalNode* tail    = rose->petals->prev;
        tail->next         = P;
        P->prev            = tail;
        P->next            = rose->petals;
        rose->petals->prev = P;
    }
    rose->size++;
}

Rose* text_to_rose(char* text) {
    if (text == NULL) return NULL;

    Rose* rose    = AllocateRose();
    char* copy    = strdup(text);
    char* tracker = copy;
    int   newpara = 0;

    append_petal(rose);                         /* create first petal         */
    char* word = next_word(&tracker, &newpara); /* fetch first word           */

    while (word != NULL) {
        Insert_word(rose->petals->prev, word);  /* insert into current petal  */
        if (newpara == 1) {                     /* current word was last in   */
            word = next_word(&tracker, &newpara);/* its paragraph             */
            if (word != NULL) append_petal(rose);/* new petal only if needed  */
        } else {
            word = next_word(&tracker, &newpara);
        }
    }

    free(copy);
    return rose;
}


// ==========================================
// 1. Traverse the Positional Tree (Reconstructs Paragraph)
// ==========================================

// ==========================================
// 2. Traverse the Alphabetical Tree & DLL
// ==========================================

// ==========================================
// 3. Traverse a Single Petal
// ==========================================
void Print_Petal(PetalNode *P, int petal_number){
    printf("\n========================================\n");
    printf(" PETAL (PARAGRAPH) #%d\n", petal_number);
    printf("========================================\n");

    printf("\n[Reconstructed Text from Positional Tree]:\n> ");
    if (P->pos_tree != NULL)
    {
        Print_PosRBT(P->pos_tree, P->pos_tree->root);
    }
    printf("\n");

    printf("\n[Alphabetical Index from Alpha Tree]:\n");
    if (P->alpha_tree != NULL)
    {
        Print_AlphaRBT(P->alpha_tree->root);
    }
}

// ==========================================
// 4. Traverse the Entire Rose
// ==========================================

int main()
{
    // 1. Define the sample text as ONE giant continuous string literal.
    // Notice it contains the "\n\t" formatting seamlessly in the middle.
    char sample_text[] = "This is the first paragraph. It has some words, and some repeated words like this and this.\n\tHere is the second paragraph. The second paragraph is short.\n\tFinally, the third paragraph. Welcome to the Rose engine.\n";

    printf("========================================\n");
    printf(" ORIGINAL TEXT INGESTION\n");
    printf("========================================\n");

    // This will print the text with all the line breaks and tabs applied
    printf("%s\n", sample_text);

    // 2. Build the Rose structure
    printf("Building the Rose data structure...\n");
    Rose *my_rose = Text_to_Rose(sample_text);

    // 3. Traverse and print the Rose
    if (my_rose != NULL)
    {
        Print_Rose(my_rose);
    }
    else
    {
        printf("Error: The Rose engine returned a NULL structure.\n");
    }

    return 0;
}

