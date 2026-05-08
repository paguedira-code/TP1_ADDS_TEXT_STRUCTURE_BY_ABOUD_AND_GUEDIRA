#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_ROSES 10
#define SCREEN_WIDTH 80

int nombre_de_roses = 0;
char noms_roses[MAX_ROSES][50];

// --- UTILS ---

void vider_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clrscr() {
    printf("\e[1;1H\e[2J");
}

void gotoxy(int x, int y) {
    printf("\e[%d;%dH", y, x);
}

void print_rose_logo() {
    char *logo[] = {
        "  RRRRRRR   OOOOOOO    SSSSSSS  EEEEEEE  ",
        "  RR    RR OO     OO  SS        EE       ",
        "  RRRRRRR  OO     OO  SSSSSSS   EEEEE    ",
        "  RR  RR   OO     OO        SS  EE       ",
        "  RR   RR   OOOOOOO    SSSSSSS  EEEEEEE  "
    };

    for(int i = 0; i < 5; i++) {
        gotoxy(20, 2 + i);
        for(int j = 0; logo[i][j] != '\0'; j++) {
            if(logo[i][j] != ' ') {
                printf("\e[48;5;204m \e[0m");
            } else {
                printf(" ");
            }
        }
    }
}

int selectionner_menu(char** options, int num_options, int x, int y) {
    int selected = 0;
    int key;

    while (1) {
        for (int i = 0; i < num_options; i++) {
            gotoxy(x, y + i);
            if (i == selected) {
                printf("\e[7m\e[38;5;204m%s\e[0m", options[i]);
            } else {
                // Non-selected options appear "greyed out" (standard color)
                printf("\e[0m%s\e[0m", options[i]);
            }
        }

        key = _getch();

        if (key == 224) {
            key = _getch();
            if (key == 72) selected--;
            if (key == 80) selected++;
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

// --- ANALYSIS SUB-MENUS ---

void menu_analyse_paragraphe(int rose_idx, int para_idx) {
    int retour_para = 0;
    char* options_analyse[4] = {
        "      1. FULL PHRASE ANALYSIS (HASH)  ",
        "      2. ACCESS WORD AT POSITION      ",
        "      3. SHOW SENTENCE STARTERS       ",
        "      4. RETOUR                       "
    };

    while (!retour_para) {
        clrscr();
        gotoxy(22, 5);
        printf("\e[38;5;204m--- ANALYSE : %s | PARA %d ---\e[0m", noms_roses[rose_idx], para_idx);

        int choix = selectionner_menu(options_analyse, 4, 20, 8);

        switch(choix) {
            case 0:
                clrscr(); gotoxy(20, 10);
                printf("\e[0mExecution de l'analyse par empreinte... (Hash)\e[0m");
                _getch(); break;
            case 1:
                clrscr(); gotoxy(20, 10);
                printf("\e[0mAcces direct au mot par position...\e[0m");
                _getch(); break;
            case 2:
                clrscr(); gotoxy(20, 10);
                printf("\e[0mAffichage des debuts de phrases...\e[0m");
                _getch(); break;
            case 3:
                retour_para = 1; break;
        }
    }
}

void menu_analyse_rose() {
    if (nombre_de_roses == 0) {
        clrscr();
        gotoxy(30, 5);
        printf("\e[38;5;196m--- AUCUNE ROSE ---\e[0m");
        gotoxy(20, 12);
        printf("\e[0mAppuyez sur une touche pour retourner...\e[0m");
        _getch(); return;
    }

    clrscr();
    gotoxy(25, 5);
    printf("\e[38;5;204m--- SELECTION DE LA ROSE A ANALYSER ---\e[0m");

    char* liste_roses[MAX_ROSES + 1];
    char buffer_roses[MAX_ROSES + 1][60];
    for(int i = 0; i < nombre_de_roses; i++) {
        sprintf(buffer_roses[i], "      %d. %-24s ", i + 1, noms_roses[i]);
        liste_roses[i] = buffer_roses[i];
    }
    sprintf(buffer_roses[nombre_de_roses], "      %d. RETOUR                       ", nombre_de_roses + 1);
    liste_roses[nombre_de_roses] = buffer_roses[nombre_de_roses];

    int choix_rose = selectionner_menu(liste_roses, nombre_de_roses + 1, 20, 8);
    if (choix_rose == nombre_de_roses) return;

    // --- NEW PARAGRAPH SELECTION MINI-MENU ---
    int total_paras = 5; // Placeholder N = 5

    clrscr();
    gotoxy(25, 5);
    printf("\e[38;5;204m--- ANALYSE ROSE : %s ---\e[0m", noms_roses[choix_rose]);
    gotoxy(15, 7);
    printf("\e[0m(number of paragraphs is : %d)\e[0m", total_paras);

    // Create the paragraph list
    char* liste_paras[6]; // 5 paragraphs + RETOUR
    char buffer_paras[6][40];
    for(int i = 0; i < total_paras; i++) {
        sprintf(buffer_paras[i], "      Paragraph %d                ", i + 1);
        liste_paras[i] = buffer_paras[i];
    }
    sprintf(buffer_paras[total_paras], "      RETOUR                      ");
    liste_paras[total_paras] = buffer_paras[total_paras];

    int choix_para = selectionner_menu(liste_paras, total_paras + 1, 20, 9);

    if (choix_para == total_paras) return; // Selected RETOUR

    menu_analyse_paragraphe(choix_rose, choix_para + 1);
}

// --- ORIGINAL LOGIC FUNCTIONS ---

void a_propos() {
    clrscr();
    gotoxy(28, 5);
    printf("\e[38;5;204m--- A PROPOS DE ROSE ---\e[0m");
    gotoxy(15, 8);
    printf("\e[0mApplication : Gestion des Roses (ROSE)\e[0m");
    gotoxy(15, 9);
    printf("\e[0mDescription : Systeme de structuration de texte base sur\e[0m");
    gotoxy(15, 10);
    printf("\e[0m              les arbres Rouge-Noir (Red-Black Trees).\e[0m");
    gotoxy(20, 14);
    printf("\e[0mAppuyez sur une touche pour retourner...\e[0m");
    _getch();
}

void creer_rose() {
    if (nombre_de_roses >= MAX_ROSES) {
        clrscr();
        gotoxy(28, 5);
        printf("\e[38;5;196m--- LIMITE ATTEINTE ---\e[0m");
        _getch(); return;
    }

    char nom_fichier[256];
    clrscr();
    gotoxy(22, 5);
    printf("\e[38;5;204m--- CREER UNE NOUVELLE ROSE (%d/%d) ---\e[0m", nombre_de_roses + 1, MAX_ROSES);
    gotoxy(15, 8);
    printf("\e[0mEntrez un nom pour cette rose : \e[0m");
    scanf(" %49[^\n]", noms_roses[nombre_de_roses]);
    gotoxy(15, 10);
    printf("\e[0mEntrez le chemin du fichier (.txt) : \e[0m");
    scanf(" %255[^\n]", nom_fichier);
    vider_buffer();

    FILE *file = fopen(nom_fichier, "r");
    if (!file) {
        gotoxy(22, 13);
        printf("\e[38;5;196mERREUR: Le fichier n'existe pas !\e[0m");
        _getch(); return;
    }
    fclose(file);

    nombre_de_roses++;
    gotoxy(25, 15);
    printf("\e[38;5;204mRose creee avec succes !\e[0m");
}

void voir_roses() {
    int retour_liste = 0;
    if (nombre_de_roses == 0) {
        clrscr(); gotoxy(30, 5);
        printf("\e[38;5;196m--- AUCUNE ROSE ---\e[0m");
        _getch(); return;
    }

    while (!retour_liste) {
        clrscr();
        gotoxy(28, 5);
        printf("\e[38;5;204m--- VOIR LES ROSES ---\e[0m");
        char options_str[MAX_ROSES + 1][60];
        char* liste_roses[MAX_ROSES + 1];

        for (int i = 0; i < nombre_de_roses; i++) {
            sprintf(options_str[i], "      %d. %-24s ", i + 1, noms_roses[i]);
            liste_roses[i] = options_str[i];
        }
        sprintf(options_str[nombre_de_roses], "      %d. RETOUR                       ", nombre_de_roses + 1);
        liste_roses[nombre_de_roses] = options_str[nombre_de_roses];

        int choix_rose = selectionner_menu(liste_roses, nombre_de_roses + 1, 20, 8);
        if (choix_rose == nombre_de_roses) {
            retour_liste = 1;
        } else {
            int retour_action = 0;
            while (!retour_action) {
                clrscr(); gotoxy(25, 5);
                printf("\e[38;5;204m--- ACTION SUR : %s ---\e[0m", noms_roses[choix_rose]);
                char* actions[2] = {"      1. AFFICHER (Affichage Rose)    ", "      2. RETOUR                       "};
                int choix_action = selectionner_menu(actions, 2, 20, 8);
                if (choix_action == 0) {
                    clrscr(); gotoxy(20, 5);
                    printf("\e[0mAffichage de la rose '%s'...\e[0m", noms_roses[choix_rose]);
                    _getch();
                } else retour_action = 1;
            }
        }
    }
}

void menu_operations() {
    if (nombre_de_roses == 0) {
        clrscr(); gotoxy(30, 5);
        printf("\e[38;5;196m--- AUCUNE ROSE ---\e[0m");
        _getch(); return;
    }

    int retour_ops = 0;
    char* menu_ops[6] = {"      1. UNION                        ", "      2. INTERSECTION                 ", "      3. DIFFERENCE                   ", "      4. COMPLEMENT (SYM. DIFF)       ", "      5. SOUS-ENSEMBLE                ", "      6. RETOUR                       "};

    while (!retour_ops) {
        clrscr(); gotoxy(25, 5);
        printf("\e[38;5;204m--- OPERATIONS SUR ROSES ---\e[0m");
        int type_op = selectionner_menu(menu_ops, 6, 20, 8);
        if (type_op == 5) { retour_ops = 1; continue; }
        // ... (remaining logic)
        retour_ops = 1;
    }
}

// --- NAVIGATION ---

void menu_demarrer() {
    int retour = 0;
    char* menu_options[5] = {
        "      1. CREER UNE NOUVELLE ROSE      ",
        "      2. VOIR LES ROSES               ",
        "      3. OPERATIONS SUR ROSES         ",
        "      4. ANALYSE ROSE (PETALES)       ",
        "      5. RETOUR                       "
    };

    while (!retour) {
        clrscr();
        gotoxy(24, 5);
        printf("\e[38;5;204m--- GESTION DES ROSES (ROSE) ---\e[0m");

        int choix = selectionner_menu(menu_options, 5, 20, 8);

        switch(choix) {
            case 0:
                creer_rose();
                gotoxy(20, 18); printf("\e[0mAppuyez sur une touche pour continuer...\e[0m");
                _getch(); break;
            case 1: voir_roses(); break;
            case 2: menu_operations(); break;
            case 3: menu_analyse_rose(); break;
            case 4: retour = 1; break;
        }
    }
}

void main_menu() {
    int quitter = 0;
    while (!quitter) {
        clrscr();
        print_rose_logo();
        char* menu_options[3] = {
            "         1. DEMARRER ROSE         ",
            "         2. A PROPOS              ",
            "         3. QUITTER               "
        };
        gotoxy(20, 9);
        printf("\e[38;5;204m--- BIENVENUE DANS L'APPLICATION ROSE ---\e[0m");
        int choix = selectionner_menu(menu_options, 3, 23, 12);
        switch(choix) {
            case 0: menu_demarrer(); break;
            case 1: a_propos(); break;
            case 2: quitter = 1; break;
        }
    }
}

int main() {
    main_menu();
    return 0;
}



\\----------------------------------------------
    int sentence_cmp(PosRBTNode *s1, PosRBTNode *s2) {
    while (s1 != &POS_NIL && s2 != &POS_NIL) {
        if (strcmp(s1->word_ref->word, s2->word_ref->word) != 0) {
            return 0;
        }

        s1 = nextinorder(s1);
        s2 = nextinorder(s2);

        int s1_ended = 0;
        if (s1 == &POS_NIL || s1->is_start_of_sentence != 0) {
            s1_ended = 1;
        }

        int s2_ended = 0;
        if (s2 == &POS_NIL || s2->is_start_of_sentence != 0) {
            s2_ended = 1;
        }

        if (s1_ended == 1 && s2_ended == 1) {
            return 1;
        }

        if (s1_ended == 1 || s2_ended == 1) {
            return 0;
        }
    }
    return 0;
}

PetalNode *intersection_Phrase(PetalNode *p1, PetalNode *p2) {
    if (p1->pos_tree->root == &POS_NIL || p2->pos_tree->root == &POS_NIL) {
        return NULL;
    }

    PetalNode *res = allocatePetalNode();
    res->is_pos = 1;

    int n2 = 0;
    unsigned long *arr2 = hash_petal(p2, &n2);
    PosRBTNode *curr1 = p1->pos_tree->root;

    while (curr1 != &POS_NIL && curr1->left != &POS_NIL) {
        curr1 = curr1->left;
    }

    while (curr1 != &POS_NIL) {
        unsigned long h1 = hash(curr1);
        int match_confirmed = 0;

        if (arr2 != NULL && bsearch(&h1, arr2, n2, sizeof(unsigned long), hash_cmp) != NULL) {
            PosRBTNode *search_node = p2->pos_tree->root;
            while (search_node != &POS_NIL && search_node->left != &POS_NIL) {
                search_node = search_node->left;
            }
            while (search_node != &POS_NIL) {
                if (hash(search_node) == h1) {
                    if (sentence_cmp(curr1, search_node) == 1) {
                        match_confirmed = 1;
                        break;
                    }
                }
                search_node = nextinorder(search_node);
                while (search_node != &POS_NIL && search_node->is_start_of_sentence == 0) {
                    search_node = nextinorder(search_node);
                }
            }
        }

        if (match_confirmed == 1) {
            Insert_word(res, curr1->word_ref->word, 1);
            curr1 = nextinorder(curr1);
            while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
                Insert_word(res, curr1->word_ref->word, 1);
                curr1 = nextinorder(curr1);
            }
        } else {
            curr1 = nextinorder(curr1);
            while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
                curr1 = nextinorder(curr1);
            }
        }
    }
    free(arr2);
    return res;
}

PetalNode *union_Phrase(PetalNode *p1, PetalNode *p2) {
    PetalNode *res = allocatePetalNode();
    res->is_pos = 1;
    int n1 = 0;
    unsigned long *arr1 = hash_petal(p1, &n1);

    PosRBTNode *curr1 = p1->pos_tree->root;
    while (curr1 != &POS_NIL && curr1->left != &POS_NIL) {
        curr1 = curr1->left;
    }
    while (curr1 != &POS_NIL) {
        Insert_word(res, curr1->word_ref->word, 1);
        curr1 = nextinorder(curr1);
        while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
            Insert_word(res, curr1->word_ref->word, 1);
            curr1 = nextinorder(curr1);
        }
    }

    PosRBTNode *curr2 = p2->pos_tree->root;
    while (curr2 != &POS_NIL && curr2->left != &POS_NIL) {
        curr2 = curr2->left;
    }
    while (curr2 != &POS_NIL) {
        unsigned long h2 = hash(curr2);
        int already_exists = 0;

        if (arr1 != NULL && bsearch(&h2, arr1, n1, sizeof(unsigned long), hash_cmp) != NULL) {
            PosRBTNode *search_node = p1->pos_tree->root;
            while (search_node != &POS_NIL && search_node->left != &POS_NIL) {
                search_node = search_node->left;
            }
            while (search_node != &POS_NIL) {
                if (hash(search_node) == h2) {
                    if (sentence_cmp(curr2, search_node) == 1) {
                        already_exists = 1;
                        break;
                    }
                }
                search_node = nextinorder(search_node);
                while (search_node != &POS_NIL && search_node->is_start_of_sentence == 0) {
                    search_node = nextinorder(search_node);
                }
            }
        }

        if (already_exists == 0) {
            Insert_word(res, curr2->word_ref->word, 1);
            curr2 = nextinorder(curr2);
            while (curr2 != &POS_NIL && curr2->is_start_of_sentence == 0) {
                Insert_word(res, curr2->word_ref->word, 1);
                curr2 = nextinorder(curr2);
            }
        } else {
            curr2 = nextinorder(curr2);
            while (curr2 != &POS_NIL && curr2->is_start_of_sentence == 0) {
                curr2 = nextinorder(curr2);
            }
        }
    }
    free(arr1);
    return res;
}

