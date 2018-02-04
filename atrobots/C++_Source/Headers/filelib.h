#ifndef FILELIB_H
#define FILELIB_H

#define BELL	7
#define ESC	27
#define f10	0x44

#include <string>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>

using namespace std;

unsigned short	textattr;
string		workstr;


/* wtf is this for?
  basex : byte = 1;
  basey : byte = 1;
  tempx : byte = 1;
  tempy : byte = 1;
  endx  : byte = 24;
  endy  : byte = 80;
*/

extern string addfront(string, short);
extern string addrear(string, short);
extern string copy(string, short, short);
extern string lstr( string, short);
extern string rstr(string, short);
extern bool exist(string);
extern bool valid(string);
extern string name_form(string);
extern string exten(string name);
extern string base_name(string);
extern string attribs(unsigned char);
extern string path(string fn) {
extern string no_path(string);
extern short file_length(string);

#endif
