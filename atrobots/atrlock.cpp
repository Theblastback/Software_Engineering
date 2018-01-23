#include <stdio.h>
#include <stdlib.h>
const int LOCKTYPE = 3;

/*
 * NOTE - functions in this file require parameters including the full text of the file.
 */

int main(int argc, char *argv[]){
    /*
     * Variables - direct from the ATRLOCK program.
     */
    char *fn1[], *fn2[], *f1[], *f2[], *s[], *s1[], *s2[], *lock_code[];
    int i, j, k, lock_pos, lock_dat, this_dat;

}


/*
 * Encode function
 * Purpose: Changes bits of characters in file effectively encoding them.
 */
static char *encode(char *result, char *s_) {
    char s[256];
    long i, lim;

    strcpy(s, s_);
    if (*lock_code == '\0')
        return strcpy(result, s);
    lim = strlen(s);
    for (i = 0; i < lim; i++) {
        lock_pos++;
        if (lock_pos > strlen(lock_code))
            lock_pos = 1;
        if ((char)s[i] <= 31 || (s[i] & (~127)) != 0 && (s[i] & 255) <= 255)
            s[i] = ' ';
        this_dat = s[i] & 15;
        s[i] = (char)((s[i] ^ lock_code[lock_pos-1] ^ lock_dat) + 1);
        lock_dat = this_dat;
    }
    return strcpy(result, s);
}

/*
 * Prepares files to be encoded.
 */
static char *prepare(char *result, char *s_, char *s1_) {
    char s[256], s1[256];
    long i, k;
    char s2[256];
    long lim;
    char str1[256];

    strcpy(s, s_);
    strcpy(s1, s1_);
    if (*s1 == '\0' || s1[0] == ';')
        *s1 = '\0';
    else {
        k = 0;
        for (i = strlen(s1); i >= 1; i--) {
            if (s1[i-1] == ';')
              k = i;
        }
        if (k > 0) {
            strcpy(s1, &lstr(s1, k - 1));
        }
    }

    *s2 = '\0';
    lim = strlen(s1);
    for (i = 0; i < lim; i++) {
        if ((s1[i] < '\b' || s1[i] > '\n') && s1[i] != ',' && s1[i] != ' ')
            sprintf(s2 + strlen(s2), "%c", s1[i]);
        else {
            if (*s2 != '\0')
            sprintf(s + strlen(s), "%s ", s2);
            *s2 = '\0';
        }
    }

    if (*s2 != '\0')
        strcat(s, s2);

    return strcpy(result, s);
}