PetalNode *difference_Phrase(PetalNode *p1, PetalNode *p2) {
    if (p1->pos_tree->root == &POS_NIL) {
        return NULL;
    }
    PetalNode *res = allocatePetalNode();
    res->is_pos = 1;
    int n2 = 0;
    unsigned long *arr2 = hash_petal(p2, &n2);

    PosRBTNode *curr1 = p1->pos_tree->root;
    while (curr1 != &POS_NIL && curr1->left != &POS_NIL) {
        curr1 = curr1->left;
    }

    while (curr1 != &POS_NIL) {
        unsigned long h1 = hash(curr1);
        int in_p2 = 0;

        if (arr2 != NULL && bsearch(&h1, arr2, n2, sizeof(unsigned long), hash_cmp) != NULL) {
            PosRBTNode *search_node = p2->pos_tree->root;
            while (search_node != &POS_NIL && search_node->left != &POS_NIL) {
                search_node = search_node->left;
            }
            while (search_node != &POS_NIL) {
                if (hash(search_node) == h1) {
                    if (sentence_cmp(curr1, search_node) == 1) {
                        in_p2 = 1;
                        break;
                    }
                }
                search_node = nextinorder(search_node);
                while (search_node != &POS_NIL && search_node->is_start_of_sentence == 0) {
                    search_node = nextinorder(search_node);
                }
            }
        }

        if (in_p2 == 0) {
            Insert_word(res, curr1->word_ref->word, 1);
            curr1 = nextinorder(curr1);
            while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
                Insert_word(res, curr1->word_ref->word, 1);
                curr1 = nextinorder(curr1);
            }
        } else {
            curr1 = nextinorder(curr1);
            while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
                curr1 = nextinorder(curr1);
            }
        }
    }
    free(arr2);
    return res;
}

