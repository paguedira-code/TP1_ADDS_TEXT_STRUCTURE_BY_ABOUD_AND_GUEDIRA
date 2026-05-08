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

/* ============================================================
 *  DECLARATIONS EXTERNES
 * ============================================================ */
extern Rose*      charger_fichier(const char* nom_fichier);
extern void       rose_free(Rose* rose);
extern PetalNode* rose_get_petal(Rose* rose, int index);
extern void       petal_free(PetalNode* petale);
extern void       petal_print_by_position(PetalNode* petale);
extern void       petal_print_by_alpha(PetalNode* petale);
extern void       petal_print_sentences(PetalNode* petale);
extern void       petal_print_stats(PetalNode* petale);
extern void       petal_print_frequency_table(PetalNode* petale);
extern int        petal_word_count(PetalNode* petale);
extern int        petal_sentence_count(PetalNode* petale);
extern char*      petal_word_at_pos(PetalNode* petale, int position);
extern void       rose_find_petals_with_word(Rose* rose, const char* mot);
extern int        rose_total_word_count(Rose* rose);
extern PetalNode* petal_union(PetalNode* a, PetalNode* b);
extern PetalNode* petal_intersection(PetalNode* a, PetalNode* b);
extern PetalNode* petal_difference(PetalNode* a, PetalNode* b);
extern PetalNode* petal_symmetric_difference(PetalNode* a, PetalNode* b);
extern int        petal_is_subset_of(PetalNode* a, PetalNode* b);
extern int        petal_is_identical(PetalNode* a, PetalNode* b);
extern PetalNode* petal_sentence_union(PetalNode* P1, PetalNode* P2);
extern PetalNode* petal_sentence_intersection(PetalNode* P1, PetalNode* P2);
extern PetalNode* petal_sentence_difference(PetalNode* P1, PetalNode* P2);
extern PetalNode* petal_sentence_symmetric_difference(PetalNode* P1, PetalNode* P2);
extern int        petal_sentence_is_subset(PetalNode* P1, PetalNode* P2);
extern int        petal_sentence_is_identical(PetalNode* P1, PetalNode* P2);

/* ============================================================
 *  CONSTANTES ET ETAT GLOBAL
 * ============================================================ */
#define MAX_ROSES    10
#define MAX_RESULTATS 20

Rose* roses[MAX_ROSES];
int   nombre_roses = 0;

PetalNode* resultats[MAX_RESULTATS];
char       noms_resultats[MAX_RESULTATS][64];
int        nombre_resultats = 0;

/* ============================================================
 *  UTILITAIRES TERMINAL
 * ============================================================ */
void effacer_ecran() {
    printf("\e[1;1H\e[2J");
}

void aller_xy(int x, int y) {
    printf("\e[%d;%dH", y, x);
}

void vider_buffer() {
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF);
}

#ifdef _WIN32
int lire_touche() {
    int touche = _getch();
    if (touche == 224) {
        touche = _getch();
        if (touche == 72) return 1000; /* fleche haut */
        if (touche == 80) return 1001; /* fleche bas  */
    }
    return touche;
}
#else
int lire_touche() {
    struct termios ancien, nouveau;
    tcgetattr(STDIN_FILENO, &ancien);
    nouveau = ancien;
    nouveau.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &nouveau);
    int touche = getchar();
    if (touche == 27) {
        getchar(); /* [ */
        int suivant = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &ancien);
        if (suivant == 'A') return 1000;
        if (suivant == 'B') return 1001;
        return touche;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &ancien);
    return touche;
}
#endif

/* ============================================================
 *  LOGO ROSE
 * ============================================================ */
void afficher_logo() {
    char* logo[] = {
        "  RRRRRRR   OOOOOOO    SSSSSSS  EEEEEEE  ",
        "  RR    RR OO     OO  SS        EE       ",
        "  RRRRRRR  OO     OO  SSSSSSS   EEEEE    ",
        "  RR  RR   OO     OO        SS  EE       ",
        "  RR   RR   OOOOOOO    SSSSSSS  EEEEEEE  "
    };
    for (int i = 0; i < 5; i++) {
        aller_xy(20, 2 + i);
        for (int j = 0; logo[i][j] != '\0'; j++) {
            if (logo[i][j] != ' ')
                printf("\e[48;5;204m \e[0m");
            else
                printf(" ");
        }
    }
}

