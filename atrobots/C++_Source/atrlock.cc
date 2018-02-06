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
 static inline void ltrim(string * s) {
     s -> erase(s -> begin(), std::find_if(s -> begin(), s -> end(), [](int ch) {
         return !std::isspace(ch);
     }));
 }

// trim from end (in place)
 static inline void rtrim(string * s) {
     s -> erase(std::find_if(s -> rbegin(), s -> rend(), [](int ch) {
         return !std::isspace(ch);
     }).base(), s -> end());
 }

// trim from both ends (in place)
 static inline void trim(string * s) {
     ltrim(s);
     rtrim(s);
 }

// trim from start (copying)
 static inline std::string ltrim_copy(string * s) {
     string value(s -> c_str());

     ltrim(&value);

     return value;
 }

// trim from end (copying)
 static inline std::string rtrim_copy(string * s) {
     string value(s -> c_str());

     rtrim(&value);

     return value;
 }

// trim from both ends (copying)
 static inline std::string trim_copy(string * s) {
     string value(s -> c_str());

     trim(&value);

     return value;
 }

 string copy(string * name, unsigned short start, unsigned short end) {
     string tmp;

     start--;
     end--;

     while (start <= end) {
         tmp = tmp + name -> at(start);
         start++;
     }

     return(tmp);
 }

 string lstr(string * s1, unsigned short l) {
     if ( s1 -> length() <= l )
         return(*s1);
     else
         return(copy(s1,1,l) );
 }

 string rstr(string * s1, unsigned short l) {
     if (s1 -> length() <= l)
         return(*s1);
     else
         return( copy(s1, (s1 -> length()-l)+1 ,l) );
 }

 void to_Uppercase (string * s){
    transform(s -> begin(), s -> end(), s -> begin(), (int (*)(int))std::toupper);
 }




/*
 * Encode function
 * Purpose: Changes bits of characters in file effectively encoding them.
 */
 void encode(string * s, string * lock_code) {
     unsigned long i;
     unsigned int lock_pos = 0;
     unsigned int this_dat = 0;
     unsigned int lock_dat = 0;
     size_t lim;

     if (lock_code -> compare("") ) {
         lim = s -> length();
         for (i = 0; i < lim; i++) {
             lock_pos++;
             if (lock_pos > lock_code -> length())
                 lock_pos = 1;
             if ((char) s -> at(i) <= 31 || (((s -> at(i) & (~127)) != 0) && ((s -> at(i) & 255) <= 255)))
                 s -> at(i) = ' ';
             this_dat = s -> at(i) & 15;
             s -> at(i) = (char) ((s -> at(i) ^ lock_code -> at(lock_pos - 1) ^ lock_dat) + 1);
             lock_dat = this_dat;
         }
     }
 }

/*
 * Prepares files to be encoded.
 */
 void prepare(string * s, string * s1) {
     unsigned int i, k;
     string s2("");

     if( (s1 -> length() == 0) || (s1 -> at(1) == ';')){
         *s1 = "";
     }else{
         k = 0;
         for(i = s1 -> length() ;i >= 1; i-- ){
             if( s1 -> at(i) == ';' ){
                 k = i;
             }
         }
         if(k > 0){
             lstr(s1, k-1);
         }
     }

     char arr[] = {' ', '\b', '\v', '\n', ','};

     for(i = 1; i <= s1 -> length(); i++)
          for ( k = 0; k < 5; k++)
               if ( s1 -> at(i) != arr[k] )
                    s2 = s2 + s1 -> at(i);
	       else
		    if ( s2.compare("") ) { // If s2 does not equal nothing, then
		         *s = *s + s2 + " ";
		         s2 = "";
		    }


     if ( s2.compare("") )
          *s = *s + s2;
 }

 /**
 *   Write line function. - We should adapt this for strings and file io
  *                             -- See below in main function for reference.
  *
 **/

 void write_line(string * s, string * s1, filebuf * f2write, string * lock_code) {
     if ( f2write -> is_open() ) {
          prepare(s, s1);
          if(s -> length() > 0){
              encode(s, lock_code);
              f2write -> sputn(s -> c_str(), s -> length());
          }
     }
 }

 string base_name(string * name) {
	unsigned short k;
	string s1("");

	k = 0;
	while ( (k <= name -> length() - 1) && (name -> at(k) != '.') ) {
		s1 = s1 + name -> at(k);
//		cout << "base_name: s1: " << s1 << endl;
		k++;
	}
	cout << endl;
	return(s1);
}
 void no_path(string * fn) {
     short i, k;

     k = -1;
     for (i = fn -> length() - 1; i >= 0; i--)
         if ((!fn -> compare("\\") || (fn -> at(i) != ':')) && (k < i))
             k = i;
     if (k != -1)
         rstr(fn, fn -> length() - k);
 }

 bool exist(string * thisfile) {
     fstream	afile;
     bool	return_value;

     afile.open(*thisfile, std::fstream::in);
     if ( (return_value = afile.good()) )
         afile.close();

	

     return(return_value);
 }

 bool valid(string * thisfile) {
     fstream afile;

//	cout << "Valid: thisfile: " << *thisfile << endl;

     if (!exist(thisfile) ) {
         afile.open(*thisfile, std::fstream::out);
         afile.close();
//	cout << "Valid: Does not exist" << endl;

         // std::remove(thisfile -> c_str());

     }
         return(true);
 }

