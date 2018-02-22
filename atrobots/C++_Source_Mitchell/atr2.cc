#include "Headers/filelib.h"
#include "Headers/myfile.h"
#include "Headers/atr2func.h"
#include <time.h>
#include <iostream>


// Begin global variable/definition listings
std::string progname		= "AT-Robots";
std::string version		= "2.11";
std::string cnotice1		= "Copyright 1997 ""99, Ed T. Toton III";
std::string cnotice2		= "All Rights Reserved.";
std::string cnotice3		= "Copyright 2014, William \"amos\" Confer";
std::string main_filename	= "atr2";
std::string robot_ext	= ".AT2";
std::string locked_ext	= ".ATL";
std::string config_ext	= ".ATS";
std::string compile_ext	= ".CMP";
std::string report_ext	= ".REP";

#define PI 3.141592

#define MININT -32768
#define MAXINT 32767

// Debugging/compiler
bool show_code = false;
bool compile_by_line = false;
#define MAX_VAR_LEN		16
bool debugging_compiler = false;

// RobotS
#define MAX_ROBOTS	31
#define MAX_CODE	1023
#define MAX_OP		3
#define STACK_SIZE	256
#define STACK_BASE	768
#define MAX_RAM		1023
#define MAX_VARS	256
#define MAX_LABELS	256
#define ACCELERATION	4
#define TURN_RATE	8
#define MAX_VEL		4
#define MAX_MISSILES	1023
#define MISSILE_SPD	32
#define HIT_RANGE	14
#define BLAST_RADIUS	25
#define CRASH_RANGE	8
#define MAX_SONAR	250
#define COM_QUEUE	512
#define MAX_QUEUE	255
#define MAX_CONFIG_POINTS 12
#define MAX_MINES	63
#define MINE_BLAST	35

// Simulation & graphics
#define SCREEN_SCALE	0.46
#define SCREEN_X	5
#define SCREEN_Y	5
#define ROBOT_SCALE	6
#define DEFAULT_DELAY	20
#define DEFAULT_SLICE	5
#define MINE_CIRCLE	(MINE_BLAST * SCREEN_SCALE) + 1
#define BLAST_CIRCLE	(BLAST_RADIUS * SCREEN_SCALE) + 1
#define MIS_RADIUS	(HIT_RANGE / 2) + 1
#define MAX_ROBOT_LINES	8

class op_rec {
public:
	short	op[MAX_OP+1];
};

typedef op_rec prog_type;


class config_rec {
public:
	short scanner, weapon, armor, engine, heatsinks, shield, mines;
};

class mine_rec {
public:
	double x, y;
	short detect, yield;
	bool detonate;
};

class robot_rec {
public:
	bool		is_locked;
	short		mem_watch;
	double	x, y, lx, ly, xv, yv, speed, shotstrength, damageadj, speedadj, meters;

	short	hd, thd, lhd, spd, tspd, armor, larmor, heat, lheat, ip, plen, scanarc,
		accuracy, shift, err, delay_left, robot_time_limit, max_time, time_left,
		lshift, arc_count, sonar_count, scanrange, last_damage, last_hit, transponder,
		shutdown, channel, lendarc, endarc, lstartarc, startarc, mines;

	short	tx[MAX_ROBOT_LINES], ltx[MAX_ROBOT_LINES], ty[MAX_ROBOT_LINES], lty[MAX_ROBOT_LINES];
	int	wins, trials, kills, deaths, startkills, shots_fired, match_shots, hits,
		damage_total, cycles_lived, error_count;
	robot_rec();

	config_rec	config;
	std::string	name;
	std::string	fn;
	bool	shields_up, lshields, overburn, keepshift, cooling, won;
	prog_type	code[MAX_CODE + 1];
	short		ram[MAX_RAM +1];
	mine_rec	mine[MAX_MINES +1];
	std::ofstream	errorlog;
};

class missile_rec {
public:
	double x, y, lx, ly, mult, mspd;
	short source, a, hd, rad, lrad, max_rad;
};

// Begin global variables
int paramcount;
char **paramstr;

// Robot variables
short num_robots;
typedef robot_rec * robot_ptr;

robot_ptr robot[MAX_ROBOTS + 5]; // Array started at -2, so I shifted it over
missile_rec missile[MAX_MISSILES + 1];

// Compiler variables
std::string	f;
short	numvars, numlabels, maxcode, lock_pos, lock_dat;
std::string	varname[MAX_VARS -1];
short	varloc[MAX_VARS -1];
std::string	labelname[MAX_VARS -1];
short	labelnum[MAX_VARS -1];
bool	show_source, compile_only;
std::string	lock_code;

// Simulator/graphics variables
bool	bout_over;
short	step_mode;	// 0 = off; for 0 < step_mode <= 9 = # of game cycles per step
short	temp_mode;	// Stores previous step_mode for return to step
short	step_count;	// Step counter used as break flag
bool	step_loop;	// Break flag for stepping

bool	old_shields, insane_missiles, debug_info, windoze, no_gfx, logging_errors,
	timing, show_arcs;
short	game_delay, time_slice, insanity, update_timer, max_gx, max_gy, stats_mode;
int	game_limit, game_cycle, matches, played, executed;

// General settings
bool	quit, report, show_cnotice;
short	kill_count, report_type;


// Begin functions

// Functions that need to be declared prior to being written
void prog_error(short, std::string);
void compile(short, std::string);
void setscreen();
void do_robot(short);
void do_mine(short, short);
void do_missile(short);
void create_robot(short, std::string);

void parse_param(std::string s) {
	fstream f;
	string fn, s1;
	bool found = false;

	s = btrim(ucase(s));
	if (s.compare("") == 0)
		return;
	if (s[0] == '#') {
		fn = rstr(s, s.length() - 1);
		if (fn.compare(base_name(fn)) == 0)
			fn = fn + config_ext;
		if (!exist(fn))
			prog_error(6, fn);

		f.open(fn.c_str(), fstream::in);
		if (!f.good())
			return;

		while (!f.eof()) {
			getline(f, s1);
			s1 = ucase(btrim(s1));
			if (s1[0] == '#')
				prog_error(7, s1);
			else
				parse_param(s1);
		}
		f.close();
	} else if ((s[0] == '/') || (s1[0] == '-') || (s1[0] == '=')) {
		s1 = rstr(s, s.length() - 1);
		switch (s1[0]) {
		case 'X':
			step_mode = value(rstr(s1, s1.length() - 1));
			found = true;
			if (step_mode == 0)
				step_mode = 1;
			if ((step_mode < 1) || (step_mode > 9))
				prog_error(24, rstr(s1, s1.length() - 1));
			break;
		case 'D':
			game_delay = value(rstr(s1, s1.length() - 1));
			found = true;
			break;
		case 'T':
			time_slice = value(rstr(s1, s1.length() - 1));
			found = true;
			break;
		case 'L':
			game_limit = value(rstr(s1, s1.length() - 1)) * 1000;
			found = true;
			break;
		case 'Q':
			sound_on = false;
			found = true;
			break;
		case 'M':
			matches = value(rstr(s1, s1.length() - 1));
			found = true;
			break;
		case 'S':
			show_source = false;
			found = true;
			break;
		case 'G':
			no_gfx = true;
			found = true;
			break;
		case 'R':
			report = true;
			found = true;
			if (s1.length() > 1)
				report_type = value(rstr(s1, s1.length() - 1));
			break;
		case 'C':
			compile_only = true;
			found = true;
			break;
		case '^':
			show_cnotice = false;
			found = true;
		case 'A':
			show_arcs = true;
			found = true;
			break;
		case 'W':
			windoze = false;
			found = true;
			break;
		case '$':
			debug_info = true;
			found = true;
			break;
		case '!':
			insane_missiles = true;
			if (s1.length() > 1) {
				insanity = value(rstr(s1, s1.length() - 1));
				found = true;
			}
			break;
		case '@':
			old_shields = true;
			found = true;
			break;
		case 'E':
			logging_errors = true;
			found = true;
			break;
		}
		if (insanity < 0)
			insanity = 0;
		else if (insanity > 15)
			insanity = 15;
	} else if (s[1] == ';') {
		found = true;
	} else if ((num_robots < MAX_ROBOTS) && (s.compare("") != 0)) {
		num_robots++;
		create_robot(num_robots, s);
		found = true;
		if (num_robots == MAX_ROBOTS)
			cout << "Maximum number of robots reached." << endl;
	} else
		prog_error(10, "");

	if (!found)
		prog_error(8, s);
}


std::string operand(short n, short m) {
	std::string s = cstr(n);

	switch (m & 7) {	// Microcode
	case 1:		// 1 = variable/memory access
		s = "@" + s;
		break;
	case 2:		// 2 = :label
		s = ":" + s;
		break;
	case 3:		// 3 = !label (unresolved)
		s = "$" + s;
		break;
	case 4:		// 4 = !label (resolved)
		s = "!" + s;
		break;
	default:		// 0 = instruction/number/constant
		s = cstr(n);
	}

	if ( (m & 8) > 0 )
		s = "[" + s + "]";

	return s;
}


std::string mnemonic(short n, short m) {
	std::string s = cstr(n);

	if ( m == 0 )
		switch (n) {
		case 0: s = "NOP"; break;
		case 1: s = "ADD"; break;
		case 2: s = "SUB"; break;
		case 3: s = "OR"; break;
		case 4: s = "AND"; break;
		case 5: s = "XOR"; break;
		case 6: s = "NOT"; break;
		case 7: s = "MPY"; break;
		case 8: s = "DIV"; break;
		case 9: s = "MOD"; break;
		case 10: s= "RET"; break;
		case 11: s= "CALL";break;
		case 12: s= "JMP"; break;
		case 13: s= "JLS"; break;
		case 14: s= "JGR"; break;
		case 15: s= "JNE"; break;
		case 16: s= "JE"; break;
		case 17: s= "SWAP";break;
		case 18: s= "DO"; break;
		case 19: s= "LOOP";break;
		case 20: s= "CMP"; break;
		case 21: s= "TEST";break;
		case 22: s= "MOV"; break;
		case 23: s= "LOC"; break;
		case 24: s= "GET"; break;
		case 25: s= "PUT"; break;
		case 26: s= "INT"; break;
		case 27: s= "IPO"; break;
		case 28: s= "OPO"; break;
		case 29: s= "DELAY";break;
		case 30: s= "PUSH"; break;
		case 31: s= "POP"; break;
		case 32: s= "ERR"; break;
		case 33: s= "INC"; break;
		case 34: s= "DEC"; break;
		case 35: s= "SHL"; break;
		case 36: s= "SHR"; break;
		case 37: s= "ROL"; break;
		case 38: s= "ROR"; break;
		case 39: s= "JZ"; break;
		case 40: s= "JNZ"; break;
		case 41: s= "JGE"; break;
		case 42: s= "JLE"; break;
		case 43: s= "SAL"; break;
		case 44: s= "SAR"; break;
		case 45: s= "NEG"; break;
		case 46: s= "JTL"; break;
		default: s = "XXX";
		}
	else
		s = operand(n, m);

	return s;
}


short max_shown() {
	switch(stats_mode) {
	case 1:
		return 12;
	case 2:
		return 32;
	default:
		return 6;
	}
}


bool graph_check(short n) {
	bool ok = true;

	if (!graphix || (n < 0) || (n > num_robots) || (n >= max_shown()))
		ok = false;

	return ok;
}

void robot_graph(short n) {
	switch (stats_mode) {
	case 1:
		viewport(480, 4 + n * 35, 635, 37 + n * 35);
		max_gx = 155;
		max_gy = 33;
		break;
	case 2:
		viewport(480, 4 + n * 70, 635, 15 + n * 13);
		max_gx = 155;
		max_gy = 11;
		break;
	default:
		viewport(480, 4 + n * 70, 635, 70 + n * 70);
		max_gx = 155;
		max_gy = 66;
	}
	setfillstyle(robot_color(n));
	setcolor(robot_color(n));
}

void update_armor(short n) {
	if ( graph_check(n) && (step_mode <= 0) ) {
		robot_graph(n);
		if (robot[n] -> armor > 0) {
			switch (stats_mode) {
			case 1:
				bar(30, 13, 29 + robot[n] -> armor, 18);
				break;
			case 2:
				bar(88, 3, 87 +(robot[n] -> armor >> 2), 8);
				break;
			default:
				bar(30, 25, 29 + (robot[n] -> armor), 30);
			}
		}
		setfillstyle(DARK_GRAY);
		if (robot[n] -> armor < 100) {
			switch (stats_mode) {
			case 1:
				bar(30 + robot[n] -> armor, 13, 29 + robot[n] -> armor, 30);
				break;
			case 2:
				bar(88, 3, 87 + (robot[n] -> armor >> 2), 8);
				break;
			default:
				bar(30, 25, 29 + robot[n] -> armor, 30);
			}
		}
	}
}

void update_heat(short n) {
	if ( graph_check(n) && (step_mode <= 0) ) {
		robot_graph(n);
		if ( robot[n] -> heat > 5 ) {
			switch (stats_mode) {
			case 1:
				bar(30, 23, 29 + (robot[n] -> heat / 5), 28);
				break;
			case 2:
				bar(127, 3, 126 + (robot[n] -> heat / 20), 8);
				break;
			default:
				bar(30, 35, 29 + (robot[n] -> heat / 5), 40);
			}
		}
		setfillstyle(DARK_GRAY);
		if (robot[n] -> heat < 500 ) {
			switch (stats_mode) {
			case 1:
				bar(30 + (robot[n] -> heat / 5), 23, 129, 28);
				break;
			case 2:
				bar(127 + (robot[n] -> heat / 20), 3, 151, 8);
				break;
			default:
				bar(30 + (robot[n] -> heat / 5), 35, 129, 40);
			}
		}
	}
}

void robot_error(short n, short i, std::string ov) {
	if (graph_check(n) && (step_mode <= 0)) {
		if (stats_mode == 0) {
			robot_graph(n);
			setfillstyle(BLACK);
			bar(66, 56, 154, 64);
			setcolor(robot_color(n));
			outtextxy(66, 56, addrear(cstr(i), 7) + hex(i));
			// chirp();
		}
	}
}

void update_lives(short n) {
	if ( graph_check(n) && (stats_mode == 0) ) {
		robot_graph(n);
		setcolor(robot_color(n) - 8);
		setfillstyle(BLACK);
		bar(11, 46, 130, 53);

		outtextxy(11, 46, "K: ");
		outtextxy(29, 46, zero_pad(robot[n] -> kills, 4));
		outtextxy(80, 46, "D :");
		outtextxy(98, 46, zero_pad(robot[n] -> deaths, 4));
	}
}

void update_cycle_window() {
	if ( !graphix ) // No graphics
		std::cout << endl << "Match " << played << "/" << matches << " Cycle: " << zero_pad(game_cycle, 9);
	else {
		viewport(480, 440, 635, 475);
		setfillstyle(BLACK);
		bar(59, 2, 154, 10);
		setcolor(LIGHT_GRAY);
		outtextxy(75, 3, zero_pad(game_cycle, 9));
		SDL_RenderPresent(renderer_main);
	}
}


