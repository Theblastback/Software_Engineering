#include "Headers/myfile.h" // filelib is already included in myfile
#include "Headers/atr2func.h"
#include <climits>
#include <ctime>


int get_rgb(short color) {
	int rgb;

	switch ( color ) {
	case BLACK:
		rgb = 0x0;
		break;

	case BLUE:
		rgb = 160 << 8;
		break;

	case GREEN:
		rgb = 173 << 16;
		break;

	case CYAN:
		rgb = (174 << 16) | (174 << 8);
		break;

	case RED:
		rgb = 175 << 24;
		break;

	case MAGENTA:
		rgb = (174 << 24) | (173 << 8);
		break;

	case BROWN:
		rgb = (174 << 24) | (87 << 16);
		break;

	case LIGHT_GRAY:
		rgb = 0xacacac << 8;
		break;

	case DARK_GRAY:
		rgb = (120 << 24) | (120 << 16) | (120 << 8);
		break;

	case LIGHT_BLUE:
		rgb = (86 << 24) | (86 << 16) | (255 << 8);
		break;

	case LIGHT_GREEN:
		rgb = (86 << 24) | (255 << 16) | (86 << 8);
		break;

	case LIGHT_CYAN:
		rgb = (87 << 24) | (255 << 16) | (255 << 8);
		break;

	case LIGHT_RED:
		rgb = (193 << 24) | (64 << 16) | (64 << 8);
		break;

	case LIGHT_MAGENTA:
		rgb = (255 << 24) | (56 << 16) | (255 << 8);
		break;

	case YELLOW:
		rgb = (255 << 24) | (56 << 16) | (255 << 8);
		break;

	case WHITE:
		rgb = 0xffffff << 8;
		break;
	}

	return rgb;
}


void arc(short cent_x, short cent_y, unsigned short st_angle, unsigned short end_angle, unsigned short radius) {
	// Utilizing the premade tables in this program
	// Converting a 360 degree circle to a 255 degree means every 1.4 degrees equals 1 degree
	short x1, x2, y1, y2;

	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);

	st_angle = st_angle / 1.4;
	end_angle = end_angle / 1.4;

	// Do this outside of the loop in order to set up loop
	x2 = (cost[st_angle] * radius) + cent_x;
	y2 = (sint[st_angle] * radius) + cent_y;

	for ( unsigned short angle = st_angle + 1; angle <= end_angle; angle++ ) {
		x1 = x2;
		y1 = y2;

		x2 = (cost[angle] * radius) + cent_x;
		y2 = (sint[angle] * radius) + cent_y;

		SDL_RenderDrawLine(renderer_main, x1, y1, x2, y2);
	}
}


void circle(short cent_x, short cent_y, unsigned short radius) {
	int rad_2 = radius * radius;
	int area = rad_2 << 2;
	int rad_r = radius << 1;


	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);

	for ( int i = 0; i < area; i++ ) {
		int x = (i % rad_r) - radius;
		int y = (i / rad_r) - radius;

		if ( ((x*x) + (y*y)) <= rad_2 )
			SDL_RenderDrawPoint(renderer_main, (cent_x + x), (cent_y + y));
	}

}


// SetFillStyle controls color of bar

void setfillstyle(short color) {
	int bg = get_rgb(color);

	bg_color.r = bg >> 24;
	bg_color.g = (bg << 8) >> 24;
	bg_color.b = (bg << 16) >> 24;
}


// SetColor controls color of line functions, rectangle function
void setcolor(short color) {
	int fg = get_rgb(color);

	fg_color.r = fg >> 24;
	fg_color.g = (fg << 8) >> 24;
	fg_color.b = (fg << 16) >> 24;
}


// SetTextColor (orwhatever its called) Determines the text color XXX Terminals only XXX
void textcolor(short color) {
/*	int rgb = get_rgb(color);

	text_color.r = rgb >> 24;
	text_color.g = (rgb << 8) >> 24;
	text_color.b = (rgb << 16) >> 24;
*/
	// For terminals. Linux terminals may not support colors
	#ifndef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	#endif
}

