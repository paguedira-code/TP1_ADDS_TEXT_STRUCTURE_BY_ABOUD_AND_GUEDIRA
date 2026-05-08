#include "pos_list.h"

PosNode* poslist_create_node(PosRBTNode* ref) {
    PosNode* n = malloc(sizeof(PosNode));
    if (n == NULL){ 
        printf("erreur malloc PosNode\n"); return NULL; 
    }
    n->pos_ref = ref;
    n->next = n;
    n->prev = n;
    return n;
}

void poslist_insert(PosNode** head, PosRBTNode* ref) {
    PosNode* n = poslist_create_node(ref);
    if (*head == NULL) {
        *head = n;
        return;
    }
    PosNode* last = (*head)->prev;
    last->next = n;
    n->prev = last;
    n->next = *head;
    (*head)->prev = n;
}

void poslist_free(PosNode* head) {
    if (head == NULL){
        return;
    }
    PosNode* c = head;
    do {
        PosNode* tmp = c;
        c = c->next;
        free(tmp);
    } while (c != head);
}

PosNode* poslist_copy(PosNode* head) {
    if(head == NULL){
        return NULL;
    }
    PosNode* res = NULL;
    PosNode* c = head;
    do {
        poslist_insert(&res, c->pos_ref);
        c = c->next;
    } while (c != head);
    return res;
}

void poslist_print(PosNode* head) {
    if (head == NULL){
        printf("[]");
        return; 
    }
    printf("[");
    PosNode* c = head;
    do {
        printf("%d", c->pos_ref->position);
        if (c->next != head) printf(" <-> ");
        c = c->next;
    } while (c != head);
    printf("]");
}
