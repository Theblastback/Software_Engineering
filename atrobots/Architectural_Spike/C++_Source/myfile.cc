#include "Headers/myfile.h"
#include "Headers/filelib.h"

using namespace std;


string lstr(string s1, unsigned short l) {
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

	remove(fn.c_str());
}

void rename_file(string fn, string fn2) {
	if ( !exist(fn) )
		return;

	std::rename(fn.c_str(), fn2.c_str());
}

// Original function was read_file(unsigned short h, void * ploc, short len)
// H is file address, len is how many bytes to read, ploc is unused(?)

void read_file(string name, short byte_len) {
	fstream file(name, fstream::in | fstream::binary); // Read file in binary mode

	if ( file.good() ) {
		char buff[byte_len];

		file.read(buff, byte_len);

		file.close();
	}
}

// data type word == unsigned short
// data type longint == int

void write_file(string name, string text) {
	fstream file(name, fstream::out|fstream::binary);

	if ( file.good() ) {
		file << text;
		file.close();
	}
}