void outtextxy(short x_coor, short y_coor, string text) {
	SDL_Rect src;
	int W, H;

	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);

	SDL_Surface * message_surf = TTF_RenderText_Solid(text_type, text.c_str(), text_color);

	SDL_Texture * message_rend = SDL_CreateTextureFromSurface(renderer_main, message_surf);
	SDL_FreeSurface(message_surf);


	SDL_QueryTexture(message_rend, NULL, NULL, &W, &H);

	src.x = x_coor;
	src.y = y_coor;
	src.w = W;
	src.h = H;

	SDL_RenderCopy(renderer_main, message_rend, NULL, &src);

	SDL_DestroyTexture(message_rend);
}


void bar(short x_coor, short y_coor, short w_coor, short h_coor) {
	SDL_Rect src = {x_coor, y_coor, x_coor, h_coor};

	SDL_SetRenderDrawColor(renderer_main, bg_color.r, bg_color.g, bg_color.b, 0xff);

	SDL_RenderFillRect(renderer_main, &src);
}

void line(short x1, short y1, short x2, short y2) {
	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);

	SDL_RenderDrawLine(renderer_main, x1, y1, x2, y2);
}

void putpixel(short x, short y, short color) {
	setcolor(color);

	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);
	SDL_RenderDrawPoint(renderer_main, x, y);
}


void textxy(short x, short y, string s) {
	setfillstyle(0);

	bar(x, y, x + s.length() * 8, y + 7);

	outtextxy(x, y, s);
}



void coltextxy(short x, short y, string s, unsigned char c) {
	setcolor(c);
	textxy(x, y, s);
}



char hexnum(unsigned char num) {
	switch (num) {
	case '0': return '0';
	case '1': return '1';
	case '2': return '2';
	case '3': return '3';
	case '4': return '4';
	case '5': return '5';
	case '6': return '6';
	case '7': return '7';
	case '8': return '8';
	case '9': return '9';
	case 'A': return 'A';
	case 'B': return 'B';
	case 'C': return 'C';
	case 'D': return 'D';
	case 'E': return 'E';
	case 'F': return 'F';
	default: return 'X';
	}
}



string hexb(unsigned char num) {
	unsigned short number = hexnum(num >> 4) + hexnum(num & 15);

	string temp = to_string(number);
	return temp;
}



string hex(unsigned short num) {
	unsigned char number = num >> 8;
	string temp = hexb(number) + hexb(num & 255);

	return temp;
}



double valuer(string i) {
	long int n = strtol(i.c_str(), NULL, 0);

	if ( (n == LONG_MAX) || (n == LONG_MIN) )
		return 0;
	else
		return ((double)n);
}



int value(string i) {
	long int n = strtol(i.c_str(), NULL, 0);

	if ( (n == LONG_MAX) || (n == LONG_MIN) )
		return 0;
	else
		return ((int)n);
}


inline string cstrr(double i) {
	string temp = to_string(i);
	return temp;
}

inline string cstr(int i) {
	string temp = to_string(i);
	return temp;
}


string zero_pad(int n, unsigned int l) {
	string s;

	s = cstr(n);
	while (s.length() <= l)
		s = "0" + s;

	return s;
}

string zero_pads(string s, unsigned int l) {
	string s1 = s;

	while (s1.length() <= l)
		s1 = "0" + s1;

	return s1;
}


string ucase(string s) {
	for (unsigned int i = 0; i < s.length(); i++)
		s.at(i) = toupper(s.at(i));

	return s;
}


string lcase(string s) {
	for (unsigned int i = 0; i < s.length(); i++)
		s.at(i) = tolower(s.at(i));

	return s;
}


string space(unsigned char i) {
	unsigned char k;
	string s = "";

	if ( i )
		for (k = 0; k < i; k ++)
			s = s + " ";

	return s;
}


string repchar(char c, unsigned char i) {
	unsigned char k;
	string s = "";

	if ( i )
		for (k = 0; k < i; k++)
			s = s + c;

	return s;
}


string ltrim(string s1) {
	while ( s1.size() && isspace(s1.front()) )
		s1.erase(s1.begin());

	return s1;
}



string rtrim(string s1) {
	while ( !s1.empty() && isspace(s1.at(s1.size()-1)) )
		s1.erase(s1.end() - 1);

	return s1;
}


string btrim(string s1) {
	s1 = ltrim(s1);
	s1 = rtrim(s1);

	return s1;
}


void flushkey() {
// Remove queued keystrokes. Need to determine how we're to handle keystrokes

}


