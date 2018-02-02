#ifndef ATR2FUNC_H
#define ATR2FUNC_H

#include "filelib.h"
#include "myfile.h"
#include <math.h>

// Incorrect sdl2 headers, please replace them with appropriate ones
#include "SDL2.h"
#include "SDL2_ttf.h"

SDL_Color	main_color;
SDL_Rect	*viewport;
SDL_Renderer	*main_renderer;

extern void 	setcolor(int);
extern void	sercolor_render(int);
extern void 	outtextxy(int, int, string);
extern void 	bar(int, int, int, int);

extern void 	set_viewport(int, int, int, int);
extern void 	textxy(short, short, string);
extern void 	coltextxy(short, short, string, unsigned char);
extern string 	zero_pad(short, short);
extern string 	zero_pads(string, short);
extern void	set_viewport(short, short, short, short);
extern void	main_viewport();
extern void	make_tables();
extern int	robot_color(short);
extern void	box(short, short, short, short);
extern void	hole(short, short, short, short);
extern double	distance(double, double, double, double);
extern double	find_angle(double, double, double, double);
extern short	find_anglei(double, double, double, double);

#endif
