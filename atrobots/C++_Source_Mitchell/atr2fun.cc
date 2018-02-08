#include "Headers/myfile.h" // filelib is already included in myfile
#include "Headers/atr2func.h"
#include <ctime>


SDL_Rect view_trim(SDL_Rect value) {
	if ( (value.w + value.x) > curnt_view.w )
		value.w = value.w - ((value.x + value.w) - curnt_view.x);

	if ( (value.h + value.y) > curnt_view.h )
		value.h = value.h - ((value.y + value.h) - curnt_view.x);

	return value;
}



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
		rgb = oxffffff << 8;
		break;
	}

	return rgb;
}



// SetFillStyle controls color of bar
void setfillstyle(short color);
	int bg = get_rgb(color);

	bg_color.r = bg >> 24;
	bg_color.g = (bg << 8) >> 24;
	bg_color.b = (bg << 16) >> 34;
}


// SetColor controls color of line functions, rectangle function
void setcolor(short color) {
	int fg = get_rgb(color);

	fg_color.r = fg >> 24;
	fg_color.g = (fg << 8) >> 24;
	fg_color.b = (fg << 16) >> 24;
}


// SetTextColor (orwhatever its called) Determines the text color
void textcolor(short color) {
	int rgb = get_rgb(color);

	text_color.r = rgb >> 24;
	text_color.g = (rgb << 8) >> 24;
	text_color.b = (rgb << 16) >> 24;
}

void outtextxy(short x_coor, short y_coor, string * text) {
	SDL_Rect src;
	int W, H;

	SDL_SetRenderDrawColor(renderer_main, text_color.r, text_color.g, text_color.b, 0xff);

	SDL_Surface * message_surf = ttf_RenderText_Solid(text_type, s -> c_str(), text_color);

	SDL_Texture * message_rend = SDL_CreateTextureFromSurface(main_renderer, message_surf);
	SDL_DestroySurface(message_surf);


	SDL_QueryTexture(message_rend, NULL, NULL, &W, &H);

	src.x = x_coor + curnt_view -> x;
	src.y = y_coor + curnt_view -> y;
	src.w = W;
	src.h = H;

	src = view_trim(src);

	SDL_RenderCopy(renderer_main, message_rend, NULL, &src);

	SDL_DestroyTexture(message_rend);
}


void bar(short x_coor, short y_coor, short w_coor, short h_coor) {
	SDL_Rect src;

	src.x = x_coor + curnt_view -> x;
	src.y = y_coor + curnt_view -> y;
	src.w = w_coor;
	src.h = h_coor;

	src = view_trim(src);

	SDL_SetRenderDrawColor(renderer_main, bg_color.r, bg_color.g, bg_color.b, 0xff);

	SDL_RenderFillRect(main_renderer, &src);
}

void line(short x1, short y1, short x2, short y2) {
	SDL_Rect rect;

	rect.x = x1 + curnt_view -> x;
	rect.y = y1 + curnt_view -> y;
	rect.w = x2 + curnt_view -> x;
	rect.w = y2 + curnt_view -> y;

	rect = view_trim(rect);

	SDL_SetRenderDrawColor(renderer_main, fg_color, fg_color.r, fg_color.g, fg_color.b, 0xff);

	SDL_RenderDrawLine(renderer_main, rect.x, rect.y, rect.w, rect.h);
}

void putpixel(short x, short y, short color) {
	setcolor(color);

	SDL_SetRenderDrawColor(renderer_main, fg_color, fg_color.r, fg_color.g, fg_color.b, 0xff);
	SDL_RenderDrawPoint(renderer_main, x + curnt_view -> x, y curnt_view -> y);
}


void textxy(short x, short y, string * s) {
	setfillstyle(0);

	bar(x, y, x + s->length() * 8; y + 7);

	outtextxy(x, y, s);
}



void coltextxy(short x, short y, string * s, unsigned char c)
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



string hexb(unsigned char num) {
	string temp = hexnum(num >> 4) + hexnum(num & 15);
	return temp;
}



string hex(unsigned short) {
	string temp = hexb(num >> 8) + hexb(num & 255);
	return temp;
}



double valuer(string * i) {
	long int n = strtol(i -> c_str(), NULL, 0);

	if ( (n == LONG_MAX) || (n == LONG_MIN) )
		return 0;
	else
		return ((double)n);
}



int value(string * i) {
	long int n = strtol(i -> c_str(), NULL, 0);

	if ( (n == LONG_MAX) || (n == LONG_MIN) )
		return 0;
	else
		return ((int)n);
}


string cstrr(double i) {
	string temp = i;
	return temp;
}

string cstr(int i) {
	string temp = i;
	return temp;
}


string zero_pad(int n, int l) {
	string s;

	s = cstr(n);
	while (s.length() <= l)
		s = "0" + s;

	return s;
}

string zero_pads(string s, int l) {
	string s1 = s;

	while (s1.length() <= l)
		s1 = "0" + s1;

	return s1;
}


string ucase(string * s) {
	for (unsigned int i = 0; i < s -> length(); i++)
		s -> at(i) = toupper(s -> at(i));

	return *s;
}


string lcase(string * s) {
	for (unsigned int i = 0; i < s-> length(); i++)
		s -> at(i) = tolower(s -> at(i));

	return *s;
}


string space(unsigned char i) {
	unsigned char k
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


string ltrim(string * s1) {
	while ( s1 -> size() && isspace(s1 -> front()) )
		s1 -> erase(s1 -> begin());

	return *s1;
}



string rtrim(string * s1) {
	while ( !s1 -> empty() && isspace(s1 -> at(s1->size()-1)) )
		s1 -> erase(s1 -> end() - 1);

	return *s1;
}


string btrim(string * s1) {
	*s1 = ltrim(s1);
	*s1 = rtrim(s1);

	return *s1;
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
	int i, k;

	delay_per_sec = 0;
	k = get_seconds_past_hour();

	while (k != get_seconds_past_hour());

	k = get_seconds_past_hour();

	do {
		SDL_Delay(1);
		delay_per_sec++;
	} while ( k == get_seconds_past_hour() );
}


void time_delay(short n) {
	int i, l;

	if ( delay_per_sec == 0 )
		calibrate_timing();

	l = ((float)(n/1000)*delay_per_sec);
}

void check_registration() {
	unsigned short w;
	short i;
	fstream f;
	string s = "ATR2.REG";

	registered = false;

	if ( exist(&s) ) {
		f.open(s, fstream::in);
		if ( f.good() ) {
			getline(f, reg_name);
			getline(f, reg_num);
			f.close();

			w = 0;
			s = ucase(&reg_name);
			s = btrim(&s);

			for (i = 0; i < s.length(); i++)
				w = w + s[i];

			w = w ^ 0x5aa5;
			if ( w == reg_num )
				registered = true;
		}
	}
}


// Ignored ror, rol, sal, and sar


void viewport(short x1, short y1, short x2, short y2) {
	if ( !graphix )
		return;

	curnt_view -> x = x1;
	curnt_view -> y = y1;
	curnt_view -> w = x2;
	curnt_view -> h = y2;
}

void main_viewport() {
	viewport(5, 5, 474, 474); // About 470 x 470
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

	setcolor(DARK_GRAY)
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

	setcolor(WHITE)
	line(x1+1, y2, x2, y2);
	line(x2, y1+1, x2, y2);

	putpixel(x1, y1, LIGHT_GRAY);
	putpixel(x2, y1, LIGHT_GRAY);
}


short hex2int(string * s) {



}
