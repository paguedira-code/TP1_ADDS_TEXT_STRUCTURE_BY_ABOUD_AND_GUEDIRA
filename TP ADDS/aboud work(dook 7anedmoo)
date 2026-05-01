AlphaRBTNode *search_word(char *word, AlphaRBT *RBT)
{
    AlphaRBTNode *ptr = RBT->root;
    while (ptr != &ALPHA_NIL && strcmp(word, ptr->word) != 0)
    {
        if (strcmp(word, ptr->word) < 0)
        {
            ptr = ptr->left;
        }
        else
        {
            ptr = ptr->right;
        }
    }

    return ptr;
    return &ALPHA_NIL;
}

AlphaRBTNode *nextinorder(AlphaRBTNode *ptr)
{
    if (ptr == &ALPHA_NIL)
    {
        return &ALPHA_NIL;
    }
    if (ptr->right != &ALPHA_NIL)
    {
        ptr = ptr->right;
        while (ptr->left != &ALPHA_NIL)
        {
            ptr = ptr->left;
        }
        return ptr;
    }

    while (ptr->parent != &ALPHA_NIL && ptr == ptr->parent->right)
    {
        ptr = ptr->parent;
    }
    return ptr->parent;
}

PetalNode *intersection_P(PetalNode *pet1, PetalNode *pet2)
{
    if (pet1->alpha_tree->root == &ALPHA_NIL || pet2->alpha_tree->root == &ALPHA_NIL)
    {
        return NULL;
    }
    PetalNode *pet = allocatePetalNode();
    AlphaRBTNode *ptr1 = pet1->alpha_tree->root;
    while (ptr1->left != &ALPHA_NIL)
    {
        ptr1 = ptr1->left;
    }
    AlphaRBTNode *ptr2 = pet2->alpha_tree->root;
    while (ptr2->left != &ALPHA_NIL)
    {
        ptr2 = ptr2->left;
    }
    while (ptr1 != &ALPHA_NIL && ptr2 != &ALPHA_NIL)
    {
        if (strcmp(ptr1->word, ptr2->word) == 0)
        {
            Insert_AlphaRBT(pet, ptr1->word, (ptr1->rep < ptr2->rep) ? ptr1->rep : ptr2->rep);
            ptr1 = nextinorder(ptr1);
            ptr2 = nextinorder(ptr2);
        }
        else
        {
            if (strcmp(ptr1->word, ptr2->word) < 0)
            {
                ptr1 = nextinorder(ptr1);
            }
            else
            {
                ptr2 = nextinorder(ptr2);
            }
        }
    }
    return pet;
}

PetalNode *union_P(PetalNode *pet1, PetalNode *pet2)
{
    PetalNode *pet = allocatePetalNode();
    AlphaRBTNode *ptr1 = pet1->alpha_tree->root;
    while (ptr1 != &ALPHA_NIL && ptr1->left != &ALPHA_NIL)
    {
        ptr1 = ptr1->left;
    }
    AlphaRBTNode *ptr2 = pet2->alpha_tree->root;
    while (ptr2 != &ALPHA_NIL && ptr2->left != &ALPHA_NIL)
    {
        ptr2 = ptr2->left;
    }
    while (ptr1 != &ALPHA_NIL && ptr2 != &ALPHA_NIL)
    {
        if (strcmp(ptr1->word, ptr2->word) == 0)
        {
            Insert_AlphaRBT(pet, ptr1->word, (ptr1->rep > ptr2->rep) ? ptr1->rep : ptr2->rep);
            ptr1 = nextinorder(ptr1);
            ptr2 = nextinorder(ptr2);
        }
        else
        {
            if (strcmp(ptr1->word, ptr2->word) < 0)
            {
                Insert_AlphaRBT(pet, ptr1->word, ptr1->rep);
                ptr1 = nextinorder(ptr1);
            }
            else
            {
                Insert_AlphaRBT(pet, ptr2->word, ptr2->rep);
                ptr2 = nextinorder(ptr2);
            }
        }
    }
    while (ptr1 != &ALPHA_NIL)
    {
        Insert_word(pet, ptr1->word, ptr1->rep);
        ptr1 = nextinorder(ptr1);
    }
    while (ptr2 != &ALPHA_NIL)
    {
        Insert_word(pet, ptr2->word, ptr2->rep);
        ptr2 = nextinorder(ptr2);
    }
    return pet;
}

