#ifndef SET_OPS_H
#define SET_OPS_H
#include "../structures.h"
#include "../PETAL_layer_2/petal.h"
#include "../RBT__layer_1/RBT.h"

/* word based */
PetalNode* petal_union                       (PetalNode* a, PetalNode* b);
PetalNode* petal_intersection                (PetalNode* a, PetalNode* b);
PetalNode* petal_difference                  (PetalNode* a, PetalNode* b);
PetalNode* petal_symmetric_difference        (PetalNode* a, PetalNode* b);
int        petal_is_subset_of                (PetalNode* a, PetalNode* b);
int        petal_is_identical                (PetalNode* a, PetalNode* b);
int        is_substring_pos                  (PetalNode* a, PetalNode* b);

/* sentence based */
PetalNode* petal_sentence_union              (PetalNode* P1, PetalNode* P2);
PetalNode* petal_sentence_intersection       (PetalNode* P1, PetalNode* P2);
PetalNode* petal_sentence_difference         (PetalNode* P1, PetalNode* P2);
PetalNode* petal_sentence_symmetric_difference(PetalNode* P1, PetalNode* P2);
int        petal_sentence_is_subset          (PetalNode* P1, PetalNode* P2);
int        petal_sentence_is_identical       (PetalNode* P1, PetalNode* P2);



#endif