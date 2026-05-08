#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <conio.h>
    #define CLEAR_SCREEN() system("cls")
    #define GETCH() _getch()
#else
    #define CLEAR_SCREEN() system("clear")
    #define GETCH() getchar()
#endif

#define MAX_ROSES 10
#define MAX_FILENAME 256
#define MAX_WORD_LENGTH 100

// ============================================================================
// GLOBAL STATE MANAGEMENT
// ============================================================================

typedef struct {
    Rose* rose;
    char filename[MAX_FILENAME];
    char name[50];
} RoseInstance;

RoseInstance loaded_roses[MAX_ROSES];
int num_loaded_roses = 0;

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void flush_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_separator(int width) {
    for (int i = 0; i < width; i++) printf("─");
    printf("\n");
}

void print_header(const char* title) {
    printf("\n");
    print_separator(80);
    printf("  %s\n", title);
    print_separator(80);
    printf("\n");
}

void print_error(const char* message) {
    printf("\n  [ERROR] %s\n", message);
    printf("  Press Enter to continue...");
    GETCH();
}

void print_success(const char* message) {
    printf("\n  [✓] %s\n", message);
    printf("  Press Enter to continue...");
    GETCH();
}

void print_info(const char* message) {
    printf("\n  [i] %s\n", message);
}

// ============================================================================
// MENU SELECTION SYSTEM
// ============================================================================

int select_menu_option(const char** options, int num_options) {
    int selected = 0;
    int key;

    while (1) {
        CLEAR_SCREEN();
        printf("\n");
        print_separator(80);
        printf("  USE ARROW KEYS (↑/↓) or W/S to navigate, ENTER to select\n");
        print_separator(80);
        printf("\n");

        for (int i = 0; i < num_options; i++) {
            if (i == selected) {
                printf("  ► %s\n", options[i]);
            } else {
                printf("    %s\n", options[i]);
            }
        }

        printf("\n");
        key = GETCH();

        if (key == 224 || key == 27) {  // Arrow key prefix or escape
            key = GETCH();
            if (key == 72 || key == 'A') selected--;  // Up
            if (key == 80 || key == 'B') selected++;  // Down
        } else if (key == 'w' || key == 'W') {
            selected--;
        } else if (key == 's' || key == 'S') {
            selected++;
        } else if (key == '\r' || key == '\n') {
            return selected;
        }

        if (selected < 0) selected = num_options - 1;
        if (selected >= num_options) selected = 0;
    }
}

// ============================================================================
// ROSE MANAGEMENT
// ============================================================================

