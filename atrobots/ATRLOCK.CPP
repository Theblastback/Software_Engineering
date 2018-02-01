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

   char *f1 [256], *f2 [256], *s[256], *s1[256], *s2[256], *lock_code[256];
   string fn1, fn2;
   char date [9];
   int i, j, k, lock_pos, lock_dat, this_dat;

   srand (time(NULL));

   ifstream f1read;
   ofstream f2write;
   ofstream f1write;

   lock_pos = 0;
   lock_dat = 0;
   if (argc < 1 || argc > 2) {
     printf("Usage: ATRLOCK robot.at2 to locked.atl");
     exit(EXIT_FAILURE);
   }
   fn1 = btrim(toupper(argv));
   if (fn1 == base_name(fn1)) {
     fn1 = fn1 + ".AT2";
   }
   if(!f1read.is_open()){
     printf("Robot %s not found!", fn1.c_str());
     exit(EXIT_FAILURE);
   }
   if (argc == 2) {
     fn2 = btrim(toupper(argc));
   } else {
     fn2 = base_name(fn1) + ".ATL";
   }
   if (fn2 == base_name(fn2)) {
     fn2 = fn2 + ".ATL";
   }
   if (!valid(fn2)) {
     printf("Output name %s not valid", fn1.c_str());
     exit(EXIT_FAILURE);
   }
   if (fn1 == fn2) {
     printf("Filenames can not be the same!");
     exit(EXIT_FAILURE);
   }
   fn1.assign(*f1[]);//figure this out too
   reset(*f1[]);  //find what this meanns in pascal
   fn2.assign(*f2[]);//figure this out too
   rewrite(*f2[]); //find what this means in pascal

   /* copy comment header */
 f2write << ";------------------------------------------------------------------------------";
   *s = "\0";
   do {
     f1read.getline (f1, *s); // getline for c++ ing f1read
     *s = btrim(*s);
     if (*s[1] ==  ' ; ') {
       f2write << toupper(*s); //check
       *s = " \0 ";
     }

   } while( !eof(f1) and (s == '\0' )); //fix not eof

   /**
   * lock header
   **/
   _strdate (date);

   f2write << ";------------------------------------------------------------------------------";
   f2write << "; ", no_path(base_name(fn1)), "Locked on %s", date;
   f2write << ";------------------------------------------------------------------------------";
   *lock_code = "\0";
   k = rand() % 21 + 20;
   for (int i = 1; i < k; i++)
   {
       *lock_code = *lock_code + char(rand() % 32 + 65);
   }
   f2write << "#LOCK", LOCKTYPE ," ",lock_code;


   /* Decode lock-Code */
   for (int i = 0; i < lock_code.length(); i++) //length isn't working find fix
       {
     lock_code[i] = char(ord(lock_code[i] - 65); // lookup ord when possible)
   }

   printf("Encoding: %s ...", fn1.c_str());

   //Encode robot
   s= btrim(s);

   if (*s.length() > 0) {
     f2write << toupper(*s);
   }

   do {
     //Read line!
     f1read >> *s1[];
     *s = "\0";
     *s1[] = btrim(toupper(*s1));

     //Write line!
     f2write << s;

   } while(!eof (f1)); //fix this too

   printf ("Done. Used LOCK Format #", LOCKTYPE, ".");
   printf ("Only ATR2 v2.08 or later can decode ");
   printf ("LOCKed robot saved as %s", fn2.c_str() );

   f1.close(); //fix for close file
   f2.close(); //" "

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
   if (strlen(s) > 0) {
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
