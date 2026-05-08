#ifndef POS_LIST_H
#define POS_LIST_H
#include "../structures.h"

PosNode* poslist_create_node (PosRBTNode* ref);
void     poslist_insert      (PosNode** head, PosRBTNode* ref);
void     poslist_free        (PosNode* head);
PosNode* poslist_copy        (PosNode* head);
void     poslist_print       (PosNode* head);

#endif