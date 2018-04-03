#include "filelib.h"
#include <iostream>

fstream		err_log;
uint16_t		textattr;
string		workstr;


// Copy function was included in base pascal compiler. Recoding it here
string copy(string name, uint16_t amount, uint16_t start) {
	string temp = "";

	// err_log << "Begin Copy" << endl;
	// err_log << "Starting name: " << name << endl;
	// err_log << "start: " << to_string(start) << "\tcount: " << to_string(count) << endl;

	cout << "name is " << name << endl;
	cout << "amount is " << amount << endl;
	cout << "start is " << start << endl;
	start++;
	temp = name.substr(start, amount);



	// err_log << "Ending name: " << temp << endl << "End copy" << endl;
	cout << "Temp is " << temp << endl;
	return temp;
}


string addfront(string b, uint16_t l) {
	while (b.length() < l)
		b = " " + b;

	return b;
}


string addrear(string b, uint16_t l) {
	while (b.length() < l)
		b = b + " ";

	return b;
}


string lstr(string s1, uint16_t l) {
	if (s1.length() <= l)
		return s1;
	else
		return (copy(s1, 1, l));
}


string rstr(string s1, uint16_t l) {
	// err_log << "Begin rstr" << endl;
	// err_log << "String :" << s1 << "\tl: " << to_string(l) << endl;

	if (s1.length() <= l) {
		return s1;
	}
	else {
		// err_log << "Rstr, length less than string" << endl;
		cout << "rstr:s1 is " << s1 << endl;
		cout << "rstr:s1.length() - l + 1 is " << (s1.length() - l + 1) << endl;
		return (copy(s1, (s1.length() - l + 1), l));
	}
}


bool exist(string thisfile) {
	fstream afile;
	bool return_type = false;

	afile.open(thisfile, fstream::in);

	if (afile.good()) {
		afile.close();

		return_type = true;
	}

	return return_type;
}


bool valid(string thisfile) {
	fstream afile;
	bool ret_type = true;


	if (!exist(thisfile)) {
		afile.open(thisfile, fstream::out);

		if (afile.good())
			afile.close();
	}

	return ret_type;
}


string name_form(string name) {
	uint16_t k;
	string s1, s2;

	s1 = "";
	s2 = "";

	k = 0;

	if (!name.compare(".") || !name.compare(".."))
		return (addrear(name, 12));

	while ((k <= name.length()) && (name.at(k) != '.')) {
		s1 = s1 + name.at(k);
		k++;
	}

	if (k < name.length()) {
		k++;

		while ((k <= name.length()) && (name.at(k) != '.')) {
			s2 = s2 + name.at(k);
			k++;
		}
	}

	return ((addrear(s1, 9) + addrear(s2, 3)));
}


string exten(string name) {
	uint16_t k;
	string s1, s2;

	s1 = "";
	s2 = "";
	k = 0;

	while ((k <= name.length()) && (name.at(k) != '.')) {
		s1 = s1 + name.at(k);
		k++;
	}

	if (k < name.length()) {
		k++;

		while ((k <= name.length()) && (name.at(k) != '.')) {
			s2 = s2 + name.at(k);
			k++;
		}
	}

	return (addrear(s2, 3));
}


std::string base_name(std::string name) {
	// err_log << "Begin base_name" << endl;
	uint16_t k;
	string s1;

	s1 = "";

	k = 0;

	while ((k <= name.length()) && (name.at(k) != '.')) {
		s1 = s1 + name.at(k);
		k++;
		if (k == name.length())
			break;
	}

	// err_log << "End base_name" << endl;
	return s1;
}


/*
ReadOnly 	= 0x01
Hidden		= 0x02
SysFile		= 0x04
VolumeID	= 0x08
Directory	= 0x10
Archive		= 0x20
AnyFile		= 0x3f
*/

string attribs(uint8_t b) {

	string s1;

	s1 = "";

	if (b & 0x01) // Check for read only
		s1 = s1 + "R";
	else
		s1 = s1 + ".";

	if (b & 0x02)
		s1 = s1 + "H";
	else
		s1 = s1 + ".";

	if (b & 0x04)
		s1 = s1 + "S";
	else
		s1 = s1 + ".";

	if (b & 0x20)
		s1 = s1 + "A";
	else
		s1 = s1 + ".";

	return s1;
}


string path(string fn) {

	uint16_t i, k;

	k = 0;

	for (i = fn.length() - 1; i >= 0; i--)
		if (((fn.at(i) == '\\') || (fn.at(i) == ':')) && (k < i))
			k = i;

	if (k != 0)
		return (lstr(fn, k));
	else
		return "";
}


string no_path(string fn) {
	// err_log << "Begin no_path" << endl;
	uint16_t i, k;
	k = 0;

	cout << to_string(fn.length()) << endl;

	for (i = 0; i < fn.length(); i++) {

		if (((fn[i] == '\\') || (fn[i] == '/') || (fn[i] == ':'))) {
			k = i;
		}


	}

	// err_log << "End no_path (Before returning)" << endl;
	cout << "K is " << k << endl;
	k = fn.length() - k;
	cout << "\tK is " << k << endl;
	if (k != 0)
		return (rstr(fn, fn.length() - k));


	else
		return (fn);

}


int32_t file_length(string fn) {
	int32_t file_size;

	fstream sr(fn, fstream::binary | fstream::ate);

	if (sr.good()) {
		file_size = sr.tellg();
		sr.close();
	}
	else
		file_size = 0;

	return file_size;
}
