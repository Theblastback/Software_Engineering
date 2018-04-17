
#pragma warning (disable:4996)
#include "myfile.h" // filelib is already included in myfile
#include "atr2func.h"
#include <climits>
#include <ctime>

SDL_Window	*window_main;
SDL_Renderer	*renderer_main;
SDL_Color	bg_color;
SDL_Color	fg_color;
TTF_Font	*text_type;
int32_t		delay_per_sec;
bool		registered, graphix, sound_on;
string		reg_name;
string		reg_num;
double		sint[256], cost[256];
uint8_t	pressed_key;


int32_t get_rgb(int16_t color) {
	int32_t rgb;

	switch (color) {
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


void arc(int16_t cent_x, int16_t cent_y, uint16_t st_angle, uint16_t end_angle, uint16_t radius) {
	// Utilizing the premade tables in this program
	// Converting a 360 degree circle to a 255 degree means every 1.4 degrees equals 1 degree
	int16_t x1, x2, y1, y2;

	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);

	st_angle = st_angle / 1.4;
	end_angle = end_angle / 1.4;

	// Do this outside of the loop in order to set up loop
	x2 = (cost[st_angle] * radius) + cent_x;
	y2 = (sint[st_angle] * radius) + cent_y;

	for (uint16_t angle = st_angle + 1; angle <= end_angle; angle++) {
		x1 = x2;
		y1 = y2;

		x2 = (cost[angle] * radius) + cent_x;
		y2 = (sint[angle] * radius) + cent_y;

		SDL_RenderDrawLine(renderer_main, x1, y1, x2, y2);
	}
}


void circle(int16_t cent_x, int16_t cent_y, uint16_t radius) {
	int32_t rad_2 = radius * radius;
	int32_t area = rad_2 << 2;
	int32_t rad_r = radius << 1;


	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);

	for (int32_t i = 0; i < area; i++) {
		int32_t x = (i % rad_r) - radius;
		int32_t y = (i / rad_r) - radius;

		if (((x*x) + (y*y)) <= rad_2)
			SDL_RenderDrawPoint(renderer_main, (cent_x + x), (cent_y + y));
	}

}


// SetFillStyle controls color of bar

void setfillstyle(int16_t color) {
	int32_t bg = get_rgb(color);

	bg_color.r = bg >> 24;
	bg_color.g = (bg << 8) >> 24;
	bg_color.b = (bg << 16) >> 24;
}


// SetColor controls color of line functions, rectangle function
void setcolor(int16_t color) {
	int32_t fg = get_rgb(color);

	fg_color.r = fg >> 24;
	fg_color.g = (fg << 8) >> 24;
	fg_color.b = (fg << 16) >> 24;
}


// SetTextColor (orwhatever its called) Determines the text color XXX Terminals only XXX
void textcolor(int16_t color) {
	/*	int32_t rgb = get_rgb(color);

	text_color.r = rgb >> 24;
	text_color.g = (rgb << 8) >> 24;
	text_color.b = (rgb << 16) >> 24;
	*/
	// For terminals. Linux terminals may not support colors
#ifndef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#endif
}

void outtextxy(int16_t x_coor, int16_t y_coor, string text) {
	cout << "Begin outtextxy" << endl;
	SDL_Rect src;
	int32_t W, H;

	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);

	SDL_Surface * message_surf = TTF_RenderText_Solid(text_type, text.c_str(), fg_color);

	SDL_Texture * message_rend = SDL_CreateTextureFromSurface(renderer_main, message_surf);

	SDL_FreeSurface(message_surf);

	SDL_QueryTexture(message_rend, NULL, NULL, &W, &H);

	src.x = x_coor;
	src.y = y_coor;
	src.w = W;
	src.h = H;

	SDL_RenderCopy(renderer_main, message_rend, NULL, &src);

	SDL_DestroyTexture(message_rend);

	cout << "End outtextxy" << endl << endl;
}


void bar(int16_t x_coor, int16_t y_coor, int16_t w_coor, int16_t h_coor) {
	SDL_Rect src = { x_coor, y_coor, x_coor, h_coor };

	SDL_SetRenderDrawColor(renderer_main, bg_color.r, bg_color.g, bg_color.b, 0xff);

	SDL_RenderFillRect(renderer_main, &src);
}

void line(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);

	SDL_RenderDrawLine(renderer_main, x1, y1, x2, y2);
}

void putpixel(int16_t x, int16_t y, int16_t color) {
	setcolor(color);

	SDL_SetRenderDrawColor(renderer_main, fg_color.r, fg_color.g, fg_color.b, 0xff);
	SDL_RenderDrawPoint(renderer_main, x, y);
}


void textxy(int16_t x, int16_t y, string s) {
	setfillstyle(0);

	bar(x, y, x + s.length() * 8, y + 7);

	outtextxy(x, y, s);
}



void coltextxy(int16_t x, int16_t y, string s, uint8_t c) {
	setcolor(c);
	textxy(x, y, s);
}



