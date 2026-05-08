#include "text.h"

int is_delimiter(char c) {
    return (c==' ' || c=='\t' || c=='\n' || c=='.' || c==',' ||
            c=='?' || c=='!' || c=='"'  || c=='\''|| c==';' ||
            c==':' || c=='-');
}

int is_paragraph_break(char* s, int i) {
    return (s[i]=='\n' && s[i+1]=='\n');
}

int is_end_of_string(char* s, int i) { return (s[i]=='\0'); }

int is_sentence_end(char c) { return (c=='.' || c=='!' || c=='?'); }

int skip_delimiters(char* s, int i, int* np, int* sf) {
    while (s[i] != '\0' && is_delimiter(s[i])) {
        if (is_sentence_end(s[i])) *sf = 1;
        if (is_paragraph_break(s, i)) *np = 1;
        i++;
    }
    return i;
}

int collect_word(char* s, int i) {
    while (s[i] != '\0' && !is_delimiter(s[i])) i++;
    return i;
}

void detect_newpara(char* s, int i, char** nxt, int* np) {
    if (is_end_of_string(s, i)) { *np = 1; *nxt = NULL; }
    else {
        *np  = is_paragraph_break(s, i) ? 1 : 0;
        s[i] = '\0';
        *nxt = s + i + 1;
    }
}

char* separate_words(char** str, int* np, int* sf) {
    if (!str || !*str || !**str) return NULL;
    char* s = *str;
    int i = 0;
    *sf = 0;
    i = skip_delimiters(s, i, np, sf);
    if (is_end_of_string(s, i)) { *str = NULL; return NULL; }
    char* start = s + i;
    i = collect_word(s, i);
    detect_newpara(s, i, str, np);
    return start;
}

void lowercase_string(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        str[i] = tolower(str[i]);
        i++;
    }
}