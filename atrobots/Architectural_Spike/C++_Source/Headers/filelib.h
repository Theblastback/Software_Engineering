#ifndef FILELIB_H
#define FILELIB_H

#define BELL	7
#define ESC	27
#define F10	0x44

#include <fstream>
#include <string>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>


unsigned short	textattr;
std::string		workstr;


/* wtf is this for?
  basex : byte = 1;
  basey : byte = 1;
  tempx : byte = 1;
  tempy : byte = 1;
  endx  : byte = 24;
  endy  : byte = 80;
*/


extern std::string addfront(std::string, unsigned short);
extern std::string addrear(std::string, unsigned short);
extern std::string copy(std::string, unsigned short, unsigned short);
extern std::string lstr( std::string, unsigned short);
extern std::string rstr(std::string, unsigned short);
extern bool exist(std::string);
extern bool valid(std::string);
extern std::string name_form(std::string);
extern std::string exten(std::string name);
extern std::string base_name(std::string);
extern std::string attribs(unsigned char);
extern std::string path(std::string fn);
extern std::string no_path(std::string);
extern short file_length(std::string);



#endif
