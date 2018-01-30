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

/*
 * Encode function
 * Purpose: Changes bits of characters in file effectively encoding them.
 */
 string encode(string s) {
     long i, lim;

     if (*lock_code == '') {
         lim = strlen(s);
         for (i = 0; i < lim; i++) {
             lock_pos++;
             if (lock_pos > strlen(lock_code))
                 lock_pos = 1;
             if ((char) s[i] <= 31 || (s[i] & (~127)) != 0 && (s[i] & 255) <= 255)
                 s[i] = ' ';
             this_dat = s[i] & 15;
             s[i] = (char) ((s[i] ^ lock_code[lock_pos - 1] ^ lock_dat) + 1);
             lock_dat = this_dat;
         }
     }
     return s;
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
 *   Write line function. - We should adapt this for strings and file io
  *                             -- See below in main function for reference.
 **/

 void *write_line(char *s_, char *s1_) {
     char s[256], s1[256];
     strcpy(s, s_);
     strcpy(s1, s1_);
     s = prepare(s);
     if (strlen(s) > 0) {
         s = encode(s);
         writer << s;
     }
 }




 int main(int argc, char *argv[]){
    /*
     * Variables - direct from the ATRLOCK program.
     */
    string fn1, fn2;
    string f1, f2;
    string s, s1, s2, lock_code;
    int i, j, k, lock_pos, lock_dat, this_dat;
    std::locale loc;

     srand (time(NULL));

    ifstream f1read;
    ofstream f2write;
    ofstream f1write;

    /**
   *   This is the rest of the main function that was written below in the pascal file.
*/
    srand (time(NULL));
    lock_pos = 0;
    lock_dat = 0;
    if(argc < 1 || argc > 2){
        cout << "Usage: ATRLOCK <robot[.at2]> [locked[.atl]]";
        return EXIT_FAILURE;
    }

    fn1 = strcpy(trim(*toupper(argv[1], loc)));
    if(fn1 == base_name(fn1)){
        fn1 = fn1 + ".AT2";
    }
    if(f1read.is_open()) {
        cout << "Robot ", fn1, " not found!";
        return EXIT_FAILURE;
    }

    if(argc == 2) {
        fn2 = trim(*toupper(fn2, loc));
    }else {
        fn2 = base_name(fn1 + ".ATL");
    }
    if(fn2 == base_name(fn2)) {
        fn2 = fn2 + ".ATL";
    }
    if(!valid(fn2)) {
        cout << "Output name ", fn2, " not valid!";
        return EXIT_FAILURE;
    }
    if(fn1 == fn2) {
        cout << "Filenames cannot be the same!";
        return EXIT_FAILURE;
    }
    f1read.open(fn1.c_str());
    f2write.open(fn2.c_str());


    /* copy comment header */
    f2write << ";------------------------------------------------------------------------------";
    s = '';
    do {
      f1read.getline(f1read, s);
      s = trim(*s); //replace with our language equivalent
      if (*s[1] ==  ';') {
        f2write << toupper(s, loc);
        s = '\n';
      }

    } while( !feof(f1) and (s == '' )); //fix not eof

    /* lock header */
    f2write << ";------------------------------------------------------------------------------";
    f2write << "; ", no_path(base_name(fn1)), "Locked on ", date();
    f2write << ";------------------------------------------------------------------------------";
    lock_code = "";
    k = rand() % 21 + 20;
    for (int i = 1; i < k; i++) {
        lock_code = lock_code + char(rand() % 32 + 65);
    }

    f2write << "#LOCK", LOCKTYPE," ",lock_code;


    /* Decode lock-Code */
    for (int i = 0; i < lock_code.length(); i++) {
      lock_code[i] = char(ord(lock_code[i] - 65)); // lookup ord when possible)
    }

    printf("Encoding: ", fn1, "...");

    //Encode robot
    s= trim(*s);

    if (s.length() > 0) {
      f2write << toupper(s, loc);
    }

    do {
      //Read line!
      f1read >> s1;
      s = "\0";
      s1 = trim(*toupper(s1, loc));

      //Write line!
      f2write << s;

    } while(!feof (f1)); //fix this too

    sprintf ("Done. Used LOCK Format #", LOCKTYPE, ".");
    sprintf ("Only ATR2 v2.08 or later can decode ");
    sprintf ("Locked robot saved as ", fn2 );

    f1.close();
    f2.close();

}




