#include "Headers/myfile.h" // filelib is already included in myfile
#include "Headers/atr2func.h"

/*
	Colors (According to SetPalette in the library manual)

	Black		= 0
	Blue 		= 1
	Green		= 2
	Cyan		= 3
	Red		= 4
	Magenta 	= 5
	Brown		= 6
	LightGray	= 7
	DarkGray	= 8
	LightBlue	= 9
	LightGreen	= 10
	LightCyan	= 11
	LightRed	= 12
	Light Magenta	= 13
	Yellow 		= 14
	White		= 15
*/


SDL_Rect view_trim(SDL_Rect value) {
	if ( (value.w + value.x) > curnt_view.w )
		value.w = value.w - ((value.x + value.w) - curnt_view.x);

	if ( (value.h + value.y) > curnt_view.h )
		value.h = value.h - ((value.y + value.h) - curnt_view.x);

	return value;
}



// SetFillStyle controls color of bar
void SetFillStyle(short color);
	switch ( color ) {
	case BLACK:
		bg_color = 0x0;
		break;

	case BLUE:
		bg_color = 160 << 8;
		break;

	case GREEN:
		bg_color = 173 << 16;
		break;

	case CYAN:
		bg_color = (174 << 16) | (174 << 8);
		break;

	case RED:
		bg_color = 175 << 24;
		break;

	case MAGENTA:
		bg_color = (174 << 24) | (173 << 8);
		break;

	case BROWN:
		bg_color = (174 << 24) | (87 << 16);
		break;

	case LIGHT_GRAY:
		bg_color = 0xacacac << 8;
		break;

	case DARK_GRAY:
		bg_color = (120 << 24) | (120 << 16) | (120 << 8);
		break;

	case LIGHT_BLUE:
		bg_color = (86 << 24) | (86 << 16) | (255 << 8);
		break;

	case LIGHT_GREEN:
		bg_color = (86 << 24) | (255 << 16) | (86 << 8);
		break;

	case LIGHT_CYAN:
		bg_color = (87 << 24) | (255 << 16) | (255 << 8);
		break;

	case LIGHT_RED:
		bg_color = (193 << 24) | (64 << 16) | (64 << 8);
		break;

	case LIGHT_MAGENTA:
		bg_color = (255 << 24) | (56 << 16) | (255 << 8);
		break;

	case YELLOW:
		bg_color = (255 << 24) | (56 << 16) | (255 << 8);
		break;

	case WHITE:
		bg_color = oxffffff << 8;
		break;
	}
}


// SetColor controls color of line functions, rectangle function
void setcolor(short color) {
	switch ( color ) {
	case BLACK:
		bg_color = 0x0;
		break;

	case BLUE:
		bg_color = 160 << 8;
		break;

	case GREEN:
		bg_color = 173 << 16;
		break;

	case CYAN:
		bg_color = (174 << 16) | (174 << 8);
		break;

	case RED:
		bg_color = 175 << 24;
		break;

	case MAGENTA:
		bg_color = (174 << 24) | (173 << 8);
		break;

	case BROWN:
		bg_color = (174 << 24) | (87 << 16);
		break;

	case LIGHT_GRAY:
		bg_color = 0xacacac << 8;
		break;

	case DARK_GRAY:
		bg_color = (120 << 24) | (120 << 16) | (120 << 8);
		break;

	case LIGHT_BLUE:
		bg_color = (86 << 24) | (86 << 16) | (255 << 8);
		break;

	case LIGHT_GREEN:
		bg_color = (86 << 24) | (255 << 16) | (86 << 8);
		break;

	case LIGHT_CYAN:
		bg_color = (87 << 24) | (255 << 16) | (255 << 8);
		break;

	case LIGHT_RED:
		bg_color = (193 << 24) | (64 << 16) | (64 << 8);
		break;

	case LIGHT_MAGENTA:
		bg_color = (255 << 24) | (56 << 16) | (255 << 8);
		break;

	case YELLOW:
		bg_color = (255 << 24) | (56 << 16) | (255 << 8);
		break;

	case WHITE:
		bg_color = oxffffff << 8;
		break;
	}
}

// SetTextColor (orwhatever its called) Determines the text color


void outtextxy(short x_coor, short y_coor, string * text) {
	SDL_Rect src;
	int W, H;

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



void textxy(short x, short y, string * s) {
	SDL_SetRenderDrawColor(renderer_main, 0x0, 0x0, 0x0, 0xff);
	bar(x, y, x + s->length() * 8; y + 7);

	outtextxy(x, y, s);
}



void coltextxy(short x, short y, string * s, SDL_Color c)
