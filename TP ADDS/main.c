#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
//NIL definition for RBTs:
AlphaRBTNode ALPHA_NIL = { NULL, NULL, 1, &ALPHA_NIL, &ALPHA_NIL, &ALPHA_NIL };
PosRBTNode   POS_NIL   = { NULL,    NULL, 1, &POS_NIL,   &POS_NIL,   &POS_NIL   };

Rose *AllocateRose()
{
    Rose *R = malloc(sizeof(Rose));
    R->head = NULL;
    R->tail = NULL;
    R->size = 0;
    return R;
}
char *seperate_words(char **str, int *newpara)
{
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
PosNode *allocatePosNode()
{
    PosNode *P = malloc(sizeof(PosNode));
    P->next = P;
    P->prev = P;
    return P;
}
AlphaRBT *AlocateAlphaRBT()
{
    AlphaRBT *tree = malloc(sizeof(AlphaRBT));
    tree->root = &ALPHA_NIL;
    return tree;
}
PosRBT *AlocatePosRBT()
{
    PosRBT *tree = malloc(sizeof(PosRBT));
    tree->root = &POS_NIL;
    return tree;
}
PetalNode *allocatePetalNode()
{
    PetalNode *Petal = malloc(sizeof(PetalNode));
    Petal->alpha_tree = NULL;
    Petal->pos_tree = NULL;
    Petal->next = Petal;
    Petal->prev = Petal;
    return Petal;
}
AlphaRBTNode *allocateAlphaRBTNode(AlphaRBT *RBT)
{
    AlphaRBTNode *P = malloc(sizeof(AlphaRBTNode));
    if (P == NULL)
    {
        printf("failed alocation");
        return &ALPHA_NIL;
    }
    P->parent = &ALPHA_NIL;
    P->left = &ALPHA_NIL;
    P->right = &ALPHA_NIL;
    P->pos_list_head = NULL;
    P->pos_list_tail = NULL;
    return P;
}

PosRBTNode *allocatePosRBTNode(PosRBT *RBT)
{
    PosRBTNode *P = malloc(sizeof(PosRBTNode));
    if (P == NULL)
    {
        printf("failed alocation");
        return &POS_NIL;
    }
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

void PosRBTfix(PosRBT *RBT, PosRBTNode *P)
{
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
    P->color = RED;
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

PosRBTNode *Insert_PosRBT(PosRBT *RBT)
{
    PosRBTNode *node = RBT->root;
    PosRBTNode *parent = &POS_NIL;
    PosRBTNode *P = allocatePosRBTNode(RBT);
    P->color = RED;

    if (node != &POS_NIL)
    {
        while (node->right != &POS_NIL)
        {
            node = node->right;
        }
        parent = node;
    }

    P->parent = parent;

    if (parent == &POS_NIL)
    {
        P->position = 1;
        RBT->root = P;
    }
    else
    {
        P->position = parent->position + 1;
        parent->right = P;
    }
    PosRBTfix(RBT, P);
    return P;
}

void Insert_word(PetalNode *P, char *word)
{
    if (P->pos_tree == NULL || P->alpha_tree == NULL)
    {
        P->pos_tree = AlocatePosRBT();
        P->alpha_tree = AlocateAlphaRBT();
    }
    AlphaRBTNode *alphanode = Insert_AlphaRBT(P->alpha_tree, word);
    PosRBTNode *posnode = Insert_PosRBT(P->pos_tree);
    posnode->word_ref = alphanode;
    PosNode *node = allocatePosNode();
    node->pos_ref = posnode;
    if (alphanode->pos_list_head == NULL)
    {
        alphanode->pos_list_head = node;
        alphanode->pos_list_tail = node;
    }
    else
    {
        alphanode->pos_list_tail->next = node;
        node->prev = alphanode->pos_list_tail;
        node->next = alphanode->pos_list_head;
        alphanode->pos_list_head->prev = node;
        alphanode->pos_list_tail = node;
    }
}
Rose *Text_to_Rose(char *Text)
{
    if (Text == NULL)
        return NULL;

    int newpara = 0;
    Rose *ROSE = AllocateRose();

    char *copy = strdup(Text);
    char *tracker = copy; // WE USE A TRACKER SO 'COPY' DOESN'T GET LOST

    char *word = seperate_words(&tracker, &newpara);
    PetalNode *P = NULL;

    newpara = 1;
    while (word != NULL)
    {
        if (newpara == 1)
        {
            P = allocatePetalNode();
            if (ROSE->head == NULL)
            {
                ROSE->head = P;
                ROSE->tail = P;
            }
            else
            {
                ROSE->tail->next = P;
                P->prev = ROSE->tail;
                ROSE->head->prev = P;
                P->next = ROSE->head;
                ROSE->tail = P;
            }
            ROSE->size++;

            Insert_word(P, word);
        }
        else
        {
            Insert_word(ROSE->tail, word);
        }

        // Use tracker, not copy!
        word = seperate_words(&tracker, &newpara);
    }

    free(copy);
    return ROSE;
}

// ==========================================
// 1. Traverse the Positional Tree (Reconstructs Paragraph)
// ==========================================
void Print_PosRBT(PosRBT *RBT, PosRBTNode *node)
{
    // Base case: Stop when we hit the sentinel 'nil' node
    if (node != &POS_NIL)
    {
        Print_PosRBT(RBT, node->left);

        // Print the word by following the pointer to the Alpha tree!
        printf("%s ", node->word_ref->word);

        Print_PosRBT(RBT, node->right);
    }
}

// ==========================================
// 2. Traverse the Alphabetical Tree & DLL
// ==========================================
void Print_AlphaRBT(AlphaRBT *RBT, AlphaRBTNode *node)
{
    if (node != &ALPHA_NIL)
    {
        Print_AlphaRBT(RBT, node->left);

        // Print the alphabetically sorted word
        printf("  - %s (Appears at positions: ", node->word);

        // Loop through the Circular Doubly Linked List of positions
        PosNode *curr = node->pos_list_head;
        if (curr != NULL)
        {
            do
            {
                printf("%d", curr->pos_ref->position);
                curr = curr->next;
                if (curr != node->pos_list_head)
                {
                    printf(", ");
                }
            } while (curr != node->pos_list_head);
        }
        printf(")\n");

        Print_AlphaRBT(RBT, node->right);
    }
}

// ==========================================
// 3. Traverse a Single Petal
// ==========================================
void Print_Petal(PetalNode *P, int petal_number)
{
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
        Print_AlphaRBT(P->alpha_tree, P->alpha_tree->root);
    }
}

// ==========================================
// 4. Traverse the Entire Rose
// ==========================================
void Print_Rose(Rose *ROSE)
{
    if (ROSE == NULL || ROSE->head == NULL)
    {
        printf("The Rose is empty.\n");
        return;
    }

    printf("\n--- BEGINNING ROSE TRAVERSAL ---\n");
    printf("Total Petals: %d\n", ROSE->size);

    PetalNode *curr = ROSE->head;
    int counter = 1;

    // We use a do-while loop because the Petal list is circular!
    do
    {
        Print_Petal(curr, counter);
        curr = curr->next;
        counter++;
    } while (curr != ROSE->head); // Stop when we loop back to the start

    printf("\n--- END OF ROSE TRAVERSAL ---\n");
}

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