char hexnum(uint8_t num) {
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



string hexb(uint8_t num) {
	uint16_t number = hexnum(num >> 4) + hexnum(num & 15);

	string temp = to_string(number);
	return temp;
}



string hex(uint16_t num) {
	uint8_t number = num >> 8;
	string temp = hexb(number) + hexb(num & 255);

	return temp;
}



double valuer(string i) {
	int64_t n = strtol(i.c_str(), NULL, 0);

	if ((n == LONG_MAX) || (n == LONG_MIN))
		return 0;
	else
		return ((double)n);
}



int32_t value(string i) {
	int64_t n = strtol(i.c_str(), NULL, 0);

	if ((n == LONG_MAX) || (n == LONG_MIN))
		return 0;
	else
		return ((int32_t)n);
}


inline string cstrr(double i) {
	string temp = to_string(i);
	return temp;
}

inline string cstr(int32_t i) {
	string temp = to_string(i);
	return temp;
}


string zero_pad(int32_t n, uint32_t l) {
	string s;

	s = cstr(n);
	while (s.length() <= l)
		s = "0" + s;

	return s;
}

string zero_pads(string s, uint32_t l) {
	string s1 = s;

	while (s1.length() <= l)
		s1 = "0" + s1;

	return s1;
}


string ucase(string s) {
	for (uint32_t i = 0; i < s.length(); i++)
		s.at(i) = toupper(s.at(i));

	return s;
}


string lcase(string s) {
	for (uint32_t i = 0; i < s.length(); i++)
		s.at(i) = tolower(s.at(i));

	return s;
}


string space(uint8_t i) {
	uint8_t k;
	string s = "";

	if (i)
		for (k = 0; k < i; k++)
			s = s + " ";

	return s;
}


string repchar(char c, uint8_t i) {
	uint8_t k;
	string s = "";

	if (i)
		for (k = 0; k < i; k++)
			s = s + c;

	return s;
}


string ltrim(string s1) {
	// err_log << "Begin ltrim" << endl;
	while (s1.size() && isspace(s1.front()))
		s1.erase(s1.begin());

	return s1;
}



string rtrim(string s1) {
	// err_log << "Begin rtrim" << endl;
	while (!s1.empty() && isspace(s1.at(s1.size() - 1)))
		s1.erase(s1.end() - 1);

	return s1;
}


string btrim(string s1) {
	// err_log << "Begin btrim" << endl;

	s1 = ltrim(s1);
	s1 = rtrim(s1);

	return s1;
}



int16_t get_seconds_past_hour() {
	time_t since_start;
	struct tm * info_time;

	time(&since_start);
	info_time = localtime(&since_start);

	return (info_time->tm_sec + (info_time->tm_min * 60));
}


void calibrate_timing() {
	// THE RTC IS LOCATED AT MEM ADDRESSES 0000:046C is current seconds past the hour
	// 0000:046E is the current hour in 24 hour time format
	int32_t k;

	delay_per_sec = 0;
	k = get_seconds_past_hour();

	while (k != get_seconds_past_hour());

	k = get_seconds_past_hour();

	do {
		SDL_Delay(1);
		delay_per_sec++;
	} while (k == get_seconds_past_hour());
}


void time_delay(uint16_t n) {
	int32_t i, l;

	if (delay_per_sec == 0)
		calibrate_timing();

	l = ((float)(n / 1000)*delay_per_sec);

	for (i = 1; i <= l; i++)
		SDL_Delay(1);
}

void check_registration() {
	uint16_t w;
	uint16_t i;
	fstream f;
	string s = "ATR2.REG";

	registered = false;

	if (exist(s)) {
		f.open(s, fstream::in);
		if (f.good()) {
			getline(f, reg_name);
			getline(f, reg_num);
			f.close();

			w = 0;
			s = ucase(reg_name);
			s = btrim(s);

			for (i = 0; i < s.length(); i++)
				w = w + s[i];

			w = w ^ 0x5aa5;
			if (w == strtoul(reg_num.c_str(), NULL, 0))
				registered = true;
		}
	}
}



// ror same as >>, but wrap discarded bits
int16_t ror(int16_t n, int16_t k) {
	while (k) {
		n = ((uint16_t)n >> 1) | ((uint16_t)n << 15);
		k--;
	}
	return n;
}

// rol same as <<, but wrap discarded bits
int16_t rol(int16_t n, int16_t k) {
	while (k) {
		n = ((uint16_t)n << 1) | ((uint16_t)n >> 15);
		k--;
	}

	return n;
}

// sal same as <<
int16_t sal(int16_t n, int16_t k) {
	return (n << k);
}

// sar same as >>, but copy signed bit over
int16_t sar(int16_t n, int16_t k) {
	n = n / (1 << k);

	return n;
}






void viewport(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	if (!graphix)
		return;

	SDL_Rect view = { x1, y1, (x2 - x1), (y2 - y1) };
	SDL_RenderSetViewport(renderer_main, &view);
}

void main_viewport() {
	viewport(5, 5, 474, 474); // About 470 x 470
}


void make_tables() {
	int16_t i;

	for (i = 0; i <= 255; i++) {
		sint[i] = sin((i / 128) * M_PI);
		cost[i] = cos((i / 128) * M_PI);
	}
}


int16_t robot_color(int16_t n) {
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

void box(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	int16_t i;

	if (!graphix)
		return;

	if (x2 < x1) {
		i = x1;
		x1 = x2;
		x2 = i;
	}

	if (y2 < y1) {
		i = y1;
		y1 = y2;
		y2 = i;
	}

	setfillstyle(LIGHT_GRAY);
	setcolor(LIGHT_GRAY);
	bar(x1, y1, x2, y2);

	setcolor(WHITE);
	line(x1, y1, x2 - 1, y1);
	line(x1, y1, x1, y2 - 1);

	setcolor(DARK_GRAY);
	line(x1 + 1, y2, x2, y2);
	line(x2, y1 + 1, x2, y2);
}

void hole(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	int16_t i;

	if (!graphix)
		return;

	if (x2 < x1) {
		i = x1;
		x1 = x2;
		x2 = i;
	}

	if (y2 < y1) {
		i = y1;
		y1 = y2;
		y2 = i;
	}

	setfillstyle(BLACK);
	setcolor(BLACK);
	bar(x1, y1, x2, y2);

	setcolor(DARK_GRAY);
	line(x1, y1, x2 - 1, y1);
	line(x1, y1, x1, y2 - 1);

	setcolor(WHITE);
	line(x1 + 1, y2, x2, y2);
	line(x2, y1 + 1, x2, y2);

	putpixel(x1, y1, LIGHT_GRAY);
	putpixel(x2, y1, LIGHT_GRAY);
}


int16_t hex2int32_t(string s) {
	uint16_t w, i;

	i = 0;
	w = 0;

	while (i < s.length()) {
		switch (s.at(i)) {
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



int16_t str2int(string s) {
	long value = strtol(s.c_str(), NULL, 0);

	if ((value < SHRT_MIN))
		return -32768;
	else if (value > SHRT_MAX)
		return 32767;
	else
		return (int16_t)value;
}


double _distance(double x1, double y1, double x2, double y2) {
	return (sqrt(pow(y1 - y2, 2) + pow(x1 - x2, 2)));
}


double find_angle(double xx, double yy, double tx, double ty) {
	int16_t v, z;
	double q = 0;

	v = abs(tx - xx);

	if (v == 0) {
		if ((tx == xx) && (ty > yy))
			q = M_PI;
		if ((tx == xx) && (ty < yy))
			q = 0;
	}
	else {
		z = abs(ty - yy);
		q = abs(atan(z / v));
		if ((tx > xx) && (ty > yy))
			q = (M_PI / 2) + q;
		if ((tx > xx) && (ty < yy))
			q = (M_PI / 2) - q;
		if ((tx < xx) && (ty < yy))
			q = M_PI + (M_PI / 2) + q;
		if ((tx < xx) && (ty > yy))
			q = M_PI + (M_PI / 2) - q;
		if ((tx == xx) && (ty > yy))
			q = M_PI / 2;
		if ((tx == xx) && (ty < yy))
			q = 0;
		if ((tx < xx) && (ty == yy))
			q = M_PI + (M_PI / 2);
		if ((tx > xx) && (ty == yy))
			q = M_PI / 2;
	}

	return q;
}


int16_t find_anglei(double xx, double yy, double tx, double ty) {
	int16_t i = (int16_t)round(find_angle(xx, yy, tx, ty) / M_PI * 128 + 256);

	while (i < 0)
		i += 255;

	i = i & 255;
	return i;
}


string bin(int16_t n) {
	int16_t i;
	string bin_string = "";

	for (i = 0; i <= 15; i++) {
		bin_string = to_string((n % 2)) + bin_string;
		n = n / 2;
	}

	return bin_string;
}

string decimal(int32_t num, int32_t length) {
	string dec_string = to_string(num);

	return (dec_string.substr(0, length));
}


char readkey() {
	int32_t code;
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			code = event.key.keysym.sym;
			// See if code is in range of keys
			if (((code > 31) && (code < 128)) || (code == SDLK_ESCAPE) || (code == SDLK_RETURN))
				return (char)code;

		}
		SDL_Delay(1); // To prevent lagging of pc
	}
	return 0;
}

bool keypressed() {
	int32_t code;
	bool ret = false;
	SDL_Event event;

	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		code = event.key.keysym.sym;
		// See if code is in range of keys
		if (((code > 31) && (code < 128)) || (code == SDLK_ESCAPE) || (code == SDLK_RETURN))
			ret = true;
		break;
	}

	return ret;
}
