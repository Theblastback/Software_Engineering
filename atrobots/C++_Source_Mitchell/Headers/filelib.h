#ifndef FILELIB_H
#define FILELIB_H

#include <fstream>
#include <string>

// Null is already defined as 0
#define BELL	7
#define ESC	27
#define F10	0x44

using namespace std;



extern uint16_t 	textattr;
extern string	workstr;


// Strings must be passed into functions as pointers or else null string errors may appear

extern string	copy(string, uint16_t, uint16_t);

extern string	addfront(string, uint16_t);
extern string	addrear(string, uint16_t);
extern string	lstr(string, uint16_t);
extern string	rstr(string, uint16_t);

extern bool 	exist(string);
extern bool 	valid(string);
extern string	name_form(string);
extern string	exten(string);
extern string	attribs(uint8_t);
extern string	path(string);
extern string	no_path(string);
extern int32_t		file_length(string);
extern string	base_name(string);

#endif
