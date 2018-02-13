#ifndef MYFILE_H
#define MYFILE_H

#include "filelib.h"

string	the_name;
unsigned char	the_eof;


extern int	file_size(string *);
extern void	delete_file(string *);
extern void	rename_file(string *, string *);

// All these parameters for these functions can be changed, or even removed. Some of these functions can be condensed down into just read file and write file
extern void	open_filein(unsigned short, string *);
extern void	open_fileout(unsigned short, string *);
extern void	open_fileboth(unsigned short, string *);
extern void	create_fileout(unsigned short, string *);
extern void	read_file(unsigned short, void *, short);
extern void	write_file(unsigned short, void *, short);
extern void	close_file(unsigned short);
extern void	file_seek(unsigned short, unsigned short, int, unsigned char, int);
extern void	long_seek(unsigned short, int, unsigned short, int);
extern void	read_long(unsigned short, void *, unsigned short);
extern void	write_long(unsigned short, void *, unsigned short);

// Remember, word is only 16 bits long (I.e. short). Longint is only 32 bits long


#endif