// Initialize the entire screen
void setscreen() {
	short i;

	if ( !graphix )
		return;

	// Window & renderer will be created in init, so no to worry here
	viewport(0, 0, 639, 479); // XXX Legitimate size of window XXX
	box(0, 0, 639, 479);

	stats_mode = 0;
	if ((num_robots >= 0) && (num_robots <= 5)) {
		stats_mode = 0;
	} else if ((num_robots >= 6) && (num_robots <= 11)) {
		stats_mode = 1;
	} else if ((num_robots >= 12) && (num_robots <= MAX_ROBOTS)) {
		stats_mode = 2;
	} else
		stats_mode = 0;
	

	// Main arena
	hole(4, 4, 475, 475);

	// Cycle window
	viewport(480, 480, 635, 475);
	hole(0, 0, 155, 45);
	setcolor(LIGHT_GRAY);
//	outtextxy(3, 3, "FreeMem: " + cstr(memavail);  XXX Not possible. Cannot get amount of bytes available in heap storage (that's what memavail is)
	outtextxy(3, 13, "Cycle:    ");
	outtextxy(3, 23, "Limit     " + zero_pad(game_limit, 9));
	outtextxy(2, 33, "Match:    " + cstr(played) + "/" + cstr(matches));
	update_cycle_window();

	// Robot windows
	for (i = 0; i <= MAX_ROBOTS; i++) {
		if ( i < max_shown() ) {
			robot_graph(i);
			hole(0, 0, max_gx, max_gy);
			if ( i <= num_robots ) {
				setcolor(robot_color(i));
				outtextxy(3, 2, base_name( no_path(robot[i] -> fn)) );
				switch (stats_mode) {
				case 1:
					outtextxy(3, 12, " A:");
					outtextxy(2, 22, " H:");
					break;
				case 2:
					setcolor(robot_color(i) & 7);
					outtextxy(80, 2, "A");
					outtextxy(118, 2, "H");
					break;
				default:
					outtextxy(3, 24, " A:");
					outtextxy(3, 34, " H:");
				}

				setcolor(robot_color(i));
				if ( stats_mode <= 1 ) {
					outtextxy(80, 2, "Wins: ");
					outtextxy(122, 2, zero_pad(robot[i] -> wins, 4));
				}

				if ( stats_mode == 0 ) {
					outtextxy(3, 56, " Error:");
					setcolor(robot_color(i) & 7);
					outtextxy(3, 12, robot[i] -> name);
					setcolor(DARK_GRAY);
					outtextxy(66, 56, "None");
				}

				robot[i] -> lx = 1000 - robot[i] -> x;
				robot[i] -> ly = 1000 - robot[i] -> y;
				update_armor(i);
				update_heat(i);
				update_lives(i);
			} else {
				setfillstyle(DARK_GRAY); // XXX This would come out checkered. New color scheme may be needed
				bar(1, 1, max_gx - 1, max_gy - 1);
			}
		}
	}
}


void graph_mode(bool on) {
	if ( on && !graphix ) {
		// Replace Graph_VGA function with sdl
		setscreen();
		graphix = true;
	} else if ( !on && graphix ) {
		// Turn graphics off
		SDL_SetRenderDrawColor(renderer_main, 0x0, 0x0, 0x0, 0xff);
		SDL_RenderClear(renderer_main);
		// Blacks out entire screen. However, to retain portability, we need the window still.
		// Without the window, key presses cannot be detected

		graphix = false;
	}
}


void print_code(short n, short p) {
	short i;

	std::cout << (hex(p) + ": ");
	for ( i = 0; i <= MAX_OP; i++ )
		std::cout << zero_pad(robot[n] -> code[p].op[i], 5), " ";

	std::cout << " =  ";
	for ( i = 0; i <= MAX_OP; i++ )
		std::cout << hex(robot[n] -> code[p].op[i]) << "h ";

	std::cout << endl << endl;
}


void check_plen(short plen) {
	if (plen > MAX_CODE )
		prog_error(16, "\nMaximum progrm length exceeded, (Limit: " + cstr(MAX_CODE + 1) + " compiled lines)");
}


void robot_config(short n) {
	short i, j, k;

	// Doing case statements like this to reduce line count
	switch (robot[n] -> config.scanner) {
	case 5: robot[n] -> scanrange = 1500; break;
	case 4: robot[n] -> scanrange = 1000; break;
	case 3: robot[n] -> scanrange = 700; break;
	case 2: robot[n] -> scanrange = 500; break;
	case 1: robot[n] -> scanrange = 350; break;
	default: robot[n] -> scanrange = 250;
	}

	switch (robot[n] -> config.weapon) {
	case 5: robot[n] -> shotstrength = 1.5; break;
	case 4: robot[n] -> shotstrength = 1.35; break;
	case 3: robot[n] -> shotstrength = 1.2; break;
	case 2: robot[n] -> shotstrength = 1; break;
	case 1: robot[n] -> shotstrength = 0.8; break;
	default: robot[n] -> shotstrength = 0.5;
	}

	switch (robot[n] -> config.armor) {
	case 5:
		robot[n] -> damageadj = 0.66;
		robot[n] -> speedadj = 0.66;
		break;
	case 4:
		robot[n] -> damageadj = 0.77;
		robot[n] -> speedadj = 0.75;
		break;
	case 3:
		robot[n] -> damageadj = 0.83;
		robot[n] -> speedadj = 0.85;
		break;
	case 2:
		robot[n] -> damageadj = 1;
		robot[n] -> speedadj = 1;
		break;
	case 1:
		robot[n] -> damageadj = 1.5;
		robot[n] -> speedadj = 1.2;
		break;
	default:
		robot[n] -> damageadj = 2;
		robot[n] -> speedadj = 1.33;
	}

	switch (robot[n] -> config.engine) {
		case 5: robot[n] -> speedadj *= 1.5; break;
		case 4: robot[n] -> speedadj *= 1.35; break;
		case 3: robot[n] -> speedadj *= 1.2; break;
		case 2: robot[n] -> speedadj *= 1; break;
		case 1: robot[n] -> speedadj *= 0.8; break;
		default: robot[n] -> speedadj *= 0.5;
	}
	// Heatsinks are handled seperately
	switch (robot[n] -> config.mines) {
	case 5: robot[n] -> mines = 24; break;
	case 4: robot[n] -> mines = 16; break;
	case 3: robot[n] -> mines = 10; break;
	case 2: robot[n] -> mines = 6; break;
	case 1: robot[n] -> mines = 4; break;
	default:
		robot[n] -> mines = 2;
		robot[n] -> config.mines = 0;
	}

	robot[n] -> shields_up = false;
	if ( (robot[n] -> config.shield < 3) || (robot[n] -> config.shield > 5) )
		robot[n] -> config.shield = 0;
	if ( (robot[n] -> config.heatsinks < 0) || (robot[n] -> config.heatsinks > 5) )
		robot[n] -> config.heatsinks = 0;
}


void reset_software(short n) {
	short i;

	for (i = 0; i <= MAX_RAM; i++)
		robot[n] -> ram[i] = 0;

	robot[n] -> ram[71] = 768;
	robot[n] -> thd = robot[n] -> hd;
	robot[n] -> tspd = 0;
	robot[n] -> scanarc = 8;
	robot[n] -> shift = 0;
	robot[n] -> err = 0;
	robot[n] -> overburn = false;
	robot[n] -> keepshift = false;
	robot[n] -> ip = 0;
	robot[n] -> accuracy = 0;
	robot[n] -> meters = 0;
	robot[n] -> delay_left = 0;
	robot[n] -> time_left = 0;
	robot[n] -> shields_up = false;
}


void reset_hardware(short n) {
	short i;
	double d, dd;

	// robot[n] -> dereference
	for ( i = 0; i < MAX_ROBOT_LINES; i++ ) {
		robot[n] -> ltx[i] = 0;
		robot[n] -> tx[i] = 0;
		robot[n] -> lty[i] = 0;
		robot[n] -> ty[i] = 0;
	}

	do {
		robot[n] -> x = rand() % 1000;
		robot[n] -> y = rand() % 1000;
		dd = 1000;
		for ( i = 0; i < num_robots; i++ ) {
			if ( robot[i] -> x < 0 )
				robot[i] -> x = 0;
			if ( robot[i] -> x > 1000 )
				robot[i] -> x = 1000;
			if ( robot[i] -> y < 0 )
				robot[i] -> y = 0;
			if ( robot[i] -> y > 1000 )
				robot[i] -> y = 1000;

			d = _distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y);
			if ( (robot[i] -> armor > 0) && (i != n) && ( d < dd) )
				dd = d;
		}
	} while ( dd > 32 );

	for ( i = 0; i < MAX_MINES; i++ ) {
		robot[n] -> mine[i].x = 1;
		robot[n] -> mine[i].y = -1;
		robot[n] -> mine[i].yield = 0;
		robot[n] -> mine[i].detonate = false;
		robot[n] -> mine[i].detect = 0;
	}
	robot[n] -> lx = -1;
	robot[n] -> ly = -1;
	robot[n] -> hd = rand() % 256;
	robot[n] -> shift = 0;
	robot[n] -> lhd = robot[n] -> hd;
	robot[n] -> lshift = robot[n] -> shift + 1;
	robot[n] -> spd = 0;
	robot[n] -> speed = 0;
	robot[n] -> cooling = false;
	robot[n] -> armor = 100;
	robot[n] -> larmor = 0;
	robot[n] -> heat = 0;
	robot[n] -> lheat = 0;
	robot[n] -> match_shots = 0;
	robot[n] -> won = false;
	robot[n] -> last_damage = 0;
	robot[n] -> last_hit = 0;
	robot[n] -> transponder = n + 1;
	robot[n] -> meters = 0;
	robot[n] -> shutdown = 400;
	robot[n] -> shields_up = false;
	robot[n] -> channel = 	robot[n] -> transponder;
	robot[n] -> startkills = robot[n] -> kills;

	robot_config(n);
}

void init_robot(short n) {
	short i, j, k, l;

	robot[n] -> wins = 0;
	robot[n] -> trials = 0;
	robot[n] -> kills = 0;
	robot[n] -> deaths = 0;
	robot[n] -> shots_fired = 0;
	robot[n] -> match_shots = 0;
	robot[n] -> hits = 0;
	robot[n] -> damage_total = 0;
	robot[n] -> cycles_lived = 0;
	robot[n] -> error_count = 0;
	robot[n] -> plen = 0;
	robot[n] -> max_time = 0;
	robot[n] -> name = "";
	robot[n] -> fn = "";
	robot[n] -> speed = 0;
	robot[n] -> arc_count = 0;
	robot[n] -> sonar_count = 0;
	robot[n] -> robot_time_limit = 0;
	robot[n] -> scanrange = 1500;
	robot[n] -> shotstrength = 1;
	robot[n] -> damageadj = 1;
	robot[n] -> speedadj = 1;
	robot[n] -> mines = 0;

	robot[n] -> config.scanner = 5;
	robot[n] -> config.weapon = 2;
	robot[n] -> config.armor = 2;
	robot[n] -> config.engine = 2;
	robot[n] -> config.heatsinks = 1;
	robot[n] -> config.shield = 0;
	robot[n] -> config.mines = 0;

	for ( i = 0; i <= MAX_RAM; i++ )
		robot[n] -> ram[i] = 0;

	robot[n] -> ram[71] = 768;

	for ( i = 0; i <= MAX_CODE; i++ )
		for ( k = 0; k <= MAX_OP; k++ )
			robot[n] -> code[i].op[k] = 0;

	reset_hardware(n);
	reset_software(n);
}


void create_robot(short n, std::string filename) {
	short i, j, k;

	for (i = 0; i <= MAX_ROBOTS + 4; i++)
		robot[i] = new robot_rec();

	init_robot(n);
	filename = ucase(btrim(filename));
	if ( filename.compare(base_name(filename)) == 0 )
		if ( filename[0] == '?' )
			filename = filename + locked_ext;
		else
			filename = filename + robot_ext;

	if ( filename[0] == '?' )
		filename = rstr(filename, filename.length() -1);

	robot[n] -> fn = base_name(no_path(filename));
	compile(n, filename);
	robot_config(n);

	k = robot[n] -> config.scanner + robot[n] -> config.armor + robot[n] -> config.weapon +
		robot[n] -> config.engine + robot[n] -> config.heatsinks + robot[n] -> config.shield + robot[n] -> config.mines;

	if ( k > MAX_CONFIG_POINTS )
		prog_error(21, cstr(k) + "/" + cstr(MAX_CONFIG_POINTS));
}


void shutdown() {
	short i;

	graph_mode(false);
	if (show_cnotice) {
		textcolor(CYAN);
		std::cout << progname << " " << version << " ";
		std::cout << cnotice1 << endl << cnotice2 << endl << cnotice3 << endl;
	}

	textcolor(LIGHT_GRAY);
	std::cout << endl;
	if ( logging_errors ) {
		for ( i = 0; i <= num_robots; i++ ) {
			std::cout << "Robot error-log created: " << base_name(robot[i] -> fn) << ".ERR"<< endl;
			robot[i] -> errorlog.close();
		}
	}

	for (i; i <= MAX_ROBOTS + 4; i++)
		delete robot[i];

	SDL_DestroyRenderer(renderer_main);
	SDL_DestroyWindow(window_main);

	TTF_Quit();
	SDL_Quit();

	std::exit(EXIT_SUCCESS);
}


void init() {
	short i;

	if ( SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cerr << "ERROR: Could not init SDL: " << SDL_GetError();
		std::exit(EXIT_FAILURE);
	}


	if ( TTF_Init() == -1 ) {
		std::cout << "ERROR: Could not init TTF: " << TTF_GetError();
		SDL_Quit();
	}

	// Load text type here *sdl_ttf critical*


	SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_OPENGL, &window_main, &renderer_main);
	

	if ( debugging_compiler || compile_by_line || show_code ) {
		std::cout << "!!! Warning !!! Compiler Debugging enabled !!!";
		readkey();
		std::cout << endl;
	}

	step_mode = 0;
	logging_errors = false;
	stats_mode = 0;
	insane_missiles = false;
	insanity = 0;
	delay_per_sec = 0;
	windoze = true;
	graphix = false;
	no_gfx = false; // <-
	sound_on = true;
	timing = true;
	matches = 1;
	played = 0;
	old_shields = false;
	quit = false;
	compile_only = false;
	show_arcs = false;
	debug_info = false;
	show_cnotice = true;
	show_source = true;
	report = false;
	kill_count = 0;
	maxcode = MAX_CODE;

	make_tables();
	srand(time(NULL)); // Equivalent of randomize()
	num_robots = -1;
	game_limit = 100000;
	game_cycle = 0;
	game_delay = DEFAULT_DELAY;
	time_slice = DEFAULT_SLICE;

	for ( i = 0; i <= MAX_MISSILES; i++ ) {
		missile[i].a = 0;
		missile[i].source = -1;
		missile[i].x = 0;
		missile[i].y = 0;
		missile[i].lx = 0;
		missile[i].ly = 0;
		missile[i].mult = 1;
	}

	registered = false;
	reg_name = "Unregistered";
	reg_num = 0xffff;
	check_registration();

	std::cout << endl;
	textcolor(CYAN);
	std::cout << progname << " " << version << " ";
	std::cout << cnotice1 << endl << cnotice2 << endl;
	textcolor(LIGHT_GRAY);
	if ( !registered ) {
		textcolor(RED);
		std::cout << "Unregistered version" << endl;
		textcolor(LIGHT_GRAY);
	} else
		std::cout << "Registered to: " << reg_name;

	std::cout << endl;

	// delete_compile_report();

	std::string tmp;
	if ( paramcount > 0 )
		for ( i = 1; i <= paramcount; i++ ) {
			tmp = paramstr[i]; // Arguement is paramstr. It is effectively argv, but global
			parse_param(btrim(ucase(tmp)));
		}
	else
		prog_error(5, "");

	temp_mode = step_mode;

	if ( logging_errors )
		for ( i = 0; i <= num_robots; i++ )
			robot[i] -> errorlog.open(base_name(robot[i] -> fn) + ".ERR");

	if ( compile_only )
	//	write_compile_report();
	if ( num_robots < 1 )
		prog_error(4, "");

	if ( !no_gfx ) // If there are graphics
		graph_mode(true);	// Start in graph mode

	if ( matches > 100000 )
		matches = 100000;
	if ( matches < 1 )
		matches = 1;
	if ( game_delay > 1000 )
		game_delay = 1000;
	if ( game_delay < 0 )
		game_delay = 0;
	if ( time_slice > 100 )
		time_slice = 100;
	if ( time_slice < 1 )
		time_slice = 1;
	if ( game_limit < 0 )
		game_limit = 0;
	if (game_limit > 100000)
		game_limit = 100000;
	if ( maxcode < 1 )
		maxcode = 1; // 2 lines of max code. 0 based
	if ( maxcode > MAX_CODE )
		maxcode = MAX_CODE;

	for ( i = num_robots + 1; i <= MAX_ROBOTS + 4; i++ )
		robot[i] = robot[0];

}



