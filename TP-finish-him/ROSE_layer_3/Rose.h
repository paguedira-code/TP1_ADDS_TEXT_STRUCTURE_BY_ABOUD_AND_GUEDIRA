#ifndef ROSE_H
#define ROSE_H
#include "../structures.h"
#include "../PETAL_layer_2/petal.h"
#include "../set_ops/set_ops.h"
#include "../text_processing/text.h"

Rose*      AllocateRose                      (void);
void       rose_free                         (Rose* r);
void       append_petal                      (Rose* r);
PetalNode* rose_get_petal                    (Rose* r, int i);
Rose*      file_to_rose                      (const char* fname);
int        rose_total_word_count             (Rose* r);
char*      rose_word_at_global_pos           (Rose* r, int gpos);
void       rose_locate_global_pos            (Rose* r, int gpos);
PetalNode* rose_longest_petal                (Rose* r);
PetalNode* rose_most_sentences               (Rose* r);
void       rose_global_stats                 (Rose* r);
int        rose_word_count                   (Rose* r, int i);
int        rose_sentence_count               (Rose* r, int i);
char*      rose_word_at_pos                  (Rose* r, int i, int pos);
int        rose_pos_exists                   (Rose* r, int i, int pos);
PetalNode* rose_extract_sentence             (Rose* r, int i, int n);
void       rose_print_sentences              (Rose* r, int i);
PetalNode* rose_extract_range                (Rose* r, int p, int i, int j);
void       rose_print_range                  (Rose* r, int p, int i, int j);
void       rose_context_of_word              (Rose* r, int p, const char* w, int k);
char*      rose_most_frequent_word           (Rose* r, int i);
char*      rose_least_frequent_word          (Rose* r, int i);
void       rose_print_frequency_table        (Rose* r, int i);
void       rose_print_stats                  (Rose* r, int i);

/* word based rose ops */
PetalNode* rose_union_petals                 (Rose* r, int i, int j);
PetalNode* rose_intersect_petals             (Rose* r, int i, int j);
PetalNode* rose_diff_petals                  (Rose* r, int i, int j);
PetalNode* rose_sym_diff_petals              (Rose* r, int i, int j);
int        rose_is_subset_petals             (Rose* r, int i, int j);
int        rose_is_identical_petals          (Rose* r, int i, int j);

/* cross rose word ops */
PetalNode* rose_cross_union                  (Rose* r1, int i, Rose* r2, int j);
PetalNode* rose_cross_intersection           (Rose* r1, int i, Rose* r2, int j);
PetalNode* rose_cross_difference             (Rose* r1, int i, Rose* r2, int j);
PetalNode* rose_cross_sym_diff               (Rose* r1, int i, Rose* r2, int j);
int        rose_cross_is_subset              (Rose* r1, int i, Rose* r2, int j);
int        rose_cross_is_identical           (Rose* r1, int i, Rose* r2, int j);

/* sentence based rose ops */
PetalNode* rose_sentence_union               (Rose* r1, int i, Rose* r2, int j);
PetalNode* rose_sentence_intersection        (Rose* r1, int i, Rose* r2, int j);
PetalNode* rose_sentence_difference          (Rose* r1, int i, Rose* r2, int j);
PetalNode* rose_sentence_symmetric_difference(Rose* r1, int i, Rose* r2, int j);
int        rose_sentence_is_subset           (Rose* r1, int i, Rose* r2, int j);
int        rose_sentence_is_identical        (Rose* r1, int i, Rose* r2, int j);

#endif