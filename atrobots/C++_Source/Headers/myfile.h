#ifndef MYFILE_H
#define MYFILE_H
#include <string>

using namespace std;

string the_name;
unsigned char the_eof;

extern string lstr(string, short);
extern int file_size(string);
extern void delete_file(string);
extern void rename_file(string, string);
extern void read_file(string, short);
extern void write_file(unsigned short, string);

#endif
