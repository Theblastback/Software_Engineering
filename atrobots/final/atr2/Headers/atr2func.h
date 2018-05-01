#ifndef ATR2FUNC_H
#define ATR2FUNC_H

#include "filelib.h"

#include <cmath>

// These SDL headers work for me, but likely will not work for you guys
// Their original names were <SDL2/...

#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_ttf.h>
#else
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_ttf.h>
#endif

#ifdef _WIN32
	#include <windows.h>
#endif

// Color definitions

#define	BLACK		0
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
extern SDL_Window		* window_main;
extern SDL_Renderer	* renderer_main;
extern SDL_Color		bg_color; // Bar color
extern SDL_Color		fg_color; // Line/rect color
extern SDL_Color		text_color;
extern TTF_Font		* text_type; // Needed to print text

extern int32_t	delay_per_sec;
extern bool		registered, graphix, sound_on;
extern string	reg_name;

extern string	reg_num;
extern double	sint[256], cost[256];
extern uint8_t	pressed_key;

extern void		arc(int16_t, int16_t, uint16_t, uint16_t, uint16_t);
extern void		circle(int16_t, int16_t, uint16_t);
extern void		setfillstyle(int16_t);
extern void		setcolor(int16_t);
extern void		textcolor(int16_t);

extern void		textxy(int16_t, int16_t, string);
extern void		outtextxy(int16_t, int16_t, string);	// Is heavily used and is modified by the viewport, so this is needed

extern void		coltextxy(int16_t, int16_t, string, uint8_t);
extern char		hexnum(uint8_t);
extern string	hexb(uint8_t);
extern string	hex(uint16_t);
extern double	valuer(string);
extern int32_t	value(string);
extern string	cstrr(double);
extern string	cstr(int32_t);
extern string	zero_pad(int32_t, uint32_t);
extern string	zero_pads(string, uint32_t);
extern string	ucase(string);
extern string	lcase(string);
extern string	space(uint8_t);
extern string	repchar(char, uint8_t);
extern string	ltrim(string);
extern string	rtrim(string);
extern string	btrim(string);

extern bool		keypressed();
extern char		readkey();

extern void		calibrate_timing();
extern void		time_delay(uint16_t);
extern void		check_registration();

// Assembly functions (Not created)

extern int16_t	rol(int16_t, int16_t);
extern int16_t	ror(int16_t, int16_t);
extern int16_t	sal(int16_t, int16_t);
extern int16_t	sar(int16_t, int16_t);


extern void		viewport(int16_t, int16_t, int16_t, int16_t);
extern void		main_viewport();
extern void		make_tables();

// Robot color will have to be changed in order to store RGB
extern int16_t	robot_color(int16_t);
extern void		box(int16_t, int16_t, int16_t, int16_t);
extern void		hole(int16_t, int16_t, int16_t, int16_t);

// Functions that existed in pascal but don't in C++/SDL2
extern void		bar(int16_t, int16_t, int16_t, int16_t);
extern void		line(int16_t, int16_t, int16_t, int16_t);
extern void		putpixel(int16_t, int16_t, int16_t);

extern void		chirp();
extern void		click();

extern int16_t	hex2int(string);
extern int16_t	str2int(string);

extern double	find_angle(double, double, double, double);
extern int16_t	find_anglei(double, double, double, double);
extern double	_distance(double, double, double, double);

extern string	bin(int16_t);
extern string	decimal(int32_t, int32_t);

extern int16_t	get_seconds_past_hour();

#endif

