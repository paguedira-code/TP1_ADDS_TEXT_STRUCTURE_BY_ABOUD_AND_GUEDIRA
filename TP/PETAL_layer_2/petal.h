#ifndef PETAL_H
#define PETAL_H
#include "../structures.h"
#include "../RBT__layer_1/RBT.h"
#include "../pos_list_layer_0/pos_list.h"

PetalNode* allocatePetalNode         (void);
void       petal_free                (PetalNode* p);
void       Insert_word               (PetalNode* p, char* w, int sflag);
void       petal_print_by_position   (PetalNode* p);
void       petal_print_by_alpha      (PetalNode* p);
char*      petal_word_at_pos         (PetalNode* p, int pos);
int        petal_pos_exists          (PetalNode* p, int pos);
int        petal_word_count          (PetalNode* p);
int        petal_sentence_count      (PetalNode* p);
int        petal_get_sentence_start  (PetalNode* p, int n);
PetalNode* petal_extract_sentence    (PetalNode* p, int n);
void       petal_print_sentences     (PetalNode* p);
PetalNode* petal_extract_range       (PetalNode* p, int i, int j);
void       petal_print_range         (PetalNode* p, int i, int j);
void       petal_context_of_word     (PetalNode* p, const char* w, int k);
int        petal_unique_word_count   (PetalNode* p);
char*      petal_most_frequent_word  (PetalNode* p);
char*      petal_least_frequent_word (PetalNode* p);
void       petal_print_frequency_table(PetalNode* p);
void       petal_print_stats         (PetalNode* p);

#endif