int subset_Phrase(PetalNode *p1, PetalNode *p2) {
    if (p1->pos_tree->root == &POS_NIL) {
        return 1;
    }
    if (p2->pos_tree->root == &POS_NIL) {
        return 0;
    }
    int n2 = 0;
    unsigned long *arr2 = hash_petal(p2, &n2);

    PosRBTNode *curr1 = p1->pos_tree->root;
    while (curr1 != &POS_NIL && curr1->left != &POS_NIL) {
        curr1 = curr1->left;
    }

    while (curr1 != &POS_NIL) {
        unsigned long h1 = hash(curr1);
        int sentence_found = 0;

        if (arr2 != NULL && bsearch(&h1, arr2, n2, sizeof(unsigned long), hash_cmp) != NULL) {
            PosRBTNode *search_node = p2->pos_tree->root;
            while (search_node != &POS_NIL && search_node->left != &POS_NIL) {
                search_node = search_node->left;
            }
            while (search_node != &POS_NIL) {
                if (hash(search_node) == h1) {
                    if (sentence_cmp(curr1, search_node) == 1) {
                        sentence_found = 1;
                        break;
                    }
                }
                search_node = nextinorder(search_node);
                while (search_node != &POS_NIL && search_node->is_start_of_sentence == 0) {
                    search_node = nextinorder(search_node);
                }
            }
        }

        if (sentence_found == 0) {
            free(arr2);
            return 0;
        }

        curr1 = nextinorder(curr1);
        while (curr1 != &POS_NIL && curr1->is_start_of_sentence == 0) {
            curr1 = nextinorder(curr1);
        }
    }
    free(arr2);
    return 1;
}