int find_rose_by_name(const char* name) {
    for (int i = 0; i < num_loaded_roses; i++) {
        if (strcmp(loaded_roses[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void load_file() {
    CLEAR_SCREEN();
    print_header("LOAD FILE");

    if (num_loaded_roses >= MAX_ROSES) {
        print_error("Maximum number of ROSES reached!");
        return;
    }

    char filename[MAX_FILENAME];
    char rose_name[50];

    printf("  Enter ROSE name: ");
    fgets(rose_name, sizeof(rose_name), stdin);
    rose_name[strcspn(rose_name, "\n")] = 0;

    if (find_rose_by_name(rose_name) != -1) {
        print_error("A ROSE with this name already exists!");
        return;
    }

    printf("  Enter file path (.txt): ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;

    // Try to load the file
    Rose* r = file_to_rose(filename);
    if (r == NULL) {
        print_error("Failed to load file or create ROSE!");
        return;
    }

    // Store the ROSE
    loaded_roses[num_loaded_roses].rose = r;
    strcpy(loaded_roses[num_loaded_roses].filename, filename);
    strcpy(loaded_roses[num_loaded_roses].name, rose_name);
    num_loaded_roses++;

    printf("\n  ROSE '%s' loaded successfully!\n", rose_name);
    printf("  Total petals (paragraphs): %d\n", r->size);
    printf("  Total words: %d\n\n", rose_total_word_count(r));
    printf("  Press Enter to continue...");
    GETCH();
}

// ============================================================================
// DISPLAY OPERATIONS
// ============================================================================

void display_full_rose() {
    CLEAR_SCREEN();
    print_header("DISPLAY FULL ROSE");

    if (num_loaded_roses == 0) {
        print_error("No ROSES loaded!");
        return;
    }

    // List available ROSES
    const char** rose_options = malloc((num_loaded_roses + 1) * sizeof(char*));
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_options[i] = loaded_roses[i].name;
    }
    rose_options[num_loaded_roses] = "BACK";

    int choice = select_menu_option(rose_options, num_loaded_roses + 1);
    free(rose_options);

    if (choice == num_loaded_roses) return;

    Rose* r = loaded_roses[choice].rose;

    CLEAR_SCREEN();
    print_header("ROSE: Full Display");

    printf("  [1] Display by Position (Original Order)\n");
    printf("  [2] Display by Alphabet\n");
    printf("  [3] BACK\n\n");

    printf("  Select: ");
    int display_choice;
    scanf("%d", &display_choice);
    flush_input_buffer();

    CLEAR_SCREEN();
    print_header("ROSE Content");

    if (display_choice == 1) {
        printf("  === DISPLAY BY POSITION ===\n\n");
        for (int i = 0; i < r->size; i++) {
            printf("  [PARAGRAPH %d]\n", i + 1);
            petal_print_by_position(r->petals[i]);
            printf("\n");
        }
    } else if (display_choice == 2) {
        printf("  === DISPLAY BY ALPHABET ===\n\n");
        for (int i = 0; i < r->size; i++) {
            printf("  [PARAGRAPH %d]\n", i + 1);
            petal_print_by_alpha(r->petals[i]);
            printf("\n");
        }
    }

    printf("  Press Enter to continue...");
    GETCH();
}

void display_single_paragraph() {
    CLEAR_SCREEN();
    print_header("DISPLAY SINGLE PARAGRAPH");

    if (num_loaded_roses == 0) {
        print_error("No ROSES loaded!");
        return;
    }

    // Select ROSE
    const char** rose_options = malloc((num_loaded_roses + 1) * sizeof(char*));
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_options[i] = loaded_roses[i].name;
    }
    rose_options[num_loaded_roses] = "BACK";

    int rose_choice = select_menu_option(rose_options, num_loaded_roses + 1);
    free(rose_options);

    if (rose_choice == num_loaded_roses) return;

    Rose* r = loaded_roses[rose_choice].rose;

    // Select paragraph
    printf("\n  Enter paragraph number (1-%d): ", r->size);
    int para_num;
    scanf("%d", &para_num);
    flush_input_buffer();

    if (para_num < 1 || para_num > r->size) {
        print_error("Invalid paragraph number!");
        return;
    }

    PetalNode* petal = rose_get_petal(r, para_num - 1);

    CLEAR_SCREEN();
    print_header("PARAGRAPH Display");

    printf("  [1] Display by Position\n");
    printf("  [2] Display by Alphabet\n");
    printf("  [3] BACK\n\n");
    printf("  Select: ");

    int display_choice;
    scanf("%d", &display_choice);
    flush_input_buffer();

    CLEAR_SCREEN();
    print_header("Paragraph Content");

    if (display_choice == 1) {
        printf("  === DISPLAY BY POSITION ===\n\n");
        petal_print_by_position(petal);
    } else if (display_choice == 2) {
        printf("  === DISPLAY BY ALPHABET ===\n\n");
        petal_print_by_alpha(petal);
    }

    printf("\n\n  Press Enter to continue...");
    GETCH();
}

// ============================================================================
// STATISTICS
// ============================================================================

void display_paragraph_stats() {
    CLEAR_SCREEN();
    print_header("PARAGRAPH STATISTICS");

    if (num_loaded_roses == 0) {
        print_error("No ROSES loaded!");
        return;
    }

    // Select ROSE
    const char** rose_options = malloc((num_loaded_roses + 1) * sizeof(char*));
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_options[i] = loaded_roses[i].name;
    }
    rose_options[num_loaded_roses] = "BACK";

    int rose_choice = select_menu_option(rose_options, num_loaded_roses + 1);
    free(rose_options);

    if (rose_choice == num_loaded_roses) return;

    Rose* r = loaded_roses[rose_choice].rose;

    // Select paragraph
    printf("\n  Enter paragraph number (1-%d): ", r->size);
    int para_num;
    scanf("%d", &para_num);
    flush_input_buffer();

    if (para_num < 1 || para_num > r->size) {
        print_error("Invalid paragraph number!");
        return;
    }

    CLEAR_SCREEN();
    print_header("Paragraph Statistics");
    rose_print_stats(r, para_num - 1);
    printf("\n  Press Enter to continue...");
    GETCH();
}

void display_full_rose_stats() {
    CLEAR_SCREEN();
    print_header("FULL ROSE STATISTICS");

    if (num_loaded_roses == 0) {
        print_error("No ROSES loaded!");
        return;
    }

    // Select ROSE
    const char** rose_options = malloc((num_loaded_roses + 1) * sizeof(char*));
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_options[i] = loaded_roses[i].name;
    }
    rose_options[num_loaded_roses] = "BACK";

    int rose_choice = select_menu_option(rose_options, num_loaded_roses + 1);
    free(rose_options);

    if (rose_choice == num_loaded_roses) return;

    CLEAR_SCREEN();
    print_header("Full ROSE Statistics");
    rose_global_stats(loaded_roses[rose_choice].rose);
    printf("\n  Press Enter to continue...");
    GETCH();
}

void display_word_frequency() {
    CLEAR_SCREEN();
    print_header("WORD FREQUENCY");

    if (num_loaded_roses == 0) {
        print_error("No ROSES loaded!");
        return;
    }

    // Select ROSE
    const char** rose_options = malloc((num_loaded_roses + 1) * sizeof(char*));
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_options[i] = loaded_roses[i].name;
    }
    rose_options[num_loaded_roses] = "BACK";

    int rose_choice = select_menu_option(rose_options, num_loaded_roses + 1);
    free(rose_options);

    if (rose_choice == num_loaded_roses) return;

    Rose* r = loaded_roses[rose_choice].rose;

    // Select paragraph
    printf("\n  Enter paragraph number (1-%d): ", r->size);
    int para_num;
    scanf("%d", &para_num);
    flush_input_buffer();

    if (para_num < 1 || para_num > r->size) {
        print_error("Invalid paragraph number!");
        return;
    }

    CLEAR_SCREEN();
    print_header("Word Frequency Table");
    rose_print_frequency_table(r, para_num - 1);
    printf("\n  Press Enter to continue...");
    GETCH();
}

// ============================================================================
// SEARCH OPERATIONS
// ============================================================================

void search_word_at_position() {
    CLEAR_SCREEN();
    print_header("SEARCH WORD AT LOCAL POSITION");

    if (num_loaded_roses == 0) {
        print_error("No ROSES loaded!");
        return;
    }

    // Select ROSE
    const char** rose_options = malloc((num_loaded_roses + 1) * sizeof(char*));
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_options[i] = loaded_roses[i].name;
    }
    rose_options[num_loaded_roses] = "BACK";

    int rose_choice = select_menu_option(rose_options, num_loaded_roses + 1);
    free(rose_options);

    if (rose_choice == num_loaded_roses) return;

    Rose* r = loaded_roses[rose_choice].rose;

    // Select paragraph
    printf("\n  Enter paragraph number (1-%d): ", r->size);
    int para_num;
    scanf("%d", &para_num);
    flush_input_buffer();

    if (para_num < 1 || para_num > r->size) {
        print_error("Invalid paragraph number!");
        return;
    }

    // Enter position
    printf("  Enter word position: ");
    int pos;
    scanf("%d", &pos);
    flush_input_buffer();

    CLEAR_SCREEN();
    print_header("Search Result");

    char* word = rose_word_at_pos(r, para_num - 1, pos);
    if (word) {
        printf("  Word at position %d: '%s'\n", pos, word);
    } else {
        printf("  No word found at position %d\n", pos);
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void find_word_across_paragraphs() {
    CLEAR_SCREEN();
    print_header("FIND WORD ACROSS ALL PARAGRAPHS");

    if (num_loaded_roses == 0) {
        print_error("No ROSES loaded!");
        return;
    }

    // Select ROSE
    const char** rose_options = malloc((num_loaded_roses + 1) * sizeof(char*));
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_options[i] = loaded_roses[i].name;
    }
    rose_options[num_loaded_roses] = "BACK";

    int rose_choice = select_menu_option(rose_options, num_loaded_roses + 1);
    free(rose_options);

    if (rose_choice == num_loaded_roses) return;

    Rose* r = loaded_roses[rose_choice].rose;

    // Enter word to search
    char word[MAX_WORD_LENGTH];
    printf("\n  Enter word to search: ");
    fgets(word, sizeof(word), stdin);
    word[strcspn(word, "\n")] = 0;

    CLEAR_SCREEN();
    print_header("Search Results");

    int found = 0;
    for (int i = 0; i < r->size; i++) {
        printf("  [PARAGRAPH %d]\n", i + 1);
        printf("    Total words: %d\n", petal_word_count(r->petals[i]));
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

// ============================================================================
// MENU: DISPLAY SUBMENU
// ============================================================================

void menu_display() {
    int done = 0;
    while (!done) {
        const char* options[] = {
            "[1] Full ROSE",
            "[2] Single Paragraph",
            "[3] Paragraph Statistics",
            "[4] Full ROSE Summary",
            "[5] Word Frequency",
            "[6] BACK"
        };

        int choice = select_menu_option(options, 6);

        switch (choice) {
            case 0: display_full_rose(); break;
            case 1: display_single_paragraph(); break;
            case 2: display_paragraph_stats(); break;
            case 3: display_full_rose_stats(); break;
            case 4: display_word_frequency(); break;
            case 5: done = 1; break;
        }
    }
}

// ============================================================================
// MENU: SEARCH SUBMENU
// ============================================================================

void menu_search() {
    int done = 0;
    while (!done) {
        const char* options[] = {
            "[1] Word at Local Position",
            "[2] Find Word Across All Paragraphs",
            "[3] BACK"
        };

        int choice = select_menu_option(options, 3);

        switch (choice) {
            case 0: search_word_at_position(); break;
            case 1: find_word_across_paragraphs(); break;
            case 2: done = 1; break;
        }
    }
}

// ============================================================================
// MENU: SET OPERATIONS (WORD-BASED)
// ============================================================================

int select_two_petals(Rose** r1, int* p1, Rose** r2, int* p2) {
    if (num_loaded_roses == 0) {
        print_error("No ROSES loaded!");
        return 0;
    }

    CLEAR_SCREEN();
    print_header("SELECT PETAL A (First operand)");

    const char** rose_options = malloc((num_loaded_roses + 1) * sizeof(char*));
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_options[i] = loaded_roses[i].name;
    }
    rose_options[num_loaded_roses] = "BACK";

    int rose1_choice = select_menu_option(rose_options, num_loaded_roses + 1);
    if (rose1_choice == num_loaded_roses) {
        free(rose_options);
        return 0;
    }

    *r1 = loaded_roses[rose1_choice].rose;

    printf("\n  Enter paragraph number for Petal A (1-%d): ", (*r1)->size);
    scanf("%d", p1);
    flush_input_buffer();
    (*p1)--;

    if (*p1 < 0 || *p1 >= (*r1)->size) {
        print_error("Invalid paragraph number!");
        free(rose_options);
        return 0;
    }

    CLEAR_SCREEN();
    print_header("SELECT PETAL B (Second operand)");

    int rose2_choice = select_menu_option(rose_options, num_loaded_roses + 1);
    free(rose_options);

    if (rose2_choice == num_loaded_roses) {
        return 0;
    }

    *r2 = loaded_roses[rose2_choice].rose;

    printf("\n  Enter paragraph number for Petal B (1-%d): ", (*r2)->size);
    scanf("%d", p2);
    flush_input_buffer();
    (*p2)--;

    if (*p2 < 0 || *p2 >= (*r2)->size) {
        print_error("Invalid paragraph number!");
        return 0;
    }

    return 1;
}

void word_ops_union() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("WORD UNION (A ∪ B)");

    PetalNode* result = rose_union_petals(r1, p1, r2, p2);

    if (result) {
        printf("  === RESULT ===\n\n");
        petal_print_by_position(result);
        petal_free(result);
    } else {
        print_error("Operation failed!");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void word_ops_intersection() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("WORD INTERSECTION (A ∩ B)");

    PetalNode* result = rose_intersect_petals(r1, p1, r2, p2);

    if (result) {
        printf("  === RESULT ===\n\n");
        petal_print_by_position(result);
        petal_free(result);
    } else {
        print_error("Operation failed!");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void word_ops_difference() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("WORD DIFFERENCE (A \\ B)");

    PetalNode* result = rose_diff_petals(r1, p1, r2, p2);

    if (result) {
        printf("  === RESULT ===\n\n");
        petal_print_by_position(result);
        petal_free(result);
    } else {
        print_error("Operation failed!");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void word_ops_symmetric_diff() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("WORD SYMMETRIC DIFFERENCE (A △ B)");

    PetalNode* result = rose_sym_diff_petals(r1, p1, r2, p2);

    if (result) {
        printf("  === RESULT ===\n\n");
        petal_print_by_position(result);
        petal_free(result);
    } else {
        print_error("Operation failed!");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void word_ops_subset() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("SUBSET CHECK (A ⊆ B ?)");

    int result = rose_is_subset_petals(r1, p1, r2, p2);

    if (result) {
        printf("  ✓ Petal A IS a subset of Petal B\n");
    } else {
        printf("  ✗ Petal A IS NOT a subset of Petal B\n");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void menu_word_operations() {
    int done = 0;
    while (!done) {
        const char* options[] = {
            "[1] Union (A ∪ B)",
            "[2] Intersection (A ∩ B)",
            "[3] Difference (A \\ B)",
            "[4] Symmetric Difference (A △ B)",
            "[5] Subset Check (A ⊆ B ?)",
            "[6] BACK"
        };

        int choice = select_menu_option(options, 6);

        switch (choice) {
            case 0: word_ops_union(); break;
            case 1: word_ops_intersection(); break;
            case 2: word_ops_difference(); break;
            case 3: word_ops_symmetric_diff(); break;
            case 4: word_ops_subset(); break;
            case 5: done = 1; break;
        }
    }
}

// ============================================================================
// MENU: SET OPERATIONS (SENTENCE-BASED)
// ============================================================================

void sentence_ops_union() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("SENTENCE UNION (A ∪ B)");

    PetalNode* result = rose_sentence_union(r1, p1, r2, p2);

    if (result) {
        printf("  === RESULT ===\n\n");
        petal_print_sentences(result);
        petal_free(result);
    } else {
        print_error("Operation failed!");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void sentence_ops_intersection() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("SENTENCE INTERSECTION (A ∩ B)");

    PetalNode* result = rose_sentence_intersection(r1, p1, r2, p2);

    if (result) {
        printf("  === RESULT ===\n\n");
        petal_print_sentences(result);
        petal_free(result);
    } else {
        print_error("Operation failed!");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void sentence_ops_difference() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("SENTENCE DIFFERENCE (A \\ B)");

    PetalNode* result = rose_sentence_difference(r1, p1, r2, p2);

    if (result) {
        printf("  === RESULT ===\n\n");
        petal_print_sentences(result);
        petal_free(result);
    } else {
        print_error("Operation failed!");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void sentence_ops_symmetric_diff() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("SENTENCE SYMMETRIC DIFFERENCE (A △ B)");

    PetalNode* result = rose_sentence_symmetric_difference(r1, p1, r2, p2);

    if (result) {
        printf("  === RESULT ===\n\n");
        petal_print_sentences(result);
        petal_free(result);
    } else {
        print_error("Operation failed!");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void sentence_ops_subset() {
    Rose *r1, *r2;
    int p1, p2;

    if (!select_two_petals(&r1, &p1, &r2, &p2)) return;

    CLEAR_SCREEN();
    print_header("SENTENCE SUBSET CHECK (A ⊆ B ?)");

    int result = rose_sentence_is_subset(r1, p1, r2, p2);

    if (result) {
        printf("  ✓ Petal A IS a subset of Petal B (sentence-based)\n");
    } else {
        printf("  ✗ Petal A IS NOT a subset of Petal B (sentence-based)\n");
    }

    printf("\n  Press Enter to continue...");
    GETCH();
}

void menu_sentence_operations() {
    int done = 0;
    while (!done) {
        const char* options[] = {
            "[1] Sentence Union (A ∪ B)",
            "[2] Sentence Intersection (A ∩ B)",
            "[3] Sentence Difference (A \\ B)",
            "[4] Sentence Symmetric Difference (A △ B)",
            "[5] Sentence Subset Check (A ⊆ B ?)",
            "[6] BACK"
        };

        int choice = select_menu_option(options, 6);

        switch (choice) {
            case 0: sentence_ops_union(); break;
            case 1: sentence_ops_intersection(); break;
            case 2: sentence_ops_difference(); break;
            case 3: sentence_ops_symmetric_diff(); break;
            case 4: sentence_ops_subset(); break;
            case 5: done = 1; break;
        }
    }
}

// ============================================================================
// MEMORY MANAGEMENT
// ============================================================================

void free_rose() {
    CLEAR_SCREEN();
    print_header("FREE A LOADED ROSE");

    if (num_loaded_roses == 0) {
        print_error("No ROSES loaded!");
        return;
    }

    const char** rose_options = malloc((num_loaded_roses + 1) * sizeof(char*));
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_options[i] = loaded_roses[i].name;
    }
    rose_options[num_loaded_roses] = "CANCEL";

    int choice = select_menu_option(rose_options, num_loaded_roses + 1);
    free(rose_options);

    if (choice == num_loaded_roses) return;

    // Free the ROSE
    rose_free(loaded_roses[choice].rose);

    // Remove from list
    for (int i = choice; i < num_loaded_roses - 1; i++) {
        loaded_roses[i] = loaded_roses[i + 1];
    }
    num_loaded_roses--;

    print_success("ROSE freed successfully!");
}

void free_all_roses() {
    CLEAR_SCREEN();
    print_header("FREE ALL ROSES");

    for (int i = 0; i < num_loaded_roses; i++) {
        rose_free(loaded_roses[i].rose);
    }
    num_loaded_roses = 0;

    print_success("All ROSES freed successfully!");
}

// ============================================================================
// ABOUT & MAIN MENU
// ============================================================================

void about() {
    CLEAR_SCREEN();
    print_header("ABOUT ROSE");

    printf("  Application: ROSE (Red-Black Tree Ordered Sentence Engine)\n\n");
    printf("  Description:\n");
    printf("    ROSE is a sophisticated text analysis system built on a three-tier\n");
    printf("    hierarchical architecture using advanced data structures.\n\n");
    printf("  Key Features:\n");
    printf("    • Circular doubly-linked list of Petals (paragraphs)\n");
    printf("    • Red-Black Trees for word indexing and positioning\n");
    printf("    • Global Lexicon for memory-efficient word storage\n");
    printf("    • Set Operations on both words and sentences\n");
    printf("    • Digital Fingerprinting (Hashing) for sentence comparison\n");
    printf("    • Optimized O(log n) search complexity\n\n");
    printf("  Creators: Aboud & Guedira\n\n");

    printf("  Press Enter to return...");
    GETCH();
}

void main_menu() {
    int done = 0;
    while (!done) {
        const char* options[] = {
            "[1] Load File",
            "[2] Display",
            "[3] Search",
            "[4] Word-Based Set Operations",
            "[5] Sentence-Based Set Operations",
            "[6] Free Memory",
            "[7] About",
            "[8] Exit"
        };

        int choice = select_menu_option(options, 8);

        switch (choice) {
            case 0: load_file(); break;
            case 1: menu_display(); break;
            case 2: menu_search(); break;
            case 3: menu_word_operations(); break;
            case 4: menu_sentence_operations(); break;
            case 5:
                {
                    const char* free_options[] = {
                        "[1] Free a Single ROSE",
                        "[2] Free All ROSES",
                        "[3] BACK"
                    };
                    int free_choice = select_menu_option(free_options, 3);
                    if (free_choice == 0) free_rose();
                    else if (free_choice == 1) free_all_roses();
                }
                break;
            case 6: about(); break;
            case 7: done = 1; break;
        }
    }
}

// ============================================================================
// ENTRY POINT
// ============================================================================

int main() {
    main_menu();
    
    // Cleanup
    for (int i = 0; i < num_loaded_roses; i++) {
        rose_free(loaded_roses[i].rose);
    }

    return 0;
}
