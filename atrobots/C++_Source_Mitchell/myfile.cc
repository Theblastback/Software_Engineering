#include "Headers/myfile.h"

string	the_name;
uint8_t the_eof;

string lstr(string s1, int32_t l){
	if (s1.length() <= 1) {
		return s1;
	} else {
		return copy(s1, 1, l);
	}
}

int32_t file_size(string fn){
	FILE * f;
	int32_t size;

	if( !exist(fn) ) {
		return -1;
	} else {
		f =fopen(fn.c_str(), "r");
		if ( f == NULL )
			return -1;
		else {
			fseek(f, 0L, SEEK_END);
			size = ftell(f);
			fclose(f);
			return size;
		}
	}

}


void delete_file(string fn) {
	if ( exist(fn) ) {
		remove(fn.c_str());
	}
}


void rename_file(string fn, string fn2) {
	if ( exist(fn) )
		rename(fn.c_str(), fn2.c_str());
}


// Changed param 1 from word pointer (memory address) to file point32_ter
void open_filein(FILE * fp, string fn) {
	fp = fopen(fn.c_str(), "rb");

	the_name = fn;
}

void open_fileboth(FILE * fp, string fn) {
	the_name = fn;

	if ( exist(fn) ) {
		// If exist, read/write file in binary, keeping its data
		fp = fopen(fn.c_str(), "rb+");
	} else {
		// If not exist, create file and write/read file in binary
		fp = fopen(fn.c_str(), "wb+");
	}
}

void open_fileout(FILE * fp, string fn) {
	the_name = fn;

	fp = fopen(fn.c_str(), "wb");
}

void create_fileout(FILE * fp, string fn) {
	open_fileout(fp, fn);
}


void read_file(FILE * fp, void * ploc, int16_t *bytes) {
	*bytes = fread(ploc, 1, *bytes, fp);
}

void write_file(FILE * fp, void * ploc, int16_t * bytes) {
	*bytes = fwrite(ploc, 1, *bytes, fp);
}

void read_long(FILE * fp, void * ploc, uint16_t *bytes) {
	*bytes = fread(ploc, 1, *bytes, fp);
}

void write_long(FILE * fp, void * ploc, uint16_t * bytes) {
	*bytes = fwrite(ploc, 1, *bytes, fp);
}

void close_file(FILE * fp) {
	fclose(fp);
}

void file_seek(FILE * h, uint16_t amount, uint8_t position, FILE * new_pos) {
	if ( position == 0 )
		fseek(h, amount, SEEK_SET);
	else if ( position == 1 )
		fseek(h, amount, SEEK_CUR);
	else if ( position == 2 )
		fseek(h, amount, SEEK_END);

	new_pos = h;
}

void long_seek(FILE * h, int32_t amount, uint8_t position, FILE * new_pos) {
	if ( position == 0 )
		fseek(h, amount, SEEK_SET);
	else if ( position == 1 )
		fseek(h, amount, SEEK_CUR);
	else if ( position == 2 )
		fseek(h, amount, SEEK_END);

	new_pos = h;
}
