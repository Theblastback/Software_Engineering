#ifndef ATR2FUNC_H
#define ATR2FUNC_H

#include "filelib.h"

#include <cmath>

// These SDL headers work for me, but likely will not work for you guys
// Their original names were <SDL2/...

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#ifndef _WIN32
	#include <windows.h>
#endif

// Color definitions

#define BLACK		0
#define	BLUE		1
#define	GREEN		2
#define	CYAN		3
#define	RED		4
#define	MAGENTA		5
#define	BROWN		6
#define	LIGHT_GRAY	7
#define	DARK_GRAY	8
#define	LIGHT_BLUE	9
#define	LIGHT_GREEN	10
#define	LIGHT_CYAN	11
#define	LIGHT_RED	12
#define	LIGHT_MAGENTA	13
#define	YELLOW		14
#define	WHITE		15


// All of these will be initialized in atr2's init function
SDL_Window	* window_main;
SDL_Renderer	* renderer_main;
SDL_Color	bg_color; // Bar color
SDL_Color	fg_color; // Line/rect color
SDL_Color	text_color;
TTF_Font	* text_type; // Needed to print text

int	delay_per_sec;
bool	registered, graphix, sound_on;
string	reg_name;

string reg_num;
double	sint[256], cost[256];
unsigned char pressed_key;

extern void	arc(short, short, unsigned short, unsigned short, unsigned short);
extern void	circle(short, short, unsigned short);
extern void	setfillstyle(short);
extern void	setcolor(short);
extern void	textcolor(short);

extern void	textxy(short, short, string);
extern void	outtextxy(short, short, string);	// Is heavily used and is modified by the viewport, so this is needed

extern void	coltextxy(short, short, string, unsigned char);
extern char	hexnum(unsigned char);
extern string	hexb(unsigned char);
extern string	hex(unsigned short);
extern double	valuer(string);
extern int	value(string);
extern string	cstrr(double);
extern string	cstr(int);
extern string	zero_pad(int, unsigned int);
extern string	zero_pads(string, unsigned int);
extern string	ucase(string);
extern string	lcase(string);
extern string	space(unsigned char);
extern string	repchar(char, unsigned char);
extern string	ltrim(string);
extern string	rtrim(string);
extern string	btrim(string);

extern char	readkey();

extern void	calibrate_timing();
extern void	time_delay(unsigned short);
extern void	check_registration();

// Assembly functions (Not created)
extern short	rol(short, short);
extern short	ror(short, short);
extern short	sal(short, short);
extern short	sar(short, short);

extern void	viewport(short, short, short, short);
extern void	main_viewport();
extern void	make_tables();

// Robot color will have to be changed in order to store RGB
extern short	robot_color(short);
extern void	box(short, short, short, short);
extern void	hole(short, short, short, short);

// Functions that existed in pascal but don't in C++/SDL2
extern void	bar(short, short, short, short);
extern void	line(short, short, short, short);
extern void	putpixel(short, short, short);

extern void	chirp();
extern void	click();

extern short	hex2int(string);
extern short	str2int(string);

extern double	find_angle(double, double, double, double);
extern short	find_anglei(double, double, double, double);

extern string	bin(short);
extern string	decimal(int, int);


#endif
