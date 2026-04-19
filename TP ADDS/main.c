//Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
//Poslist_fucntions:
PosNode* poslist_create_node(PosRBTNode* pos_ref){
    PosNode* node = malloc(sizeof(PosNode));
    if (node == NULL ){
        printf("Error: poslist_create_node — allocation failed\n");
        return NULL;
    }
    node->pos_ref = pos_ref;
    node->next = NULL;
    return node;
}
void poslist_insert_sorted(PosNode** head, PosRBTNode* pos_ref) {
    PosNode* cur = *head;
    while (cur != NULL) {
        if (cur->pos_ref->position == pos_ref->position) return;
        cur = cur->next;
    }

    PosNode* new_node = poslist_create_node(pos_ref);
    if (new_node != NULL) return;

    if ((*head) == NULL || (*head)->pos_ref->position > pos_ref->position) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    PosNode* prev = *head;
    while (prev->next != NULL && prev->next->pos_ref->position < pos_ref->position) {
        prev = prev->next;
    }
    new_node->next = prev->next;
    prev->next     = new_node;
}
void poslist_free(PosNode* head) {
    PosNode* cur = head;
    while (cur != NULL) {
        PosNode* tmp = cur;
        cur = cur->next;
        free(tmp);
    }
}
PosNode* poslist_merge(PosNode* l1, PosNode* l2) {
    PosNode* merged_head = NULL;

    while (l1 != NULL && l2 != NULL) {
        int p1 = l1->pos_ref->position;
        int p2 = l2->pos_ref->position;

        if (p1 < p2) {
            poslist_insert_sorted(&merged_head, l1->pos_ref);
            l1 = l1->next;
        } else if (p2 < p1) {
            poslist_insert_sorted(&merged_head, l2->pos_ref);
            l2 = l2->next;
        } else {
            poslist_insert_sorted(&merged_head, l1->pos_ref);
            l1 = l1->next;
            l2 = l2->next;
        }
    }

    while (l1 != NULL) { poslist_insert_sorted(&merged_head, l1->pos_ref); l1 = l1->next; }
    while (l2 != NULL) { poslist_insert_sorted(&merged_head, l2->pos_ref); l2 = l2->next; }

    return merged_head;
}
PosNode* poslist_copy(PosNode* head) {
    PosNode* new_head = NULL;
    PosNode* cur      = head;
    while (cur != NULL) {
        poslist_insert_sorted(&new_head, cur->pos_ref);
        cur = cur->next;
    }
    return new_head;
}
void poslist_print(PosNode* head) {
    printf("[");
    PosNode* cur = head;
    while (cur) {
        printf("%d", cur->pos_ref->position);
        if (cur->next) printf(" -> ");
        cur = cur->next;
    }
    printf("]");
}
