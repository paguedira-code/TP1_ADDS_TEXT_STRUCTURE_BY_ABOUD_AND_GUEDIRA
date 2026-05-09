#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include "structures.h"

extern Rose* load_file(const char* filename);
extern void rose_free(Rose* r);
extern PetalNode* rose_get_petal(Rose* r, int index);
extern void petal_free(PetalNode* p);
extern void petal_print_by_position(PetalNode* p);
extern void petal_print_by_alpha(PetalNode* p);
extern void petal_print_stats(PetalNode* p);
extern void petal_print_frequency_table(PetalNode* p);
extern int petal_word_count(PetalNode* p);
extern char* petal_word_at_pos(PetalNode* p, int position);
extern char* petal_most_frequent_word(PetalNode* p);
extern int petal_unique_word_count(PetalNode* p);
extern void rose_find_petals_with_word(Rose* r, const char* word);
extern int rose_total_word_count(Rose* r);
extern PetalNode* petal_union(PetalNode* a, PetalNode* b);
extern PetalNode* petal_intersection(PetalNode* a, PetalNode* b);
extern PetalNode* petal_difference(PetalNode* a, PetalNode* b);
extern PetalNode* petal_symmetric_difference(PetalNode* a, PetalNode* b);
extern int petal_is_subset_of(PetalNode* a, PetalNode* b);
extern int petal_is_identical(PetalNode* a, PetalNode* b);

#define MAX_ROSES 10
#define MAX_RESULTS 20

Rose* roses[MAX_ROSES];
int roses_count = 0;

PetalNode* results[MAX_RESULTS];
char results_names[MAX_RESULTS][64];
int results_count = 0;

void clear_screen()
{
    printf("\e[1;1H\e[2J");
}

void go_xy(int x, int y)
{
    printf("\e[%d;%dH", y, x);
}

