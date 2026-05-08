#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED   1
#define BLACK 0

typedef struct PosRBTNode   PosRBTNode;
typedef struct AlphaRBTNode AlphaRBTNode;
typedef struct PosNode      PosNode;
typedef struct PetalNode    PetalNode;

struct PosNode {
    PosRBTNode* pos_ref;
    PosNode*    next;
    PosNode*    prev;
};

struct AlphaRBTNode {
    char*         word;
    PosNode*      pos_list;
    int           color;
    int           rep;
    AlphaRBTNode* parent;
    AlphaRBTNode* left;
    AlphaRBTNode* right;
};

struct PosRBTNode {
    int           position;
    AlphaRBTNode* word_ref;
    int           color;
    PosRBTNode*   parent;
    PosRBTNode*   left;
    PosRBTNode*   right;
    int           sent_flag;
};

typedef struct {
    AlphaRBTNode* root;
} AlphaRBT;

typedef struct {
    PosRBTNode* root;
    PosRBTNode* max;
} PosRBT;

struct PetalNode {
    AlphaRBT*  alpha_tree;
    PosRBT*    pos_tree;
    PetalNode* next;
    PetalNode* prev;
};

typedef struct Rose {
    PetalNode* petals;
    int        size;
    char       nom[256];
} Rose;

typedef struct SentenceEntry {
    unsigned int          hash;
    PosRBTNode*           start;
    struct SentenceEntry* next;
} SentenceEntry;

extern AlphaRBTNode ALPHA_NIL;
extern PosRBTNode   POS_NIL;

#endif