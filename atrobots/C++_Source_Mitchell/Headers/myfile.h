#ifndef MYFILE_H
#define MYFILE_H

#include "filelib.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

extern string		the_name;
extern unsigned char	the_eof;


extern int	file_size(string *);
extern void	delete_file(string *);
extern void	rename_file(string *, string *);
extern string	lstr(string *, int);

extern void	open_filein(FILE *, string *);
extern void	open_fileout(FILE *, string *);
extern void	open_fileboth(FILE *, string *);
extern void	create_fileout(FILE *, string *);

extern void	read_file(FILE *, void *, short *);
extern void	write_file(FILE *, void *, short *);
extern void	read_long(FILE *, void *, unsigned short *);
extern void	write_long(FILE *, void *, unsigned short *);
extern void	close_file(FILE *);

extern void	file_seek(FILE *, unsigned short, unsigned char, FILE *);
extern void	long_seek(FILE *, int, unsigned char, FILE *);

#endif
