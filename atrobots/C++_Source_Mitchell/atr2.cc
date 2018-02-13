#include "Headers/filelib.h"
#include "Headers/myfile.h"
#include "Headers/atr2fun.h"


// Begin global variable/definition listings
string progname		= "AT-Robots";
string version		= "2.11";
string cnotice1		= "Copyright 1997 ''99, Ed T. Toton III";
string cnotice2		= "All Rights Reserved.";
string cnotice3		= "Copyright 2014, William \"amos\" Confer";
string main_filename	= "atr2";
string robot_ext	= ".AT2";
string locked_ext	= ".ATL";
string config_ext	= ".ATS";
string compile_ext	= ".CMP";
string report_ext	= ".REP";


#define MININT -32768
#define MAXINT 32767

// Debugging/compiler
#define SHOW_CODE		false
#define COMPILE_BY_LINE 	false
#define MAX_VAR_LEN		16
#define DEBUGGING_COMPILER	false

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

// Simulation and graphics
#define SCREEN_SCALE	0.46
#define SCREEN_X	5
#define SCREEN_Y	5
#define RPBPT_SCAL	6
#define DEFAULT_DELAY	20
#define DEFAULT_SLICE	5
#define MINE_CIRCLE	(MINE_BLAST * SCREEN_SCALE) + 1
#define BLAST_CIRCLE	(BLAST_RADIUS * SCREEN_SCALE) + 1
#define MIS_RADIUS	(HIT_RANGE / 2) + 1
#define MAX_ROBOT_LINES	8

struct op_rec {
	short	op[MAX_OP];
};

struct op_rec prog_type[MAX_CODE];

struct config_rec {
	short scanner, weapon, armor, engine, heatsinks, shield, mines;
};

struct mine_rec {
	double x, y;
	short detect, yield;
	bool detonate;
};

struct robot_rec {
	bool	is_locked;
	short	mem_watch;
	double	x, y, lx, ly, xv, yv, speed, shotstrength, damageadj, speedadj, meters;

	short	hd, thd, lhd, spd, tspd, armor, larmor, heat, lheat, ip, plen, scanarc,
		accuracy, shift, err, delay_left, robot_time_limit, max_time, time_left,
		lshift, arc_count, sonar_count, scannrange, last_damage, last_hit, transponder,
		shutdown, channel, lendarc, endarc, lstartarc, startarc, mines;

	short	tx[MAX_ROBOT_LINES +1], ltx[MAX_ROBOT_LINES +1], ty[MAX_ROBOT_LINES +1], lty[MAX_ROBOT_LINES +1];
	int	wins, trials, kills, deaths, startkills, shots_fired, match_shots, hits,
		damage_total, cycles_lived, error_count;

	struct	config_rec	config;
	string	name;
	string	fn;
	bool	shields_up, lshields, overburn, keepshift, cooling, won;
	struct 	prog_type	code;
	short	ram[MAX_RAM];
	struct 	mine_rec	mine[MAX_MINES];
	string	errorlog;
}

string parsetype[MAX_OP];
struct missile_rec {
	double x, y, lx, ly, mult, mspd;
	short source, a, hd, rad, lrad, max_rad;
}

// Begin global variables

// Robot variables
short num_robots;
struct robot_ptr robot[MAX_ROBOTS + 4]; // Array started at -2, so I shifted it over
struct missile_rec missile[MAX_MISSILES];

// Compiler variables
string	f;
short	numvars, numlabels, maxcode, lock_pos, lock_dat;
string	varname[MAX_VARS -1];
short	varloc[MAX_VARS -1];
string	labalname[MAX_VARS -1];
short	labelnum[MAX_VARS -1];
bool	show_source, compile_only;
string	lock_code;

// Simulator/graphics variables
bool	bout_over;
short	step_mode;	// 0 = off; for 0 < step_mode <= 9 = # of game cycles per step
short	temp_mode;	// Stores previous step_mode for return to step
short	step_count	// Step counter used as break flag
bool	step_loop;	// Break flag for stepping