short get_seconds_past_hour() {
	time_t since_start;
	struct tm * info_time;

	time(&since_start);
	info_time = localtime(&since_start);

	return ( info_time->tm_sec + (info_time->tm_min * 60));
}


void calibrate_timing() {
// THE RTC IS LOCATED AT MEM ADDRESSES 0000:046C is current seconds past the hour
// 0000:046E is the current hour in 24 hour time format
	int k;

	delay_per_sec = 0;
	k = get_seconds_past_hour();

	while (k != get_seconds_past_hour());

	k = get_seconds_past_hour();

	do {
		SDL_Delay(1);
		delay_per_sec++;
	} while ( k == get_seconds_past_hour() );
}


void time_delay(unsigned short n) {
	int i, l;

	if ( delay_per_sec == 0 )
		calibrate_timing();

	l = ((float)(n/1000)*delay_per_sec);

	for ( i = 1; i <= l; i++)
		SDL_Delay(1);
}

void check_registration() {
	unsigned short w;
	unsigned short i;
	fstream f;
	string s = "ATR2.REG";

	registered = false;

	if ( exist(s) ) {
		f.open(s, fstream::in);
		if ( f.good() ) {
			getline(f, reg_name);
			getline(f, reg_num);
			f.close();

			w = 0;
			s = ucase(reg_name);
			s = btrim(s);

			for (i = 0; i < s.length(); i++)
				w = w + s[i];

			w = w ^ 0x5aa5;
			if ( w == strtoul(reg_num.c_str(), NULL, 0) )
				registered = true;
		}
	}
}


/*
// ror same as >>, but wrap discarded bits
short ror(short n, short k) {
	short wrapped = 0;
	fo
*/
// rol same as <<, but wrap discared bits
// sal same as <<
// sar same as >>, but copy signed bit over







void viewport(short x1, short y1, short x2, short y2) {
	if ( !graphix )
		return;

	SDL_Rect view = {x1, y1, (x2 - x1), (y2 - y1)};
	SDL_RenderSetViewport(renderer_main, &view);
}

void main_viewport() {
	viewport(5, 5, 474, 474); // About 470 x 470
}


void make_tables() {
	short i;

	for (i = 0; i <= 255; i++) {
		sint[i] = sin((i/128) * M_PI);
		cost[i] = cos((i/128) * M_PI);
	}
}


short robot_color(short n) {
	// Please excuse all of the one line operations here. It's to reduce space

	switch (n % 14) {
	case BLACK:		return LIGHT_GREEN;
	case BLUE:		return LIGHT_RED;
	case GREEN:		return LIGHT_BLUE;
	case CYAN:		return LIGHT_CYAN;
	case RED:		return LIGHT_MAGENTA;
	case MAGENTA:		return YELLOW;
	case BROWN:		return LIGHT_GRAY;
	case LIGHT_GRAY:	return BROWN;
	case DARK_GRAY:		return GREEN;
	case LIGHT_BLUE:	return RED;
	case LIGHT_GREEN:	return LIGHT_MAGENTA;
	case LIGHT_CYAN:	return CYAN;
	case LIGHT_RED:		return MAGENTA;
	case LIGHT_MAGENTA:	return WHITE;
	default:		return WHITE;
	}
}

void box(short x1, short y1, short x2, short y2) {
	short i;

	if ( !graphix )
		return;

	if ( x2 < x1 ) {
		i = x1;
		x1 = x2;
		x2 = i;
	}

	if ( y2 < y1 ) {
		i = y1;
		y1 = y2;
		y2 = i;
	}

	setfillstyle(LIGHT_GRAY);
	setcolor(LIGHT_GRAY);
	bar(x1, y1, x2, y2);

	setcolor(WHITE);
	line(x1, y1, x2-1, y1);
	line(x1, y1, x1, y2-1);

	setcolor(DARK_GRAY);
	line(x1+1, y2, x2, y2);
	line(x2, y1+1, x2, y2);
}