/* ============================================================
 *  MENU NAVIGATION (fleches + entree)
 * ============================================================ */
int selectionner_menu(char** options, int nombre_options, int colonne, int ligne) {
    int selection = 0;
    int touche;
    while (1) {
        for (int i = 0; i < nombre_options; i++) {
            aller_xy(colonne, ligne + i);
            if (i == selection)
                printf("\e[7m\e[38;5;204m%s\e[0m", options[i]);
            else
                printf("\e[0m%s\e[0m", options[i]);
        }
        touche = lire_touche();
        if (touche == 1000 || touche == 'w' || touche == 'W') selection--;
        else if (touche == 1001 || touche == 's' || touche == 'S') selection++;
        else if (touche == '\r' || touche == '\n') return selection;
        if (selection < 0)              selection = nombre_options - 1;
        if (selection >= nombre_options) selection = 0;
    }
}

/* ============================================================
 *  SEPARATEUR
 * ============================================================ */
void afficher_separateur(int ligne) {
    aller_xy(15, ligne);
    printf("\e[38;5;204m");
    for (int i = 0; i < 50; i++) printf("-");
    printf("\e[0m");
}

/* ============================================================
 *  SELECTION D'UN PETALE
 *  Retourne un petale depuis une rose chargee ou un resultat
 * ============================================================ */