bool	old_shields, insane_missiles, debug_info, windoze, no_gfx, logging_errors,
	timing, show_arcs;
short	game_delay, time_slice, insanity, update_timer, max_gx, max_gy, stats_mode;
int	game_limit, game_cycle, matches, played, executed;

// General settings
bool	quit, report, show_cnotice;
short	kill_count, report_type;


// Begin functions

string operand(short n, short m) {
	string s = cstr(n);

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


string mnemonic(short n, short m) {
	string s = cstr(n);

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


// Log error here

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
	switch (stats_mode) {
	case 1:
		viewport(480, 4 + (n*35), 635, 37 + (n * 35));
		max_gx = 155;
		max_gy = 33;
		break;
	case 2:
		viewport(480, 4 + (n*13), 635, 15 + (n * 13));
		max_gx = 155;
		max_gy = 11;
		break;
	default:
		viewport(480, 4 + (n * 70), 635, 70 + (n * 70));
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

// Robot_error


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
		cout << endl << "Match " << played << "/" << matches << " Cycle: " << zero_pad(game_cycle, 9);
	else {
		viewport(480, 440, 635, 475);
		setfillstyle(BLACK);
		bar(59, 2, 154, 10);
		setcolor(LIGHT_GRAY);
		outtextxy(75, 3, zero_pad(game_cycle, 9));
	}
}


// Initialize the entire screen
void setscreen() {
	short i;

	if ( !graphix )
		return;

	// Window and renderer will be created in init, so no to worry here
	viewport(0, 0, 639, 479); // XXX Legitimate size of window XXX
	box(0, 0, 639, 479);

	stats_mode = 0;
	// This switch will only compile in GNU c compiler
	switch (num_robots) {
	case 0 ... 5:
		stats_mode = 0;
		break;

	case 6 ... 11:
		stats_mode = 1;
		break;

	case 12 ... max_robots:
		stats_mode = 2;
		break;

	default:
		stats_mode = 0;
	}

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
		if ( i < MAX_SHOWN ) {
			robot_graph(i);
			hole(0, 0, max_gx, max_gy);
			if ( i <= NUM_ROBOTS ) {
				setcolor(robot_color(i));
				outtextxy(3, 2, base_name(no_path(robot[i] -> fn)));
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
					outtextxy(122, 2, zero_pad(wins, 4));
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
				setfillpatttern(DARK_GRAY); // XXX This would come out checkered. New color scheme may be needed
				bar(1, 1, max_gx - 1, max_gy - 1);
			}
		}
	}
}


void graph_mode(bool on) {
	if ( on && !graphixs ) {
		// Replace Graph_VGA function with sdl
		SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_OPENGL, &window_main, &renderer_main);

		setscreen();
		graphix = true;
	} else if ( !on && graphix ) {
		SDL_DestroyRenderer(renderer_main);
		SDL_DestroyWindow(window_main);
		graphix = false;
	}
}


void prog_error(short n, string ss) {
// Do stuff

}


void print_code(short n, short p) {
	short i;

	cout << (hex(p) + ": ");
	for ( i = 0; i <= MAX_OP; i++ )
		cout << zero_pad(robot[n] -> code[p].op[i], 5), " ";

	cout << " =  ";
	for ( i = 0; i <= MAX_OP; i++ )
		cout << hex(robot[n] -> code[p].op[i]) << "h ";

	cout << endl << endl;
}

// parse1(


void check_plen(short plen) {
	if (plen > MAX_CODE )
		prog_error(16, "\nMaximum progrm length exceeded, (Limit: " + cstr(MAX_CODE + 1) + " compiled lines)");
}


// Crutial function. Needed to load and run bots
void compile(short n, string filename) {


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

	for (i = 0; i <= MAX_RAM; i++) {
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