void hole(short x1, short y1, short x2, short y2) {
	short i;

	if ( !graphix )
		return;

	if ( x2 < x1 ) {
		i = x1;
		x1 = x2;
		x2 = i;
	}

	if ( y2 < y1 ) {
		i = y1;
		y1 = y2;
		y2 = i;
	}

	setfillstyle(BLACK);
	setcolor(BLACK);
	bar(x1, y1, x2, y2);

	setcolor(DARK_GRAY);
	line(x1, y1, x2-1, y1);
	line(x1, y1, x1, y2-1);

	setcolor(WHITE);
	line(x1+1, y2, x2, y2);
	line(x2, y1+1, x2, y2);

	putpixel(x1, y1, LIGHT_GRAY);
	putpixel(x2, y1, LIGHT_GRAY);
}


short hex2int(string s) {
	unsigned short w, i;

	i = 0;
	w = 0;

	while ( i < s.length() ) {
		switch(s.at(i)) {
		case '0': w = (w << 4) | 0x0; break;
		case '1': w = (w << 4) | 0x1; break;
		case '2': w = (w << 4) | 0x2; break;
		case '3': w = (w << 4) | 0x3; break;
		case '4': w = (w << 4) | 0x4; break;
		case '5': w = (w << 4) | 0x5; break;
		case '6': w = (w << 4) | 0x6; break;
		case '7': w = (w << 4) | 0x7; break;
		case '8': w = (w << 4) | 0x8; break;
		case '9': w = (w << 4) | 0x9; break;
		case 'A': w = (w << 4) | 0xa; break;
		case 'B': w = (w << 4) | 0xb; break;
		case 'C': w = (w << 4) | 0xc; break;
		case 'D': w = (w << 4) | 0xd; break;
		case 'E': w = (w << 4) | 0xe; break;
		case 'F': w = (w << 4) | 0xf; break;
		default:
			i = s.length();
		}
		i++;
	}
	return w;
}



short str2int(string s) {
	long value = strtol(s.c_str(), NULL, 0);

	if ( (value < SHRT_MIN) )
		return -32768;
	else if (value > SHRT_MAX)
		return 32767;
	else
		return (short)value;
}


double _distance(double x1, double y1, double x2, double y2) {
	return (sqrt(pow(y1-y2, 2) + pow(x1-x2, 2)));
}


double find_angle(double xx, double yy, double tx, double ty) {
	short v, z;
	double q = 0;

	v = abs(tx - xx);

	if ( v == 0 ) {
		if ( (tx == xx) && (ty > yy) )
			q = M_PI;
		if ( (tx == xx) && (ty < yy) )
			q = 0;
	} else {
		z = abs(ty - yy);
		q = abs(atan(z / v));
		if ( (tx > xx) && (ty > yy) )
			q = (M_PI / 2) + q;
		if ( (tx > xx) && (ty < yy) )
			q = (M_PI / 2) - q;
		if ( (tx < xx) && (ty < yy) )
			q = M_PI + (M_PI / 2) + q;
		if ( (tx < xx) && (ty > yy) )
			q = M_PI + (M_PI / 2) - q;
		if ( (tx == xx) && (ty > yy) )
			q = M_PI / 2;
		if ( (tx == xx) && (ty < yy) )
			q = 0;
		if ( (tx < xx) && ( ty == yy) )
			q = M_PI + (M_PI / 2);
		if ( (tx > xx) && (ty == yy) )
			q = M_PI / 2;
	}

	return q;
}


short find_anglei(double xx, double yy, double tx, double ty) {
	short i = (short)round(find_angle(xx, yy, tx, ty) / M_PI * 128 + 256);

	while ( i < 0 )
		i += 255;

	i = i & 255;
	return i;
}


string bin(short n) {
	short i;
	string bin_string = "";

	for (i = 0; i <= 15; i++) {
		bin_string = to_string((n % 2)) + bin_string;
		n = n / 2;
	}

	return bin_string;
}

string decimal(int num, int length) {
	string dec_string = to_string(num);

	return (dec_string.substr(0, length));
}


char readkey() {
	int code;
	SDL_Event event;

	while ( SDL_PollEvent(&event) ) {
		switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			code = event.key.keysym.sym;
			// See if code is in range of keys
			if ( (code > 31) && ( code < 128 ) )
				return (char)code;

		}
		SDL_Delay(1); // To prevent lagging of pc
	}
	return 0;
}

bool keypressed() {
	int code;
	bool ret = false;
	SDL_Event event;

	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		code = event.key.keysym.sym;
		// See if code is in range of keys
		if ((code > 31) && (code < 128))
			ret = true;
		break;
	}

	return ret;
}