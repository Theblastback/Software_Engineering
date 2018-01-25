
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
const int LOCKTYPE = 3;

/*
 * NOTE - functions in this file require parameters including the full text of the file.
 */

using namespace std;

int main(int argc, char *argv[]){
    /*
     * Variables - direct from the ATRLOCK program.
     */
    char *fn1[], *fn2[], *f1[], *f2[], *s[], *s1[], *s2[], *lock_code[];
    int i, j, k, lock_pos, lock_dat, this_dat;


    /**
    *   This is the rest of the main function that was written below in the pascal file.
    **/

    srand (time(NULL));
    lock_pos = 0;
    lock_dat = 0;
    if (paramcount() < 1 || paramcount() > 2) {
      sprintf("Usage: ATRLOCK <robot[.at2]> [locked[.atl]]");
      exit();
    }
    *fn1[] = btrim(ucase(paramstr(1)));
    if (*fn1[] == base_name(*fn1[])) {
      *fn1[] = *fn1[] + ".AT2";
    }
    if(!file.is_open()){
      sprintf("Robot ", *fn1[], " not found!");
      exit;
    }
    if (paramcount == 2) {
      *fn2[] = btrim(ucase(paramstr(2)));
    } else {
      *fn2[] = base_name(*fn1[]) + ".ATL";
    }
    if (*fn2[] == base_name(*fn2[])) {
      *fn2[] = *fn2[] + ".ATL";
    }
    if (!valid(*fn2[]) {
      sprintf("Output name ", *fn1[], " not valid");
      exit;
    }
    if (*fn1[] == *fn2[]) {
      sprintf("Filenames can not be the same!");
      exit;
    }
    *fn1[].assign(*f1[]);
    reset(*f1[]);
    *fn2[].assign(*f2[]);
    rewrite(*f2[]);

    /**
    * lock header
    **/
    f2write << ";------------------------------------------------------------------------------";
    f2write << "; " no_path(base_name(*fn1[])), "Locked on ", date);
    f2write << ";------------------------------------------------------------------------------";
    lock_code = "";
    k = rand() % 21 + 20;
    for (int i = 1, i < k, i++) {
        lock_code = lock_code + char(rand() % 32 + 65);
    }
    f2write << "#LOCK", locktype," ",lock_code;



}


/*
 * Encode function
 * Purpose: Changes bits of characters in file effectively encoding them.
 */
string encode(string s) {
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

  /**
  *   Write line function.
  **/

  void *write_line(char *s_, char *s1_) {
      char s[256], s1[256];
      strcpy(s, s_);
      strcpy(s1, s1_);
      s = prepare(s);
      if strlen(s) > 0 {
        s = encode(s);
        writer << s;
      }
  }
