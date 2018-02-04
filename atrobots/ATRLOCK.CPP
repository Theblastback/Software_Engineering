/* everything between =Nick= was written by Nick */
/* =Nick= */
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <string>
#include <time.h>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <ctype.h>


using namespace std;

const int LOCKTYPE = 3;

/*
* NOTE - functions in this file require parameters including the full text of the file.
*/
// trim from start (in place)
 static inline void ltrim(string &s) {
     s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
         return !std::isspace(ch);
     }));
 }

// trim from end (in place)
 static inline void rtrim(string &s) {
     s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
         return !std::isspace(ch);
     }).base(), s.end());
 }

// trim from both ends (in place)
 static inline void trim(string &s) {
     ltrim(s);
     rtrim(s);
 }

// trim from start (copying)
 static inline std::string ltrim_copy(string s) {
     ltrim(s);
     return s;
 }

// trim from end (copying)
 static inline std::string rtrim_copy(string s) {
     rtrim(s);
     return s;
 }

// trim from both ends (copying)
 static inline std::string trim_copy(string s) {
     trim(s);
     return s;
 }

 string copy(string name, short start, short end) {
     string tmp;

     start--;
     end--;

     while (start <= end) {
         tmp = tmp + name[start];
         start++;
     }

     return(tmp);
 }

 string lstr(string s1, short l) {
     if ( s1.length() <= l )
         return(s1);
     else
         return(copy(s1,1,l) );
 }

 string rstr(string s1, short l) {
     if (s1.length() <= l)
         return(s1);
     else
         return( copy(s1, (s1.length()-l)+1 ,l) );
 }

 string to_Uppercase (string s){
    transform(s.begin(), s.end(), s.begin(), ::toupper);

    return s;
 }




/*
 * Encode function
 * Purpose: Changes bits of characters in file effectively encoding them.
 */
 string encode(string s, string lock_code) {
     long i;
     int lock_pos = 0;
     int this_dat = 0;
     int lock_dat = 0;
     size_t lim;

     if (lock_code == "") {
         lim = s.length();
         for (i = 0; i < lim; i++) {
             lock_pos++;
             if (lock_pos > lock_code.length())
                 lock_pos = 1;
             if ((char) s[i] <= 31 || (s[i] & (~127)) != 0 && ((s[i] & 255) <= 255))
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
 string prepare(string s, string s1) {
     int i, j, k, l;
     string s2;

     if(s1.length() == 0 || strcmp(&s1[1], ";")){
         s1 = "";
     }else{
         k = 0;
         for(i = s1.length(); i--; i >=1){
             if(strcmp(&s1[i],";")){
                 k = i;
             }
         }
         if(k > 0){
             s1 = lstr(s1, k-1);
         }
     }
     s2 = "";
     for(i = 1; i <= s1.length(); i++){
         string arr[] = {" ", "#8", "#9", "#10", ","};
         size_t arrSize = sizeof(arr) / sizeof(int);
         int *end = arr + arrSize;
        // find the value 0:
         int *result = std::find(arr, end, s1);
         if (result = end) {
             s2 = s1+s1[i];
         }else{
             if(s2 != ""){
                 s = s+s2+" ";
                 s2 = "";
             }
         }

     }
     if(s2 != "")
         s = s+s2;
     return s;
 }

 /**
 *   Write line function. - We should adapt this for strings and file io
  *                             -- See below in main function for reference.
  *
 **/

 void write_line(string s, string s1, ofstream f2write, string lock_code) {
     s = prepare(s, s1);
     if(s.length() > 0){
         s = encode(s, lock_code);
         f2write << s;
     }
 }

 string base_name(string name) {
	short k;
	string s1;

	k = 1;
	while ( (k <= name.length()) && (name[k].compare(".")) ) {
		s1 = s1 + name[k];
		k++;
	}
	return(s1);
}
 string no_path(string fn) {
     short i, k;

     k = -1;
     for (i = fn.length(); i >= 0; i--)
         if ((!fn.compare("\ ") || !fn[i].compare(":")) && (k < i))
             k = i;
     if (k != -1)
         return (rstr(fn, fn.length() - k));
     else
         return(fn);
 }

 bool exist(string thisfile) {
     fstream afile;
     bool	return_value;

     afile.open(thisfile, std::fstream::in);
     if ( (return_value = afile.good()) )
         afile.close();

     return(return_value);
 }

 bool valid(string thisfile) {
     fstream afile;
     bool check;
     string iocode;

     if (!exist(thisfile) ) {
         afile.open(thisfile, std::fstream::out);
         afile.close();

         std::remove(thisfile);
         return(false);
     } else
         return(true);
 }

int main(int argc, char *argv[]){
   /*
    * Variables - direct from the ATRLOCK program.
    */
    //char *f1 [256], *f2 [256], *s[256], *s1[256], *s2[256], *lock_code[256];
   string fn1, fn2,f1,f2,s,s1,s2, lock_code;
   string arg(argv[1]);
   string arg2(argv[2]);
   //char date [9];
   int i, j, k, lock_pos, lock_dat, this_dat;
   time_t now = time(0);

   srand (time(NULL));

   ifstream f1read;
   ofstream f2write;
   ofstream f1write;
   lock_pos = 0;
   lock_dat = 0;

   if(argc < 1 || argc > 2){
       cout << "Usage: ATRLOCK <robot[.at2]> [locked[.atl]]";
       return EXIT_FAILURE;
   }

   fn1 = strcpy(trim(toupper(argc)));
   if(fn1 == base_name(fn1)){
       fn1 = fn1 + ".AT2";
   }
   if(f1read.is_open()) {
       cout << "Robot ", fn1, " not found!";
       return EXIT_FAILURE;
   }

   if(argc == 2) {
       fn2 = trim(to_Uppercase(fn2));
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
   s = "";
   do {
     getline(f1read, s);
     s = trim(s); //replace with our language equivalent
     if (s[1] ==  ';') {
       f2write << to_Uppercase(s);
       s = '\n';
     }

   } while( !f1read.eof() and (s == "" )); //fix not eof

   /**
   * lock header
   **/

   f2write << ";------------------------------------------------------------------------------";
   f2write << "; ", no_path(base_name(fn1)), "Locked on ", ctime(&now);
   f2write << ";------------------------------------------------------------------------------";
   lock_code = "";
   k = rand() % 21 + 20;
   for (int i = 1; i < k; i++)
   {
       lock_code = lock_code + char(rand() % 32 + 65);
   }
   f2write << "#LOCK", LOCKTYPE ," ",lock_code;


   /* Decode lock-Code */
   for (int i = 0; i < sizeof(lock_code); i++) //length isn't working find fix
       {
     lock_code[i] = char(lock_code[i] - 65); // lookup ord when possible)
   }

   printf("Encoding: %s ...", fn1.c_str());

   //Encode robot
   s = trim(s);
   if((s.length()) > 0){
       write_line("", to_Uppercase(s1), f2write, lock_code);
   }
   while(!f1read.eof()){
       if (f1read.is_open()) {
           while (getline(f1read,s1)) {
               cout << s1 << '\n';
           }
           f1read.close();
       }
       write_line(s, s1, f1write, lock_code);
   }
  printf ("Done. Used LOCK Format #", LOCKTYPE, ".");
  printf ("Only ATR2 v2.08 or later can decode ");
  printf ("Locked robot saved as ", fn2 );

  f1.close();
  f2.close();

}
