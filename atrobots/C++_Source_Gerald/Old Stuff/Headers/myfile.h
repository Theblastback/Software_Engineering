#ifndef MYFILE_H
#define MYFILE_H

#include <string>

std::string the_name;
unsigned char the_eof;

extern std::string lstr(std::string, unsigned short);
extern int file_size(std::string);
extern void delete_file(std::string);
extern void rename_file(std::string, std::string);
extern void read_file(std::string, unsigned short);
extern void write_file(std::string, std::string);

#endif
