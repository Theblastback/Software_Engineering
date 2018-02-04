#include "Headers/myfile.h"
#include "Headers/filelib.h"


string lstr(string s1, short l) {
	if ( s1.length() <= l )
		return(s1);
	else
		return( copy(s1,1,l) );
}

int file_size(string fn) {
	if ( !exist(fn) )
		return(-1);

	return(file_length(fn));
}

void delete_file(string fn) {
	if ( !exist(fn) )
		return;

	remove(fm.c_str());
}

void rename_file(string fn, string fn2) {
	if ( !exist(fn) )
		return;

	std::rename(f.c_str(), fn2.c_str());
}

// Original function was read_file(unsigned short h, void * ploc, short len)
// H is file address, len is how many bytes to read, ploc is unused(?)

void read_file(string name, short byte_len) {
	fstream file(name, fstream::in | fstream::bin); // Read file in binary mode

	if ( file.good() ) {
		char *buff[byte_len];

		file.read(buff, byte_len);

		file.close();
	}
}

// data type word == unsigned short
// data type longint == int

void write_file(unsigned short name, string text);
	fstream file(name, fstream::out|fstream::bin);

	if ( file.good() ) {
		text >> file;
		file.close();
	}
}

/*	Likely unneeded at this point in time
Procedure file_seek(h,d:word; m:byte; var p:word);
var
 w:word;
begin

    __asm__("mov   %bx,     h"
            "mov   %cx,     %0"
            "mov   %dx,     d"
            "mov   %al,     m"
            "mov   %ah,     %42h"
            "int   %21h"
            "mov   w,      %ax");
 p:=w;
end;

Procedure long_seek(h:word; d:longint; m:byte; var p:longint);
var
 d1,d2:word;
begin
 d1:=d shr 16;
 d2:=d and $FFFF;

    __asm__("mov   %bx,     h"
            "mov   %cx,     d1"
            "mov   %dx,     d2"
            "mov   %al,     m"
            "mov   %ah,     %42h"
            "int   %21h"
            "mov   d1,      %ax"
            "mov   d2,      %dx");

 p:=d2; p:=p shl 16;
 p:=p or d1;
end;
*/
