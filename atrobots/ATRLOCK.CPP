 /* everything between =Nick= was written by Nick */
 /* =Nick= */
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

const int LOCKTYPE = 3;

/*
 * NOTE - functions in this file require parameters including the full text of the file.
 */

int main(int argc, char *argv[]){
    /*
     * Variables - direct from the ATRLOCK program.
     */

    char *s[], *s1[], *s2[], *lock_code[];
    string fn1, fn2;
    char date [9];
    int i, j, k, lock_pos, lock_dat, this_dat;

    srand (time(NULL));

    instream f1read (f1);

    ofstream f2write (f2);
    ofstream f1write (f2);

    lock_pos = 0;
    lock_dat = 0;
    if (argc < 1 || argc > 2) {
      sprintf("Usage: ATRLOCK <robot[.at2]> [locked[.atl]]");
      exit();
    }
    fn1 = btrim(to_upper(paramstr(1)));
    if (fn1 == base_name(fn1)) {
      fn1 = fn1 + ".AT2";
    }
    if(!file.is_open()){
      sprintf("Robot ", fn1, " not found!");
      exit;
    }
    if (paramcount == 2) {
      fn2 = trim(ucase(paramstr(2)));
    } else {
      fn2 = base_name(fn1) + ".ATL";
    }
    if (fn2 == base_name(fn2)) {
      fn2 = fn2 + ".ATL";
    }
    if (!valid(fn2) {
      sprintf("Output name ", fn1, " not valid");
      exit;
    }
    if (fn1 == fn2) {
      sprintf("Filenames can not be the same!");
      exit;
    }
    fn1.assign(*f1[]);
    reset(*f1[]);
    fn2.assign(*f2[]);
    rewrite(*f2[]);

    /* copy comment header */
    writer << ";------------------------------------------------------------------------------";
    s = '/0';
    do {
      instream.getline (f1, s);
      s = btrim(s); //replace with our language equivalent
      if (s[1] ==  ' ; ') {
        sprintf (to_upper (s));
        s = ' \0 ';
      }

    } while( !eof(f1) and (s == '\0' )); //fix not eof

    /**
    * lock header
    **/
    _strdate (date);

    f2write << ";------------------------------------------------------------------------------";
    f2write << "; " no_path(base_name(fn1)), "Locked on %s", date);
    f2write << ";------------------------------------------------------------------------------";
    lock_code = "";
    k = rand() % 21 + 20;
    for (int i = 1, i < k, i++) {
        lock_code = lock_code + char(rand() % 32 + 65);
    }
    f2write << "#LOCK", locktype," ",lock_code;


    /* Decode lock-Code */
    for (int i = 0; i < lock_code.length(); i++) {
      lock_code[i] = char(ord(lock_code[i] - 65); // lookup ord when possible)
    }

    printf("Encoding: ", *fn1, "...");

    //Encode robot
    s= btrim(s);

    if (s.length() > 0) {
      f2write << to_upper(s);
    }

    do {
      //Read line!
      f1read >> *s1[];
      s = "\0";
      *s1[] = btrim(to_upper(s1));

      //Write line!
      f2write << s;

    } while(!eof (f1)); //fix this too

    sprintf ("Done. Used LOCK Format #", locktype, ".");
    sprintf ("Only ATR2 v2.08 or later can decode ");
    sprintf ("LOCKed robot saved as ", fn2 );

    f1.close();
    f2.close();

}

/* Nick */

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
/* =Nick= */

void *write_line(char *s_, char *s1_) {
    char s[256], s1[256];
    strcpy(s, s_);
    strcpy(s1, s1_);
    s = prepare(s);
    if strlen(s) > 0 {
      s = encode(s);
      f2write << s;
    }
}


string btrim (string s1){

  s1 = ltrim(rtrim(s1));

  return trimmed;
}

string ltrim (string s1){
  int i;
  while ( s1.length > 0 && s1.copy( s.length, 1) =  || s1.copy(s1.length, 1) == 8 || s1.copy(s1.length, 1) == 9){
    s1 = s1.copy(1, s1.length -1);
  }
  return s1;
}

string rtrim (string s1, int i){


  return s1;
}
