#include "atr2func.h"
#include "filelib.h"
#include <ctime>
#include <iostream>
#include <string>

#define LOCKTYPE	3

// There shouldn't be a conflict with name declarations as the program will want to access the local variable, not the global
string	fn1, fn2;
fstream	f1, f2;
string	s, s1, s2, lock_code;
int16_t	i, j, k, lock_pos, lock_dat, this_dat;


string encode(string s) {
	int16_t	i;
	static int count = 1;
	cout << "atrlock encode section: s is '" << s <<"'" << endl;
	if (lock_code.compare("")) {
		for (i = 0; i < s.length(); i++) {
			lock_pos++;
			if (lock_pos >= lock_code.length()) {
				lock_pos = 0;
			}

			if (((s[i] >= 0) && (s[i] <= 31)) || ((s[i] >= 128) && (s[i] <= 255))) {
				s[i] = ' ';
			}

			this_dat = s[i] & 15;
			cout << s[i] << endl;
			s[i] = (char)((s[i] ^ (lock_code[lock_pos] ^ lock_dat)) + 1);
			cout << "\t" << s[i] << endl;
			lock_dat = this_dat;
		}
	}
	cout << "#######################################";
	cout << count++ << " " << s << endl;
	return s;
}


string prepare(string s, string s1) {
	int16_t i, k;
	string s2;
	cout << "atrlock prepare: s1 is '" << s1 << "'" << endl;
	if ((s1.length() == 0) || (s1[0] == ';')) {
		s1 = "";
	}
	else {
		k = 0;

		for (i = (s1.length()-1); i >= 0; i--) {
			if (s1[i] == ';')
				k = i;
		}

		if (k > 0)
			s1 = lstr(s1, k - 1);
		cout << "prepare s1 after lstr is '" << s1 << "'" << endl;
	}

	s2 = "";
	for (i = 0; i < s1.length(); i++) {
		if (!((s1[i] == ' ') || (s1[i] == 8) || (s1[i] == 9) || (s1[i] == 10) || (s1[i] == ','))) {
			s2 = s2 + s1[i];
		}
		else {
			if (s2.compare("")) {
				s = s + s2 + " ";
				s2 = "";
			}
		}
	}
	cout << "prepare s is: '" << s << "'" << endl;
	if (s2.compare(""))
		s = s + s2;
	cout << "prepare s after compare is: '" << s << "'" << endl;
	return s;
}

void write_line(string s, string s1) {
	cout << "atrlock write_line: s1 is '" << s1 << "'" << endl;
	s = prepare(s, s1);
	cout << "write_line: s is '" << s << "'" << endl;
	if (s.length() > 0) {
		s = encode(s);
		f2 << s << endl;
	}
}


int main(int argc, char ** argv) {
	//srand(time(NULL)); // Imitates the randomize function
	string arg1, arg2;

	// To put in the date in the header
	time_t cur_time = time(NULL);
	tm *date = localtime(&cur_time);




	lock_pos = 0;
	lock_dat = 0;

	if ((argc < 2) || (argc > 3)) {
		std::cout << "Usage: atrlock <robot[.at2]> [locked[.atl]]" << endl;
		exit(EXIT_FAILURE);
	}

	arg1 = argv[1];

	fn1 = btrim(ucase(arg1));

	if (!fn1.compare(base_name(fn1)))
		fn1 = fn1 + ".AT2";

	if (!exist(fn1)) {
		std::cout << "Robot " << fn1 << " not found!" << endl;
		exit(EXIT_FAILURE);
	}

	if (argc == 3) {
		arg2 = argv[2];
		fn2 = btrim(ucase(arg2));
	}
	else {
		fn2 = base_name(fn1) + ".ATL";
	}

	if (!valid(fn2)) {
		cout << "Filenames can not be the same!" << endl;
		exit(EXIT_FAILURE);
	}

	f1.open(fn1, fstream::in); // Was being read in later in the code
	f2.open(fn2, fstream::out);

	// Copy comment header
	f2 << ";------------------------------------------------------------------------------" << endl;
	s = "";
	while ((!f1.eof()) && (!s.compare(""))) {
		std::getline(f1, s);
		s = btrim(s);

		if (s[0] == ';') {
			f2 << s << endl;
			s = "";
		}
	}


	// Lock the header
	string temp5 = base_name(fn1);

	f2 << ";------------------------------------------------------------------------------" << endl;
	f2 << "; " << no_path(temp5) << " Lock on " << 1 + date->tm_mon << "/" << date->tm_mday << "/" << 1900 + date->tm_year << endl;
	f2 << ";------------------------------------------------------------------------------" << endl;

	//exit(0);
	lock_code = "";

	k = 15;
	//k = (rand() % 21) + 20;
	for (i = 1; i <= k; i++) {
		lock_code = lock_code + (char)((15 % 33) + 65);
	}
	//lock_code = "";
	lock_code = "RLQUDX`EBH^CLDAHFGWWW`DFWHIQ";
	f2 << "#LOCK " << LOCKTYPE << " " << lock_code << endl;

	// Decode lock_code
	for (i = 0; i < lock_code.length(); i++) {
		lock_code[i] = ((char)lock_code[i] - 65);
	}

	std::cerr<< "Encoding " << fn1 << "...";

	// Encode robot
	s = btrim(s);
	if (s.length() > 0)
		write_line("", ucase(s));
	while (!f1.eof()) {
		std::getline(f1, s1);
		s = "";
		s1 = btrim(ucase(s1));
		cout << "atrlock main before encode is: '" << s1 << "'" << endl;
		write_line(s, s1);
	}

	cout << "Done. Used LOCK Format #" << LOCKTYPE << "." << endl;
	cout << "Only ATR2 v2.08 or later can decode." << endl;
	cout << "Locked robot saved as \"" << fn2 << "\"" << endl;

	f1.close();
	f2.close();

	return EXIT_SUCCESS;
}