PetalNode* selectionner_petale(const char* titre) {
    effacer_ecran();
    aller_xy(20, 3);
    printf("\e[38;5;204m--- %s ---\e[0m", titre);

    if (nombre_roses == 0 && nombre_resultats == 0) {
        aller_xy(15, 6);
        printf("Aucune rose chargee et aucun resultat disponible.");
        aller_xy(15, 8);
        printf("Appuyez sur une touche...");
        lire_touche();
        return NULL;
    }

    char* options_source[2] = {
        "      1. Depuis une rose chargee      ",
        "      2. Depuis les resultats sauves  "
    };
    aller_xy(15, 5);
    printf("Source du petale :");
    int choix_source = selectionner_menu(options_source, 2, 18, 7);

    if (choix_source == 0) {
        /* selection depuis rose chargee */
        if (nombre_roses == 0) {
            aller_xy(15, 12);
            printf("Aucune rose chargee.");
            lire_touche();
            return NULL;
        }
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- CHOISIR UNE ROSE ---\e[0m");
        char* liste_roses[MAX_ROSES + 1];
        char  tampons_roses[MAX_ROSES + 1][70];
        for (int i = 0; i < nombre_roses; i++) {
            sprintf(tampons_roses[i], "      %d. %-30s (%d para.)", i + 1, roses[i]->nom, roses[i]->size);
            liste_roses[i] = tampons_roses[i];
        }
        sprintf(tampons_roses[nombre_roses], "      Annuler                                       ");
        liste_roses[nombre_roses] = tampons_roses[nombre_roses];
        int choix_rose = selectionner_menu(liste_roses, nombre_roses + 1, 18, 6);
        if (choix_rose == nombre_roses) return NULL;

        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- CHOISIR UN PARAGRAPHE ---\e[0m");
        int   total_para = roses[choix_rose]->size;
        char* liste_para[101];
        char  tampons_para[101][40];
        for (int i = 0; i < total_para && i < 100; i++) {
            sprintf(tampons_para[i], "      Paragraphe %-3d                ", i + 1);
            liste_para[i] = tampons_para[i];
        }
        int nb_para = (total_para < 100) ? total_para : 100;
        sprintf(tampons_para[nb_para], "      Annuler                       ");
        liste_para[nb_para] = tampons_para[nb_para];
        int choix_para = selectionner_menu(liste_para, nb_para + 1, 18, 6);
        if (choix_para == nb_para) return NULL;
        return rose_get_petal(roses[choix_rose], choix_para);

    } else {
        /* selection depuis resultats */
        if (nombre_resultats == 0) {
            aller_xy(15, 12);
            printf("Aucun resultat sauvegarde.");
            lire_touche();
            return NULL;
        }
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- CHOISIR UN RESULTAT ---\e[0m");
        char* liste_res[MAX_RESULTATS + 1];
        char  tampons_res[MAX_RESULTATS + 1][70];
        for (int i = 0; i < nombre_resultats; i++) {
            sprintf(tampons_res[i], "      [R%d] %-40s", i + 1, noms_resultats[i]);
            liste_res[i] = tampons_res[i];
        }
        sprintf(tampons_res[nombre_resultats], "      Annuler                                          ");
        liste_res[nombre_resultats] = tampons_res[nombre_resultats];
        int choix_res = selectionner_menu(liste_res, nombre_resultats + 1, 18, 6);
        if (choix_res == nombre_resultats) return NULL;
        return resultats[choix_res];
    }
}

/* ============================================================
 *  SAUVEGARDER OU ABANDONNER UN RESULTAT
 * ============================================================ */
void sauvegarder_ou_abandonner(PetalNode* resultat) {
    if (!resultat) return;
    effacer_ecran();
    aller_xy(20, 3);
    printf("\e[38;5;204m--- RESULTAT ---\e[0m");
    aller_xy(15, 5);
    petal_print_by_alpha(resultat);

    afficher_separateur(20);
    char* options[2] = {
        "      Sauvegarder ce resultat         ",
        "      Abandonner                      "
    };
    int choix = selectionner_menu(options, 2, 18, 22);
    if (choix == 0) {
        if (nombre_resultats >= MAX_RESULTATS) {
            aller_xy(15, 25);
            printf("Limite de resultats atteinte.");
            lire_touche();
            petal_free(resultat);
            return;
        }
        effacer_ecran();
        aller_xy(20, 5);
        printf("Nom du resultat : ");
        scanf(" %63[^\n]", noms_resultats[nombre_resultats]);
        vider_buffer();
        resultats[nombre_resultats] = resultat;
        nombre_resultats++;
        aller_xy(15, 8);
        printf("\e[38;5;204mResultat sauvegarde avec succes.\e[0m");
        lire_touche();
    } else {
        petal_free(resultat);
        aller_xy(15, 25);
        printf("Resultat abandonne.");
        lire_touche();
    }
}

/* ============================================================
 *  [1] CHARGER UN FICHIER
 * ============================================================ */
void menu_charger_fichier() {
    if (nombre_roses >= MAX_ROSES) {
        effacer_ecran();
        aller_xy(20, 8);
        printf("\e[38;5;196mLimite de roses atteinte (%d/%d).\e[0m", nombre_roses, MAX_ROSES);
        lire_touche();
        return;
    }
    effacer_ecran();
    aller_xy(20, 3);
    printf("\e[38;5;204m--- CHARGER UN FICHIER ---\e[0m");
    aller_xy(15, 6);
    printf("Chemin du fichier (.txt) : ");
    char nom_fichier[256];
    scanf(" %255[^\n]", nom_fichier);
    vider_buffer();

    FILE* test = fopen(nom_fichier, "r");
    if (!test) {
        aller_xy(15, 9);
        printf("\e[38;5;196mErreur : fichier introuvable.\e[0m");
        lire_touche();
        return;
    }
    fclose(test);

    Rose* nouvelle_rose = charger_fichier(nom_fichier);
    if (!nouvelle_rose) {
        aller_xy(15, 9);
        printf("\e[38;5;196mEchec du chargement.\e[0m");
        lire_touche();
        return;
    }

    roses[nombre_roses] = nouvelle_rose;
    nombre_roses++;
    aller_xy(15, 9);
    printf("\e[38;5;204mFichier charge avec succes !\e[0m");
    aller_xy(15, 10);
    printf("Nombre de paragraphes detectes : %d", nouvelle_rose->size);
    aller_xy(15, 11);
    printf("Nombre total de mots : %d", rose_total_word_count(nouvelle_rose));
    aller_xy(15, 13);
    printf("Appuyez sur une touche pour continuer...");
    lire_touche();
}

/* ============================================================
 *  [2] AFFICHAGE
 * ============================================================ */
void menu_affichage_rose_complete() {
    if (nombre_roses == 0) {
        effacer_ecran();
        aller_xy(20, 8);
        printf("Aucune rose chargee.");
        lire_touche();
        return;
    }
    effacer_ecran();
    aller_xy(20, 3);
    printf("\e[38;5;204m--- CHOISIR UNE ROSE ---\e[0m");
    char* liste[MAX_ROSES + 1];
    char  tampons[MAX_ROSES + 1][70];
    for (int i = 0; i < nombre_roses; i++) {
        sprintf(tampons[i], "      %d. %-35s", i + 1, roses[i]->nom);
        liste[i] = tampons[i];
    }
    sprintf(tampons[nombre_roses], "      Retour                                       ");
    liste[nombre_roses] = tampons[nombre_roses];
    int choix_rose = selectionner_menu(liste, nombre_roses + 1, 18, 6);
    if (choix_rose == nombre_roses) return;

    char* options_affichage[3] = {
        "      Par position (texte original)   ",
        "      Par ordre alphabetique          ",
        "      Retour                          "
    };
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- AFFICHAGE : %s ---\e[0m", roses[choix_rose]->nom);
        int choix = selectionner_menu(options_affichage, 3, 18, 6);
        if (choix == 2) { retour = 1; continue; }
        effacer_ecran();
        PetalNode* courant = roses[choix_rose]->petals;
        int        index   = 0;
        do {
            printf("\n\e[38;5;204m  === Paragraphe %d ===\e[0m\n", index + 1);
            if (choix == 0) petal_print_by_position(courant);
            else            petal_print_by_alpha(courant);
            courant = courant->next;
            index++;
        } while (courant != roses[choix_rose]->petals);
        printf("\n  Appuyez sur une touche...");
        lire_touche();
    }
}

void menu_affichage_paragraphe_unique() {
    PetalNode* petale = selectionner_petale("CHOISIR UN PARAGRAPHE");
    if (!petale) return;

    char* options[4] = {
        "      Par position                    ",
        "      Par ordre alphabetique          ",
        "      Par phrases                     ",
        "      Retour                          "
    };
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- AFFICHAGE DU PARAGRAPHE ---\e[0m");
        int choix = selectionner_menu(options, 4, 18, 6);
        effacer_ecran();
        switch (choix) {
            case 0: petal_print_by_position(petale); break;
            case 1: petal_print_by_alpha(petale);    break;
            case 2: petal_print_sentences(petale);   break;
            case 3: retour = 1; continue;
        }
        printf("\n  Appuyez sur une touche...");
        lire_touche();
    }
}

void menu_statistiques() {
    char* options[4] = {
        "      Statistiques d'un paragraphe    ",
        "      Resume complet (toute la rose)  ",
        "      Frequence des mots (rose)       ",
        "      Retour                          "
    };
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- STATISTIQUES ---\e[0m");
        int choix = selectionner_menu(options, 4, 18, 6);
        if (choix == 3) { retour = 1; continue; }

        if (choix == 0) {
            PetalNode* petale = selectionner_petale("STATISTIQUES DU PARAGRAPHE");
            if (!petale) continue;
            effacer_ecran();
            aller_xy(20, 3);
            printf("\e[38;5;204m--- STATISTIQUES ---\e[0m\n\n");
            petal_print_stats(petale);
            printf("\n  Appuyez sur une touche...");
            lire_touche();
        } else if (choix == 1) {
            if (nombre_roses == 0) {
                effacer_ecran();
                aller_xy(15, 8);
                printf("Aucune rose chargee.");
                lire_touche();
                continue;
            }
            effacer_ecran();
            aller_xy(20, 3);
            printf("\e[38;5;204m--- CHOISIR UNE ROSE ---\e[0m");
            char* liste[MAX_ROSES + 1];
            char  tampons[MAX_ROSES + 1][70];
            for (int i = 0; i < nombre_roses; i++) {
                sprintf(tampons[i], "      %d. %-35s", i + 1, roses[i]->nom);
                liste[i] = tampons[i];
            }
            sprintf(tampons[nombre_roses], "      Retour                                       ");
            liste[nombre_roses] = tampons[nombre_roses];
            int choix_rose = selectionner_menu(liste, nombre_roses + 1, 18, 6);
            if (choix_rose == nombre_roses) continue;
            effacer_ecran();
            printf("\n\e[38;5;204m  === Resume de : %s ===\e[0m\n\n", roses[choix_rose]->nom);
            printf("  %-12s %-10s %-10s %-10s %-20s\n", "Paragraphe", "Mots", "Uniques", "Phrases", "Plus frequent");
            printf("  %-12s %-10s %-10s %-10s %-20s\n", "----------", "----", "-------", "-------", "-------------");
            PetalNode* courant = roses[choix_rose]->petals;
            int        index   = 0;
            do {
                int   tw  = petal_word_count(courant);
                int   sc  = petal_sentence_count(courant);
                char* mf  = petal_most_frequent_word(courant);
                printf("  %-12d %-10d %-10d %-10d %-20s\n", index + 1, tw, sc, sc, mf ? mf : "N/A");
                courant = courant->next;
                index++;
            } while (courant != roses[choix_rose]->petals);
            printf("\n  Appuyez sur une touche...");
            lire_touche();
        } else if (choix == 2) {
            PetalNode* petale = selectionner_petale("TABLE DE FREQUENCE");
            if (!petale) continue;
            effacer_ecran();
            aller_xy(20, 3);
            printf("\e[38;5;204m--- TABLE DE FREQUENCE ---\e[0m\n\n");
            petal_print_frequency_table(petale);
            printf("\n  Appuyez sur une touche...");
            lire_touche();
        }
    }
}

void menu_affichage() {
    char* options[4] = {
        "      1. Rose complete                ",
        "      2. Paragraphe unique            ",
        "      3. Statistiques                 ",
        "      4. Retour                       "
    };
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- AFFICHAGE ---\e[0m");
        int choix = selectionner_menu(options, 4, 18, 6);
        switch (choix) {
            case 0: menu_affichage_rose_complete();    break;
            case 1: menu_affichage_paragraphe_unique();break;
            case 2: menu_statistiques();               break;
            case 3: retour = 1;                        break;
        }
    }
}

/* ============================================================
 *  [3] RECHERCHE
 * ============================================================ */
void menu_recherche() {
    char* options[4] = {
        "      1. Mot a une position locale    ",
        "      2. Mot dans tous les paragraphes",
        "      3. Retour                       "
    };
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- RECHERCHE ---\e[0m");
        int choix = selectionner_menu(options, 3, 18, 6);

        if (choix == 2) { retour = 1; continue; }

        if (choix == 0) {
            PetalNode* petale = selectionner_petale("CHOISIR UN PARAGRAPHE");
            if (!petale) continue;
            effacer_ecran();
            aller_xy(20, 3);
            printf("\e[38;5;204m--- MOT A UNE POSITION ---\e[0m");
            aller_xy(15, 6);
            printf("Nombre de mots dans ce paragraphe : %d", petal_word_count(petale));
            aller_xy(15, 8);
            printf("Entrez la position (1 a %d) : ", petal_word_count(petale));
            int position;
            scanf("%d", &position);
            vider_buffer();
            char* mot = petal_word_at_pos(petale, position);
            aller_xy(15, 10);
            if (mot)
                printf("\e[38;5;204mMot a la position %d : %s\e[0m", position, mot);
            else
                printf("\e[38;5;196mPosition invalide.\e[0m");
            aller_xy(15, 12);
            printf("Appuyez sur une touche...");
            lire_touche();

        } else if (choix == 1) {
            if (nombre_roses == 0) {
                effacer_ecran();
                aller_xy(15, 8);
                printf("Aucune rose chargee.");
                lire_touche();
                continue;
            }
            effacer_ecran();
            aller_xy(20, 3);
            printf("\e[38;5;204m--- CHOISIR UNE ROSE ---\e[0m");
            char* liste[MAX_ROSES + 1];
            char  tampons[MAX_ROSES + 1][70];
            for (int i = 0; i < nombre_roses; i++) {
                sprintf(tampons[i], "      %d. %-35s", i + 1, roses[i]->nom);
                liste[i] = tampons[i];
            }
            sprintf(tampons[nombre_roses], "      Retour                                       ");
            liste[nombre_roses] = tampons[nombre_roses];
            int choix_rose = selectionner_menu(liste, nombre_roses + 1, 18, 6);
            if (choix_rose == nombre_roses) continue;

            effacer_ecran();
            aller_xy(20, 3);
            printf("\e[38;5;204m--- RECHERCHER UN MOT ---\e[0m");
            aller_xy(15, 6);
            printf("Mot a rechercher : ");
            char mot_cherche[128];
            scanf(" %127[^\n]", mot_cherche);
            vider_buffer();
            effacer_ecran();
            aller_xy(20, 3);
            printf("\e[38;5;204m--- RESULTATS POUR '%s' ---\e[0m\n\n", mot_cherche);
            rose_find_petals_with_word(roses[choix_rose], mot_cherche);
            printf("\n  Appuyez sur une touche...");
            lire_touche();
        }
    }
}

/* ============================================================
 *  FONCTION GENERIQUE POUR LES OPERATIONS ENSEMBLISTES
 * ============================================================ */
void executer_operation(int type_operation, int mode_phrase) {
    effacer_ecran();
    aller_xy(20, 3);
    printf("\e[38;5;204m--- SELECTION DU PETALE A ---\e[0m");
    PetalNode* petale_a = selectionner_petale("PETALE A");
    if (!petale_a) return;

    effacer_ecran();
    aller_xy(20, 3);
    printf("\e[38;5;204m--- SELECTION DU PETALE B ---\e[0m");
    PetalNode* petale_b = selectionner_petale("PETALE B");
    if (!petale_b) return;

    PetalNode* resultat = NULL;

    if (!mode_phrase) {
        /* operations sur mots */
        switch (type_operation) {
            case 0: resultat = petal_union(petale_a, petale_b);                break;
            case 1: resultat = petal_intersection(petale_a, petale_b);         break;
            case 2: resultat = petal_difference(petale_a, petale_b);           break;
            case 3: resultat = petal_symmetric_difference(petale_a, petale_b); break;
            case 4:
                effacer_ecran();
                aller_xy(20, 5);
                printf("\e[38;5;204m--- RESULTAT SOUS-ENSEMBLE ---\e[0m");
                aller_xy(15, 8);
                if (petal_is_subset_of(petale_a, petale_b))
                    printf("\e[38;5;204mA est un sous-ensemble de B : VRAI\e[0m");
                else
                    printf("\e[38;5;196mA est un sous-ensemble de B : FAUX\e[0m");
                aller_xy(15, 10);
                printf("Appuyez sur une touche...");
                lire_touche();
                return;
        }
    } else {
        /* operations sur phrases */
        switch (type_operation) {
            case 0: resultat = petal_sentence_union(petale_a, petale_b);                break;
            case 1: resultat = petal_sentence_intersection(petale_a, petale_b);         break;
            case 2: resultat = petal_sentence_difference(petale_a, petale_b);           break;
            case 3: resultat = petal_sentence_symmetric_difference(petale_a, petale_b); break;
            case 4:
                effacer_ecran();
                aller_xy(20, 5);
                printf("\e[38;5;204m--- RESULTAT SOUS-ENSEMBLE PHRASES ---\e[0m");
                aller_xy(15, 8);
                if (petal_sentence_is_subset(petale_a, petale_b))
                    printf("\e[38;5;204mA est un sous-ensemble de B : VRAI\e[0m");
                else
                    printf("\e[38;5;196mA est un sous-ensemble de B : FAUX\e[0m");
                aller_xy(15, 10);
                printf("Appuyez sur une touche...");
                lire_touche();
                return;
        }
    }

    if (resultat) sauvegarder_ou_abandonner(resultat);
}

/* ============================================================
 *  [4] OPERATIONS SUR MOTS
 * ============================================================ */
void menu_operations_mots() {
    char* options[6] = {
        "      1. Union          (A U B)       ",
        "      2. Intersection   (A n B)       ",
        "      3. Difference     (A \\ B)      ",
        "      4. Diff. Symetrique (A D B)     ",
        "      5. Sous-ensemble  (A c B ?)     ",
        "      6. Retour                       "
    };
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- OPERATIONS SUR MOTS ---\e[0m");
        int choix = selectionner_menu(options, 6, 18, 6);
        if (choix == 5) { retour = 1; continue; }
        executer_operation(choix, 0);
    }
}

/* ============================================================
 *  [5] OPERATIONS SUR PHRASES
 * ============================================================ */
void menu_operations_phrases() {
    char* options[6] = {
        "      1. Union de phrases             ",
        "      2. Intersection de phrases      ",
        "      3. Difference de phrases        ",
        "      4. Diff. Symetrique de phrases  ",
        "      5. Sous-ensemble de phrases     ",
        "      6. Retour                       "
    };
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- OPERATIONS SUR PHRASES ---\e[0m");
        int choix = selectionner_menu(options, 6, 18, 6);
        if (choix == 5) { retour = 1; continue; }
        executer_operation(choix, 1);
    }
}

/* ============================================================
 *  [6] RESULTATS SAUVEGARDES
 * ============================================================ */
void menu_resultats() {
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- RESULTATS SAUVEGARDES (%d/%d) ---\e[0m", nombre_resultats, MAX_RESULTATS);

        if (nombre_resultats == 0) {
            aller_xy(15, 6);
            printf("Aucun resultat sauvegarde.");
            aller_xy(15, 8);
            printf("Appuyez sur une touche...");
            lire_touche();
            return;
        }

        char* liste[MAX_RESULTATS + 1];
        char  tampons[MAX_RESULTATS + 1][70];
        for (int i = 0; i < nombre_resultats; i++) {
            sprintf(tampons[i], "      [R%d] %-40s", i + 1, noms_resultats[i]);
            liste[i] = tampons[i];
        }
        sprintf(tampons[nombre_resultats], "      Retour                                          ");
        liste[nombre_resultats] = tampons[nombre_resultats];

        int choix = selectionner_menu(liste, nombre_resultats + 1, 18, 6);
        if (choix == nombre_resultats) { retour = 1; continue; }

        /* actions sur le resultat selectionne */
        char* actions[3] = {
            "      Afficher ce resultat            ",
            "      Supprimer ce resultat           ",
            "      Retour                          "
        };
        int retour_action = 0;
        while (!retour_action) {
            effacer_ecran();
            aller_xy(20, 3);
            printf("\e[38;5;204m--- %s ---\e[0m", noms_resultats[choix]);
            int action = selectionner_menu(actions, 3, 18, 6);
            if (action == 2) { retour_action = 1; continue; }
            if (action == 0) {
                effacer_ecran();
                aller_xy(20, 3);
                printf("\e[38;5;204m--- RESULTAT : %s ---\e[0m\n\n", noms_resultats[choix]);
                petal_print_by_alpha(resultats[choix]);
                printf("\n  Appuyez sur une touche...");
                lire_touche();
            } else if (action == 1) {
                petal_free(resultats[choix]);
                for (int k = choix; k < nombre_resultats - 1; k++) {
                    resultats[k] = resultats[k + 1];
                    strncpy(noms_resultats[k], noms_resultats[k + 1], 63);
                }
                nombre_resultats--;
                retour_action = 1;
            }
        }
    }
}

/* ============================================================
 *  [7] LIBERER
 * ============================================================ */
void menu_liberer() {
    char* options[3] = {
        "      Liberer une rose                ",
        "      Liberer toutes les roses        ",
        "      Retour                          "
    };
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(20, 3);
        printf("\e[38;5;204m--- LIBERER LA MEMOIRE ---\e[0m");
        int choix = selectionner_menu(options, 3, 18, 6);

        if (choix == 2) { retour = 1; continue; }

        if (choix == 0) {
            if (nombre_roses == 0) {
                effacer_ecran();
                aller_xy(15, 8);
                printf("Aucune rose chargee.");
                lire_touche();
                continue;
            }
            effacer_ecran();
            aller_xy(20, 3);
            printf("\e[38;5;204m--- CHOISIR UNE ROSE A LIBERER ---\e[0m");
            char* liste[MAX_ROSES + 1];
            char  tampons[MAX_ROSES + 1][70];
            for (int i = 0; i < nombre_roses; i++) {
                sprintf(tampons[i], "      %d. %-35s", i + 1, roses[i]->nom);
                liste[i] = tampons[i];
            }
            sprintf(tampons[nombre_roses], "      Retour                                       ");
            liste[nombre_roses] = tampons[nombre_roses];
            int choix_rose = selectionner_menu(liste, nombre_roses + 1, 18, 6);
            if (choix_rose == nombre_roses) continue;

            rose_free(roses[choix_rose]);
            for (int k = choix_rose; k < nombre_roses - 1; k++)
                roses[k] = roses[k + 1];
            nombre_roses--;
            aller_xy(15, 20);
            printf("\e[38;5;204mRose liberee avec succes.\e[0m");
            lire_touche();

        } else if (choix == 1) {
            for (int i = 0; i < nombre_roses; i++) rose_free(roses[i]);
            nombre_roses = 0;
            aller_xy(15, 20);
            printf("\e[38;5;204mToutes les roses ont ete liberees.\e[0m");
            lire_touche();
        }
    }
}

/* ============================================================
 *  A PROPOS
 * ============================================================ */
void menu_a_propos() {
    effacer_ecran();
    aller_xy(28, 3);
    printf("\e[38;5;204m--- A PROPOS DE ROSE-PETALE ---\e[0m");
    afficher_separateur(5);
    aller_xy(15, 7);
    printf("Application  : Systeme d'analyse textuelle ROSE-PETALE");
    aller_xy(15, 9);
    printf("Description  : Structure duale basee sur deux arbres Rouge-Noir");
    aller_xy(15, 10);
    printf("               par paragraphe — Alpha-RBT (mots) et Pos-RBT (positions).");
    aller_xy(15, 12);
    printf("Structures   : Liste circulaire doublement chainee + ARN + Table de hachage");
    aller_xy(15, 14);
    printf("Operations   : Theorie des ensembles sur mots et phrases (O(n+m))");
    afficher_separateur(16);
    aller_xy(15, 18);
    printf("ESI — Ecole Nationale Superieure d'Informatique");
    aller_xy(15, 19);
    printf("Module : Algorithmique et Structures de Donnees Dynamiques");
    aller_xy(20, 22);
    printf("Appuyez sur une touche pour retourner...");
    lire_touche();
}

/* ============================================================
 *  MENU DEMARRER
 * ============================================================ */
void menu_demarrer() {
    char* options[8] = {
        "      1. Charger un fichier           ",
        "      2. Affichage                    ",
        "      3. Recherche                    ",
        "      4. Operations sur mots          ",
        "      5. Operations sur phrases       ",
        "      6. Resultats sauvegardes        ",
        "      7. Liberer la memoire           ",
        "      8. Retour                       "
    };
    int retour = 0;
    while (!retour) {
        effacer_ecran();
        aller_xy(22, 3);
        printf("\e[38;5;204m--- GESTION DES ROSES ---\e[0m");
        aller_xy(15, 5);
        printf("Roses chargees : %d/%d", nombre_roses, MAX_ROSES);
        int choix = selectionner_menu(options, 8, 18, 7);
        switch (choix) {
            case 0: menu_charger_fichier();    break;
            case 1: menu_affichage();          break;
            case 2: menu_recherche();          break;
            case 3: menu_operations_mots();    break;
            case 4: menu_operations_phrases(); break;
            case 5: menu_resultats();          break;
            case 6: menu_liberer();            break;
            case 7: retour = 1;                break;
        }
    }
}

/* ============================================================
 *  MENU PRINCIPAL
 * ============================================================ */
void menu_principal() {
    int quitter = 0;
    while (!quitter) {
        effacer_ecran();
        afficher_logo();
        aller_xy(18, 9);
        printf("\e[38;5;204m--- BIENVENUE DANS ROSE-PETALE ---\e[0m");
        char* options[3] = {
            "         1. DEMARRER ROSE         ",
            "         2. A PROPOS              ",
            "         3. QUITTER               "
        };
        int choix = selectionner_menu(options, 3, 23, 12);
        switch (choix) {
            case 0: menu_demarrer(); break;
            case 1: menu_a_propos(); break;
            case 2:
                effacer_ecran();
                aller_xy(25, 10);
                printf("\e[38;5;204mAu revoir !\e[0m\n\n");
                /* liberer toute la memoire avant de quitter */
                for (int i = 0; i < nombre_roses;    i++) rose_free(roses[i]);
                for (int i = 0; i < nombre_resultats; i++) petal_free(resultats[i]);
                quitter = 1;
                break;
        }
    }
}

/* ============================================================
 *  POINT D'ENTREE
 * ============================================================ */
int main() {
    menu_principal();
    return 0;
}