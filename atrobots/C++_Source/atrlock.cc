/* everything between =Nick= was written by Nick */
/* =Nick= */
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <time.h>
#include <iostream>
#include <algorithm>
#include <cctype>


using namespace std;

const int LOCKTYPE = 3;


/*
* NOTE - functions in this file require parameters including the full text of the file.
*/
// trim from start (in place)
 static inline void ltrim(string s) {
     s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
         return !std::isspace(ch);
     }));
 }

// trim from end (in place)
 static inline void rtrim(string s) {
     s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
         return !std::isspace(ch);
     }).base(), s.end());
 }

// trim from both ends (in place)
 static inline string trim(string s) {
     ltrim(s);
     rtrim(s);
     string value(s);

     return value;
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

 string copy(string name, unsigned short start, unsigned short end) {
     string tmp;

     start--;
     end--;

     while (start <= end) {
         tmp = tmp + name[start];
         start++;
     }

     return(tmp);
 }

 string lstr(string s1, unsigned short l) {
     if ( s1.length() <= l )
         return(s1);
     else
         return(copy(s1,1,l) );
 }

 string rstr(string s1, unsigned short l) {
     if (s1.length() <= l)
         return(s1);
     else
         return( copy(s1, (s1.length()-l)+1 ,l) );
 }

 string to_Uppercase (string s){
    transform(s.begin(), s.end(), s.begin(), (int (*)(int))std::toupper);

    return s;
 }




/*
 * Encode function
 * Purpose: Changes bits of characters in file effectively encoding them.
 */
 string encode(string s, string lock_code) {
     unsigned long i;
     unsigned int lock_pos = 0;
     unsigned int this_dat = 0;
     unsigned int lock_dat = 0;
     size_t lim;

     if (lock_code.compare("") ) {
         lim = s.length();
         for (i = 0; i < lim; i++) {
             lock_pos++;
             if (lock_pos > lock_code.length())
                 lock_pos = 1;
             if ((char) s[i] <= 31 || (((s[i] & (~127)) != 0) && ((s[i] & 255) <= 255)))
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
     unsigned int i, k;
     string s2;

     if( (s1.length() == 0) || (s1[1] == ';')){
         s1 = "";
     }else{
         k = 0;
         for(i = s1.length();i >= 1; i-- ){
             if( s1[i] == ';' ){
                 k = i;
             }
         }
         if(k > 0){
             s1 = lstr(s1, k-1);
         }
     }
     s2 = "";
     char arr[] = {' ', '\b', '\v', '\n', ','};

     for(i = 1; i <= s1.length(); i++)
          for ( k = 0; k < 5; k++)
               if ( s1[i] != arr[k] )
                    s2 = s2 + s1[i];
	       else
		    if ( s2.compare("") ) { // If s2 does not equal nothing, then
		         s = s + s2 + " ";
		         s2 = "";
		    }


     if ( s2.compare("") )
          s = s + s2;
     return s;
 }

 /**
 *   Write line function. - We should adapt this for strings and file io
  *                             -- See below in main function for reference.
  *
 **/

 void write_line(string s, string s1, filebuf * f2write, string lock_code) {
     if ( f2write -> is_open() ) {
          s = prepare(s, s1);
          if(s.length() > 0){
              s = encode(s, lock_code);
              f2write -> sputn(s.c_str(), s.length());
          }
     }
 }

 string base_name(string name) {
	unsigned short k;
	string s1;

	k = 1;
	while ( (k <= name.length()) && (name[k] == '.') ) {
		s1 = s1 + name[k];
		k++;
	}
	return(s1);
}
 string no_path(string fn) {
     short i, k;

     k = -1;
     for (i = fn.length(); i >= 0; i--)
         if ((!fn.compare("\\") || (fn[i] != ':')) && (k < i))
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
     string iocode;

     if (!exist(thisfile) ) {
         afile.open(thisfile, std::fstream::out);
         afile.close();

         std::remove(thisfile.c_str());
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
   //char date [9];
   unsigned int i, k;
   time_t now = time(0);

   srand (time(NULL));

   ifstream f1read;
   ofstream f2write;
   ofstream f1write;

   if(argc < 1 || argc > 3){
       cout << "Usage: ATRLOCK <robot[.at2]> [locked[.atl]]";
       return EXIT_FAILURE;
   }

   string arg(argv[1]);
   string arg2(argv[2]);

   fn1 = trim(to_Uppercase(arg));

   if( !fn1.compare(base_name(fn1)) ){
       fn1 = fn1 + ".AT2";
   }
   if(f1read.is_open()) {
       cout << "Robot " << fn1 << " not found!";
       return EXIT_FAILURE;
   }

   if(argc == 2) {
       fn2 = trim(to_Uppercase(arg2));
   }else {
       fn2 = base_name(fn1 + ".ATL");
   }
   if(fn2 == base_name(fn2)) {
       fn2 = fn2 + ".ATL";
   }
   if(!valid(fn2)) {
       cout << "Output name " << fn2 << " not valid!";
       return EXIT_FAILURE;
   }
   if( !fn1.compare(fn2)) {
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
   f2write << "; " << no_path(base_name(fn1)) << "Locked on " << ctime(&now);
   f2write << ";------------------------------------------------------------------------------";
   lock_code = "";
   k = rand() % 21 + 20;
   for (i = 1; i < k; i++)
   {
       lock_code = lock_code + char(rand() % 32 + 65);
   }
   f2write << "#LOCK" << LOCKTYPE << " " << lock_code;


   /* Decode lock-Code */
   for (i = 0; i < sizeof(lock_code); i++) //length isn't working find fix
       {
     lock_code[i] = char(lock_code[i] - 65); // lookup ord when possible)
   }

   printf("Encoding: %s ...", fn1.c_str());

   //Encode robot
   s = trim(s);
   if((s.length()) > 0){
       write_line("", to_Uppercase(s1), f2write.rdbuf(), lock_code);
   }
   while(!f1read.eof()){
       if (f1read.is_open()) {
           while (getline(f1read,s1)) {
               cout << s1 << endl;
           }
           f1read.close();
       }
       write_line(s, s1, f1write.rdbuf(), lock_code);
   }
  cout << "Done. Used LOCK Format #" << LOCKTYPE << ".";
  cout << "Only ATR2 v2.08 or later can decode ";
  cout << "Locked robot saved as " << fn2;

  f2write.close();

}
