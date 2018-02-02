#include "Headers/atr2func.h"

void setcolor(int color) {
	switch (color) {
	case 0: // Black
		mask_color(0, 0, 0, 255);
		break;

	case 1: // blue

	case 2: // green

	case 3: // cyan

	case 4: // red

	case 5: // magenta

	case 6: // brown

	case 7: // light gray

	case 8: // dark gray

	case 9: // light blue

	case 10: // light green

	case 11: // light cyan

	case 12: // light red

	case 13: // light magenta

	case 14: // yellow

	case 15: // white

	}
}


void mask_color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	text_color -> r = red;
	text_color -> g = green;
	text_color -> b = blue;
	text_color -> a = alpha;
}


void outtextxy(int x_coor, int y_coor, string sent) {
	// Create surface of text
	SDL_Surface *tmp_surf = TTF_RenderText_Solid(main_font, sent.c_str(), main_color);
	SDL_Texture *tmp_text = SDL_CreateTextureFromSurface(main_renderer, tmp_surf);
	SDL_DestroySurface(tmp_surf);

	// Grab length and width of text
	SDL_QueryTexture(tmp_text, NULL, NULL, &temp_w, &temp_h);

	SDL_Rect * temp_rect = (SDL_Rect *) malloc(sizeof(SDL_Rect));
	temp_rect -> x = x_coor;
	temp_rect -> y = y_coor;
	temp_rect -> w = temp_w;
	temp_rect -> h = temp_h;

	SDL_RenderCopy(main_renderer, tmp_text, NULL, temp_rect);

	SDL_DestroyTexture(tmp_text);
	free(temp_rext);
}

void bar(int X, int Y, int W, int H) {
	SDL_Rect *tmp = (SDL_Rect *) malloc(sizeof(SDL_Rect));

	if ( !tmp ) // If unsuccessful malloc, abandon operation
		return;

	tmp -> x = X;
	tmp -> y = Y;
	tmp -> w = W;
	tmp -> h = H;

	SDL_RenderFillRect(main_renderer, tmp);
	free(tmp);
}

void set_viewport(int X, int Y, int W, int H) {
	viewport -> x = X;
	viewport -> y = Y;
	viewport -> w = W;
	viewport -> h = H;
}

void textxy(short x, short y, string s) {
	SDL_SetRenderDrawColor(0x0, 0x0, 0x0, 0x0); // Set to black
	bar(x, y, x + s.length()*8, y+7);
	outtextxy(x, y, s);
}

void coltextxy(short x, short y, string s, unsigned char c) {
	setcolor(c);
	textxy(x, y, s);
}

string zero_pad(short n, short l) {
	string s;

	s = cstr(n);
	while (s.length() < l)
		s = "0" + s;

	return s;
}

string zero_pads(string s, short l) {
	string s1;

	s1 = s;
	while ( s1.length() < l )
		s1 = "0" + s1;

	return s1;
}