void clear_buffer()
{
    int c;
    c = getchar();
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

#ifdef _WIN32
int read_key()
{
    int k = _getch();
    if (k == 224) {
        k = _getch();
        if (k == 72) {
            return 1000;
        }
        if (k == 80) {
            return 1001;
        }
    }
    return k;
}
#else
int read_key()
{
    struct termios old_t, new_t;
    tcgetattr(STDIN_FILENO, &old_t);
    new_t = old_t;
    new_t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_t);
    int k = getchar();
    if (k == 27) {
        getchar();
        int nx = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
        if (nx == 'A') {
            return 1000;
        }
        if (nx == 'B') {
            return 1001;
        }
        return k;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
    return k;
}
#endif

void print_logo()
{
    char* logo[] = {
        "  RRRRRRR   OOOOOOO    SSSSSSS  EEEEEEE  ",
        "  RR    RR OO     OO  SS        EE       ",
        "  RRRRRRR  OO     OO  SSSSSSS   EEEEE    ",
        "  RR  RR   OO     OO        SS  EE       ",
        "  RR   RR   OOOOOOO    SSSSSSS  EEEEEEE  "
    };
    int i = 0;
    while (i < 5) {
        go_xy(20, 2 + i);
        int j = 0;
        while (logo[i][j] != '\0') {
            if (logo[i][j] != ' ') {
                printf("\e[48;5;204m \e[0m");
            }
            else {
                printf(" ");
            }
            j = j + 1;
        }
        i = i + 1;
    }
}

int select_menu(char** options, int n_options, int col, int line)
{
    int sel = 0;
    int key;
    while (1) {
        int i = 0;
        while (i < n_options) {
            go_xy(col, line + i);
            if (i == sel) {
                printf("\e[7m\e[38;5;204m%s\e[0m", options[i]);
            }
            else {
                printf("\e[0m%s\e[0m", options[i]);
            }
            i = i + 1;
        }
        key = read_key();
        if (key == 1000 || key == 'w' || key == 'W') {
            sel = sel - 1;
        }
        else if (key == 1001 || key == 's' || key == 'S') {
            sel = sel + 1;
        }
        else if (key == '\r' || key == '\n') {
            return sel;
        }
        if (sel < 0) {
            sel = n_options - 1;
        }
        if (sel >= n_options) {
            sel = 0;
        }
    }
}

void print_separator(int line)
{
    go_xy(15, line);
    printf("\e[38;5;204m");
    int i = 0;
    while (i < 50) {
        printf("-");
        i = i + 1;
    }
    printf("\e[0m");
}

PetalNode* select_petal(const char* title)
{
    clear_screen();
    go_xy(20, 3);
    printf("\e[38;5;204m--- %s ---\e[0m", title);

    if (roses_count == 0 && results_count == 0) {
        go_xy(15, 6);
        printf("No rose loaded and no result available.");
        go_xy(15, 8);
        printf("Press a key...");
        read_key();
        return NULL;
    }

    char* source_options[2] = {
        "      1. From a loaded rose          ",
        "      2. From saved results          "
    };
    go_xy(15, 5);
    printf("Petal source :");
    int src = select_menu(source_options, 2, 18, 7);

    if (src == 0) {
        if (roses_count == 0) {
            go_xy(15, 12);
            printf("No rose loaded.");
            read_key();
            return NULL;
        }
        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- PICK A ROSE ---\e[0m");
        char* roses_list[MAX_ROSES + 1];
        char roses_buf[MAX_ROSES + 1][70];
        int i = 0;
        while (i < roses_count) {
            sprintf(roses_buf[i], "      %d. %-30s (%d para.)", i + 1, roses[i]->name, roses[i]->size);
            roses_list[i] = roses_buf[i];
            i = i + 1;
        }
        sprintf(roses_buf[roses_count], "      Cancel                                       ");
        roses_list[roses_count] = roses_buf[roses_count];
        int rc = select_menu(roses_list, roses_count + 1, 18, 6);
        if (rc == roses_count) {
            return NULL;
        }

        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- PICK A PARAGRAPH ---\e[0m");
        int total = roses[rc]->size;
        char* para_list[101];
        char para_buf[101][40];
        int max_show = total;
        if (max_show > 100) {
            max_show = 100;
        }
        int j = 0;
        while (j < max_show) {
            sprintf(para_buf[j], "      Paragraph %-3d                ", j + 1);
            para_list[j] = para_buf[j];
            j = j + 1;
        }
        sprintf(para_buf[max_show], "      Cancel                       ");
        para_list[max_show] = para_buf[max_show];
        int pc = select_menu(para_list, max_show + 1, 18, 6);
        if (pc == max_show) {
            return NULL;
        }
        return rose_get_petal(roses[rc], pc);
    }

    if (results_count == 0) {
        go_xy(15, 12);
        printf("No saved result.");
        read_key();
        return NULL;
    }
    clear_screen();
    go_xy(20, 3);
    printf("\e[38;5;204m--- PICK A RESULT ---\e[0m");
    char* res_list[MAX_RESULTS + 1];
    char res_buf[MAX_RESULTS + 1][70];
    int k = 0;
    while (k < results_count) {
        sprintf(res_buf[k], "      [R%d] %-40s", k + 1, results_names[k]);
        res_list[k] = res_buf[k];
        k = k + 1;
    }
    sprintf(res_buf[results_count], "      Cancel                                          ");
    res_list[results_count] = res_buf[results_count];
    int choice = select_menu(res_list, results_count + 1, 18, 6);
    if (choice == results_count) {
        return NULL;
    }
    return results[choice];
}

void save_or_discard(PetalNode* result)
{
    if (result == NULL) {
        return;
    }
    clear_screen();
    go_xy(20, 3);
    printf("\e[38;5;204m--- RESULT ---\e[0m");
    go_xy(15, 5);
    petal_print_by_alpha(result);

    print_separator(20);
    char* options[2] = {
        "      Save this result                ",
        "      Discard                         "
    };
    int choice = select_menu(options, 2, 18, 22);
    if (choice == 0) {
        if (results_count >= MAX_RESULTS) {
            go_xy(15, 25);
            printf("Result limit reached.");
            read_key();
            petal_free(result);
            return;
        }
        clear_screen();
        go_xy(20, 5);
        printf("Result name: ");
        scanf(" %63[^\n]", results_names[results_count]);
        clear_buffer();
        results[results_count] = result;
        results_count = results_count + 1;
        go_xy(15, 8);
        printf("\e[38;5;204mResult saved.\e[0m");
        read_key();
    }
    else {
        petal_free(result);
        go_xy(15, 25);
        printf("Result discarded.");
        read_key();
    }
}

void menu_load_file()
{
    if (roses_count >= MAX_ROSES) {
        clear_screen();
        go_xy(20, 8);
        printf("\e[38;5;196mRose limit reached (%d/%d).\e[0m", roses_count, MAX_ROSES);
        read_key();
        return;
    }
    clear_screen();
    go_xy(20, 3);
    printf("\e[38;5;204m--- LOAD A FILE ---\e[0m");
    go_xy(15, 6);
    printf("File path (.txt): ");
    char filename[256];
    scanf(" %255[^\n]", filename);
    clear_buffer();

    FILE* test = fopen(filename, "r");
    if (test == NULL) {
        go_xy(15, 9);
        printf("\e[38;5;196mError: file not found.\e[0m");
        read_key();
        return;
    }
    fclose(test);

    Rose* new_rose = load_file(filename);
    if (new_rose == NULL) {
        go_xy(15, 9);
        printf("\e[38;5;196mLoad failed.\e[0m");
        read_key();
        return;
    }

    roses[roses_count] = new_rose;
    roses_count = roses_count + 1;
    go_xy(15, 9);
    printf("\e[38;5;204mFile loaded successfully.\e[0m");
    go_xy(15, 10);
    printf("Paragraphs detected: %d", new_rose->size);
    go_xy(15, 11);
    printf("Total words: %d", rose_total_word_count(new_rose));
    go_xy(15, 13);
    printf("Press a key to continue...");
    read_key();
}

void menu_display_full_rose()
{
    if (roses_count == 0) {
        clear_screen();
        go_xy(20, 8);
        printf("No rose loaded.");
        read_key();
        return;
    }
    clear_screen();
    go_xy(20, 3);
    printf("\e[38;5;204m--- PICK A ROSE ---\e[0m");
    char* list[MAX_ROSES + 1];
    char buf[MAX_ROSES + 1][70];
    int i = 0;
    while (i < roses_count) {
        sprintf(buf[i], "      %d. %-35s", i + 1, roses[i]->name);
        list[i] = buf[i];
        i = i + 1;
    }
    sprintf(buf[roses_count], "      Back                                       ");
    list[roses_count] = buf[roses_count];
    int rc = select_menu(list, roses_count + 1, 18, 6);
    if (rc == roses_count) {
        return;
    }

    char* opts[3] = {
        "      By position (text order)        ",
        "      By alphabet                     ",
        "      Back                            "
    };
    int back = 0;
    while (back == 0) {
        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- DISPLAY: %s ---\e[0m", roses[rc]->name);
        int choice = select_menu(opts, 3, 18, 6);
        if (choice == 2) {
            back = 1;
            continue;
        }
        clear_screen();
        PetalNode* cur = roses[rc]->petals;
        int idx = 0;
        do {
            printf("\n\e[38;5;204m  === Paragraph %d ===\e[0m\n", idx + 1);
            if (choice == 0) {
                petal_print_by_position(cur);
            }
            else {
                petal_print_by_alpha(cur);
            }
            cur = cur->next;
            idx = idx + 1;
        } while (cur != roses[rc]->petals);
        printf("\n  Press a key...");
        read_key();
    }
}

void menu_display_single_paragraph()
{
    PetalNode* p = select_petal("PICK A PARAGRAPH");
    if (p == NULL) {
        return;
    }
    char* opts[3] = {
        "      By position                     ",
        "      By alphabet                     ",
        "      Back                            "
    };
    int back = 0;
    while (back == 0) {
        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- DISPLAY PARAGRAPH ---\e[0m");
        int choice = select_menu(opts, 3, 18, 6);
        clear_screen();
        if (choice == 0) {
            petal_print_by_position(p);
        }
        else if (choice == 1) {
            petal_print_by_alpha(p);
        }
        else {
            back = 1;
            continue;
        }
        printf("\n  Press a key...");
        read_key();
    }
}

void menu_statistics()
{
    char* opts[4] = {
        "      Stats of one paragraph          ",
        "      Full rose summary               ",
        "      Word frequency table            ",
        "      Back                            "
    };
    int back = 0;
    while (back == 0) {
        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- STATISTICS ---\e[0m");
        int choice = select_menu(opts, 4, 18, 6);
        if (choice == 3) {
            back = 1;
            continue;
        }
        if (choice == 0) {
            PetalNode* p = select_petal("PARAGRAPH STATS");
            if (p == NULL) {
                continue;
            }
            clear_screen();
            go_xy(20, 3);
            printf("\e[38;5;204m--- STATS ---\e[0m\n\n");
            petal_print_stats(p);
            printf("\n  Press a key...");
            read_key();
        }
        else if (choice == 1) {
            if (roses_count == 0) {
                clear_screen();
                go_xy(15, 8);
                printf("No rose loaded.");
                read_key();
                continue;
            }
            clear_screen();
            go_xy(20, 3);
            printf("\e[38;5;204m--- PICK A ROSE ---\e[0m");
            char* list[MAX_ROSES + 1];
            char buf[MAX_ROSES + 1][70];
            int i = 0;
            while (i < roses_count) {
                sprintf(buf[i], "      %d. %-35s", i + 1, roses[i]->name);
                list[i] = buf[i];
                i = i + 1;
            }
            sprintf(buf[roses_count], "      Back                                       ");
            list[roses_count] = buf[roses_count];
            int rc = select_menu(list, roses_count + 1, 18, 6);
            if (rc == roses_count) {
                continue;
            }
            clear_screen();
            printf("\n\e[38;5;204m  === Summary of: %s ===\e[0m\n\n", roses[rc]->name);
            printf("  %-12s %-10s %-10s %-20s\n", "Paragraph", "Words", "Unique", "Most frequent");
            printf("  %-12s %-10s %-10s %-20s\n", "---------", "-----", "------", "-------------");
            PetalNode* cur = roses[rc]->petals;
            int idx = 0;
            do {
                int tw = petal_word_count(cur);
                int uw = petal_unique_word_count(cur);
                char* mf = petal_most_frequent_word(cur);
                if (mf == NULL) {
                    mf = "N/A";
                }
                printf("  %-12d %-10d %-10d %-20s\n", idx + 1, tw, uw, mf);
                cur = cur->next;
                idx = idx + 1;
            } while (cur != roses[rc]->petals);
            printf("\n  Press a key...");
            read_key();
        }
        else if (choice == 2) {
            PetalNode* p = select_petal("FREQUENCY TABLE");
            if (p == NULL) {
                continue;
            }
            clear_screen();
            go_xy(20, 3);
            printf("\e[38;5;204m--- FREQUENCY TABLE ---\e[0m\n\n");
            petal_print_frequency_table(p);
            printf("\n  Press a key...");
            read_key();
        }
    }
}

void menu_display()
{
    char* opts[4] = {
        "      1. Full rose                    ",
        "      2. Single paragraph             ",
        "      3. Statistics                   ",
        "      4. Back                         "
    };
    int back = 0;
    while (back == 0) {
        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- DISPLAY ---\e[0m");
        int choice = select_menu(opts, 4, 18, 6);
        if (choice == 0) {
            menu_display_full_rose();
        }
        else if (choice == 1) {
            menu_display_single_paragraph();
        }
        else if (choice == 2) {
            menu_statistics();
        }
        else {
            back = 1;
        }
    }
}

void menu_search()
{
    char* opts[3] = {
        "      1. Word at local position       ",
        "      2. Word in all paragraphs       ",
        "      3. Back                         "
    };
    int back = 0;
    while (back == 0) {
        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- SEARCH ---\e[0m");
        int choice = select_menu(opts, 3, 18, 6);

        if (choice == 2) {
            back = 1;
            continue;
        }

        if (choice == 0) {
            PetalNode* p = select_petal("PICK A PARAGRAPH");
            if (p == NULL) {
                continue;
            }
            clear_screen();
            go_xy(20, 3);
            printf("\e[38;5;204m--- WORD AT POSITION ---\e[0m");
            go_xy(15, 6);
            printf("Words in this paragraph: %d", petal_word_count(p));
            go_xy(15, 8);
            printf("Position (1 to %d): ", petal_word_count(p));
            int pos;
            scanf("%d", &pos);
            clear_buffer();
            char* w = petal_word_at_pos(p, pos);
            go_xy(15, 10);
            if (w != NULL) {
                printf("\e[38;5;204mWord at %d: %s\e[0m", pos, w);
            }
            else {
                printf("\e[38;5;196mInvalid position.\e[0m");
            }
            go_xy(15, 12);
            printf("Press a key...");
            read_key();
        }
        else {
            if (roses_count == 0) {
                clear_screen();
                go_xy(15, 8);
                printf("No rose loaded.");
                read_key();
                continue;
            }
            clear_screen();
            go_xy(20, 3);
            printf("\e[38;5;204m--- PICK A ROSE ---\e[0m");
            char* list[MAX_ROSES + 1];
            char buf[MAX_ROSES + 1][70];
            int i = 0;
            while (i < roses_count) {
                sprintf(buf[i], "      %d. %-35s", i + 1, roses[i]->name);
                list[i] = buf[i];
                i = i + 1;
            }
            sprintf(buf[roses_count], "      Back                                       ");
            list[roses_count] = buf[roses_count];
            int rc = select_menu(list, roses_count + 1, 18, 6);
            if (rc == roses_count) {
                continue;
            }
            clear_screen();
            go_xy(20, 3);
            printf("\e[38;5;204m--- SEARCH WORD ---\e[0m");
            go_xy(15, 6);
            printf("Word: ");
            char target[128];
            scanf(" %127[^\n]", target);
            clear_buffer();
            int j = 0;
            while (target[j] != '\0') {
                if (target[j] >= 'A' && target[j] <= 'Z') {
                    target[j] = target[j] + ('a' - 'A');
                }
                j = j + 1;
            }
            clear_screen();
            go_xy(20, 3);
            printf("\e[38;5;204m--- RESULTS FOR '%s' ---\e[0m\n\n", target);
            rose_find_petals_with_word(roses[rc], target);
            printf("\n  Press a key...");
            read_key();
        }
    }
}

void run_word_op(int op_type)
{
    clear_screen();
    go_xy(20, 3);
    printf("\e[38;5;204m--- PICK PETAL A ---\e[0m");
    PetalNode* a = select_petal("PETAL A");
    if (a == NULL) {
        return;
    }
    clear_screen();
    go_xy(20, 3);
    printf("\e[38;5;204m--- PICK PETAL B ---\e[0m");
    PetalNode* b = select_petal("PETAL B");
    if (b == NULL) {
        return;
    }

    PetalNode* res = NULL;

    if (op_type == 0) {
        res = petal_union(a, b);
    }
    else if (op_type == 1) {
        res = petal_intersection(a, b);
    }
    else if (op_type == 2) {
        res = petal_difference(a, b);
    }
    else if (op_type == 3) {
        res = petal_symmetric_difference(a, b);
    }
    else if (op_type == 4) {
        clear_screen();
        go_xy(20, 5);
        printf("\e[38;5;204m--- SUBSET CHECK ---\e[0m");
        go_xy(15, 8);
        int sub = petal_is_subset_of(a, b);
        int id = petal_is_identical(a, b);
        if (id == 1) {
            printf("\e[38;5;204mA and B are identical.\e[0m");
        }
        else if (sub == 1) {
            printf("\e[38;5;204mA is a subset of B: TRUE\e[0m");
        }
        else {
            printf("\e[38;5;196mA is a subset of B: FALSE\e[0m");
        }
        go_xy(15, 10);
        printf("Press a key...");
        read_key();
        return;
    }

    if (res != NULL) {
        save_or_discard(res);
    }
}

void menu_word_operations()
{
    char* opts[6] = {
        "      1. Union          (A U B)       ",
        "      2. Intersection   (A n B)       ",
        "      3. Difference     (A \\ B)      ",
        "      4. Sym. Diff      (A D B)       ",
        "      5. Subset check   (A c B ?)     ",
        "      6. Back                         "
    };
    int back = 0;
    while (back == 0) {
        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- WORD OPERATIONS ---\e[0m");
        int choice = select_menu(opts, 6, 18, 6);
        if (choice == 5) {
            back = 1;
            continue;
        }
        run_word_op(choice);
    }
}

void menu_results()
{
    int back = 0;
    while (back == 0) {
        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- SAVED RESULTS (%d/%d) ---\e[0m", results_count, MAX_RESULTS);

        if (results_count == 0) {
            go_xy(15, 6);
            printf("No saved result.");
            go_xy(15, 8);
            printf("Press a key...");
            read_key();
            return;
        }

        char* list[MAX_RESULTS + 1];
        char buf[MAX_RESULTS + 1][70];
        int i = 0;
        while (i < results_count) {
            sprintf(buf[i], "      [R%d] %-40s", i + 1, results_names[i]);
            list[i] = buf[i];
            i = i + 1;
        }
        sprintf(buf[results_count], "      Back                                          ");
        list[results_count] = buf[results_count];

        int choice = select_menu(list, results_count + 1, 18, 6);
        if (choice == results_count) {
            back = 1;
            continue;
        }

        char* actions[3] = {
            "      Show this result                ",
            "      Delete this result              ",
            "      Back                            "
        };
        int action_back = 0;
        while (action_back == 0) {
            clear_screen();
            go_xy(20, 3);
            printf("\e[38;5;204m--- %s ---\e[0m", results_names[choice]);
            int act = select_menu(actions, 3, 18, 6);
            if (act == 2) {
                action_back = 1;
                continue;
            }
            if (act == 0) {
                clear_screen();
                go_xy(20, 3);
                printf("\e[38;5;204m--- RESULT: %s ---\e[0m\n\n", results_names[choice]);
                petal_print_by_alpha(results[choice]);
                printf("\n  Press a key...");
                read_key();
            }
            else {
                petal_free(results[choice]);
                int k = choice;
                while (k < results_count - 1) {
                    results[k] = results[k + 1];
                    strncpy(results_names[k], results_names[k + 1], 63);
                    k = k + 1;
                }
                results_count = results_count - 1;
                action_back = 1;
            }
        }
    }
}

void menu_free()
{
    char* opts[3] = {
        "      Free one rose                   ",
        "      Free all roses                  ",
        "      Back                            "
    };
    int back = 0;
    while (back == 0) {
        clear_screen();
        go_xy(20, 3);
        printf("\e[38;5;204m--- FREE MEMORY ---\e[0m");
        int choice = select_menu(opts, 3, 18, 6);

        if (choice == 2) {
            back = 1;
            continue;
        }

        if (choice == 0) {
            if (roses_count == 0) {
                clear_screen();
                go_xy(15, 8);
                printf("No rose loaded.");
                read_key();
                continue;
            }
            clear_screen();
            go_xy(20, 3);
            printf("\e[38;5;204m--- PICK A ROSE TO FREE ---\e[0m");
            char* list[MAX_ROSES + 1];
            char buf[MAX_ROSES + 1][70];
            int i = 0;
            while (i < roses_count) {
                sprintf(buf[i], "      %d. %-35s", i + 1, roses[i]->name);
                list[i] = buf[i];
                i = i + 1;
            }
            sprintf(buf[roses_count], "      Back                                       ");
            list[roses_count] = buf[roses_count];
            int rc = select_menu(list, roses_count + 1, 18, 6);
            if (rc == roses_count) {
                continue;
            }

            rose_free(roses[rc]);
            int k = rc;
            while (k < roses_count - 1) {
                roses[k] = roses[k + 1];
                k = k + 1;
            }
            roses_count = roses_count - 1;
            go_xy(15, 20);
            printf("\e[38;5;204mRose freed.\e[0m");
            read_key();
        }
        else {
            int i = 0;
            while (i < roses_count) {
                rose_free(roses[i]);
                i = i + 1;
            }
            roses_count = 0;
            go_xy(15, 20);
            printf("\e[38;5;204mAll roses freed.\e[0m");
            read_key();
        }
    }
}

void menu_about()
{
    clear_screen();
    go_xy(28, 3);
    printf("\e[38;5;204m--- ABOUT ROSE-PETAL ---\e[0m");
    print_separator(5);
    go_xy(15, 7);
    printf("Application: ROSE-PETAL textual analyzer");
    go_xy(15, 9);
    printf("Description: Dual structure based on two Red-Black trees");
    go_xy(15, 10);
    printf("             per paragraph -- Alpha-RBT (words) and Pos-RBT (positions).");
    go_xy(15, 12);
    printf("Structures : Doubly linked circular list + RBT");
    go_xy(15, 14);
    printf("Operations : Word-level set theory (O(n+m))");
    print_separator(16);
    go_xy(15, 18);
    printf("ESI -- Ecole Nationale Superieure d'Informatique");
    go_xy(15, 19);
    printf("Module: Algorithms and Dynamic Data Structures");
    go_xy(20, 22);
    printf("Press a key to go back...");
    read_key();
}

void menu_start()
{
    char* opts[7] = {
        "      1. Load a file                  ",
        "      2. Display                      ",
        "      3. Search                       ",
        "      4. Word operations              ",
        "      5. Saved results                ",
        "      6. Free memory                  ",
        "      7. Back                         "
    };
    int back = 0;
    while (back == 0) {
        clear_screen();
        go_xy(22, 3);
        printf("\e[38;5;204m--- ROSE MANAGER ---\e[0m");
        go_xy(15, 5);
        printf("Loaded roses: %d/%d", roses_count, MAX_ROSES);
        int choice = select_menu(opts, 7, 18, 7);
        if (choice == 0) {
            menu_load_file();
        }
        else if (choice == 1) {
            menu_display();
        }
        else if (choice == 2) {
            menu_search();
        }
        else if (choice == 3) {
            menu_word_operations();
        }
        else if (choice == 4) {
            menu_results();
        }
        else if (choice == 5) {
            menu_free();
        }
        else {
            back = 1;
        }
    }
}

void main_menu()
{
    int quit = 0;
    while (quit == 0) {
        clear_screen();
        print_logo();
        go_xy(18, 9);
        printf("\e[38;5;204m--- WELCOME TO ROSE-PETAL ---\e[0m");
        char* opts[3] = {
            "         1. START ROSE            ",
            "         2. ABOUT                 ",
            "         3. QUIT                  "
        };
        int choice = select_menu(opts, 3, 23, 12);
        if (choice == 0) {
            menu_start();
        }
        else if (choice == 1) {
            menu_about();
        }
        else {
            clear_screen();
            go_xy(25, 10);
            printf("\e[38;5;204mGoodbye!\e[0m\n\n");
            int i = 0;
            while (i < roses_count) {
                rose_free(roses[i]);
                i = i + 1;
            }
            int j = 0;
            while (j < results_count) {
                petal_free(results[j]);
                j = j + 1;
            }
            quit = 1;
        }
    }
}

int main()
{
    main_menu();
    return 0;
}