void draw_robot(short n) {
	short i, t;
	double xx, yy;

	if ( (n < 0) || (n > num_robots) )
		return;

	if ( robot[n] -> x > 1000 )
		robot[n] -> x = 1000;
	if ( robot[n] -> y > 1000 )
		robot[n] -> y = 1000;
	if ( robot[n] -> x < 0 )
		robot[n] -> x = 0;
	if ( robot[n] -> y < 0 )
		robot[n] -> y = 0;

	// Set up for draw
	xx = (robot[n] -> x * SCREEN_SCALE) + SCREEN_X;
	yy = (robot[n] -> y * SCREEN_SCALE) + SCREEN_Y;
	robot[n] -> hd = (robot[n] -> hd + 1024) & 255;
	robot[n] -> tx[0] = (xx + sint[robot[n] -> hd] * 5) + 0.5;
	robot[n] -> ty[0] = (yy - cost[robot[n] -> hd] * 5) + 0.5;

	robot[n] -> tx[1] = (xx + sint[(robot[n] -> hd + 0x68) & 255] * ROBOT_SCALE) + 0.5;
	robot[n] -> ty[1] = (yy - cost[(robot[n] -> hd + 0x68) & 255] * ROBOT_SCALE) + 0.5;

	robot[n] -> tx[2] = (xx + sint[(robot[n] -> hd + 0x98) & 255] * ROBOT_SCALE) + 0.5;
	robot[n] -> ty[2] = (yy - cost[(robot[n] -> hd + 0x98) & 255] * ROBOT_SCALE) + 0.5;
	t = (robot[n] -> hd + (robot[n] -> shift & 255) + 1024) & 255;

	robot[n] -> tx[3] = xx + 0.5;
	robot[n] -> ty[3] = yy + 0.5;

	robot[n] -> tx[4] = (xx + sint[t] * ROBOT_SCALE * 0.8) + 0.5;
	robot[n] -> ty[4] = (yy - cost[t] * ROBOT_SCALE * 0.8) + 0.5;

	robot[n] -> tx[5] = (xx + sint[(t + robot[n] -> scanarc + 1024) & 255] * robot[n] -> scanrange * SCREEN_SCALE) + 0.5;
	robot[n] -> ty[5] = (yy - cost[(t + robot[n] -> scanarc + 1024) & 255] * robot[n] -> scanrange * SCREEN_SCALE) + 0.5;

	robot[n] -> tx[6] = (xx + sint[(t - robot[n] -> scanarc + 1024) & 255] * robot[n] -> scanrange * SCREEN_SCALE) + 0.5;
	robot[n] -> ty[6] = (yy - cost[(t - robot[n] -> scanarc + 1024) & 255] * robot[n] -> scanrange * SCREEN_SCALE) + 0.5;

	robot[n] -> startarc = (( (265 - ((t + robot[n] -> scanarc) & 255)) / 256 * 360) + 90) + 0.5;
	robot[n] -> endarc = (( (265 - ((t - robot[n] -> scanarc) & 255)) / 256 * 360) + 90) + 0.5;

	if ( graphix ) {
		main_viewport();
		setcolor(0);
		if ( robot[n] -> lshields )
			circle(robot[n] -> ltx[3], robot[n] -> lty[3], ROBOT_SCALE);

		if ( robot[n] -> arc_count > 0 ) {
			line(robot[n] -> ltx[3], robot[n] -> lty[3], robot[n] -> ltx[5], robot[n] -> lty[5]);
			line(robot[n] -> ltx[3], robot[n] -> lty[3], robot[n] -> ltx[6], robot[n] -> lty[6]);
			if ( robot[n] -> scanrange < 1500 )
				arc(robot[n] -> ltx[3], robot[n] -> lty[3], robot[n] -> lstartarc, robot[n] -> lendarc, (robot[n] -> scanrange * SCREEN_SCALE) + 0.5);
		}

		if ( robot[n] -> sonar_count > 0 )
			circle(robot[n] -> ltx[3], robot[n] -> lty[3], (robot[n] -> scanrange * SCREEN_SCALE) + 0.5);

		if ( robot[n] -> armor > 0 ) { // Only erases bot if bot is alive
			line(robot[n] -> ltx[0], robot[n] -> lty[0], robot[n] -> ltx[1], robot[n] -> lty[1]);
			line(robot[n] -> ltx[1], robot[n] -> lty[1], robot[n] -> ltx[2], robot[n] -> lty[2]);
			line(robot[n] -> ltx[2], robot[n] -> lty[2], robot[n] -> ltx[0], robot[n] -> lty[0]);
			line(robot[n] -> ltx[3], robot[n] -> lty[3], robot[n] -> ltx[4], robot[n] -> lty[4]);
		}
	}

	if ( robot[n] -> armor > 0 ) { // If bot is alive, redraw
		if ( robot[n] -> arc_count > 0 )
			(robot[n] -> arc_count)--;
		if ( robot[n] -> sonar_count > 0 )
			(robot[n] -> sonar_count)--;
		if (graphix) { // Only draw if graphics are enabled
			setcolor(robot_color(n) & 7);
			if (robot[n]->shields_up)
				circle(robot[n]->tx[3], robot[n]->ty[3], ROBOT_SCALE);

			line(robot[n]->tx[0], robot[n]->ty[0], robot[n]->tx[1], robot[n]->ty[1]);
			line(robot[n]->tx[1], robot[n]->ty[1], robot[n]->tx[2], robot[n]->ty[2]);
			line(robot[n]->tx[2], robot[n]->ty[2], robot[n]->tx[0], robot[n]->ty[0]);
			setcolor(LIGHT_GRAY);

			line(robot[n]->tx[3], robot[n]->ty[3], robot[n]->tx[4], robot[n]->ty[4]);
			setcolor(DARK_GRAY);

			if (show_arcs && (robot[n]->arc_count > 0)) {
				line(robot[n]->tx[3], robot[n]->ty[3], robot[n]->tx[5], robot[n]->ty[6]);
				line(robot[n]->tx[3], robot[n]->ty[3], robot[n]->tx[6], robot[n]->ty[6]);
				if (robot[n]->scanrange < 1500)
					arc(robot[n]->tx[3], robot[n]->ty[3], robot[n]->startarc, robot[n]->endarc, (robot[n]->scanrange * SCREEN_SCALE) + 0.5);
			}
			if (show_arcs && (robot[n]->sonar_count > 0))
				circle(robot[n]->tx[4], robot[n]->ty[4], (MAX_SONAR * SCREEN_SCALE) + 0.5);
		}
	}

	robot[n] -> lx = robot[n] -> x;
	robot[n] -> ly = robot[n] -> y;
	robot[n] -> lhd = robot[n] -> hd;
	robot[n] -> lshift = robot[n] -> shift;
	robot[n] -> lshields = robot[n] -> shields_up;

	for ( i = 0; i < MAX_ROBOT_LINES; i++ ) {
		robot[n] -> ltx[i] = robot[n] -> tx[i];
		robot[n] -> lty[i] = robot[n] -> ty[i];
	}

	robot[n] -> lstartarc = robot[n] -> startarc;
	robot[n] -> lendarc = robot[n] -> endarc;
}


// The direct memory access from ram has been replaced with a psuedo ram array
short get_from_ram(short n, short i, short j) {
	short  k, l;

	if ( (i < 0) || (i > (MAX_RAM) +(((MAX_CODE + 1) <<3)-1)) ) {
		k = 0;
		robot_error(n, 4, cstr(i));
	} else {
		if ( i < MAX_RAM )
			k = robot[n] -> ram[i];
		else {
			l = i - MAX_RAM - 1;
			k = robot[n] -> code[l << 2].op[l & 3];
		}
	}
	return k;
}


short get_val(short n, short c, short o) {
	short i, j, k;

	k = 0;
	j = (robot[n] -> code[c].op[MAX_OP] << (4 * 0)) & 15;
	i = robot[n] -> code[c].op[o];

	if ( (j & 7) == 1 )
		k = get_from_ram(n, i, j);
	else
		k = i;
	if ( (j & 8) > 0 )
		k = get_from_ram(n, k, j);

	return k;
}

void put_val(short n, short c, short o, short v) {
	short i, j, k;
	i = 0; j = 0; k = 0;

	j = (robot[n] -> code[c].op[MAX_OP] >> (4 * o)) & 15;
	i = robot[n] -> code[c].op[o];
	if ( (j & 7) == 1 ) {
		if ( (i < 0) || (i > MAX_RAM) )
			robot_error(n, 4, cstr(i));
		else {
			if ( (j & 8) > 0 ) {
				i = robot[n] -> ram[i];
				if ( (i < 0) || (i > MAX_RAM) )
					robot_error(n, 4, cstr(i));
				else
					robot[n] -> ram[i] = v;
			} else
				robot[n] -> ram[i] = v;
		}
	} else
		robot_error(n, 3, "");
}

void push(short n, short v) {
	if ( (robot[n] -> ram[71] >= STACK_BASE) && (robot[n] -> ram[71] < (STACK_BASE + STACK_SIZE)) ) {
		robot[n] -> ram[robot[n] -> ram[71]] = v ;
		(robot[n] -> ram[71])++;
	} else
		robot_error(n, 1, cstr(robot[n] -> ram[71]));
}

short pop(short n) {
	short k;
	if ( (robot[n] -> ram[71] > STACK_BASE) && (robot[n] -> ram[71] <= (STACK_BASE + STACK_SIZE)) ) {
		(robot[n] -> ram[71])--;
		k = robot[n] -> ram[robot[n] -> ram[71]];
	} else
		robot_error(n, 5, cstr(robot[n] -> ram[71]));

	return k;
}


short find_label(short n, short l, short m) {
	short i, j, k;

	k = -1;
	if ( m == 3 )
		robot_error(n, 9, "");
	else
		if ( m == 4 )
			k = l;
		else
			for ( i = robot[n] -> plen; i >= 0; i-- ) {
				j = robot[n] -> code[i].op[MAX_OP] & 15;
				if ( (j == 2) && (robot[n] -> code[i].op[0] == l) )
					k = i;
			}
	return k;
}


void init_mine(short n, short detectrange, short size) {
	short i, k;

	k = -1;
	for ( i = 0; i <= MAX_MINES; i++ ) {
		if ( ((robot[n] -> mine[i].x < 0) || (robot[n] -> mine[i].x > 1000) || (robot[n] -> mine[i].y < 0) || (robot[n] -> mine[i].y > 1000) ||
			(robot[n] -> mine[i].yield <= 0)) && ( k < 0 ) ) {
			k = i;
		}
	}
	if ( k >= 0 ) {
		robot[n] -> mine[k].x = robot[n] -> x;
		robot[n] -> mine[k].y = robot[n] -> y;
		robot[n] -> mine[k].detect = detectrange;
		robot[n] -> mine[k].yield = size;
		robot[n] -> mine[k].detonate = false;
		// click(); XXX Sound function. Uncomment later
	}
}

short count_missiles() {
	short i, k;

	k = 0;
	for ( i = 0; i <= MAX_MISSILES; i++ ) {
		if ( missile[i].a > 0 )
			k++;
	}

	return k;
}


void init_missile(double xx, double yy, double xxv, double yyx, short dir, short s, short blast, bool ob) {
	short i, k;
	double m;
	bool sound;

	k = -1;
	// click(); XXX Sound function. Uncomment later
	for (i = MAX_MISSILES; i >= 0; i--)
		if (missile[i].a == 0)
			k = i;

	if ( k >= 0 ) {
		missile[k].source = s;
		missile[k].x = xx;
		missile[k].lx = missile[k].x;
		missile[k].y = yy;
		missile[k].ly = missile[k].y;
		missile[k].rad = 0;
		missile[k].lrad = 0;

		if ( ob )
			missile[k].mult = 1.25;
		else
			missile[k].mult = 1;

		if ( blast > 0 ) {
			missile[k].max_rad = blast;
			missile[k].a = 2;
		} else {
			if ( (s >= 0) && (s <= num_robots) )
				missile[k].mult = missile[k].mult * (robot[s] -> shotstrength);

			m = missile[k].mult;
			if ( ob )
				m = m + 0.25;

			missile[k].mspd = MISSILE_SPD * missile[k].mult;
			if ( insane_missiles ) {
				missile[k].mspd = 100 + (50 * insanity) * missile[k].mult;
			if ( (s >= 0) && ( s <= num_robots) )
				robot[s] -> heat += (20 * m);
				(robot[s] -> shots_fired)++;
				(robot[s] -> match_shots)++;
			}
		}
		missile[k].a = 1;
		missile[k].hd = dir;
		missile[k].max_rad = MIS_RADIUS;

		/* XXX Debug code here

		*/
	}
}

void damage(short n, short d, bool physical) {
	short i, k, h, dd;
	double m;

	if ( (n < 0) || (n > num_robots) || (robot[n] -> armor <= 0) )
		return;
	if ( robot[n] -> config.shield < 3 )
		robot[n] -> shields_up = false;

	h = 0;
	if ( (robot[n] -> shields_up) && (!physical) ) {
		dd = d;
		if ( (old_shields) && (robot[n] -> config.shield >= 3) ) {
			d = 0;
			h = 0;
		} else {
			switch (robot[n] -> config.shield) {
			case 3:
				d = (dd * 2 / 3) + 0.5;
				if ( d < 1 )
					d = 1;
				h = (dd * 2 / 3) + 0.5;
				break;
			case 4:
				h = dd/2;
				d = dd - h;
				break;
			case 5:
				d = (dd * 1 / 3) + 0.5;
				if ( d < 1 )
					d = 1;
				h = (dd * 1 / 3) + 0.5;
				if ( h < 1 )
					h = 1;
				break;
			}
		}
	}
	if ( d < 0 )
		d = 0;
	/* XXX Debug code here


	*/
	if ( d > 0 ) {
		d = (d * robot[n] -> damageadj) + 0.5;
		if ( d < 1 )
			d = 1;
	}
	robot[n] -> armor -= d;
	robot[n] -> heat += h;
	robot[n] -> last_damage = 0;

	if ( robot[n] -> armor <= 0 ) {
		robot[n] -> armor = 0;
		update_armor(n);
		robot[n] -> heat = 500;
		update_heat(n);

		robot[n] -> armor = 0;
		kill_count++;
		(robot[n] -> deaths)++;
		update_lives(n);
		if ( graphix && timing )
			time_delay(10);

		draw_robot(n);
		robot[n] ->  heat = 0;
		update_heat(n);
		init_missile(robot[n] -> x, robot[n] -> y, 0, 0, 0, n, BLAST_CIRCLE, false);
		if ( robot[n] -> overburn )
			m = 1.3;
		else
			m = 1;

		for ( i = 0; i <= num_robots; i++ ) {
			if ( (i != n) && (robot[i] -> armor > 0) ) {
				k = (_distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y)) + 0.5;
				if ( k < BLAST_RADIUS )
					damage(i, (abs(BLAST_RADIUS - k) * m) + 0.5, false);
			}
		}
	}
}


short scan(short n) {
	double r, d, acc;
	short dir, range, i, j, k, l, nn, xx, yy, sign;

	nn = -1;
	range = MAXINT;
	if ( (n < 0) || (n > num_robots) )
		return NULL;

	if ( robot[n] -> scanarc < 0 )
		robot[n] -> scanarc = 0;

	robot[n] -> accuracy = 0;
	nn = -1;
	dir = (robot[n] -> shift + robot[n] -> hd) & 255;
	// XXX Debug code here

	for ( i = 0; i <= num_robots; i++ ) {
		if ( (i != n) && (robot[i] -> armor > 0) ) {
			j = find_anglei(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y);
			d = _distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y);
			k = d + 0.5;
			if ( (k < range) && ( k <= robot[n] -> scanrange) && ( (abs(j - dir) <= abs(robot[n] -> scanarc)) || (abs(j - dir) >= (256 - abs(robot[n] -> scanarc))) ) ) {

				dir = (dir + 1024) & 255;
				xx = (sint[dir] * d + robot[n] -> x) + 0.5;
				yy = ((-1 * cost[dir])* d + robot[n] ->  y) + 0.5;
				r = _distance(xx, yy, robot[i] -> x, robot[i] -> y);
				// XXX Debug code here
				if ( (robot[n] -> scanarc > 0) || (r < (HIT_RANGE - 2)) ) {
					range = k;
					robot[n] -> accuracy = 0;
					if ( robot[n] -> scanarc > 0 ) {
						j = (j + 1024) & 255;
						dir = (dir + 1024) & 255;
						if ( j < dir )
							sign = -1;
						if ( j > dir )
							sign = 1;
						if ( (j > 190) && (dir < 66) ) {
							dir = dir + 256;
							sign = -1;
						}
						if ( (dir > 190) && (j < 66) ) {
							j = j + 256;
							sign = 1;
						}
						acc = abs(j - dir) / robot[n] -> scanarc * 2;
						if ( sign < 0 )
							robot[n] -> accuracy = -1 * abs((int)(acc + 0.5));
						else
							robot[n] -> accuracy = abs((int)(acc + 0.5));

						if ( robot[n] -> accuracy > 2 )
							robot[n] -> accuracy = 2;
						if ( robot[n] -> accuracy < -2 )
							robot[n] -> accuracy = -2;
					}
					// XXX Debug code here

				}
			}
		}
		if ( (nn >= 0) && (nn <= num_robots) ) {
			robot[n] -> ram[5] = robot[nn] -> transponder;
			robot[n] -> ram[6] = (robot[nn] -> hd - (robot[n] -> hd + robot[n] -> shift) + 1024) & 255;
			robot[n] -> ram[7] = robot[nn] -> spd;
			robot[n] -> ram[13] = (robot[nn] -> speed * 100) + 0.5;
		}
	}

	return range;
}


