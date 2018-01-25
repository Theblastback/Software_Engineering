#include "Headers/filelib.h"

string addfront(string b, short l) {
	// Find new length of font
	while (b.length() < l)
		b = " " + b;

	return(b);
}

string addrear(string b, short l) {
	while ( b.length() < l)
		b = b + " ";

	return(b);
}

// Newly added. Created to emulate the copy frunction from pascal
string copy(string name, short start, short end) {
	string tmp;

	start--;
	end--;

	while (start <= end) {
		tmp = tmp + name[start];
	 	start++;
	}

	return(tmp);
}

string lstr( string s1, short l) {
	if (s1.length() <= l)
		return(s1);
	else
		lstr( (copy(s1,1,l) );
}

string rstr(string s1, short l) {
	if (s1.length() <= l) 
		return(s1);
	else
		return( copy(s1, (s1.length()-l)+1 ,l) );
}

bool exist(string thisfile) {
	fstream afile;
	bool	return_value;

	afile.open(thisfile, std::fstream::in);
	if ( (return_value = afile.good()) )
		afile.close();

	return(return_value);
}

bool valid(string thisfile) {
	fstream afile;
	bool check;

	if ( !exist(thisfile) ) {
		afile.open(thisfile, std::fstream::out);
		afile.close();

		std::remove(thisfile);
		return(false);
	} else
		return(true);
}

string name_form( string name ) {
	short k;
	string s1, s2;

	k:=1;
	if ( name.compare(".")  || name.compare("..") ) {
		return(addrear(name,12));
	}

	while ( k <= name.length() ) and ( !name[k].compare(".")) ) {
		s1 = s1 + name[k];
		k++;
	}

	if ( k < name.length() ) {
		k++;
		while ( (k <= name.length()) && ( name[k].compare(".") ) {
			s2:=s2+name[k];
			k++;
		}
	}
	return( (addrear(s1,9)+addrear(s2,3) );
}

string exten(string name ) {
	short k;
	string s1, s2;

	k:=1;

	while ( (k <= name.length()) && (name[k].compare(".") ) {
		s1 = s1 + name[k];
		k++;
	}

	if ( k < name.length() ) {
		k++;
		while ( (k <= name.length()) && (name[k].compare(".") ) {
			s2 = s2 + name[k];
			k++;
		}
	}
	return(addrear(s2,3));
}


string base_name(string name) {
	short k;
	string s1;

	k:=1;
	while ( (k <= name.length()) && (name[k].compare(".")) ) {
		s1 = s1 + name[k];
		k++;
	}
	return(s1);
}


string attribs(unsigned char b) {
	string s1;
	s1 = " ";

	// Check for readonly status
	if ( b & 0x1 )
		s1 = s1 + "R"
	else
		s1 = s1 + ".";

	// Check for hidden status
	if ( b and 0x2 )
		s1 = s1 + "H"
	else
		s1 = s1 + ".";

	// Check if file is system file
	if ( b and 0x4 )
		s1 = s1 + "S"
	else
		s1 = s1 + ".";

	// Check if file is archive
	if ( b and 0x20 )
		s1 = s1 + "A"
	else
		s1 = s1 + ".";

	return(s1);
}

string path(string fn) {
	short i, k;

	k = -1;

	// In pascal, strings start at index of 1, but in c they start at 0
	for ( i = fn.length(); i >= 0; i--)
		if ( (!fn[i].compare("\") || !fn[i].compare(":")) && ( k < i ) )
			k = i;


	if ( k != -1 )
		return( lstr(fn,k) )
	else
		return("");
}

string no_path(string fn) {
	short i, k;

	k = -1;
	for ( i = fn.length(); i >= 0; i--)
	if ( (!fn[i].compare("\") || !fn[i].compare(":")) && (k<i) )
		k:=i;

	if ( k != -1 )
		return( rstr(fn,length(fn)-k )
	else
		return(fn);
}

short file_length(string fn) {
	struct stat buf;
	int buf_ret = stat(fn.c_str(), &buf);

	return(buf_ret == 0 ? buf.st_size() : 0);
}
