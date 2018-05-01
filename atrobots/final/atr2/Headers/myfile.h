#ifndef MYFILE_H
#define MYFILE_H

#include "filelib.h"

#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>


using namespace std;

extern string	the_name;
extern uint8_t	the_eof;


extern int32_t	file_size(string *);
extern void		delete_file(string *);
extern void		rename_file(string *, string *);
extern string	lstr(string *, int32_t);

extern void		open_filein(FILE *, string *);
extern void		open_fileout(FILE *, string *);
extern void		open_fileboth(FILE *, string *);
extern void		create_fileout(FILE *, string *);

extern void		read_file(FILE *, void *, int16_t *);
extern void		write_file(FILE *, void *, int16_t *);
extern void		read_long(FILE *, void *, uint16_t *);
extern void		write_long(FILE *, void *, uint16_t *);
extern void		close_file(FILE *);

extern void		file_seek(FILE *, uint16_t, uint8_t, FILE *);
extern void		long_seek(FILE *, int, uint8_t, FILE *);

#endif
