#ifndef RBT_H
#define RBT_H
#include "../structures.h"
#include "../pos_list_layer_0/pos_list.h"

extern AlphaRBTNode ALPHA_NIL;
extern PosRBTNode   POS_NIL;

AlphaRBT*     AlocateAlphaRBT      (void);
PosRBT*       AlocatePosRBT        (void);
AlphaRBTNode* allocateAlphaRBTNode (void);
PosRBTNode*   allocatePosRBTNode   (void);
AlphaRBTNode* Insert_AlphaRBT      (AlphaRBT* T, char* w);
PosRBTNode*   Insert_PosRBT        (PosRBT* T);
AlphaRBTNode* alpha_inorder_first  (AlphaRBT* T);
AlphaRBTNode* alpha_inorder_next   (AlphaRBTNode* nd);
PosRBTNode*   pos_inorder_first    (PosRBT* T);
PosRBTNode*   pos_inorder_next     (PosRBTNode* nd);
void          pos_rbt_free         (PosRBTNode* nd);
void          alpha_rbt_free       (AlphaRBTNode* nd);
void          Print_PosRBT         (PosRBT* T, PosRBTNode* nd);
void          Print_AlphaRBT       (AlphaRBTNode* nd);

#endif