void com_transmit(short n, short chan, short data) {
	short i;

	for ( i = 0; i <= num_robots; i++ ) {
		if ( (robot[i] -> armor > 0) && (i != n) && (robot[i] -> channel == chan) ) {
			if ( (robot[i] -> ram[10] < 0) || (robot[i] -> ram[10] > MAX_QUEUE) )
				robot[i] -> ram[10] = 0;
			if ( (robot[i] -> ram[11] < 0) || (robot[i] -> ram[11] > MAX_QUEUE) )
				robot[i] -> ram[11] = 0;
			robot[i] -> ram[robot[i] -> ram[11] + COM_QUEUE] = data;
			(robot[i] -> ram[11])++;
			if ( robot[i] -> ram[11] > MAX_QUEUE )
				robot[i] -> ram[11] = 0;
			if ( robot[i] -> ram[11] == robot[i] -> ram[10] )
				(robot[i] -> ram[10])++;
			if ( robot[i] -> ram[10] > MAX_QUEUE )
				robot[i] -> ram[10] = 0;
		}
	}
}

short com_receive(short i) {
	short k;

	if ( robot[i] -> ram[10] != robot[i] -> ram[11] ) {
		if ( (robot[i] -> ram[10] < 0) || (robot[i] -> ram[10] > MAX_QUEUE) )
			robot[i] -> ram[10] = 0;
		if ( (robot[i] -> ram[11] < 0) || (robot[i] -> ram[11] > MAX_QUEUE) )
			robot[i] -> ram[11] = 0;

		k = robot[i] -> ram[robot[i] -> ram[10] + COM_QUEUE];
		(robot[i] -> ram[10])++;

		if ( robot[i] -> ram[10] > MAX_QUEUE )
			robot[i] -> ram[10] = 0;
	} else
		robot_error(i, 12, "");

	return k;
}

short in_port(short n, short p, short *time_used) {
	short v, i, j, k, l, nn;

	v = 0;

	switch (p) {
		case 1: v = robot[n] -> spd; break;
		case 2: v = robot[n] -> heat; break;
		case 3: v = robot[n] -> hd; break;
		case 4: v = robot[n] -> shift; break;
		case 5: v = (robot[n] -> shift + robot[n] -> hd) & 255; break;
		case 6: v = robot[n] -> armor;
		case 7:
			v = scan(n);
			*time_used += 1;
			if ( show_arcs )
				robot[n] -> arc_count = 2;
			break;
		case 8:
			v = robot[n] -> accuracy;
			*time_used += 1;
			break;
		case 9: // This got mixed with case 16. Check again
			nn = -1;
			*time_used += 3;
			k = MAXINT;
			for ( i = 0; i <= num_robots; i++ ) {
				j = _distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y) + 0.5;
				if ( (n != j) && (j < k) && (j < MAX_SONAR) && (robot[i] -> armor > 0) ) {
					k = j;
					nn = i;
				}
			}

			v = k;
			break;
		case 10:
			v = (rand() % 65536) + (rand() % 3);
			break;
		case 16:
			nn = -1;
			if ( show_arcs )
				robot[n] -> sonar_count = 2;

			*time_used += 40;
			l = -1;
			k = MAXINT;

			for ( i = 0; i <= MAX_ROBOTS; i ++ ) {
				j = _distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y) + 0.5;
				if ( (n != j) && (j < k) && (j < MAX_SONAR) && (robot[i] -> armor > 0) ) {
					k = j;
					l = i;
					nn = i;
				}
			}

			if ( l > 0 ) {
				v = (int)(((find_angle(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y) / PI * 128 + 1024 + (rand() % 66)) - 32) + 0.5) & 255;
			} else
				v = MININT;
			if ( (nn >= 0) && (n <= num_robots) )
				robot[n] -> ram[5] = robot[nn] -> transponder;
			break;
		case 17: v = robot[n] -> scanarc; break;
		case 18:
			if ( robot[n] -> overburn )
				v = 1;
			else
				v = 0;
			break;
		case 19: v = robot[n] -> transponder; break;
		case 20: v = robot[n] -> shutdown; break;
		case 21: v = robot[n] -> channel; break;
		case 22: v = robot[n] -> mines; break;
		case 23:
			if ( robot[n] -> config.mines >= 0 ) {
				k = 0;
				for ( i = 0; i <= MAX_MINES; i++ ) {
					if ( (robot[n] -> mine[i].x >= 0) && (robot[n] -> mine[i].x <= 1000) &&
					    (robot[n] -> mine[i].y >= 0) && (robot[n] -> mine[i].y <= 1000) &&
					    (robot[n] -> mine[i].yield > 0) ) {

						k++;
					}
				}
				v = k;
			} else
				v = 0;
			break;
		case 24:
			if ( robot[n] -> config.shield > 0) {
				if ( robot[n] -> shields_up )
					v = 1;
				else
					v = 0;
			} else {
				v = 0;
				robot[n] -> shields_up = false;
			}
			break;
		default:
			robot_error(n, 11, cstr(p));
	}
	return v;
}


void out_port(short n, short p, short v, short *time_used) {
	short i;

	switch (p) {
		case 11: robot[n] -> tspd = v; break;
		case 12: robot[n]->shift = (robot[n]->shift + v + 1024) & 255; break;
		case 13: robot[n] -> shift = (v + 1024) & 255; break;
		case 14: robot[n] -> thd = (robot[n] -> thd + v + 1024) & 255; break;
		case 15:
			*time_used += 3;
			if ( v > 4 )
				v = 4;
			else if ( v < -4 )
				v = -4;
			init_missile(robot[n] -> x, robot[n] -> y, robot[n] -> xv, robot[n] -> yv, (robot[n] -> hd + robot[n] -> shift + v) & 255, n, 0, robot[n] -> overburn);
			break;
		case 17: robot[n] -> scanarc = v; break;
		case 18:
			if ( v == 0 )
				robot[n] -> overburn = false;
			else
				robot[n] -> overburn = true;
			break;
		case 19: robot[n] -> transponder = v; break;
		case 20: robot[n] -> shutdown = v; break;
		case 21: robot[n] -> channel = v; break;
		case 22:
			if ( robot[n] -> config.mines >= 0 ) {
				if ( robot[n] -> mines > 0 ) {
					init_mine(n, v, MINE_BLAST);
					robot[n] -> mines -= 1;
				} else
					robot_error(n, 14, "");
			} else
				robot_error(n, 13, "");
			break;
		case 23:
			if ( robot[n] -> config.mines >= 0 ) {
				for ( i = 0; i <= MAX_MINES; i++ )
					robot[n] -> mine[i].detonate = true;
			} else
				robot_error(n, 13, "");
			break;
		case 24:
			if ( robot[n] -> config.shield >= 3 ) {
				if ( v == 0 )
					robot[n] -> shields_up = false;
				else
					robot[n] -> shields_up = true;
			} else {
				robot[n] -> shields_up = false;
				robot_error(n, 15, "");
			}
			break;
		default:
			robot_error(n, 11, cstr(p));
	}
	if ( robot[n] -> scanarc > 64 )
		robot[n] -> scanarc = 64;
	else if ( robot[n] -> scanarc < 0 )
		robot[n] -> scanarc = 0;

}


void call_int(short n, short int_num, short * time_used) {
	short i, j, k;

	switch ( int_num ) {
	case 0: damage(n, 1000, true); break;
	case 1:
		reset_software(n);
		*time_used = 10;
		break;
	case 2:
		*time_used = 5;
		robot[n] -> ram[69] = robot[n] -> x + 0.5; // double to signed short may cause problems
		robot[n] -> ram[70] = robot[n] -> y + 0.5;
		break;
	case 3:
		*time_used = 2;
		if ( robot[n] -> ram[65] == 0 )
			robot[n] -> keepshift = false;
		else
			robot[n] -> keepshift = true;
		robot[n] -> ram[70] = robot[n] -> shift & 255;
		break;
	case 4:
		if (robot[n]->ram[65] == 0)
			robot[n]->overburn = false;
		else
			robot[n] -> overburn = true;
		break;
	case 5:
		*time_used = 2;
		robot[n] -> ram[70] = robot[n] -> transponder;
		break;
	case 6:
		*time_used = 2;
		robot[n] -> ram[69] = game_cycle >> 16;
		robot[n] -> ram[70] = game_cycle >> 65535;
		break;
	case 7:
		j = robot[n] -> ram[69];
		k = robot[n] -> ram[70];
		if ( j < 0 )
			j = 0;
		else if ( j > 1000 )
			j = 1000;
		if ( k < 0 )
			k = 0;
		else if ( k > 1000 )
			k = 1000;

		robot[n] -> ram[65] = (short)((find_angle( (short)robot[n] -> x + 0.5, (short)robot[n] -> y + 0.5, j, k) / PI * 128 + 256) + 0.5) & 255;
		*time_used = 32;
		break;
	case 8:
		robot[n] -> ram[70] = robot[n] -> ram[5];
		*time_used = 1;
		break;
	case 9:
		robot[n] -> ram[69] = robot[n] -> ram[6];
		robot[n] ->  ram[70] = robot[n] -> ram[7];
		break;
	case 10:
		k = 0;
		for ( i = 0; i <= num_robots; i++ )
			if ( robot[i] -> armor > 0 )
				k++;

		robot[n] -> ram[68] = k;
		robot[n] -> ram[69] = played;
		robot[n] -> ram[70] = matches;
		*time_used = 4;
		break;
	case 11:
		robot[n] -> ram[68] = (robot[n] -> speed * 100) + 0.5;
		robot[n] -> ram[69] = robot[n] -> last_damage;
		robot[n] -> ram[70] = robot[n] -> last_hit;
		*time_used = 5;
		break;
	case 12:
		robot[n] -> ram[70] = robot[n] -> ram[8];
		*time_used;
		break;
	case 13:
		robot[n] -> ram[8] = 0;
		*time_used = 1;
		break;
	case 14:
		com_transmit(n, robot[n] -> channel, robot[n] -> ram[65]);
		*time_used = 1;
		break;
	case 15:
		if ( robot[n] -> ram[10] != robot[n] -> ram[11] )
			robot[n] -> ram[70] = com_receive(n);
		else
			robot_error(n, 12, "");
		*time_used = 1;
		break;
	case 16:
		if ( robot[n] -> ram[11] >= robot[n] -> ram[10] )
			robot[n] -> ram[70] = robot[n] -> ram[11] - robot[n] -> ram[10];
		else
			robot[n] -> ram[70] = MAX_QUEUE + 1 - robot[n] -> ram[10] + robot[n] -> ram[11];

		*time_used = 1;
		break;
	case 17:
		robot[n] -> ram[10] = 0;
		robot[n] -> ram[11] = 0;
		*time_used = 1;
		break;
	case 18:
		robot[n] -> ram[68] = robot[n] -> kills;
		robot[n] -> ram[69] = robot[n] -> kills - robot[n] -> startkills;
		robot[n] -> ram[70] = robot[n] -> deaths;
		*time_used = 3;
		break;
	case 19:
		robot[n] -> ram[9] = 0;
		robot[n] -> meters = 0;
		break;
	default:
		robot_error(n, 10, cstr(int_num));
	}
}


void jump(short n, short o, bool * inc_ip) {
	short loc;

	loc = find_label(n, get_val(n, robot[n] -> ip, o), robot[n] -> code[robot[n] -> ip].op[MAX_OP] >> (o * 4));
	if ( (loc >= 0) && (loc <= robot[n] -> plen) ) {
		*inc_ip = false;
		robot[n] -> ip = loc;
	} else
		robot_error(n, 2, cstr(loc));
}

// A bunch of debug related functions go here


bool gameover() {
	short n, k;

	if ( (game_cycle >= game_limit) && ( game_limit > 0 ) )
		return true;

	if ( (game_cycle & 31) == 0 ) {
		k = 0;
		for ( n = 0; n <= num_robots; n ++ )
			if ( robot[n] -> armor > 0 )
				k++;

		if ( k <= 1 )
			return true;
		else
			return false;

	} else
		return false;
}


void toggle_graphix() {
	graph_mode(!graphix);
	if ( graphix == false ) {
		textcolor(7);
		std::cout << "Match " << played << "/" << matches << ", Battle in progress&&" << endl << endl;
	} else
		setscreen();
}

bool invalid_microcode(short n, short ip) {
	bool invalid;
	short i, k;

	for ( i = 0; i <= 2; i++ ) {
		k = (robot[n] -> code[ip].op[MAX_OP] >> (i << 2)) & 7;
		if ( (k < 0) || (k > 4) )
			invalid = true;
	}

	return invalid;
}

void process_keypress(char c) {
	// Should probably replace these case statements with SDL's scancodes

	switch (c) {
	case 'C': calibrate_timing(); break;
	case 'T': timing = !timing;; break;
	case 'A': show_arcs = !show_arcs; break;
	case 'S':
	case 'Q':
		if ( sound_on )
		//	chirp();
		sound_on = !sound_on;
		if ( sound_on )
		//	chirp();
		break;
	case '$': debug_info = !debug_info; break;
	case 'W': windoze = !windoze; break;
	case 8: bout_over = true; break;
	case 27:
		quit = true;
		step_loop = false;
		break;
	}
}


std::string victor_string(short k, short n) {
	std::string s = "";

	if ( k == 1 )
		s = "Robot #" + cstr(n + 1) + " (" + robot[n] -> fn + ") wins!";
	if ( k == 0 )
		s = "Simultaneos destruction, match is a tie.";
	if ( k > 1 )
		s = "No clear victor, match is a tie.";

	return s;
}


