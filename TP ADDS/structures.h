#ifndef STRUCTURES_H
#define STRUCTURES_H

/* ============================================================
   COLOR DEFINITIONS (Red-Black Tree)
   ============================================================ */
#define RED 1
#define BLACK 0

/* ============================================================
   FORWARD DECLARATIONS
   ============================================================ */
typedef struct PosRBTNode PosRBTNode;
typedef struct AlphaRBTNode AlphaRBTNode;
typedef struct PosNode PosNode;
typedef struct PetalNode PetalNode;

/* ============================================================
   LAYER 0 — Position List Node (DLL)
   ============================================================ */
struct PosNode
{
   PosRBTNode *pos_ref; /* pointer to node in Pos-RBT */
   PosNode *next;       /* next in sorted order       */
   PosNode *prev;       /* previous node (DLL)        */
};

/* ============================================================
   LAYER 1 — Alpha-RBT Node (word-based)
   ============================================================ */
struct AlphaRBTNode
{
   char *word; /* heap-allocated string       */
   PosNode *pos_list_head;
   PosNode *pos_list_tail; /* DLL of position references  */
   int color;              /* RED (1) or BLACK (0)        */
   AlphaRBTNode *parent;
   AlphaRBTNode *left;
   AlphaRBTNode *right;
};

/* ============================================================
   LAYER 1 — Pos-RBT Node (position-based)
   ============================================================ */
struct PosRBTNode
{
   int position;           /* position in paragraph       */
   AlphaRBTNode *word_ref; /* pointer to word node        */
   int color;              /* RED (1) or BLACK (0)        */
   PosRBTNode *parent;
   PosRBTNode *left;
   PosRBTNode *right;
};

/* ============================================================
   LAYER 1 — RBT WRAPPERS
   ============================================================ */
typedef struct
{
   AlphaRBTNode *root;
   AlphaRBTNode *nil; /* sentinel node */
} AlphaRBT;

typedef struct
{
   PosRBTNode *root;
   PosRBTNode *nil; /* sentinel node */
} PosRBT;

/* ============================================================
   LAYER 2 — Petal (paragraph)
   ============================================================ */
struct PetalNode
{
   AlphaRBT *alpha_tree; /* word index     */
   PosRBT *pos_tree;     /* position index */
   PetalNode *next;      /* circular list  */
   PetalNode *prev;
};

/* ============================================================
   LAYER 3 — Rose (collection of paragraphs)
   ============================================================ */
typedef struct Rose
{
   PetalNode *head;
   PetalNode *tail;
   int size;
} Rose;

#endif /* STRUCTURES_H */

