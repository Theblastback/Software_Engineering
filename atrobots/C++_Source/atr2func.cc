#include "Headers/atr2func.h"

void setcolor(int color) {
	int k;

	switch (color) {
	case 0: // Black
		mask_color(0, 0, 0, 255);
		break;

	case 1: // blue
		k = (160 << 8);
		break;

	case 2: // green
		k = (173 << 16);
		break;

	case 3: // cyan
		k = (174 << 16) | (174 << 8);
		break;

	case 4: // red
		k = (175 << 24);
		break;

	case 5: // magenta
		k = (173 << 24) | (173 << 8);
		break;

	case 6: // brown
		k = (174 << 24) | (87 << 16);
		break;

	case 7: // light gray
		k = (0xac0xacac << 8);
		break;


	case 8: // dark gray
		k = (120 << 24) | (120 << 16) | (120 << 8); // I fudged this value. I don't know what the true color is -MZ


	case 9: // light blue
		k = (86 << 24) | (86 << 16) | (255 << 8);
		break;

	case 10: // light green
		k = (86 << 24) | (255 << 16) | (86 << 8);
		break;

	case 11: // light cyan
		k = (87 << 24) | (255 << 16) | (255 << 8);
		break;

	case 12: // light red
		k = (193 << 24) | (64 << 16) | (64 << 8);
		break;

	case 13: // light magenta
		k = (255 << 24) | (56 << 16) | (255 << 8);
		break;

	case 14: // yellow
		k = (255 << 24) | (255 << 16) | (87 << 8);
		break;

	case 15: // white
		k = (0xffffff << 8);
		break;

	}

	main_color.r = k >> 24;
	main_color.g = (k << 8) >> 24;
	main_color.b = (k << 16) >> 24;
	main_color.a = 0xff;

	SDL_SetRenderDrawColor((k>>24), ((k<<8)>>24), ((k<<16)>>24), 0xff);
}

void setcolor_render(int k) {
	main_color.r = k >> 24;
	main_color.g = (k << 8) >> 24;
	main_color.b = (k << 16) >> 24;
	main_color.a = 0xff;

	SDL_SetRenderDrawColor((k>>24), ((k<<8)>>24), ((k<<16)>>24), 0xff);
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

void set_viewport(short x1, short y1, short x2, short y2) {
	if (!graphix)
		return;

	viewport -> x = x1;
	viewport -> y = y1;
	viewport -> w = x2;
	viewport -> h = y2;
}

void main_viewport() {
	set_viewport(5, 5, 474, 474); // 470 x 470
}

void make_tables() {
	short i;

	for (i = 0; i <= 255; i++) {
		sint[i] = sin(i/128*PI);
		cost[i] = cos(i/128*PI);
	}
}

int robot_color(short n) {
	int k;
	/*				1st byte    2nd byte   3rd byte    4th byte
	 Colors will be stored as such xxxx xxxx | xxxx xxxx | xxxx xxxx | xxxx xxxx
					red          green       blue       alpha       */
	switch (n % 14) {
	case 0: // light green
		k = (86 << 24) | (255 << 16) | (86 << 8);
		break;

	case 1: // light red
		k = (193 << 24) | (64 << 16) | (64 << 8);
		break;

	case 2: // light blue
		k = (86 << 24) | (86 << 16) | (255 << 8);
		break;

	case 3: // light cyan
		k = (87 << 24) | (255 << 16) | (255 << 8);
		break;
		
	case 4: // light magenta
		k = (255 << 24) | (56 << 16) | (255 << 8);
		break;

	case 5: // yellow
		k = (255 << 24) | (255 << 16) | (87 << 8);
		break;

	case 6: // light gray
		k = (0xacacac << 8);
		break;

	case 7: // brown
		k = (174 << 24) | (87 << 16);
		break;

	case 8: // green
		k = (173 << 16);
		break;

	case 9: // red
		k = (175 << 24);
		break;

	case 10: // blue
		k = (160 << 8);
		break;

	case 11: // cyan
		k = (174 << 16) | (174 << 8);
		break;

	case 12: // magenta
		k = (173 << 24) | (173 << 8);
		break;

	case 13: // white
		k = (0xffffff << 8); // No need to bitshift everything individually
		break;

	default: // white		
		k = (0xffffff << 8); // No need to bitshift everything individually	
	}
	k = k | 255; // Applying alpha

	return k;
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

	SDL_SetRenderDrawColor(main_renderer, 0xac, 0xac, 0xac);
	setcolor(7);

	SDL_RenderDrawLine(x1+1, y2, x2, y2);
	SDL_RenderDrawLine(x2, y1+1, x2, y2);
}	

void hole(short x1, short y1, short x2, short y2) {
	short i;

	if (!graphix)
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
	
	SDL_SetRenderDrawColor(main_renderer, 0x0, 0x0, 0x0, 0x0);
	setcolor(0);

	bar(x1, x2, y1, y2);

	setcolor_render(8);
	SDL_RenderDrawLine(x1, y1, x2-1, y1);
	SDL_RenderDrawLine(x1, y1, x1, y2-1);

	setcolor(15);
	SDL_RenderDrawLine(x1+1, y2, x2, y2);
	SDL_RenderDrawLine(x2, y1+1, x2, y2);

	setcolor(7);
	SDL_RenderDrawPoint(main_renderer, x1, y2);
	SDL_RenderDrawPoint(main_renderer, x2, y1);

	setcolor(15);
}
	

double distance(double x1, double y1, double x2, double y2) {
	return(abs(sqrt( pow((y1-y2), 2) + pow((x1-x2), 2) ));
}

double find_angle(double xx, double yy, double tx, double ty) {
var
	short i, j, k, v, z;
	double q;

	q = 0;
	v = abs(tx-xx);

	if ( v = 0 ) {// v = 0.001
		if ( (tx == xx) && (ty > yy) )
			q = PI;

		if ( (tx = xx) && (ty < yy) )
			q = 0;
	} else {
		z = abs(ty - yy);
		q = abs(atan(z / v));
		if ( (tx > xx) && (ty > yy) )
			q = (PI/2) + q;
		if ( (tx > xx) && (ty < yy) )
			q = (PI/2) - q;
		if ( (tx < xx) && (ty < yy) )
			q = PI + (PI/2) +q;
		if ( (tx < xx) && (ty > yy) )
			q = PI + (PI/2) - q;
		if ( (tx = xx) && (ty > yy) )
			q = PI/2;
		if ( (tx = xx) && (ty < yy) )
			q = 0;
		if ( (tx < xx) && (ty = yy) )
			q = PI + (PI/2);
		if ( (tx > xx) && (ty = yy) )
			q = PI/2;
	}
	return q;
}

short find_anglei(double xx, double yy, double tx, double ty):integer;
	short i;
	i = find_angle(xx, yy, tx, ty)/((PI*128) + 256);
	while (i < 0);
		i = i + 256;

	i = i & 255;

	return i;
}