int main(int argc, char *argv[]){
   /*
    * Variables - direct from the ATRLOCK program.
    */
    //char *f1 [256], *f2 [256], *s[256], *s1[256], *s2[256], *lock_code[256];
   string fn1, fn2,f1,f2,s,s1,s2, lock_code;
   //char date [9];
   unsigned int i, k, num_lock_code;
   time_t now = time(0);

   srand (time(NULL));

   ifstream f1read;
   ofstream f2write;
   ofstream f1write;


   if( (argc == 1) || (argc > 3) ){
       cout << "Usage: ATRLOCK <robot[.at2]> [locked[.atl]]";
       exit(EXIT_FAILURE);
   }



   string arg = argv[1];

   string arg2 = argv[2];

   to_Uppercase(&arg);
   trim(&arg);
   fn1 = arg;

   if( !fn1.compare(base_name(&fn1)) ){

       fn1 = fn1 + ".AT2";
   }


   if(f1read.is_open()) {
       cout << "Robot " << fn1 << " not found!";
       return EXIT_FAILURE;
   }

   if(argc == 2) {
       to_Uppercase(&arg2); trim(&arg2);
       fn2 = arg2;
	cout << "fn2: " << fn2 << endl;
   }else {
       arg2 = fn1 + ".ATL";
       fn2 = base_name(&arg2);
//		cout << "fn2 else: " << fn2 << endl;

   }
   if(fn2 == base_name(&fn2)) {
       fn2 = fn2 + ".ATL";
   }

   if(!valid(&fn2)) {
       cout << "Output name " << fn2 << " not valid!";
       return EXIT_FAILURE;
   }
   if( !fn1.compare(fn2)) {
       cout << "Filenames cannot be the same!";
       return EXIT_FAILURE;
   }


//	cout << "Fn1: " << fn1 << "\tFn2: " << fn2 << endl;

   f1read.open(fn1.c_str());
   f2write.open(fn2.c_str());

   /* copy comment header */
 f2write << ";------------------------------------------------------------------------------";
   s = "";
   do {
     getline(f1read, s);
     trim(&s); //replace with our language equivalent
     if (s[1] ==  ';') {
       to_Uppercase(&s);
       f2write << s;
       s = '\n';
     }

   } while( !f1read.eof() && (!s.compare("") )); //fix not eof

   /**
   * lock header
   **/

   arg = base_name(&fn1);
   no_path(&arg);

//	cout << "Path of arg: " << arg << endl;

   f2write << ";------------------------------------------------------------------------------";
   f2write << "; " << arg << "Locked on " << ctime(&now);
   f2write << ";------------------------------------------------------------------------------";
   lock_code = "";
   k = rand() % 21 + 20;
   for (i = 1; i < k; i++)
   {
       lock_code = lock_code + char(rand() % 32 + 65);
   }
   f2write << "#LOCK" << LOCKTYPE << " " << lock_code;

//	cout << "Wrote to file" << endl;


   /* Decode lock-Code */
	// need to convert string to number
	sscanf(lock_code.c_str(), "%u", &num_lock_code);
//	num_lock_code = 0;

//	for (i = 0; i < lock_code.length(); i++)
//		num_lock_code = (num_lock_code * 10) + (lock_code[i] - '0');


   for (i = 0; i < num_lock_code; i++) //length isn't working find fix
       {
     lock_code[i] = char(lock_code[i] - 65); // lookup ord when possible)
   }

   cout << "Encoding: " << fn1 << "..." << endl;

   //Encode robot
   trim(&s);
   if((s.length()) > 0){
       to_Uppercase(&s1);
       arg = "";
       write_line(&arg, &s1, f2write.rdbuf(), &lock_code);
   }
   while(!f1read.eof()){
       if (f1read.is_open()) {
           while (getline(f1read,s1)) {
               cout << s1 << endl;
           }
           f1read.close();
       }
       write_line(&s, &s1, f1write.rdbuf(), &lock_code);
   }
  cout << "Done. Used LOCK Format #" << LOCKTYPE << ".";
  cout << "Only ATR2 v2.08 or later can decode ";
  cout << "Locked robot saved as " << fn2;

  f2write.close();

}