void show_statistics() {
	short i, j, k, n, sx, sy;

	if ( windoze == false )
		return;

	if ( graphix = true ) { // Display results in window
		sx = 24;
		sy = 93 - num_robots * 3;
		viewport(0, 0, 639, 479);
		box(sx, sy, sx + 591, sy + 102 + num_robots * 12);
		hole(sx + 4, sy + 4, sx + 587, sy + 98 + num_robots * 12);
		setfillstyle(1); // 50/50 checkering

		bar(sx + 5, sy + 5, sx + 586, sy + 97 + num_robots * 12);
		setcolor(WHITE);
		outtextxy(sx + 16, sy + 20, "Robot            Scored   Wins   Matches   Armor   Kills   Deaths    Shots");
		outtextxy(sx + 16, sy + 30, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

		n = -1;
		k = 0;
		for ( i = 0; i <= num_robots; i++ ) {
			if ( (robot[n] -> armor > 0) || (robot[n] -> won) ) {
				k++;
				n = i;
			}
		}

		for ( i = 0; i <= num_robots; i++ ) {
			setcolor(robot_color(i));
			if ( (k == 1) && (n == i) )
				j = 1;
			else
				j = 0;

			outtextxy(sx + 16, sy + 42 + i * 12, addfront(cstr(i + 1), 2) +
				" - " + addrear(robot[n] -> fn, 15) + cstr(j) +
				addfront(cstr(robot[n] -> wins), 8) + addfront(cstr(robot[n] -> trials), 8) +
				addfront(cstr(robot[n] -> armor) + "%", 9) + addfront(cstr(robot[n] -> kills), 7) +
				addfront(cstr(robot[n] -> deaths), 8) + addfront(cstr(robot[n] -> match_shots), 9));
		}

		setcolor(WHITE);
		outtextxy(sx + 16, sy + 64 + num_robots * 12, victor_string(k, n));

		if ( windoze ) {
			outtextxy(sx + 16, sy + 76 +num_robots * 12, "Press any key to continue&&");
			readkey();
		}
	} else { // No graphics; Display results on commandline/terminal
		textcolor(WHITE);
		std::cout << endl << space(79) << endl;
		std::cout << "Match " << played << "/" << matches << " results:" << endl << endl;
		std::cout << "Robot            Scored   Wins   Matches   Armor   Kills   Deaths    Shots" << endl;
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		n = -1;
		k = 0;

		for ( i = 0; i <= num_robots; i++ ) {
			if ( (robot[n] -> armor > 0) || (robot[n] -> won) ) {
				k++;
				n = i;
			}
		}

		for ( i = 0; i <= num_robots; i++ ) {
			textcolor(robot_color(i));
			if ( (k == 1) && (n == i) )
				j = 1;
			else
				j = 0;

			std::cout << addfront(cstr(i + 1), 2) << " - " << addrear(robot[n] -> fn, 15) << cstr(j) <<
				addfront(cstr(robot[n] -> wins), 8) << addfront(cstr(robot[n] -> trials), 8)  <<
				addfront(cstr(robot[n] -> armor) + "%", 9) << addfront(cstr(robot[n] -> kills), 7) <<
				addfront(cstr(robot[n] -> deaths), 8) << addfront(cstr(robot[n] -> match_shots), 9) << endl;

		}
		textcolor(WHITE);
		std::cout << endl << victor_string(k, n) << endl << endl;
	}
}


void score_robots() {
	short i, k, n;
	k = 0;


	for ( i = 0; i <= num_robots; i++ ) {
		robot[i] -> trials += 1;
		if ( robot[i] -> armor > 0 ) {
			k++;
			n = i;
		}
	}
	if ( (k == 1) && (n >= 0) ) {
		robot[n] -> wins += 1;
		robot[n] -> won = true;
	}
}

void init_bout() {
	short i;

	game_cycle = 0;

	for ( i = 0; i <= MAX_MISSILES; i++ ) {
		missile[i].a = 0;
		missile[i].source = -1;
		missile[i].x = 0;
		missile[i].y = 0;
		missile[i].lx = 0;
		missile[i].ly = 0;
		missile[i].mult = 1;
	}
	for ( i = 0; i <= num_robots; i++ ) {
		robot[i] -> mem_watch = 128;
		reset_hardware(i);
		reset_software(i);
	}

	if ( graphix )
		setscreen();
	if ( graphix && (step_mode > 0) )
		//init_debug_window; XXX Debug function here

	if ( graphix == false )
		textcolor(LIGHT_GRAY);
}


// Primary function that runs the games.
void bout() {
	short i, j, k;
	unsigned char c;
	int timer, n;

	if ( quit )
		return;

	played++;
	init_bout();
	bout_over = false;

	if (step_mode == 0)
		step_loop = false;
	else
		step_loop = true;

	step_count = -1; // will be set to 0 upon first iteration of do while loop
	if ( graphix && (step_mode > 0) )
		for ( i = 0; i <= num_robots; i++ )
			draw_robot(i);

	// Begin start of main loop
	do {
		game_cycle++;
		for ( i = 0; i <= num_robots; i++ )
			if ( robot[i] -> armor > 0 )
				do_robot(i);

		for ( i = 0; i <= MAX_MISSILES; i++ )
			if ( missile[i].a > 0 )
				do_missile(i);

		for ( i = 0; i <= num_robots; i++ )
			for ( k = 0; k <= MAX_MINES; k++ )
				if ( robot[i] -> mine[k].yield > 0 )
					do_mine(i, k);


		if ( graphix && timing )
			time_delay(game_delay);

		if ( keypressed() )
			c = toupper(readkey());
		else
			c = 255;

		switch (c) {
		case 'X':
			if ( !robot[0] -> is_locked ) {
				if ( graphix == false )
					toggle_graphix();

				if ( robot[0] -> armor > 0 ) {
					if ( temp_mode > 0 )
						step_mode = temp_mode;
					else
						step_mode = 1;
				}
				step_count = -1;
				// init_debug_window() XXX Debug function
			}
			break;
		case '+':
		case '=':
			if (game_delay < 100) {
				// Triple dot notation only compiles on gcc
				if ((game_delay >= 0) && (game_delay <= 4))
					game_delay = 5;
				else if ((game_delay >= 5) && (game_delay <= 9))
					game_delay = 10;
				else if ((game_delay >= 10) && (game_delay <= 14))
					game_delay = 15;
				else if ((game_delay >= 15) && (game_delay <= 19))
					game_delay = 20;
				else if ((game_delay >= 20) && (game_delay <= 29))
					game_delay = 30;
				else if ((game_delay >= 30) && (game_delay <=39))
					game_delay = 40;
				else if ((game_delay >= 40) && (game_delay <= 49))
					game_delay = 50;
				else if ((game_delay >= 50) && (game_delay <= 59))
					game_delay = 60;
				else if ((game_delay >= 60) && (game_delay <= 74)) 
					game_delay = 75;
				else if ((game_delay >= 75) && (game_delay <= 100))
					game_delay = 100;
			}
			break;
		case '-':
		case '_':
			if ( game_delay > 0 ) {
				if ((game_delay >= 0 && 5))
					game_delay = 0;
				else if ((game_delay >= 6) && (game_delay <= 10))
					game_delay = 5;
				else if ((game_delay >= 11) && (game_delay <= 15))
					game_delay = 10;
				else if ((game_delay >= 16) && (game_delay <= 20))
					game_delay = 15; 
				else if ((game_delay >= 21) && (game_delay <= 30))
					game_delay = 20; 
				else if ((game_delay >= 31) && (game_delay <= 40))
					game_delay = 30;
				else if ((game_delay >= 41) && (game_delay <= 50))
					game_delay = 40; 
				else if ((game_delay >= 51) && (game_delay <= 60))
					game_delay = 50; 
				else if ((game_delay >= 61) && (game_delay <= 75))
					game_delay = 65; 
				else if ((game_delay >= 76) && (game_delay <= 100))
					game_delay = 75; 

			}
			break;
		case 'G': toggle_graphix(); break;
		default:
			process_keypress(c);
		}


		if ( game_delay < 0 )
			game_delay = 0;
		else if ( game_delay > 100 )
			game_delay = 100;

		if ((game_delay >= 0) && (game_delay <= 1))
			k = 100;
		else if ((game_delay >= 2) && (game_delay <= 5))
			k = 50;
		else if ((game_delay >= 6) && (game_delay <= 10))
			k = 25;
		else if ((game_delay >= 11) && (game_delay <= 25))
			k = 20; 
		else if ((game_delay >= 26) && (game_delay <= 40)) 
			k = 10; 
		else if ((game_delay >= 41) && (game_delay <= 70))
			k = 5;
		else if ((game_delay >= 71) && (game_delay <= MAXINT)) 
			k = 1;
		else
			k = 10;


		if ( !graphix )
			k = 100;

		if ( graphix ) {
			if ( ((game_cycle % k) == 0) || (game_cycle == 10) )
				update_cycle_window();
			else {
				if ( update_timer != get_seconds_past_hour() >> 1 )
					update_cycle_window();

				update_timer = get_seconds_past_hour() >> 1;
			}
		}
	} while ( !quit || !gameover || !bout_over );

	update_cycle_window();

	score_robots();
	show_statistics();
}


void write_report() {
	short i;
	fstream f;

	f.open(main_filename + report_ext, fstream::out);

	f << (num_robots + 1) << endl;
	for ( i = 0; i <= num_robots; i++ ) {
		switch (report_type) {
		case 2:
			f << robot[i] -> wins << " " << robot[i] -> trials << " " << robot[i] -> kills << " " << robot[i] -> deaths << " " << robot[i] -> fn << " " << endl;
			break;
		case 3:
			f << robot[i] -> wins << " " << robot[i] -> trials << " " << robot[i] -> kills << " " << robot[i] -> deaths << " " << robot[i] -> armor <<
			" " << robot[i] -> heat << " " << robot[i] -> shots_fired << " " << robot[i] -> fn << " " << endl;
			break;
		case 4:
			f << robot[i] -> wins << " " << robot[i] -> trials << " " << robot[i] -> kills << " " << robot[i] -> deaths << " " << robot[i] -> armor <<
			" " << robot[i] -> heat << " " << robot[i] -> shots_fired << " " << robot[i] -> hits << " " << robot[i] -> damage_total <<
			" " << robot[i] -> cycles_lived << " " << robot[i] -> error_count << " " << robot[i] -> fn << " " << endl;
			break;
		default:
			f << robot[i] -> wins << " " << robot[i] -> trials << " " << robot[i] -> fn << " " << endl;
		}
	}

	f.close();
}


void begin_window() {
	std::string s;

	if ( !graphix || !windoze )
		return;

	setscreen();
	viewport(0, 0, 639, 479);
	box(100, 150, 539, 200);
	hole(105, 155, 534, 195);

	setfillstyle(1); // Checkered
	bar(105, 155, 534, 195);

	setcolor(15);
	s = "Press any key to begin!";
	outtextxy(320 - ((s.length() << 3) >> 1), 172, s);

	readkey();
	setscreen();
}

void true_main() {
	short i, j, k, l, n, w;

	if ( graphix )
		begin_window();

	if ( matches > 0 )
		for ( i = 1; i <= matches; i++ )
			bout();

	if ( graphix == false )
		std::cout << endl;

	if ( quit )
		return;

	// Calculates overall statistics
	if ( matches > 1 ) {
		std::cout << endl << endl;

		graph_mode(false);
		textcolor(WHITE);
		std::cout << "Bout complete! (" << matches << ")" << endl << endl;

		k = 0;
		w = 0;

		for ( i = 0; i <= num_robots; i++ ) {
			if ( robot[i] -> wins == w )
				k++;
			if ( robot[i] -> wins > w ) {
				k = 1;
				n = i;
				w = robot[i] -> wins;
			}
		}

		std::cout << "Robot            Wins   Matches   Kills   Deaths    Shots" << endl;
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

		for ( i = 0; i <= num_robots; i++ ) {
			textcolor(robot_color(i));
			std::cout << addfront(cstr(i + 1), 2) << " - " << addrear(robot[i] -> fn, 8) <<
				addfront(cstr(robot[i] -> wins), 7) << addfront(cstr(robot[i] -> trials), 8)  <<
				addfront(cstr(robot[n] -> kills), 8) << addfront(cstr(robot[n] -> deaths), 8) <<
				addfront(cstr(robot[n] -> shots_fired), 9) << endl;
		}

		textcolor(WHITE);
		std::cout << endl;
		if (k == 1) {
			std::cout << "Robot #" << to_string(n + 1) << " (" << robot[n]->fn << ") wins the bout! (score :" << to_string(w) << "/" << to_string(matches) << endl;
		} else
			std::cout << "There is no clear victor!" << endl;

		std::cout << endl;
	} else if (graphix) {
		graph_mode(false);
		show_statistics();
	}

	if ( report )
		write_report();

}


void execute_instruction(short n) {
	short i, j, k;
	short time_used, loc;
	bool inc_ip;
	char c;

	robot[n]->ram[0] = robot[n]->tspd;
	robot[n]->ram[1] = robot[n]->thd;
	robot[n]->ram[2] = robot[n]->shift;
	robot[n]->ram[3] = robot[n]->accuracy;

	time_used = 1;
	inc_ip = true;
	loc = 0;

	if ((robot[n]->ip > robot[n]->plen) || (robot[n]->ip < 0))
		robot[n]->ip = 0;

	if (invalid_microcode(n, robot[n]->ip))
		robot_error(n, 15, hex(robot[n]->code[robot[n]->ip].op[MAX_OP]));

	if (graphix && (step_mode > 0) && (n == 0)) {

		step_count++;
		update_cycle_window();
		//update_debug_window();
		if (((step_count % step_mode) == 0))
			step_loop = true;
		else
			step_loop = false;

		while (step_loop && (!(quit | gameover() | bout_over))) {
			if (keypressed()) {
				// Loop does nothing until key is pressed
				c = toupper(readkey());
				switch (c) {
				case 'X':
					temp_mode = step_mode;
					step_mode = 0;
					step_loop = false;
					//close_debug_window();
					break;
				case ' ':
					step_count = 0;
					step_loop = false;
					break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					step_mode = value(to_string(c));
					step_count = 0;
					step_loop = false;
					break;
				case '0':
					step_mode = 10;
					step_count = 0;
					step_loop = false;
					break;
				case '-':
				case '_':
					if (robot[n]->mem_watch > 0) {
						setcolor(BLACK);
						for (i = 0; i <= 9; i++)
							outtextxy(35, 212 + (10 * i), decimal(robot[n]->mem_watch + i, 4) + " :");
						robot[n]->mem_watch -= 1;
						// update_debug_memory();
					}
					break;
				case '+':
				case '=':
					if (robot[n]->mem_watch < 1014) {
						setcolor(BLACK);
						for (i = 0; i <= 9; i++)
							outtextxy(35, 212 + (10 * i), decimal(robot[n]->mem_watch + i, 4) + " :");
						robot[n]->mem_watch += 1;
						// update_debug_memory();
					}
					break;
				case '[':
				case '{':
					if (robot[n]->mem_watch > 0) {
						setcolor(BLACK);
						for (i = 0; i <= 9; i++)
							outtextxy(35, 212 + (10 * i), decimal(robot[n]->mem_watch + i, 4) + " :");
						robot[n]->mem_watch -= 10;
						if (robot[n]->mem_watch < 0)
							robot[n]->mem_watch = 0;

						// update_debug_memory();
					}
					break;
				case ']':
				case '}':
					if (robot[n]->mem_watch > 1014) {
						setcolor(BLACK);
						for (i = 0; i <= 9; i++)
							outtextxy(35, 212 + (10 * i), decimal(robot[n]->mem_watch + i, 4) + " :");
						robot[n]->mem_watch += 10;
						if (robot[n]->mem_watch > 1014)
							robot[n]->mem_watch = 1014;
						// update_debug_memory();
					}
					break;
				case 'G':
					toggle_graphix();
					temp_mode = step_mode;
					step_mode = 0;
					step_loop = false;
					break;
				default:
					process_keypress(c);
				}
			}
		}

		if (!(((robot[n]->code[robot[n]->ip].op[MAX_OP] & 7) == 0) || (robot[n]->code[robot[n]->ip].op[MAX_OP] == 1))) {
			time_used = 0;
		} else {
			auto ip = robot[n]->ip;

			switch (get_val(n, robot[n]->ip, 0)) {
			case 0:
				executed++;
				break;
			case 1:
				put_val(n, robot[n]->ip, 1, get_val(n, robot[n]->ip, 1) + get_val(n, robot[n]->ip, 2));
				executed++;
				break;
			case 2:
				put_val(n, ip, 1, get_val(n, ip, 1) + get_val(n, ip, 2));
				executed++;
				break;
			case 3:
				put_val(n, ip, 1, get_val(n, ip, 1) | get_val(n, ip, 2));
				executed++;
				break;
			case 4:
				put_val(n, ip, 1, get_val(n, ip, 1) & get_val(n, ip, 2));
				executed++;
				break;
			case 5:
				put_val(n, ip, 1, get_val(n, ip, 1) ^ get_val(n, ip, 2));
				executed++;
				break;
			case 6:
				put_val(n, ip, 1, !(get_val(n, ip, 1)));
				executed++;
				break;
			case 7:// (*MPY*)
				put_val(n, ip, 1, get_val(n, ip, 1)*get_val(n, ip, 2));
				time_used = 10;
				executed++;
				break;
			case 8:// (*DIV*)
				j = get_val(n, ip, 2);
				if (j != 0)
					put_val(n, ip, 1, get_val(n, ip, 1) / j);
				else
					robot_error(n, 8, "");
				time_used = 10;
				executed++;
				break;
			case 9:// (*MOD*)
				j = get_val(n, ip, 2);
				if (j != 0)
					put_val(n, ip, 1, get_val(n, ip, 1) % j);
				else robot_error(n, 8, "");
				time_used = 10;
				executed++;
				break;

			case 10:// (*RET*)
				ip = pop(n);
				if ((ip < 0) || (ip > robot[n]->plen))
					robot_error(n, 7, cstr(ip));
				executed++;
				break;

			case 11:// (*GSB*)
				loc = find_label(n, get_val(n, ip, 1), robot[n]->code[ip].op[MAX_OP] >> (1 * 4));
				if (loc >= 0) {
					push(n, ip);
					inc_ip = false;
					ip = loc;
				} else
					robot_error(n, 2, cstr(get_val(n, ip, 1)));
				executed++;
				break;

			case 12:// (*JMP*)
				jump(n, 1, &inc_ip);
				executed++;
				break;
			case 13:// (*JLS, JB*)
				if ((robot[n]->ram[64] & 2) > 0)
					jump(n, 1, &inc_ip);
				time_used = 0;
				executed++;
				break;

			case 14:// (*JGR, JA*)
				if ((robot[n]->ram[64] & 4) > 0)
					jump(n, 1, &inc_ip);
				time_used = 0;
				executed++;
				break;

			case 15:// (*JNE*)
				if ((robot[n]->ram[64] & 1) == 0)
					jump(n, 1, &inc_ip);
				time_used = 0;
				executed++;
				break;

			case 16:// (*JEQ, JE*)
				if ((robot[n]->ram[64] & 1) > 0)
					jump(n, 1, &inc_ip);
				time_used = 0;
				executed++;
				break;

			case 17:// (*SWAP, XCHG*)
				robot[n]->ram[4] = get_val(n, ip, 1);
				put_val(n, ip, 1, get_val(n, ip, 2));
				put_val(n, ip, 2, robot[n]->ram[4]);
				time_used = 3;
				executed++;
				break;

			case 18:// (*DO*)
				robot[n]->ram[67] = get_val(n, ip, 1);
				executed++;
				break;

			case 19:// (*LOOP*)
				robot[n]->ram[67]--;
				if (robot[n]->ram[67] > 0)
					jump(n, 1, &inc_ip);
				executed++;
				break;

			case 20:// (*CMP*)
				k = get_val(n, ip, 1) - get_val(n, ip, 2);
				robot[n]->ram[64] = robot[n]->ram[64] & 0xFFF0;
				if (k == 0)
					robot[n]->ram[64] = robot[n]->ram[64] | 1;
				if (k < 0)
					robot[n]->ram[64] = robot[n]->ram[64] | 2;
				if (k > 0)
					robot[n]->ram[64] = robot[n]->ram[64] | 4;
				if ((get_val(n, ip, 2) == 0) && (k = 0))
					robot[n]->ram[64] = robot[n]->ram[64] | 8;
				executed++;
				break;

			case 21:// (*TEST*)
				k = get_val(n, ip, 1) & get_val(n, ip, 2);
				robot[n]->ram[64] = robot[n]->ram[64] & 0xFFF0;
				if (k == get_val(n, ip, 2))
					robot[n]->ram[64] = robot[n]->ram[64] | 1;
				if (k == 0)
					robot[n]->ram[64] = robot[n]->ram[64] | 8;
				executed++;
				break;

			case 22:// (*MOV, SET*)
				put_val(n, ip, 1, get_val(n, ip, 2));
				executed++;
				break;

			case 23:// (*LOC*)
				put_val(n, ip, 1, robot[n]->code[ip].op[2]);
				time_used = 2;
				executed++;
				break;

			case 24:// (*GET*)
				k = get_val(n, ip, 2);
				if ((k >= 0) & (k <= MAX_RAM))
					put_val(n, ip, 1, robot[n]->ram[k]);
				else if ((k > MAX_RAM) & (k <= (MAX_RAM + 1) + (((MAX_CODE + 1) << 3) - 1))) {
					j = k - MAX_RAM - 1;
					put_val(n, ip, 1, robot[n]->code[j >> 2].op[j & 3]);
				} else robot_error(n, 4, cstr(k));
				time_used = 2;
				executed++;
				break;

			case 25:// (*PUT*)
				k = get_val(n, ip, 2);
				if ((k >= 0) & (k <= MAX_RAM))
					robot[n]->ram[k] = get_val(n, ip, 1);
				else
					robot_error(n, 4, cstr(k));
				time_used = 2;
				executed++;
				break;

			case 26:// (*INT*)
				call_int(n, get_val(n, ip, 1), &time_used);
				executed++;
				break;

			case 27:// (*IPO, IN*)
				time_used = 4;
				put_val(n, ip, 2, in_port(n, get_val(n, ip, 1), &time_used));
				executed++;
				break;

			case 28:// (*OPO, OUT*)
				time_used = 4;
				out_port(n, get_val(n, ip, 1), get_val(n, ip, 2), &time_used);
				executed++;
				break;

			case 29:// (*DEL, DELAY*)
				time_used = get_val(n, ip, 1);
				executed++;
				break;

			case 30:// (*PUSH*)
				push(n, get_val(n, ip, 1));
				executed++;
				break;

			case 31:// (*POP*)
				put_val(n, ip, 1, pop(n));
				executed++;
				break;

			case 32:// (*ERR*)
				robot_error(n, get_val(n, ip, 1), "");
				time_used = 0;
				executed++;
				break;

			case 33:// (*INC*)
				put_val(n, ip, 1, get_val(n, ip, 1) + 1);
				executed++;
				break;

			case 34:// (*DEC*)
				put_val(n, ip, 1, get_val(n, ip, 1) - 1);
				executed++;
				break;

			case 35:// (*SHL*)
				put_val(n, ip, 1, get_val(n, ip, 1) << get_val(n, ip, 2));
				executed++;
				break;

			case 36:// (*SHR*)
				put_val(n, ip, 1, get_val(n, ip, 1) >> get_val(n, ip, 2));
				executed++;
				break;

			case 37:// (*ROL*)
				put_val(n, ip, 1, rol(get_val(n, ip, 1), get_val(n, ip, 2)));
				executed++;
				break;

			case 38:// (*ROR*)
				put_val(n, ip, 1, ror(get_val(n, ip, 1), get_val(n, ip, 2)));
				executed++;
				break;

			case 39:// (*JZ*)
				time_used = 0;
				if ((robot[n]->ram[64] & 8) > 0)
					jump(n, 1, &inc_ip);
				executed++;
				break;

			case 40:// (*JNZ*)
				time_used = 0;
				if ((robot[n]->ram[64] & 8) == 0)
					jump(n, 1, &inc_ip);
				executed++;
				break;

			case 41:// (*JAE, JGE*)
				if (((robot[n]->ram[64] & 1) > 0) || ((robot[n]->ram[64] & 4) > 0))
					jump(n, 1, &inc_ip);
				time_used = 0;
				executed++;
				break;

			case 42:// (*JBE, JLE*)
				if (((robot[n]->ram[64] & 1) > 0) || ((robot[n]->ram[64] & 2) > 0))
					jump(n, 1, &inc_ip);
				time_used = 0;
				executed++;
				break;

			case 43:// (*SAL*)
				put_val(n, ip, 1, sal(get_val(n, ip, 1), get_val(n, ip, 2)));
				executed++;
				break;

			case 44:// (*SAR*)
				put_val(n, ip, 1, sar(get_val(n, ip, 1), get_val(n, ip, 2)));
				executed++;
				break;

			case 45:// (*NEG*)
				put_val(n, ip, 1, 0 - get_val(n, ip, 1));
				executed++;
				break;

			case 46:// (*JTL*)
				loc = get_val(n, ip, 1);
				if ((loc >= 0) & (loc <= robot[n]->plen)) {
					inc_ip = false;
					ip = loc;
				} else
					robot_error(n, 2, cstr(loc));
				break;
			default:
				robot_error(n, 6, "");
			}

			robot[n]->delay_left += time_used;
			if (inc_ip)
				robot[n]->ip += 1;

			//if ( graphix && (n == 0) && (step_mode >0))
				//update_debug_window();
		}
	}
}

void do_robot(short n) {
	short i, j, k, l, tthd, heat_mult, ttx, tty;
	if (n < 0 || n > num_robots)
		return;
	if (robot[n]->armor <= 0)
		return;
	robot[n]->time_left = time_slice;
		if (robot[n]->time_left > robot[n]->robot_time_limit && robot[n]->robot_time_limit > 0)
			robot[n]->time_left = robot[n]->robot_time_limit;
		if (robot[n]->time_left>robot[n]->max_time && robot[n]->max_time > 0)
			robot[n]->time_left = robot[n]->max_time;
		executed = 0;
		//execute timeslice
		while (robot[n]->time_left > 0 && !robot[n]->cooling && executed < 20 + time_slice && robot[n]->armor > 0) {
			if (robot[n]->delay_left < 0)
				robot[n]->delay_left = 0;
			if (robot[n]->delay_left > 0) {
				robot[n]->delay_left--;
				robot[n]->time_left--;
			}
			if (robot[n]->time_left >= 0 && robot[n]->delay_left == 0)
				execute_instruction(n);
			if (robot[n]->heat >= robot[n]->shutdown) {
				robot[n]->cooling = true;
				robot[n]->shields_up = false;
			}
			if (robot[n]->heat >= 500) {
				damage(n, 1000, true);
			}
		}

		robot[n]->thd = (robot[n]->thd + 1024) & 255;
		robot[n]->hd = (robot[n]->hd + 1024) & 255;
		robot[n]->shift = (robot[n]->shift + 1024) & 255;
		if (robot[n]->tspd < -75)
			robot[n]->tspd = -75;
		
		if (robot[n]->tspd > 100)
			robot[n]->tspd = 100;

		if (robot[n]->spd < -75)
			robot[n]->spd = -75;

		if (robot[n]->spd > 100)
			robot[n]->spd = 100;

		if (robot[n]->heat < 0)
			robot[n]->heat = 0;

		if (robot[n]->last_damage < MAXINT)
			robot[n]->last_damage++;

		if (robot[n]->last_hit < MAXINT)
			robot[n]->last_hit++;


		//update heat
		if (robot[n]->shields_up && (game_cycle & 3 == 0))
			robot[n]->heat++;

		if (!robot[n]->shields_up) {
			if (robot[n]->heat > 0)
				switch (robot[n]->config.heatsinks) {
				case 5:
					if (game_cycle & 1 == 0)
						robot[n]->heat--;
					break;
					
				case 4:
					if (game_cycle % 3 == 0)
						robot[n]->heat--;
					break;
					
				case 3:
					if (game_cycle & 3 == 0)
						robot[n]->heat--;
					break;
					
				case 2:
					if (game_cycle & 7 == 0)
						robot[n]->heat--;
					break;
					
				case 1:
					break;
				
				default:				
					if (game_cycle & 3 == 0)
					robot[n]->heat++;					
				}
				
			if (robot[n]->overburn && (game_cycle % 3 == 0))		
				robot[n]->heat++;				
			if (robot[n]->heat > 0)		
				robot[n]->heat--;		
			if (robot[n]->heat > 0 && (game_cycle & 7 == 0) && (abs(robot[n]->tspd) <= 25))		
				robot[n]->heat--;			
			if (robot[n]->heat <= robot[n]->shutdown - 50 || (robot[n]->heat <= 0))		
				robot[n]->cooling = false;			
		}
		
		if (robot[n]->cooling)
			robot[n]->tspd = 0;
			
		heat_mult = 1;		
		if ((robot[n]->heat >= 80) && (robot[n]->heat <= 99))
			heat_mult = 0.98;
		else if ((robot[n] -> heat >= 100) && (robot[n] -> heat <= 149))
			heat_mult = 0.95;
		else if ((robot[n] -> heat >= 150)  && (robot[n] -> heat <= 199))
			heat_mult = 0.85;
		else if ((robot[n] -> heat >= 200) && (robot[n] -> heat <= 249))
			heat_mult = 0.75;
		else if ((robot[n] -> heat >= 250) && (robot[n] -> heat <= MAXINT))
			heat_mult = 0.50;

		if (robot[n]->overburn)
			heat_mult = heat_mult * 1.3;
		if (robot[n]->heat >= 475 && (game_cycle & 3 == 0))
			damage(n, 1, true);
		if (robot[n]->heat >= 450 && (game_cycle & 7 == 0))
			damage(n, 1, true);
		if (robot[n]->heat >= 400 && (game_cycle & 15 == 0))
			damage(n, 1, true);
		if (robot[n]->heat >= 350 && (game_cycle & 31 == 0))
			damage(n, 1, true);
		if (robot[n]->heat >= 300 && (game_cycle & 63 == 0))
			damage(n, 1, true);

		//update robot in physical world
		//acceleration
		if (abs(robot[n]->spd - robot[n]->tspd) <= ACCELERATION)
			robot[n]->spd = robot[n]->tspd;
		else {
			if (robot[n]->tspd > robot[n]->spd) {
				robot[n]->spd += ACCELERATION;
			} else {
				robot[n]->spd -= ACCELERATION;
			}
		}

		tthd = robot[n]->hd + robot[n]->shift;
		if (abs(robot[n]->hd - robot[n]->thd) <= TURN_RATE || (abs(robot[n]->hd - robot[n]->thd) >= 256 - TURN_RATE))
			robot[n]->hd = robot[n]->thd;
		else if (robot[n]->hd != robot[n]->thd) {
			k = 0;
			if ((robot[n]->thd > robot[n]->hd) && (abs(robot[n]->hd - robot[n]->thd) <= 128) ||
				(robot[n]->thd < robot[n]->hd) && (abs(robot[n]->hd - robot[n]->thd) >= 128))
				k = 1;
			if (k = 1)
				robot[n]->hd = (robot[n]->hd + TURN_RATE) & 255;
			else
				robot[n]->hd = (robot[n]->hd + 256 - TURN_RATE) & 255;
		}

		robot[n]->hd = robot[n]->hd & 255;
		if (robot[n]->keepshift)
			robot[n]->shift = (tthd - robot[n]->hd + 1024) & 255;

		robot[n]->speed = robot[n]->spd / 100 * (MAX_VEL * heat_mult * robot[n]->speedadj);
		robot[n]->xv = sint[robot[n]->hd] * robot[n]->speed;
		robot[n]->yv = -cost[robot[n]->hd] * robot[n]->speed;
		if (robot[n]->hd == 0 || robot[n]->hd == 128)
			robot[n]->xv = 0;
		if (robot[n]->hd == 64 || robot[n]->hd == 192)
			robot[n]->yv = 0;
		if (robot[n]->xv != 0)
			ttx = robot[n]->x + robot[n]->xv;
		else
			ttx = robot[n]->x;
		if (robot[n]->yv != 0)
			tty = robot[n]->y + robot[n]->yv;
		else
			tty = robot[n] -> y;
		if (ttx < 0 || (tty < 0) || (ttx > 1000) || (tty > 1000)) {
			robot[n]->ram[8]++;
			robot[n]->tspd = 0;
			if (abs(robot[n]->speed) >= MAX_VEL / 2)
				damage(n, 1, true);
			robot[n]->spd = 0;
		}
		for (i = 0; i <= num_robots; i++)
			if (i != n && robot[i]->armor > 0 && _distance(ttx, tty, robot[i]->x, robot[i]->y) < CRASH_RANGE) {
				robot[n]->tspd = 0;
				robot[n]->spd = 0;
				ttx = robot[n] -> x;
				tty = robot[n] -> y;
				robot[i]->tspd = 0;
				robot[i]->spd = 0;
				robot[n]->ram[8]++;
				robot[i]->ram[8];
				if (abs(robot[n]->speed) >= MAX_VEL / 2) {
					damage(n, 1, true);
					damage(i, 1, true);
				}
			}
		if (ttx < 0)
			ttx = 0;
		if (tty < 0)
			tty = 0;
		if (ttx > 1000)
			ttx = 1000;
		if (tty > 1000)
			tty = 1000;
		robot[n]->meters = robot[n]->meters + _distance(robot[n]->x, robot[n]->y, ttx, tty);
		if (robot[n]->meters > MAXINT)
			robot[n]->meters = robot[n]->meters - MAXINT;
		robot[n]->ram[9] = trunc(robot[n]->meters);
		robot[n]->x = ttx;
		robot[n]->y = tty;

		//draw robot
		if (robot[n]->armor < 0)
			robot[n]->armor = 0;
		if (robot[n]->heat < 0)
			robot[n]->heat = 0;
		if (graphix) {
			if (robot[n]->armor != robot[n]->larmor)
				update_armor(n);
			if (robot[n]->heat / 5 != robot[n]->lheat / 5)
				update_heat(n);
			draw_robot(n);
		}

		robot[n]->lheat = robot[n]->heat;
		robot[n]->larmor = robot[n]->armor;

		robot[n]->cycles_lived++;
	}

void do_mine(short n, short m) {
	short i, j, k, l;
	double d, r;
	bool source_alive;

	// I am not in the mood to constantly retype that;
	auto _ = robot[n]->mine[m];
	
	if ((_.x >= 0) && (_.x <= 100) && (_.y >= 0) && (_.y <= 1000) && (_.yield > 0)) {
		for (i = 0; i <= num_robots; i++) {
			if ((robot[n]->armor > 0) && (i != n)) {
				d = _distance(_.x, _.y, robot[i]->x, robot[i]->y);
				if (d <= _.detect)
					_.detonate = true;
			}
		}
		if (_.detonate) { // Blow up the mine
			init_missile(_.x, _.y, 0, 0, 0, n, MINE_CIRCLE, false);
			kill_count = 0;

			if (robot[n]->armor > 0)
				source_alive = true;
			else
				source_alive = false;

			for (i = 0; i <= num_robots; i++) {
				if (robot[i]->armor > 0) {
					k = _distance(_.x, _.y, robot[i]->x, robot[i]->y) + 0.5;
					if (k < _.yield) {
						damage(i, short(abs(_.yield -k) + 0.5), false);
						if ((n >= 0) && (n <= num_robots) && (i != n))
							robot[n]->damage_total += abs(_.yield -k) + 0.5;
					}
				}
			}
			if ((kill_count > 0) && (source_alive) && (robot[n]->armor <= 0))
				kill_count--;
			if (kill_count > 0) {
				robot[n]->kills += kill_count;
				update_lives(n);
			}

			// Blot over where the mine used to be
			if (graphix) {
				putpixel((_.x * SCREEN_SCALE + 0.5) + SCREEN_X, (_.y * SCREEN_SCALE + 0.5) + SCREEN_Y, BLACK);
				_.yield = 0;
				_.x = -1;
				_.y = -1;
			}
		} else { // Begin drawing mine
			if (graphix && ((game_cycle & 1) == 0) ) {
				main_viewport();
				setcolor(robot_color(n));
				line((_.x*SCREEN_SCALE + 0.5) + SCREEN_X, (_.y*SCREEN_SCALE + 0.5) + SCREEN_Y - 1, (_.x*SCREEN_SCALE + 0.5) + SCREEN_X, (_.y*SCREEN_SCALE + 0.5) + SCREEN_Y + 1);
				line((_.x*SCREEN_SCALE + 0.5) + SCREEN_X + 1, (_.y*SCREEN_SCALE + 0.5) + SCREEN_Y, (_.x*SCREEN_SCALE + 0.5) + SCREEN_X - 1, (_.y*SCREEN_SCALE + 0.5) + SCREEN_Y);
			}
		}
	}
}

void do_missile(short n) {
	double llx, lly, r, d, dir, xv, yv;
	short i, j, k, l, xx, yy, tx, ty, dd, dam;

	if (missile[n].a == 0)
		return;

	
}

void log_error(short i, short n, std::string ov) {
    std::string s;

    if (!logging_errors) {
	return;
    }

    switch (i) {
        case 1:
            s =  "Stack full - Too many CALLs?" ;
            break;

        case 2:
            s =  "Label not found. Hmmm." ;
            break;

        case 3:
            s =  "Can't assign value - Tisk tisk." ;
            break;

        case 4:
            s =  "Illegal memory reference" ;
            break;

        case 5:
            s =  "Stack empty - Too many RETs?" ;
            break;

        case 6:
            s =  "Illegal instruction. How bizarre." ;
            break;

        case 7:
            s =  "Return out of range - Woops!" ;
            break;

        case 8:
            s =  "Divide by zero" ;
            break;

        case 9:
            s =  "Unresolved !label. WTF?" ;
            break;

        case 10:
            s =  "Invalid Interrupt Call" ;
            break;

        case 11:
            s =  "Invalid Port Access" ;
            break;

        case 12:
            s =  "Com Queue empty" ;
            break;

        case 13:
            s =  "No mine-layer, silly." ;
            break;

        case 14:
            s =  "No mines left" ;
            break;

        case 15:
            s =  "No shield installed - Arm the photon torpedoes instead. :/" ;
            break;

        case 16:
            s =  "Invalid Microcode in instruction." ;
            break;

        default:
            s =  "Unknown error." ;
    }
    robot[n] -> errorlog
            << " <" << i << "> "<< s << " (Line #" << robot[n]->ip << ") [Cycle: " << to_string(game_cycle) << ", Match: " << to_string(played) << "/" << to_string(matches) <<
            "]" << endl;
    robot[n] -> errorlog << " " << mnemonic(robot[n]->code[robot[n]->ip].op[0], robot[n]->code[robot[n]->ip].op[3] & 15) << "  " <<
            operand(robot[n]->code[robot[n]->ip].op[1], (robot[n]->code[robot[n]->ip].op[3] >> 4) & 15) << ", " <<
            operand(robot[n]->code[robot[n]->ip].op[2], (robot[n]->code[robot[n]->ip].op[3] >> 8) & 15) << endl;
    if ( ov.compare("") == 0)
        robot[n] -> errorlog << "    (Values: "<< ov << ")" << endl;
    else
        robot[n] -> errorlog << " ";

    robot[n] -> errorlog << " AX="<< addrear(cstr(robot[n]->ram[65]) + ",", 7) << endl;
    robot[n] -> errorlog << " BX="<< addrear(cstr(robot[n]->ram[66]) + ",", 7) << endl;
    robot[n] -> errorlog << " CX="<< addrear(cstr(robot[n]->ram[67]) + ",", 7) << endl;
    robot[n] -> errorlog << " DX="<< addrear(cstr(robot[n]->ram[68]) + ",", 7) << endl;
    robot[n] -> errorlog << " EX="<< addrear(cstr(robot[n]->ram[69]) + ",", 7) << endl;
    robot[n] -> errorlog << " FX="<< addrear(cstr(robot[n]->ram[70]) + ",", 7) << endl;
    robot[n] -> errorlog << " Flags = " << robot[n]->ram[64] << endl;
    robot[n] -> errorlog << " AX="<< addrear(hex(robot[n]->ram[65]) + ",", 7) << endl;
    robot[n] -> errorlog << " BX="<< addrear(hex(robot[n]->ram[66]) + ",", 7) << endl;
    robot[n] -> errorlog << " CX="<< addrear(hex(robot[n]->ram[67]) + ",", 7) << endl;
    robot[n] -> errorlog << " DX="<< addrear(hex(robot[n]->ram[68]) + ",", 7) << endl;
    robot[n] -> errorlog << " EX="<< addrear(hex(robot[n]->ram[69]) + ",", 7) << endl;
    robot[n] -> errorlog << " FX="<< addrear(hex(robot[n]->ram[70]) + ",", 7) << endl;
    robot[n] -> errorlog << " Flags = "<< hex(robot[n]->ram[64]) << endl;

    return;
}

void prog_error(short n, std::string ss) {
    std::string s;
    graph_mode(false);

    textcolor(WHITE);

    std::cout << "ERROR #" << to_string(n) << ": ";

    switch (n) {
        case 0:
            s = ss;
            break;
        case 1:
            s = "Invalid :label - " + ss + ", silly mortal.";
            break;
        case 2:
            s = "Undefined identifier - " + ss + ". A typo perhaps?";
            break;
        case 3:
            s = "Memory access out of range - " + ss + "";
            break;
        case 4:
            s = "Not enough robots for combat. Maybe we should just drive in circles.";
            break;

        case 5:
            s = "Robot names & settings must be specified. An empty arena is no fun.";
            break;
        case 6:
            s = "Config file not found - " + ss + "";
            break;
        case 7:
            s = "Cannot access a config file from a config file - " + ss + "";
            break;
        case 8:
            s = "Robot not found " + ss + ". Perhaps you mistyped it?";
            break;
        case 9:
            s = "Insufficient RAM to load robot: " + ss + "&& This is not good.";
            break;
        case 10:
            s = "Too many robots! We can only handle " + cstr(MAX_ROBOTS + 1) + "! Blah.. limits are limits.";
            break;
        case 11:
            s = "You already have a perfectly good #def for " + ss + ", silly.";
            break;
        case 12:
            s = "Variable name too long! (Max:" + cstr(MAX_VAR_LEN) + ") " + ss + "";
            break;
        case 13:
            s = "!Label already defined " + ss + ", silly.";
            break;
        case 14:
            s = "Too many variables! (Var Limit: " + cstr(MAX_VARS) + ")";
            break;
        case 15:
            s = "Too many !labels! (!Label Limit: " + cstr(MAX_LABELS) + ")";
            break;
        case 16:
            s = "Robot program too long! Boldly we simplify, simplify along&&" + ss;
            break;
        case 17:
            s = "!Label missing error. !Label #" + ss + ".";
            break;
        case 18:
            s = "!Label out of range: " + ss + "";
            break;
        case 19:
            s = "!Label not found. " + ss + "";
            break;
        case 20:
            s = "Invalid config option: " + ss + ". Inventing a new device?";
            break;
        case 21:
            s = "Robot is attempting to cheat; Too many config points (" + ss + ")";
            break;
        case 22:
            s = "Insufficient data in data statement: " + ss + "";
            break;
        case 23:
            s = "Too many asterisks: " + ss + "";
            break;
        case 24:
            s = "Invalid step count: " + ss + ". 1-9 are valid conditions.";
            break;
        case 25:
            s = "'" + ss + "'";
            break;
        default:
            s = ss;
    }
    std::cout << s << endl << endl;
    exit(EXIT_FAILURE);

}


void parse1(int n, int p, string * s) {
    int i, j, k, opcode, microcode;
    bool found, indirect;
    std::string ss;

    for (i = 0; i <= MAX_OP - 1; i++) {
        k = 0;
        found = false;
        opcode = 0;
        microcode = 0;
        s[i] = btrim(ucase(s[i]));
        indirect = false;
        /*
        Microcode:
           0 = instruction, number, constant
           1 = variable, memory access
           2 = :label
           3 = !label (unresolved)
           4 = !label (resolved)
          8h mask = inderect addressing (enclosed in [])
        */

        if (s[i].compare("") == 0) {
            opcode = 0;
            microcode = 0;
            found = true;
        }
        if ( !(lstr(s[i], 1).compare("[")) && !(rstr(s[i], 1).compare("]")) ) {
            s[i] = copy(s[i], 2, s[i].length() - 2);
            indirect = true;
        }

        //Labels
        if (!found && (s[i][1] == '!')) {
            ss = s[i];
            ss = btrim(rstr(ss, ss.length() - 1));
            if (numlabels > 0) {
                for (j = 0; j <= numlabels; j++) {
                    if (ss == labelname[j]) {
                        found = true;
                        if (labelnum[j] >= 0) {
                            opcode = labelnum[j]; //resolved label
                            microcode = 4;
                        } else {
                            opcode = j; //unresolved label
                            microcode = 3;
                        }
                    }
                    if (!found) {
                        numlabels++;
                        if (numlabels > MAX_LABELS) {
                            prog_error(15, "");
                        } else {
                            labelname[numlabels] = ss;
                            labelnum[numlabels] = -1;
                            opcode = numlabels;
                            microcode = 3; //unresolved label
                            found = true;
                        }
                    }
                }
            }

            if ( (numvars > 0) && !found) {
                for (j = 1; j <= numvars; j++) {
                    if (!strcmp(s[i].c_str(), varname[j].c_str())) {
                        opcode = varloc[j];
                        microcode = 1;
                        found = true;
                    }
                    if (!strcmp(s[i].c_str(), "NOP")) {
                        opcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "ADD")) {
                        opcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "SUB")) {
                        opcode = 2;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "OR")) {
                        opcode = 3;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "AND")) {
                        opcode = 4;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "XOR")) {
                        opcode = 5;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "NOT")) {
                        opcode = 6;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "MPY")) {
                        opcode = 7;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "DIV")) {
                        opcode = 8;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "MOD")) {
                        opcode = 9;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "RET")) {
                        opcode = 10;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "RETURN")) {
                        opcode = 10;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "GSB")) {
                        opcode = 11;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "GOSUB")) {
                        opcode = 11;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "CALL")) {
                        opcode = 11;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JMP")) {
                        opcode = 12;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JUMP")) {
                        opcode = 12;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "GOTO")) {
                        opcode = 12;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JLS")) {
                        opcode = 13;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JB")) {
                        opcode = 13;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JGR")) {
                        opcode = 14;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JA")) {
                        opcode = 14;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JNE")) {
                        opcode = 15;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JEQ")) {
                        opcode = 16;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JE")) {
                        opcode = 16;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "XCHG")) {
                        opcode = 17;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "SWAP")) {
                        opcode = 17;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "DO")) {
                        opcode = 18;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "LOOP")) {
                        opcode = 19;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "CMP")) {
                        opcode = 20;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "TEST")) {
                        opcode = 21;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "SET")) {
                        opcode = 22;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "MOV")) {
                        opcode = 22;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "LOC")) {
                        opcode = 23;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "ADDR")) {
                        opcode = 23;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "GET")) {
                        opcode = 24;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "PUT")) {
                        opcode = 25;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "INT")) {
                        opcode = 26;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "IPO")) {
                        opcode = 27;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "IN")) {
                        opcode = 27;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "OPO")) {
                        opcode = 28;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "OUT")) {
                        opcode = 28;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "DEL")) {
                        opcode = 29;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "DELAY")) {
                        opcode = 29;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "PUSH")) {
                        opcode = 30;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "POP")) {
                        opcode = 31;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "ERR")) {
                        opcode = 32;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "ERROR")) {
                        opcode = 32;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "INC")) {
                        opcode = 33;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "DEC")) {
                        opcode = 34;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "SHL")) {
                        opcode = 35;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "SHR")) {
                        opcode = 36;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "ROL")) {
                        opcode = 37;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "ROR")) {
                        opcode = 38;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JZ")) {
                        opcode = 39;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JNZ")) {
                        opcode = 40;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JAE")) {
                        opcode = 41;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JGE")) {
                        opcode = 41;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JLE")) {
                        opcode = 42;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JBE")) {
                        opcode = 42;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "SAL")) {
                        opcode = 43;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "SAR")) {
                        opcode = 44;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "NEG")) {
                        opcode = 45;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "JTL")) {
                        opcode = 46;
                        found = true;
                    }

                        //Registers
                    else if (!strcmp(s[i].c_str(), "COLCNT")) {
                        opcode = 8;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "METERS")) {
                        opcode = 9;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "COMBASE")) {
                        opcode = 10;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "COMEND")) {
                        opcode = 11;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "FLAGS")) {
                        opcode = 64;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "AX")) {
                        opcode = 65;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "BX")) {
                        opcode = 66;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "CX")) {
                        opcode = 67;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "DX")) {
                        opcode = 68;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "EX")) {
                        opcode = 69;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "FX")) {
                        opcode = 70;
                        microcode = 1;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "SP")) {
                        opcode = 71;
                        microcode = 1;
                        found = true;
                    }

                        //Constants
                    else if (!strcmp(s[i].c_str(), "MAXINT")) {
                        opcode = 32767;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "MININT")) {
                        opcode = -32768;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_SPEDOMETER")) {
                        opcode = 1;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_HEAT")) {
                        opcode = 2;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_COMPASS")) {
                        opcode = 3;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_TANGLE")) {
                        opcode = 4;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_TURRET_OFS")) {
                        opcode = 4;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_THEADING")) {
                        opcode = 5;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_TURRET_ABS")) {
                        opcode = 5;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_ARMOR")) {
                        opcode = 6;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_DAMAGE")) {
                        opcode = 6;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_SCAN")) {
                        opcode = 7;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_ACCURACY")) {
                        opcode = 8;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_RADAR")) {
                        opcode = 9;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_RANDOM")) {
                        opcode = 10;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_RAND")) {
                        opcode = 10;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_THROTTLE")) {
                        opcode = 11;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_TROTATE")) {
                        opcode = 12;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_OFS_TURRET")) {
                        opcode = 12;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_TAIM")) {
                        opcode = 13;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_ABS_TURRET")) {
                        opcode = 13;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_STEERING")) {
                        opcode = 14;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_WEAP")) {
                        opcode = 15;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_WEAPON")) {
                        opcode = 15;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_FIRE")) {
                        opcode = 15;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_SONAR")) {
                        opcode = 16;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_ARC")) {
                        opcode = 17;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_SCANARC")) {
                        opcode = 17;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_OVERBURN")) {
                        opcode = 18;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_TRANSPONDER")) {
                        opcode = 19;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_SHUTDOWN")) {
                        opcode = 20;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_CHANNEL")) {
                        opcode = 21;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_MINELAYER")) {
                        opcode = 22;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_MINETRIGGER")) {
                        opcode = 23;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_SHIELD")) {
                        opcode = 24;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "P_SHIELDS")) {
                        opcode = 24;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_DESTRUCT")) {
                        opcode = 0;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_RESET")) {
                        opcode = 1;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_LOCATE")) {
                        opcode = 2;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_KEEPSHIFT")) {
                        opcode = 3;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_OVERBURN")) {
                        opcode = 4;
                        microcode = 0;
                        found = true;
                    }
			  if (!strcmp(s[i].c_str(), "I_ID")) {
                        opcode = 5;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_TIMER")) {
                        opcode = 6;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_ANGLE")) {
                        opcode = 7;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_TID")) {
                        opcode = 8;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_TARGETID")) {
                        opcode = 8;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_TINFO")) {
                        opcode = 9;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_TARGETINFO")) {
                        opcode = 9;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_GINFO")) {
                        opcode = 10;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_GAMEINFO")) {
                        opcode = 10;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_RINFO")) {
                        opcode = 11;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_ROBOTINFO")) {
                        opcode = 11;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_COLLISIONS")) {
                        opcode = 12;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_RESETCOLCNT")) {
                        opcode = 13;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_TRANSMIT")) {
                        opcode = 14;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_RECEIVE")) {
                        opcode = 15;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_DATAREADY")) {
                        opcode = 16;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_CLEARCOM")) {
                        opcode = 17;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_KILLS")) {
                        opcode = 18;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_DEATHS")) {
                        opcode = 18;
                        microcode = 0;
                        found = true;
                    } else if (!strcmp(s[i].c_str(), "I_CLEARMETERS")) {
                        opcode = 19;
                        microcode = 0;
                        found = true;
                    }

                    //Memory addresses
                    if (!found && (s[i][0] == '@') && ((s[i][1] >= '0') && (s[i][1] <= '9'))) {
                        opcode = str2int((rstr(s[i], s[i].length() - 1)));
                        if (opcode < 0 || opcode > (MAX_RAM + 1) + (((MAX_CODE + 1) << 3) - 1)) {
                            prog_error(3, s[i]);
                            microcode = 1;
                            found = true;
                        }
                    }

                    //Numbers
                    if (!found && ((s[i][0] == '-') || ((s[i][0] >= '0') && (s[i][0] <= '9'))) ) {
                        opcode = str2int(s[i]);
                        found = true;
                    }

                    if (found) {
                        robot[n]->code[p].op[i] = opcode;
                        if (indirect) {
                            microcode = microcode | 8;
                        }

                        robot[n]->code[p].op[MAX_OP] = robot[n]->code[p].op[MAX_OP] | (microcode << (i * 4));
                    } else if (s[i].compare(""))
                        prog_error(2, s[i]);
                }
            }
            if (show_code)
                print_code(n, p);
            if (compile_by_line)
                readkey();
        }
    }
}

