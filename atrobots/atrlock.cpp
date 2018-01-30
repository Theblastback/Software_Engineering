 /* everything between =Nick= was written by Nick */
 /* =Nick= */
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>


const int LOCKTYPE = 3;

/*
 * NOTE - functions in this file require parameters including the full text of the file.
 */

 using namespace std;

int main(int argc, char *argv[]){
    /*
     * Variables - direct from the ATRLOCK program.
     */
     string fn1, fn2, f1, f2;
     char *s[256], *s1[256], *s2[256], *lock_code[256];
     int i, j, k, lock_pos, lock_dat, this_dat;

     srand (time(NULL));

    ifstream f1read;
    ofstream f2write;
    ofstream f1write;

    /* copy comment header */
    cout << ";------------------------------------------------------------------------------";
    s = '/0';
    do {
      f1read.getline (f1, s);
      s = trim(s); //replace with our language equivalent
      if (*s[1] ==  ';') {
        sprintf (ucase (s));
        s = '\n';
      }

    } while( !feof(f1) and (s == '\0' )); //fix not eof


    /* Decode lock-Code */
    for (int i = 0; i < lock_code.length(); i++) {
      lock_code[i] = char(ord(lock_code[i] - 65)); // lookup ord when possible)
    }

    printf("Encoding: ", fn1, "...");

    //Encode robot
    s= trim(s);

    if (s.length() > 0) {
      f2write << toupper(s);
    }

    do {
      //Read line!
      f1read >> *s1;
      s = "\0";
      *s1 = trim(toupper(s1));

      //Write line!
      f2write << s;

    } while(!feof (f1)); //fix this too

    sprintf ("Done. Used LOCK Format #", locktype, ".");
    sprintf ("Only ATR2 v2.08 or later can decode ");
    sprintf ("LOCKed robot saved as ", fn2 );

    f1.close();
    f2.close();

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
/* =Nick= */

 /*
  * Code from Stack Overflow User jotik
  */

 // trim from start (in place)
 static inline void ltrim(std::string &s) {
     s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
         return !std::isspace(ch);
     }));
 }

// trim from end (in place)
 static inline void rtrim(std::string &s) {
     s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
         return !std::isspace(ch);
     }).base(), s.end());
 }

// trim from both ends (in place)
 static inline void trim(std::string &s) {
     ltrim(s);
     rtrim(s);
 }

// trim from start (copying)
 static inline std::string ltrim_copy(std::string s) {
     ltrim(s);
     return s;
 }

// trim from end (copying)
 static inline std::string rtrim_copy(std::string s) {
     rtrim(s);
     return s;
 }

// trim from both ends (copying)
 static inline std::string trim_copy(std::string s) {
     trim(s);
     return s;
 }
