#include "rose.h"

Rose* AllocateRose() {
    Rose* r  = malloc(sizeof(Rose));
    r->petals = NULL;
    r->size   = 0;
    return r;
}

void append_petal(Rose* r) {
    PetalNode* p = allocatePetalNode();
    if (r->petals == NULL) {
        r->petals = p;
        p->next = p;
        p->prev = p;
    } else {
        PetalNode* tail = r->petals->prev;
        tail->next      = p;
        p->prev         = tail;
        p->next         = r->petals;
        r->petals->prev = p;
    }
    r->size++;
}

void rose_free(Rose* r) {
    if (!r) return;
    if (!r->petals) { free(r); return; }
    PetalNode* c = r->petals;
    PetalNode* nx;
    do {
        nx = c->next;
        petal_free(c);
        c = nx;
    } while (c != r->petals);
    free(r);
}

PetalNode* rose_get_petal(Rose* r, int i) {
    if (!r->petals || i < 0 || i >= r->size) return NULL;
    PetalNode* c = r->petals;
    for (int j = 0; j < i; j++) c = c->next;
    return c;
}

char *file_to_string(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("The file %s doesn't exist\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    
    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    fclose(file);
    return buffer;
}


Rose *File_to_Rose(const char *filename) {
    char *raw_text = file_to_string(filename);
    if (raw_text == NULL) return NULL;

    Rose *ROSE = AllocateRose();
    int newpara = 1; 
    int sf = 0;      
    
    char *copy = strdup(raw_text);
    char *tracker = copy;
    
    lowercase_string(copy);

    char *word = separate_words(&tracker, &newpara, &sf);
    
    while (word != NULL) {
        if (newpara == 1) {
            append_petal(ROSE);
            Insert_word(ROSE->petals, word, 1);
            newpara = 0; 
        } else {
            Insert_word(ROSE->petals, word, sf);
        }

        word = separate_words(&tracker, &newpara, &sf);
    }

    free(copy);
    free(raw_text);
    
    return ROSE;
}

int rose_sentence_count(Rose* r, int i) {
    PetalNode* p = rose_get_petal(r, i);
    return p ? petal_sentence_count(p) : 0;
}

PetalNode* rose_extract_sentence(Rose* r, int i, int n) {
    PetalNode* p = rose_get_petal(r, i);
    return p ? petal_extract_sentence(p, n) : NULL;
}

void rose_print_sentences(Rose* r, int i) {
    PetalNode* p = rose_get_petal(r, i);
    if (!p) { printf("  index invalide\n"); return; }
    petal_print_sentences(p);
}

char* rose_word_at_pos(Rose* r, int i, int pos) {
    PetalNode* p = rose_get_petal(r, i);
    return p ? petal_word_at_pos(p, pos) : NULL;
}

int rose_pos_exists(Rose* r, int i, int pos) {
    PetalNode* p = rose_get_petal(r, i);
    return p ? petal_pos_exists(p, pos) : 0;
}

int rose_word_count(Rose* r, int i) {
    PetalNode* p = rose_get_petal(r, i);
    return p ? petal_word_count(p) : 0;
}

PetalNode* rose_extract_range(Rose* r, int p, int i, int j) {
    PetalNode* pt = rose_get_petal(r, p);
    return pt ? petal_extract_range(pt, i, j) : NULL;
}

void rose_print_range(Rose* r, int p, int i, int j) {
    PetalNode* pt = rose_get_petal(r, p);
    if (!pt) { printf("  index invalide\n"); return; }
    petal_print_range(pt, i, j);
}

void rose_context_of_word(Rose* r, int p, const char* w, int k) {
    PetalNode* pt = rose_get_petal(r, p);
    if (!pt) { printf("  index invalide\n"); return; }
    petal_context_of_word(pt, w, k);
}

char* rose_most_frequent_word(Rose* r, int i) {
    PetalNode* p = rose_get_petal(r, i);
    return p ? petal_most_frequent_word(p) : NULL;
}

char* rose_least_frequent_word(Rose* r, int i) {
    PetalNode* p = rose_get_petal(r, i);
    return p ? petal_least_frequent_word(p) : NULL;
}

void rose_print_frequency_table(Rose* r, int i) {
    PetalNode* p = rose_get_petal(r, i);
    if (!p) { printf("  index invalide\n"); return; }
    petal_print_frequency_table(p);
}

void rose_print_stats(Rose* r, int i) {
    PetalNode* p = rose_get_petal(r, i);
    if (!p) { printf("  index invalide\n"); return; }
    petal_print_stats(p);
}

PetalNode* rose_longest_petal(Rose* r) {
    if (!r->petals) return NULL;
    PetalNode* c = r->petals, *best = c;
    do {
        if (petal_word_count(c) > petal_word_count(best)) best = c;
        c = c->next;
    } while (c != r->petals);
    return best;
}

PetalNode* rose_most_sentences(Rose* r) {
    if (!r->petals) return NULL;
    PetalNode* c = r->petals, *best = c;
    do {
        if (petal_sentence_count(c) > petal_sentence_count(best)) best = c;
        c = c->next;
    } while (c != r->petals);
    return best;
}

void rose_global_stats(Rose* r) {
    if (!r->petals) { printf("  rose vide\n"); return; }
    PetalNode* c = r->petals;
    int idx = 0;
    do {
        printf("\n  === Paragraphe %d ===\n", idx++);
        petal_print_stats(c);
        c = c->next;
    } while (c != r->petals);
}

char* rose_word_at_global_pos(Rose* r, int gpos) {
    if (!r->petals || gpos < 1) return NULL;
    PetalNode* c = r->petals;
    int off = gpos;
    do {
        int cnt = petal_word_count(c);
        if (off <= cnt) return petal_word_at_pos(c, off);
        off -= cnt;
        c = c->next;
    } while (c != r->petals);
    return NULL;
}

void rose_locate_global_pos(Rose* r, int gpos) {
    if (!r->petals || gpos < 1) { printf("  position invalide\n"); return; }
    PetalNode* c = r->petals;
    int off = gpos, idx = 0;
    do {
        int cnt = petal_word_count(c);
        if (off <= cnt) {
            char* w = petal_word_at_pos(c, off);
            printf("  pos globale %-4d → para %-3d | pos locale %-4d | mot: %s\n",
                   gpos, idx, off, w ? w : "NULL");
            return;
        }
        off -= cnt;
        c = c->next;
        idx++;
    } while (c != r->petals);
    printf("  position %d dépasse la taille du document\n", gpos);
}

int rose_total_word_count(Rose* r) {
    if (r->petals == NULL) {
        return 0;
    }
    PetalNode* c = r->petals;
    int tot = 0;
    do { tot += petal_word_count(c); c = c->next; } while (c != r->petals);
    return tot;
}


