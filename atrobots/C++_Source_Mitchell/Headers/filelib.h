#ifndef FILELIB_H
#define FILELIB_H

#include <fstream>
#include <string>

// Null is already defined as 0
#define BELL	7
#define ESC	27
#define F10	0x44

using namespace std;



extern unsigned short 	textattr;
extern string	workstr;


// Strings must be passed into functions as pointers or else null string errors may appear

extern string	copy(string, unsigned short, unsigned short);

extern string	addfront(string, unsigned short);
extern string	addrear(string, unsigned short);
extern string	lstr(string, unsigned short);
extern string	rstr(string, unsigned short);

extern bool 	exist(string);
extern bool 	valid(string);
extern string	name_form(string);
extern string	exten(string);
extern string	attribs(unsigned char);
extern string	path(string);
extern string	no_path(string);
extern int	file_length(string);
extern string	base_name(string);

#endif