void compile(short n, std::string filename) {
    fstream f;
    string pp[MAX_OP + 1];
    std::string s, s1, s2, s3, orig_s, msg;
    short i, j, k, l, linecount, mask, locktype;
    std::string ss[MAX_OP];
    char c, lc;

    lock_code = "";
    lock_pos = 0;
    locktype = 0;
    lock_dat = 0;
    //Needs to be a filestream in the main function

    if (!exist(filename))
        prog_error(8, filename);

    textcolor(robot_color(n));
    std::cout << "Compiling robot #" << to_string(n + 1) << ": " << filename << endl;

    robot[n]->is_locked = false;
    textcolor(robot_color(n));
    numvars = 0;
    numlabels = 0;
    for (k = 0; k <= MAX_CODE; k++) {
        for (i = 0; i <= MAX_OP; i++) {
            robot[n]->code[k].op[i] = 0;
        }
    }
    robot[n]->plen = 0;

    f.open(filename.c_str(), fstream::in);


    s = "";
    linecount = 0;

    //First pass, compile
    while (!f.eof() && (!s.compare("#END"))) {
        getline(f, s);
        linecount++;

        if (locktype < 3)
            lock_pos = 0;
        if (!lock_code.compare(""))
            for (i = 0; i < s.length(); i++) {
                lock_pos++;
                if (lock_pos > lock_code.length())
                    lock_pos = 1;
                switch (locktype) {
                    case 3:
                        s[i] = char((s[i] - 1) ^ (lock_code[lock_pos] ^ lock_dat));
                        break;
                    case 2:
                        s[i] = char(s[i] ^ (lock_code[lock_pos] ^ 1));
                        break;
                    default:
                        lock_dat = char(s[i] ^ lock_code[lock_pos]);
                }
                lock_dat = s[i] & 15;
            }
        s = btrim(s);
        orig_s = s;

        for (i = 0; i < s.length(); i++) {
            if ( ((s[i] >= 0) && (s[i] <= 32)) || ((s[i] >= 128) && (s[i] <= 255)) )
		s[i] = char(" ");
        }

        if (show_source && (( !lock_code.compare("")) || debugging_compiler))
            std::cout << zero_pad(linecount, 3) + ":" + zero_pad(robot[n]->plen, 3) + " " << s << endl;


	// Unfinished
	  if (debugging_compiler)
		  if (readkey() == 27)
			  exit(EXIT_FAILURE);
	  k = 0;
	  for (i = s.length(); i >= 1; i--)
		  if (s[i] == ';')
			  k = i;

	  if (k > 0)
		  s = lstr(s, k - 1);
	 
	  s = btrim(ucase(s));
	  for (i = 0; i <= MAX_OP; i++) {
		  pp[i] = "";
	  }
	  if (s.length() > 0 && s[1] != ';') {
		  switch (s[1]) {
		  case '#':
			  s1 = ucase(btrim(rstr(s, s.length() - 1)));
			  msg = btrim(rstr(orig_s, orig_s.length() - 5));
			  k = 0;
			  for (i = 1; i <= s1.length(); i++)
				  if ( (k == 0) && (s1[i] == ' '))
					  k = i;
			  k--;
			  if (k > 1) {
				  s2 = lstr(s1, k);
				  s3 = ucase(btrim(rstr(s1, s1.length() - k)));
				  k = 0;
				  if (numvars > 0)
					  for (i = 1; i <= numvars; i++)
						  if (!s3.compare(varname[i]))
							  k = i;
				  if (!strcmp(s2.c_str(), "DEF") && numvars < MAX_VARS) {
					  if (s3.length() > MAX_VAR_LEN) {
						  prog_error(14, "");
					  } else {
						  if (k > 0)
							  prog_error(11, s3);
						  else {
							  numvars++;
							  if (numvars > MAX_VARS)
								  prog_error(14, "");
							  else {
								  varname[numvars] = s3;
								  varloc[numvars] = 127 + numvars;
							  }

						  }

					  }
				  } else if (!strcmp(lstr(s2, 4).c_str(), "LOCK")) {
					  robot[n]->is_locked = true;
					  if (s2.length() > 4)
						  locktype = value(rstr(s2, s2.length() - 4));
					  lock_code = btrim(ucase(s3));
					  std::cout << "Robot is of LOCKed format from this point forward. [" << locktype << "]" << endl;
					  for (i = 1; i <= lock_code.length(); i++) {
						  lock_code[i] = char(lock_code[i] - 65);
					  }
				  } else if (!strcmp(s2.c_str(), "MSG"))
					  robot[n]->name = msg;
				  else if (!strcmp(s2.c_str(), "TIME")) {
					  robot[n]->robot_time_limit = value(s3);
					  if (robot[n]->robot_time_limit < 0)
						  robot[n]->robot_time_limit = 0;

				  } else if (!strcmp(s2.c_str(), "CONFIG")) {
					  if (!strcmp(lstr(s3, 8).c_str(), "SCANNER="))
						  robot[n]->config.scanner = value(rstr(s3, s3.length() - 8));

					  else if (!strcmp(lstr(s3, 7).c_str(), "SHIELD="))
						  robot[n]->config.shield = value(rstr(s3, s3.length() - 7));
					  else if (!strcmp(lstr(s3, 7).c_str(), "WEAPON="))
						  robot[n]->config.weapon = value(rstr(s3, s3.length() - 7));
					  else if (!strcmp(lstr(s3, 6).c_str(), "ARMOR="))
						  robot[n]->config.armor = value(rstr(s3, s3.length() - 6));
					  else if (!strcmp(lstr(s3, 7).c_str(), "ENGINE="))
						  robot[n]->config.engine = value(rstr(s3, s3.length() - 7));
					  else if (!strcmp(lstr(s3, 10).c_str(), "HEATSINKS="))
						  robot[n]->config.heatsinks = value(rstr(s3, s3.length() - 10));
					  else if (!strcmp(lstr(s3, 6).c_str(), "MINES="))
						  robot[n]->config.mines = value(rstr(s3, s3.length() - 6));
					  else
						  prog_error(20, s3);

					  if (robot[n]->config.scanner < 0)
						  robot[n]->config.scanner = 0;
					  if (robot[n]->config.scanner > 5)
						  robot[n]->config.scanner = 5;

					  if (robot[n]->config.shield < 0)
						  robot[n]->config.shield = 0;
					  if (robot[n]->config.shield > 5)
						  robot[n]->config.shield = 5;

					  if (robot[n]->config.weapon < 0)
						  robot[n]->config.weapon = 0;
					  if (robot[n]->config.weapon > 5)
						  robot[n]->config.weapon = 5;

					  if (robot[n]->config.armor < 0)
						  robot[n]->config.armor = 0;
					  if (robot[n]->config.armor > 5)
						  robot[n]->config.armor = 5;

					  if (robot[n]->config.engine < 0)
						  robot[n]->config.engine = 0;
					  if (robot[n]->config.engine > 5)
						  robot[n]->config.engine = 5;

					  if (robot[n]->config.heatsinks < 0)
						  robot[n]->config.heatsinks = 0;
					  if (robot[n]->config.heatsinks > 5)
						  robot[n]->config.heatsinks = 5;

					  if (robot[n]->config.mines < 0)
						  robot[n]->config.mines = 0;
					  if (robot[n]->config.mines > 5)
						  robot[n]->config.mines = 5;
				  } else
					  std::cout << "WARNING: unknown directive '" << s2 << "' " << endl;

			  }
			  break;
		  case '*':
			  check_plen(robot[n]->plen);
			  for (i = 0; i < MAX_OP; i++)
				  pp[i] = "";
			  for (i = 1; i < s.length(); i++)
				  if (s[i] == '*')
					  prog_error(23, s);
			  k = 0;
			  i = 1;
			  s1 = "";
			  if (s.length() <= 2)
				  prog_error(23, s);
			  while (i < s.length() && k <= MAX_OP) {
				  i++;
				  /*
				  * if ord(s[i]) in [33..41,43..127] then pp[k]:=pp[k]+s[i]
				  else if (ord(s[i]) in [0..32,128..255]) and
				  (ord(s[i-1]) in [33..41,43..127]) then inc(k);
				  */
			  }
			  for (i = 0; i <= MAX_OP; i++)
				  robot[n]->code[robot[n]->plen].op[i] = value(pp[i]);
			  robot[n]->plen++;
			  break;
		  case ':':
			  check_plen(robot[n]->plen);
			  s1 = rstr(s, s.length() - 1);
			  for (i = 0; i <= s1.length(); i++)
				  //if not (s1[i] in ['0'..'9']) then
				  prog_error(1, s);
			  robot[n]->code[robot[n]->plen].op[0] = value(s1);
			  robot[n]->code[robot[n]->plen].op[MAX_OP] = 2;
			  if (show_code)
				  print_code(n, robot[n]->plen);
			  robot[n]->plen++;
			  break;
		  case '!':
			  check_plen(robot[n]->plen);
			  s1 = btrim(rstr(s, s.length() - 1));
			  k = 0;
			  for (i = s1.length()-1; i >= 0; i--) {
				  //in [';',#8,#9,#10,' ',','] then k:=i;
			  }
			  if (k > 0)
				  s1 = lstr(s1, k - 1);
			  k = 0;
			  for (i = 0; i <= numlabels; i++) {
				  if (!strcmp(labelname[i].c_str(), s1.c_str())) {
					  if (labelnum[i] >= 0)
						  prog_error(13, "!" + s1 + "(" + cstr(labelnum[i]) + ")");
					  k = i;
				  }
				  if (k == 0) {
					  numlabels++;
					  if (numlabels > MAX_LABELS)
						  prog_error(15, "");
					  k = numlabels;
				  }
				  labelname[k] = s1;
				  labelnum[k] = robot[n]->plen;
			  }
			  break;
		  default:
			  check_plen(robot[n]->plen);
			  //parse instruction
			  //remove comments
			  k = 0;
			  for (i = s.length() -1; i >= 0; i--)
				  if (s[i] == ';')
					  k = i;
			  if (k > 0)
				  s = lstr(s, k - 1);
			  //setup variables for parsing
			  k = 0;
			  for (j = 0; j <= MAX_OP; j++)
				  pp[j] = "";
			  for (j = 0; j < s.length(); j++) {
				  c = s[j];
				  if ( (!((c == '[') || (c == 8) || (c == 9) || (c == 10) || (c == '.'))) && k <= MAX_OP)
					  pp[k] = pp[k] + c;
				  else if (!((lc == ' ') || (lc == '8') || (lc == '9') || (lc == 10) || (lc == '.')) ) {
					  k = k + 1;
				  }
				  lc = c;
			  }
			  parse1(n, robot[n]->plen, pp);
			  robot[n]->plen++;
			  break;
		  }
	  }
	  //f.close();?
	  /*
	  * Add our implied NOP if there's room. This was originally to make sure
	  * no one tries using an empty robot program, kinda pointless otherwise
	  */
	  if (robot[n]->plen <= maxcode) {
		  for (i = 0; i <= MAX_OP; i++)
			  pp[i] = "";
		  pp[0] = "NOP";
		  parse1(n, robot[n]->plen, pp);
	  } else {
		  robot[n]->plen--;
	  }

	  //second pass, resolving !labels
	  if (numlabels > 0) {
		  for (i = 0; i <= robot[n] -> plen; i++) {
			  for (j = 0; j <= MAX_OP - 1; j++) {
				  if (robot[n]->code[i].op[MAX_OP] >> (j * 4) == 3) {
					  k = robot[n]->code[i].op[j];
					  if (k > 0 && k <= numlabels) {
						  l = labelnum[k];
						  if (l<0)
							  prog_error(19, "\"!" + labelname[k] + "\" (" + cstr(l) + ")");
						  if (l<0 || l > maxcode)
							  prog_error(18, "\"!" + labelname[k] + "\" (" + cstr(l) + ")");
						  else {
							  robot[n]->code[i].op[j] = l;
							  mask = ~(0xf << (j * 4));
							  robot[n]->code[i].op[MAX_OP] = (robot[n]->code[i].op[MAX_OP] & mask) | (4 << (j * 4));
						  }
					  }
				  } else {
					  prog_error(17, cstr(k));
				  }
			  }
		  }
	  }
    }
    textcolor(7);
}

int main(int argc, char ** argv) {
	paramcount = argc;
	paramstr = argv;

	init();
	true_main();
	shutdown();

	return EXIT_SUCCESS;
}


// Just to dynamically allocate new memory (SUPER out of place, to avoid an error)
robot_rec::robot_rec() {
}