char *file_to_string(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("The file %s doesn't exist\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL)
    {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    fclose(file);
    return buffer;
}

PetalNode *difference(PetalNode *A, PetalNode *B)
{
    AlphaRBTNode *ptra = A->alpha_tree->root;
    AlphaRBTNode *ptrb = B->alpha_tree->root;
    if (ptra == &ALPHA_NIL)
    {
        return NULL;
    }
    if (ptrb == &ALPHA_NIL)
    {
        return A;
    }
    while (ptra->left != &ALPHA_NIL)
    {
        ptra = ptra->left;
    }
    while (ptrb->left != &ALPHA_NIL)
    {
        ptrb = ptrb->left;
    }
    int diff;
    PetalNode *diffAB = allocatePetalNode();
    while (ptra != NULL)
    {
        diff = strcmp(ptra->word, ptrb->word);
        if (diff == 0 && ptra->rep > ptrb->rep)
        {
            Insert_AlphaRBT(diffAB->alpha_tree, ptra->word, ptra->rep - ptrb->rep);
            ptra = nextinorder(ptra);
            ptrb = nextinorder(ptrb);
        }
        if (diff < 0)
        {
            Insert_AlphaRBT(diffAB->alpha_tree, ptra->word, ptra->rep);
            ptra = nextinorder(ptra);
        }
        if (diff > 0)
        {
            ptrb = nextinorder(ptrb);
        }
    }

    return diffAB;
}



int is_subset_pos(PetalNode *A, PetalNode *B)
{
    PosRBTNode *ptra = A->pos_tree->root;
    PosRBTNode *ptrb = B->pos_tree->root;
    if (ptra == &POS_NIL)
    {
        return 1;
    }
    if (ptrb == &POS_NIL)
    {
        return 0;
    }

    while (ptra->left != &POS_NIL)
    {
        ptra = ptra->left;
    }
    while (ptrb->left != &POS_NIL)
    {
        ptrb = ptrb->left;
    }
    PosRBTNode *refa = ptra;
    PosRBTNode *refb = ptrb;
    char *refw = ptra->word_ref->word;
    while (ptrb != &POS_NIL)
    {

        while (ptrb != &POS_NIL && strcmp(refw, ptrb->word_ref->word) != 0)
        {
            ptrb = nextinorderPos(ptrb);
        }
        if (ptrb == &POS_NIL)
        {
            return 0;
        }
        refb = nextinorderPos(ptrb);

        while (ptra != &POS_NIL && ptrb != &POS_NIL && strcmp(ptra->word_ref->word, ptrb->word_ref->word) != 0)
        {
            ptra = nextinorderPos(ptra);
            ptrb = nextinorderPos(ptrb);
        }
        if (ptra == &POS_NIL)
        {
            return 1;
        }
        if (ptrb == &POS_NIL)
        {
            return 0;
        }
        ptrb = refb;
        ptra = refa;
    }
    return 0;
}

int is_subset_nopos(PetalNode *A, PetalNode *B)
{
    AlphaRBTNode *ptra = A->alpha_tree->root;
    AlphaRBTNode *ptrb = B->alpha_tree->root;
    if (ptra == &ALPHA_NIL)
    {
        return 1;
    }
    if (ptrb == &ALPHA_NIL)
    {
        return 0;
    }

    while (ptra->left != &ALPHA_NIL)
    {
        ptra = ptra->left;
    }
    while (ptrb->left != &ALPHA_NIL)
    {
        ptrb = ptrb->left;
    }

    while (ptrb != &ALPHA_NIL)
    {
        if (ptra == &ALPHA_NIL)
        {
            return 1;
        }
        if (strcmp(ptra->word, ptrb->word) < 0)
        {
            return 0;
        }

        if (strcmp(ptra->word, ptrb->word) > 0)
        {
            ptrb = nextinorder(ptrb);
        }
        else
        {
            if (ptra->rep > ptrb->rep)
            {
                return 0;
            }
            ptra = nextinorder(ptra);
            ptrb = nextinorder(ptrb);
        }
    }
    return 0;
}
PetalNode * compliment(PetalNode* A,PetalNode* E){
    if(is_subset_nopos(A,E)==0){
        return NULL;
    }
    return difference(E,A);
}
