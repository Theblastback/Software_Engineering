#include "Headers/filelib.h"
#include "Headers/myfile.h"
#include "Headers/atr2fun.h"
#include <iostream>


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
#define ROBOT_SCALE	6
#define DEFAULT_DELAY	20
#define DEFAULT_SLICE	5
#define MINE_CIRCLE	(MINE_BLAST * SCREEN_SCALE) + 1
#define BLAST_CIRCLE	(BLAST_RADIUS * SCREEN_SCALE) + 1
#define MIS_RADIUS	(HIT_RANGE / 2) + 1
#define MAX_ROBOT_LINES	8

struct op_rec {
	short	op[MAX_OP+1];
}

struct op_rec prog_type[MAX_CODE+1];

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

	short	tx[MAX_ROBOT_LINES], ltx[MAX_ROBOT_LINES], ty[MAX_ROBOT_LINES], lty[MAX_ROBOT_LINES];
	int	wins, trials, kills, deaths, startkills, shots_fired, match_shots, hits,
		damage_total, cycles_lived, error_count;

	struct	config_rec	config;
	string	name;
	string	fn;
	bool	shields_up, lshields, overburn, keepshift, cooling, won;
	struct 	prog_type	code;
	short	ram[MAX_RAM +1];
	struct 	mine_rec	mine[MAX_MINES +1];
	ostream	errorlog;
}

string parsetype[MAX_OP +1];
struct missile_rec {
	double x, y, lx, ly, mult, mspd;
	short source, a, hd, rad, lrad, max_rad;
}

// Begin global variables
ofstream log;

// Robot variables
short num_robots;
struct robot_ptr robot[MAX_ROBOTS + 5]; // Array started at -2, so I shifted it over
struct missile_rec missile[MAX_MISSILES + 1];

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


// Logs errors to the console for debugging robots.

void log_error(int i, int n, string ov) {
    log.open("errlog.txt");
    log << endl;
    log << endl;
    if (!logging_errors)
        log << "Turn on debugging mode to error check your code!" << endl;
    switch (i) {
        case 1:
            log << "Stack full - Too many CALLs?" << endl;
            break;

        case 2:
            log << "Label not found. Hmmm." << endl;
            break;

        case 3:
            log << "Can't assign value - Tisk tisk." << endl;
            break;

        case 4:
            log << "Illegal memory reference" << endl;
            break;

        case 5:
            log << "Stack empty - Too many RETs?" << endl;
            break;

        case 6:
            log << "Illegal instruction. How bizarre." << endl;
            break;

        case 7:
            log << "Return out of range - Woops!" << endl;
            break;

        case 8:
            log << "Divide by zero" << endl;
            break;

        case 9:
            log << "Unresolved !label. WTF?" << endl;
            break;

        case 10:
            log << "Invalid Interrupt Call" << endl;
            break;

        case 11:
            log << "Invalid Port Access" << endl;
            break;

        case 12:
            log << "Com Queue empty" << endl;
            break;

        case 13:
            log << "No mine-layer, silly." << endl;
            break;

        case 14:
            log << "No mines left" << endl;
            break;

        case 15:
            log << "No shield installed - Arm the photon torpedoes instead. :/" << endl;
            break;

        case 16:
            log << "Invalid Microcode in instruction." << endl;
            break;

        default:
            log << "Unknown error." << endl;
    }
    log << endl;
    log << endl;
    log
            << " <", i, "> ", s, " (Line #", robot[n]->ip, ") [Cycle: ", robot[n]->game_cycle, ", Match: ", robot[n]->played, "/", robot[n]->matches,
            "]" << endl;
    log << " ", mneonic(robot[n]->code[robot[n]->ip].op[0], robot[n]->code[robot[n]->ip].op[3] & 15), "  ",
            operand(robot[n]->code[robot[n]->ip].op[1], (robot[n]->code[robot[n]->ip].op[3] >> 4) & 15), ", ",
            operand(robot[n]->code[robot[n]->ip].op[2], (robot[n]->code[robot[n]->ip].op[3] >> 8) & 15) << endl;
    if (ov != '')
        log << "    (Values: ", ov, ")" << endl;
    else
        log << " ";
    log << " AX=", addrear(cstr(robot[n]->ram[65]) + ",", 7) << endl;
    log << " BX=", addrear(cstr(robot[n]->ram[66]) + ",", 7) << endl;
    log << " CX=", addrear(cstr(robot[n]->ram[67]) + ",", 7) << endl;
    log << " DX=", addrear(cstr(robot[n]->ram[68]) + ",", 7) << endl;
    log << " EX=", addrear(cstr(robot[n]->ram[69]) + ",", 7) << endl;
    log << " FX=", addrear(cstr(robot[n]->ram[70]) + ",", 7) << endl;
    log << " Flags = ", robot[n]->ram[64] << endl;
    log << " AX=", addrear(hex(robot[n]->ram[65]) + ",", 7) << endl;
    log << " BX=", addrear(hex(robot[n]->ram[66]) + ",", 7) << endl;
    log << " CX=", addrear(hex(robot[n]->ram[67]) + ",", 7) << endl;
    log << " DX=", addrear(hex(robot[n]->ram[68]) + ",", 7) << endl;
    log << " EX=", addrear(hex(robot[n]->ram[69]) + ",", 7) << endl;
    log << " FX=", addrear(hex(robot[n]->ram[70]) + ",", 7) << endl;
    log << " Flags = ", hex(robot[n]->ram[64]) << endl;
    log.close();
    return;
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
		cout << endl << "Match " << played << "/" << matches << " Cycle: " << zero_pad(game_cycle, 9);
	else {
		viewport(480, 440, 635, 475);
		setfillstyle(BLACK);
		bar(59, 2, 154, 10);
		setcolor(LIGHT_GRAY);
		outtextxy(75, 3, zero_pad(game_cycle, 9));
		SDL_RenderPresent();
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
		robot[n] -> dd = 1000;
		for ( i = 0; i < NUM_ROBOTS; i++ ) {
			if ( robot[i] -> x < 0 )
				robot[i] -> x = 0;
			if ( robot[i] -> x > 1000 )
				robot[i] -> x = 1000;
			if ( robot[i] -> y < 0 )
				robot[i] -> y = 0;
			if ( robot[i] -> y > 1000 )
				robot[i] -> 1000;

			robot[n] -> d = distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y);
			if ( (robot[i] -> armor > 0) && (i != n) && (robot[n] -> d < robot[n] -> dd) )
				robot[n] -> dd = robot[n] -> d;
		}
	} while ( robot[n] -> dd > 32 );

	for ( i = 0; i < MAX_MINES; i++ ) {
		robot[n] -> mine[i] -> x = -1;
		robot[n] -> mine[i] -> y = -1;
		robot[n] -> mine[i] -> yield = 0;
		robot[n] -> mine[i] -> detonate = false;
		robot[n] -> mine[i] -> detect = 0;
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


void create_robot(short n, string filename) {
	short i, j, k;

	if ( (robot[n] = (struct robot_rec *) malloc(sizeof(struct robot_rec))) == NULL ) { // Malloc failed
		prog_error(9, base_name(no_path(filename)));
		return;
	}

	robot[n] -> config = (struct config_rec *) malloc(sizeof(struct config_rec));
	if ( robot[n] -> config == NULL ) {
		prog_error(9, base_name(no_path(filename)));
		free(robot[n]);
		return;
	}

	robot[n] -> code = (struct prog_type *) malloc(sizeof(struct prog_type));
	if ( robot[n] -> code == NULL ) {
		prog_error(9, base_name(no_path(filename)));
		free(robot[n] -> config);
		free(robot[n]);
		return;
	}

	for (i = 0; i <= MAX_RAM; i++) {
		robot[n] -> mine[i] = (struct mine_rec *) malloc(sizeof(struct mine_rec));
		if ( robot[n] -> mine[i] == NULL ) {
			prog_error(9, base_name(no_path(filename)));
			for ( k = 0; k < i; k++ )
				free(robot[n] -> mine[k]);

			free(robot[n] -> config);
			free(robot[n] -> code);
			free(robot[n]);
			return;
		}
	}
	init_robot(n);
	robot[n] -> filename = ucase(btrim(filename));
	if ( robot[n] -> filename.compare(base_name(robot[b] -> filename)) == 0 )
		if ( robot[n] -> filename[0] == '?' )
			robot[n] -> filename = robot[n] -> filename + locked_ext;
		else
			robot[n] -> filename = robot[n] -> filename + robot_ext;

	if ( robot[n] -> filename[0] == '?' )
		robot[n] -> filename = rstr(robot[n] -> filename, robot[n] -> filename.length() -1);

	robot[n] -> fn = base_name(no_path(robot[n] -> filename));
	compile(n, robot[n] -> filename);
	robot_config(n);

	k = robot[n] -> config.scanner + robot[n] -> config.armor + robot[n] -> config.weapon +
		robot[n] -> config.engine + robot[n] -> config.heatsinks + robot[n] -> config.shield + robot[n] -> config.mines;

	if ( k > MAX_CONFIG_POINTS )
		prog_error(21, cstr(k) + "/" + cstr(MAX_CONFIG_POINTS));
}


void shutdown() {
	short i;

	graph_mode(false);
	if ( show_cnotice ) {
		textcolor(CYAN);
		cout << progname << " " << version << " ";
		cout << cnotice1 << endl << cnotice2 << endl << cnotice3 << endl;

	textcolor(LIGHT_GRAY);
	cout << endl;
	if ( logging_errors ) {
		for ( i = 0; i <= num_robots; i++ ) {
			cout << "Robot error-log created: " << base_name(robot[i] -> fn << ".ERR"<< endl;
			robot[i] -> errorlog.close();
		}
	}

	exit(EXIT_SUCCESS);
}


void init() {
	short i;

	if ( debugging_compiler || compile_by_line || show_code ) {
		cout << "!!! Warning !!! Compiler Debugging enabled !!!";
		flushkey();
		readkey();
		cout << endl;
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
	maxcode = max_code;

	make_tables();
	srand(time(NULL)); // Equivalent of randomize()
	num_robots = -1;
	game_limit = 100000;
	game_cycle = 0;
	game_delay = default_delay;
	time_slice = default_slice;

	for ( i = 0; i <= MAX_MISSILES; i++ ) {
		missile[i] -> a = 0;
		missile[i] -> source = -1;
		missile[i] -> x = 0;
		missile[i] -> y = 0;
		missile[i] -> lx = 0;
		missile[i] -> ly = 0;
		missile[i] -> multi = 1;
	}

	registered = false;
	reg_name = "Unregistered";
	reg_num = 0xffff;
	check_registration();

	cout << endl;
	textcolor(CYAN);
	cout << progname << " " << version << " ";
	cout << cnotice1 << endl << cnotice2 << endl;
	textcolor(LIGHT_GRAY);
	if ( !registered ) {
		textcolor(RED);
		cout << "Unregistered version" << endl;
		textcolor(LIGHT_GRAY);
	} else
		cout << "Registered to: " << reg_name;

	cout << endl;

	delete_compile_report();

	string tmp;
	if ( paramcount > 0 )
		for ( i = 1; i <= paramcount; i++ ) {
			tmp = arguement[i]; // Arguement is paramstr. It is effectively argv, but global
			parse_param(btrim(ucase(tmp))
		}
	else
		pro_error(5, "");

	temp_mode = step_mode;

	if ( logging_errors )
		for ( i = 0; i <= num_robots; i++ )
			robot[i] -> errorlog.open(base_name(robot[i] -> fn) + ".ERR");

	if ( compile_only )
		write_compile_report();
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
	if ( game_limit > 100000 )
		game_limit = 100000l
	if ( maxcode < 1 )
		max_code = 1; // 2 lines of max code. 0 based
	if ( maxcode > MAX_CODE )
		maxcode = MAX_CODE;

	for ( i = NUM_ROBOTS + 1; i <= MAX_ROBOTS + 4; i++ )
		robot[i] = robot[0];

}


void draw_robot(short n) {
	short i, t;
	double xx, yy;

	if ( (n < 0) || (n > NUM_ROBOTS) )
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
	robot[n] -> ty[4] = (yy - cost[y] * ROBOT_SCALE * 0.8) + 0.5;

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
				arc(robot[n] -> ltx[3], robot[n] -> lty[3], robot[n] -> lstartarc, robot[n] -> lendarc, (robot[n] -> scanrance * SCREEN_SCALE) + 0.5);
		}

		if ( robot[n] -> sonar_count > 0 )
			circle(robot[n] -> ltx[3], robot[n] -> lty[3], robot[n] -> lstartarc, robot[n] -> lendard, (robot[n] -> scanrange * SCREEN_SCALE) + 0.5);

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
		if ( graphix ) { // Only draw if graphics are enabled
			setcolor(get_rgb(robot_color(n) & 7));
			if ( robot[n] -> shields_up )
				circle(robot[n] -> tx[3], robot -> ty[3], ROBOT_SCALE);

			line(robot[n] ->tx[0], robot[n] ->ty[0], robot[n] ->tx[1], robot[n] ->ty[1]);
			line(robot[n] ->tx[1], robot[n] ->ty[1], robot[n] ->tx[2], robot[n] ->ty[2]);
			line(robot[n] ->tx[2], robot[n] ->ty[2], robot[n] ->tx[0], robot[n] ->ty[0]);
			setcolor(LIGHT_GRAY);

			line(robot[n] ->tx[3], robot[n] ->ty[3], robot[n] -> tx[4], robot[n] ->ty[4]);
			setcolor(DARK_GRAY);

			if ( showarcs && (robot[n] -> arc_count > 0) ) {
				line(robot[n] ->tx[3], robot[n] ->ty[3], robot[n] ->tx[5], robot[n] ->ty[6]);
				line(robot[n] ->tx[3], robot[n] ->ty[3], robot[n] ->tx[6], robot[n] ->ty[6]);
				if ( robot[n] -> scanrange < 1500 )
					arc(robot[n] ->tx[3], robot[n] ->ty[3], robot[n] -> startarc, robot[n] -> endarc, (robot[n] -> scanrange * SCREEN_SCALE) + 0.5);
			}
			if ( show_arcs && (robot[n] -> sonar_count > 0) )
				circle(robot[n] ->tx[4], robot[n] ->ty[4], (robot[n] -> max_sonar * SCREEN_SCALE) + 0.5);
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


short get_val(short n, short c, short 0) {
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
				if ( (i < 0) || (i > max_ram) )
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
	if ( (robot[n] -> ram[71] > STACK_BASE) && (robot[n] -> ram[71] <= (STACK_BASE + STACK_SIZE)) {
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


void init_mine(short n, detectrange, size) {
	short i, k;

	k = -1;
	for ( i = 0; i <= MAX_MINES; i++ ) {
		if ( ((robot[n] -> mine[i].x < 0) || (robot[n] -> mine[i].x > 1000) || (robot[n] -> mine[i].y < 0) || (robot[n] -> mine[i].y > 1000) ||
			(robot[n] -> mine[i].yield <= 0)) && ( k < 0 ) ) {
			k = i;
		}
	}
	if ( k >= 0 ) {
		robot[n] -> mine[k].x = x;
		robot[n] -> mine[k].y = y;
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
	for ( i = MAX_MISSILES; i >= 0; i-- )
		if ( missile[i].a == 0 )
			k = i

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

		if ( missile[k].blast > 0 ) {
			missile[k].max_rad = missile[k].blast;
			missile[k].a = 2;
		} else {
			if ( (missile[k].s >= 0) && (missile[k].s <= NUM_ROBOTS) )
				missile[k].mult = missile[k].mult * (robot[s] -> shotstrength);

			m = missile[k].mult;
			if ( ob )
				m = m + 0.25;

			missile[k].mspd = MISSILE_SPD * mult;
			if ( insane_missiles ) {
				missile[k].mspd = 100 + (50 * insanity) * mult;
			if ( (s >= 0) && ( s <= NUM_ROBOTS) )
				robot[s] -> heat += (20 * m);
				(robot[s] -> shots_fired)++;
				(robot[s] -> match_shots)++;
			}
		}
		missile[k].a = 1;
		missile[k].hd = dir;
		missile[k].max_rad = missile[k].mis_radius;

		/* XXX Debug code here

		*/
	}
}

void damage(short n, short d, bool physical) {
	short i, k, h, dd;
	real m;

	if ( (n < 0) || (n > NUM_ROBOTS) || (robot[n] -> armor <= 0) )
		return;
	if ( robot[n] -> config.shield < 3 )
		robot[n] -> shields_up = false;

	h = 0;
	if ( (robot[n] -> shields_up) && (!physical) ) {
		dd = d;
		if ( (robot[n] -> old_shields) && (robot[n] -> config.shield >= 3) ) {
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

	if ( robot[n] -> armor <= 0 )
		robot[n] -> armor = 0;
		update_armor(n);
		robot[n] -> heat = 500;
		update_heat(n);

		robot[n] -> armor = 0;
		(robot[n] -> kill count)++;
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

		for ( i = 0; i <= NUM_ROBOTS; i++ ) {
			if ( (i != n) && (robot[i] -> armor > 0) ) {
				k = (distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y)) + 0.5;
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
	rand = MAXINT;
	if ( (n < 0) || (n > NUM_ROBOTS) )
		return NULL;

	if ( robot[n] -> scanarc < 0 )
		robot[n] -> scanarc = 0;

	robot[n] -> accuracy = 0;
	nn = -1;
	dir = (robot[n] -> shift + robot[n] -> hd) & 255;
	// XXX Debug code here

	for ( i = 0; i <= NUM_ROBOTS; i++ ) {
		if ( (i != n) && (robot[i] -> armor > 0) ) {
			j = find_anglei(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y);
			d = distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y);
			k = d + 0.5;
			if ( (k < range) && ( k <= robot[n] -> scanrange) &&
			     ( (abs(j - dir) <= abs(robot[n] -> scanarc)) || (abs(j - dir) >= (256 - abs(robot[n] -> scanarc))) ) {

				dir = (dir + 1024) & 255;
				xx = (sint[dir] * d + robot[n] -> x) + 0.5;
				yy = ((-1 * cost[dir])* d + robot[n] ->  y) + 0.5;
				r = distance(xx, yy, robot[i] -> x, robot[i] -> y);
				// XXX Debug code here
				if ( (robot[n] -> scanarc > 0) || robot[n] -> r < robot[n] -> hit_range - 2) ) {
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
		if ( (nn >= 0) && (nn <= NUM_ROBOTS) ) {
			robot[n] -> ram[5] = robot[nn] -> transponder;
			robot[n] -> ram[6] = (robot[nn] -> hd - (robot[n] -> hd + robot[n] -> shift) + 1024) & 255;
			robot[n] -> ram[7] = robot[nn] -> spd;
			robot[n] -> robot[13] = (robot[nn] -> speed * 100) + 0.5;
		}
	}

	return range;
}


void com_transmit(short n, short chan, short data) {
	short i;

	for ( i = 0; i <= NUM_ROBOTS; i++ ) {
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

short com_receive(short n) {
	short i, k;

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
		robot_error(n, 12, "");

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
		case 8;
			v = robot[n] -> accuracy;
			*time_used += 1;
			break;
		case 9: // This got mixed with case 16. Check again
			nn = -1;
			*time_used += 3;
			k = MAXINT;
			for ( i = 0; i <= NUM_ROBOTS; i++ ) {
				j = distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y) + 0.5;
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
				j = distance(robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y) + 0.5;
				if ( (n != j) && (j < k) && (j < MAX_SONAR) && (robot[i] -> armor > 0) ) {
					k = j;
					l = i;
					nn = i;
				}
			}

			if ( l > 0 ) {
				v = (int)((find_angle(
						robot[n] -> x, robot[n] -> y, robot[i] -> x, robot[i] -> y
							) / PI * 128 + 1024 + (rand() % 66)
					    ) - 32) + 0.5) & 255;
			} else
				v = MININT;
			if ( (nn >= 0) && (n <= NUM_ROBOTS) )
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
		case 12: robot[n] -> shift = (robot[n] -> shift + v + 1024) & 255; break
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
					mine[i].detonate = true;
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
			robot[n] -> keep_shift = false;
		else
			robot[n] -> keep_shift = true;
		robot[n] -> ram[70] = robot[n] -> shift & 255;
		break;
	case 4:
		if ( robot[n] -> ram[65] == 0 )
			robot[n] -> overburn = false
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

		robot[n] -> ram[65] = ((fine_angle((short)robot[n] -> x + 0.5, (short)robot[n] -> y + 0.5, j, k) / PI * 128 + 256) + 0.5) & 255;
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
		for ( i = 0; i <= NUM_ROBOTS; i++ )
			if ( robot[i] -> armor > 0 )
				k++;

		robot[n] -> ram[68] = k;
		robot[n] -> ram[69] = played;
		robot[n] -> ram[70] = matches;
		*time_used = 4;
		break;
	case 11:
		robot[n] -> ram[68] = (speed * 100) + 0.5;
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

	if ( (game_cycle >= game_limit) && ( game_limit > 0 )
		return true;

	if ( (game_cycle & 31) == 0 ) {
		k = 0;
		for ( n = 0; n <= NUM_ROBOTS; n ++ )
			if ( robot[i] -> armor > 0 )
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
		cout << "Match " << played << "/" << matches << ", Battle in progress..." << endl << endl;
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
	case 'A': sow_arcs = !show_arcs; break;
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


string victor_string(short k, short n) {
	string s = "";

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
		sy = 93 - NUM_ROBOTS * 3;
		viewport(0, 0, 639, 479);
		box(sx, sy, sx + 591, sy + 102 + NUM_ROBOTS * 12);
		hole(sx + 4, sy + 4, sx + 587, sy + 98 + NUM_ROBOTS * 12);
		setfillpattern(1); // 50/50 checkering

		bar(sx + 5, sy + 5, sx + 586, sy + 97 + NUM_ROBOTS * 12);
		set_color(WHITE);
		outtextxy(sx + 16, sy + 20, "Robot            Scored   Wins   Matches   Armor   Kills   Deaths    Shots");
		outtextxy(sx + 16, sy + 30, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

		n = -1;
		k = 0;
		for ( i = 0; i <= NUM_ROBOTS; i++ ) {
			if ( (robot[n] -> armor > 0) || (robot[n] -> won) ) {
				k++;
				n = i;
			}
		}

		for ( i = 0; i <= NUM_ROBOTS; i++ ) {
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
		outtextxy(sx + 16, sy + 64 + NUM_ROBOTS * 12, victor_string(k, n));

		if ( windoze ) {
			outtextxy(sx + 16, sy + 76 +NUM_ROBOTS * 12, "Press any key to continue...");
			flushey();
			readkey();
		}
	} else { // No graphics; Display results on commandline/terminal
		textcolor(WHITE);
		cout << endl << space(79) << endl;
		cout << "Match " << played << "/" << matches << " results:" << endl << endl;
		cout << "Robot            Scored   Wins   Matches   Armor   Kills   Deaths    Shots" << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		n = -1;
		k = 0;

		for ( i = 0; i <= NUM_ROBOTS; i++ ) {
			if ( (robot[n] -> armor > 0) || (robot[n] -> won) ) {
				k++;
				n = i;
			}
		}

		for ( i = 0; i <= NUM_ROBOTS; i++ ) {
			textcolor(robot_color(i));
			if ( (k == 1) && (n == i) )
				j = 1;
			else
				j = 0;

			cout << addfront(cstr(i + 1), 2) << " - " << addrear(robot[n] -> fn, 15) << cstr(j) <<
				addfront(cstr(robot[n] -> wins), 8) << addfront(cstr(robot[n] -> trials), 8)  <<
				addfront(cstr(robot[n] -> armor) + "%", 9) << addfront(cstr(robot[n] -> kills), 7) <<
				addfront(cstr(robot[n] -> deaths), 8) << addfront(cstr(robot[n] -> match_shots), 9) << endl;

		}
		textcolor(WHITE);
		cout << endl << victor_string(k, n) << endl << endl;
	}
}


void score_robots() {
	short i, k, n;

	for ( i = 0; i <= NUM_ROBOTS; i++ ) {
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

void init_bout();
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
	for ( i = 0; i <= NUM_ROBOTS; i++ ) {
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

	if ( step_mode == 0 )
		step_loop = false
	else
		step_loop = true;

	step_count = -1; // will be set to 0 upon first iteration of do while loop
	if ( graphix && (step_mode > 0) )
		for ( i = 0; i <= NUM_ROBOTS; i++ )
			draw_robot(i);

	// Begin start of main loop
	do {
		game_cycle++;
		for ( i = 0; i <= NUM_ROBOTS; i++ )
			if ( robot[i] -> armor > 0 )
				do_robot(i);

		for ( i = 0; i <= MAX_MISSILES; i++ )
			if ( missile[i].a > 0 )
				do_missile();

		for ( i = 0; i <= NUM_ROBOTS; i++ )
			for ( k = 0; k <= MAX_MINES; k++ )
				if ( robot[i] -> mine[k].yield > 0 )
					do_mine(i, k);


		if ( graphix && timing )
			time_delay(game_delay);

		if ( keypressed )
			c = upcase(readkey());
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
			if ( game_delay < 100 ) {
				// Triple dot notation only compiles on gcc
				switch (game_delay) {
				case 0 ... 4: game_delay = 5; break;
				case 5 ... 9: game_delay = 10; break;
				case 10 ... 14: game_delay = 15; break;
				case 15 ... 19: game_delay = 20; break;
				case 20 ... 29: game_delay = 30; break;
				case 30 ... 39: game_delay = 40; break;
				case 40 ... 49: game_delay = 50; break;
				case 50 ... 59: game_delay = 60; break;
				case 60 ... 74: game_delay = 75; break;
				case 75 ... 100: game_delay = 100; break;
				}
			}
			break;
		case '-':
		case '_':
			if ( game_delay > 0 ) {
				switch (game_delay) {
				case 0 ... 5: game_delay = 0; break;
				case 6 ... 10: game_delay = 5; break;
				case 11 ... 15: game_delay = 10; break;
				case 16 ... 20: game_delay = 15; break;
				case 21 ... 30: game_delay = 20; break;
				case 31 ... 40: game_delay = 30; break;
				case 41 ... 50: game_delay = 40; break;
				case 51 ... 60: game_delay = 50; break;
				case 61 ... 75: game_delay = 65; break;
				case 76 ... 100: game_delay = 75; break;
				}
			}
			break;
		case 'G': toggle_graphix(); break;
		default:
			process_keypress(c);
		}

		flushkey();

		if ( game_delay < 0 )
			game_delay = 0;
		else if ( game_delay > 100 )
			game_delay = 100;

		switch (game_delay) {
		case 0 ... 1: k = 100; break;
		case 2 ... 5: k = 50; break;
		case 6 ... 10: k = 25; break;
		case 11 ... 25: k = 20; break;
		case 26 ... 40: k = 10; break;
		case 41 ... 70: k = 5; break;
		case 71 ... MAXINT: k = 1; break;
		default:
			k = 10;
		}

		if ( !graphix )
			k = 100;

		if ( graphix ) {
			if ( ((game_cycle % k) == 0) || (game_cycle == 10) )
				update_cycle_window;
			else {
				if ( update_timer != get_seconds_after_hour() >> 1 )
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

	f << (NUM_ROBOTS + 1) << endl;
	for ( i = 0; i <= NUM_ROBOTS; i++ ) {
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
	string s;

	if ( !graphix || !windoze )
		return;

	setscreen();
	viewport(0, 0, 639, 479);
	box(100, 150, 539, 200);
	hole(105, 155. 534, 195);

	setfillpattern(1); // Checkered
	bar(105, 155, 534, 195);

	setcolor(15);
	s = "Press any key to begin!";
	outtextxy(320 - ((s.length() << 3) >> 1), 172, s);

	flushkey();
	readkey();
	setscreen();
}

void true_main() {
	short i, j, k, l, n, w;

	if ( graphix )
		begin_main();

	if ( matches > 0 )
		for ( i = 1; i <= matches; i++ )
			bout();

	if ( graphix == false )
		cout << endl;

	if ( quit )
		return;

	// Calculates overall statistics
	if ( matches > 1 ) {
		cout << endl << endl;

		graph_mode(false);
		textcolor(WHITE);
		cout << "Bout complete! (" << matches << ")" << endl << endl;

		k = 0;
		w = 0;

		for ( i = 0; i <= NUM_ROBOTS; i++ ) {
			if ( robot[i] -> wins == w )
				k++;
			if ( robot[i] -> wins > w ) {
				k = 1;
				n = i;
				w = robot[i] -> wins;
			}
		}

		cout << "Robot            Wins   Matches   Kills   Deaths    Shots" << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

		for ( i = 0; i <= NUM_ROBOTS; i++ ) {
			textcolor(robot_color(i));
			cout << addfront(cstr(i + 1), 2) << " - " << addrear(robot[i] -> fn, 8) <<
				addfront(cstr(robot[i] -> wins), 7) << addfront(cstr(robot[i] -> trials), 8)  <<
				addfront(cstr(robot[n] -> kills), 8) << addfront(cstr(robot[n] -> deaths), 8) <<
				addfront(cstr(robot[n] -> shots_fired), 9) << endl;
		}

		textcolor(WHITE);
		cout << endl;
		if ( k == 1 ) {
			cout << "Robot #" << to_string(n + 1) << " (" << robot[n] -> fn << ") wins the bout! (score :" << to_string(w) << "/" << to_string(matches) << endl;
		else
			cout << "There is no clear victor!" << endl;

		cout << endl;
	} else if (graphix) {
		graph_mode(false);
		show_statistics();
	}

	if ( report )
		write_report();

}


void do_robot(short n){
  short i, j, k, l, tthd;
  double heat_mult, ttx, tty;
  if (n < 0 || n > num_robots) {
    exit();
  }
  for (robot[*n]) {
    if (armor <= 0) {
      exit();
    }
  }
  time_left = time_slice;
  if (time_left > robot_time_limit && robot_time_limit > 0) {
    time_left = robot_time_limit;
  }
  if (time_left > max_time && max_time > 0) {
    time_left = max_time;
  }
  executed = 0;

  while (time_left > 0 && !cooling && executed < 20 + time_slice && armor > 0) {
    if (delay_left < 0) {
      delay_left = 0;
    }
    if (delay_left > 0) {
      delay_left--;
      time_left--;
    }
    if (time_left >= 0 && delay_left = 0) {
      execute_instruction(n);
    }
    if (heat >= shutdown) {
      cooling = true;
      shields_up = false;
    }
    if (heat >= 500) {
      damage(n, 1000, true);
    }
  }

  thd = (thd + 1024) + 255;
  hd = (hd + 1024) + 255;
  shift = (shift + 1024) + 255;
  if (tspd < -75) {
    tspd = -75;
  }
  if (tspd > 100) {
    tspd = 100;
  }
  if (spd < -75) {
    spd = -75;
  }
  if (spd > 100) {
    spd = 100;
  }
  if (heat < 0) {
    heat = 0;
  }
  if (last_damage < MAXINT) {
    last_damage++;
  }
  if (last_hit < MAXINT) {
    last_hit++;
  }

  /* update heat */

  if (shields_up && (game_cycle + 3 == 0)) {
    heat++;
  }
  if (!shields_up) {
    if (heat > 0) {
      switch(config.heatsinks){
      case 1:
        if (game_cycle + 1 == 0) {
          heat--;
        }
        break;
      case 2:
          if (game_cycle % 3 == 0) {
            heat--;
          }
          break;
      case 3:
        if (game_cycle + 3 == 0) {
          heat--;
        }
        break;
      case 4:
        if (game_cycle + 7 == 0) {
          heat--;
        }
        break;
      default:
        if (game_cycle + 3 == 0) {
          heat++;
        }
        break;
      }
    }
    if (overburn && (game_cycle % 3 == 0)) {
      heat++;
    }
    if (heat > 0) {
      heat--;
    }
    if (heat > 0 && (game_cycle + 7 == 0) && (abs(tspd) <= 25)) {
      heat--;
    }
    if ((heat <= shutdown - 50) || (heat <= 0)) {
      cooling = false;
    }
  }
  if (cooling) {
    tspd = 0;
  }
  heat_mult = 1;
  if (heat >= 80 && heat <= 99) {
    heat_mult = .98;
  }
  if (heat >= 100 && heat <= 149) {
    heat_mult = .95;
  }
  if (heat >= 150 && heat <= 199) {
    heat_mult = .85;
  }
  if (heat >= 200 && heat <= 249) {
    heat_mult = .75;
  }
  if (heat >= 250 && heat <= MAXINT) {
    heat_mult = .50;
  }
  if (overburn) {
    heat_mult = heat_mult * 1.30;
  }
  if (heat >= 475 && (game_cycle + 3 == 0)) {
    damage(n, 1, true);
  }else if (heat >= 450 && (game_cycle + 7 == 0)) {
    damage(n, 1, true);
  }else if (heat >= 400 && (game_cycle + 15 == 0)) {
    damage(n, 1, true);
  }else if (heat >= 350 && (game_cycle + 31 == 0)) {
    damage(n, 1, true);
  }else if (heat >= 300 && (game_cycle + 63 == 0)) {
    damage(n, 1, true);
  }else{
    damage(n, 1, true);
  }

  /* update robot in physical world */

  if (abs(spd - tspd) <= acceleration) {
    spd = tspd;
  }else{
    if (tspd > spd) {
      spd++;
      acceleration++;
    }else{
      spd--;
      acceleration--;
    }
  }

  /* turning */

  tthd = hd + shift;
  if (abs(hd - thd) <= turn_rate || abs(hd - thd) >= 256 - turn_rate) {
    hd = thd;
  }else if (hd != thd) {
    k = 0;
    if (((thd > hd) && (abs(hd - thd) <= 128)) || ((thd < hd) && (abs(hd - thd) >= 128))) {
      k = 1;
    }
    if (k = 1) {
      hd = (hd + turn_rate) + 255;
    }else{
      hd = (hd + 256 - turn_rate) + 255;
    }
  }
  hd = hd + 255;
  if (keepshift) {
    shift = (tthd - hd + 1024) + 255;
  }
  speed = spd / 100 * (max_vel * heat_mult * speedadj);
  xv = sint[hd] * speed;
  yv = cost[hd] * speed;
  if (hd = 0 || hd = 128) {
    xv = 0;
  }
  if (hd = 64 || hd = 192) {
    yv = 0;
  }
  if (xv != 0) {
    ttx = x + xv;
  }else{
    ttx = x;
  }
  if (yv != 0) {
    tty = y + yv;
  }else{
    tty = y;
  }
  if (ttx < 0 || tty < 0 || ttx > 1000 || tty > 1000) {
    ram[8]++;
    tspd = 0;
    if (abs(speed) >= max_vel / 2) {
      damage(n, 1, true);
      spd = 0;
    }
    ttx = x;
    tty = y;
  }
  for(int i = 0; i < num_robots; i++){
    if (i != n && (robot[*i].armor > 0) && (distance(ttx, tty, robot[*i].x, robot[*i].y) < crash_range)) {
      tspd = 0;
      spd = 0;
      ttx = x;
      tty = y;
      robot[*i].tspd = 0;
      robot[*i].spd = 0;
      ram[8]++;
      robot[*i].ram[8]++;
      if (abs(speed) >= max_vel / 2) {
        damage(i, 1, true);
      }
    }
  }
  if (ttx < 0) {
    ttx = 0;
  }
  if (tty < 0) {
    tty = 0;
  }
  if (ttx > 1000) {
    ttx = 1000;
  }
  if (tty > 1000) {
    tty = 1000;
  }
  meters = meters + distance(x, y, ttx, tty);
  if (meters >= MAXINT) {
    meters = meters - MAXINT;
  }
  ram[9] = trunc(meters);
  x = ttx;
  y = tty;

  /* draw robot */

  if (armor < 0) {
    armor = 0;
  }
  if (heat < 0) {
    heat = 0;
  }
  if (graphix) {
    if (armor != larmor) {
      update_armor(n);
    }
    if ((heat / 5) != (lheat / 5)) {
      update_heat(n);
    }
    draw_robot(n);
  }
  lheat = heat;
  larmor = armor;

  cycles_lived++;
}

void do_mine(int n, int m) {
  int i, j, k, l;
  double d, r;
  bool source_alive;
  for(robot[*n].mine[m]){
    if (x >= 0 && x <= 1000 && y >= 0 && y <= 1000 && yield > 0) {
      for (int i = 0; i < num_robots; i++) {
        if (robot[*i].armor > 0 && i != n) {
          d = distance(x, y, robot[*i].x, robot[*i].y);
          if (d <= detect) {
            detonate = true;
          }
        }
        if (detonate) {
          init_missile(x, y, 0, 0, 0, n, mine_circle, false);
          kill_count = 0;
          if (robot[*i].armor > 0) {
            source_alive = true;
          }else{
            source_alive = false;
          }
          for (int i = 0; i < num_robots; i++) {
            if (i != n && robot[*i].armor > 0) {
              k = round(distance(x, y, robot[*i].x, robot[*i].y));
              if (k < yield) {
                damage(i, round(abs(yield - k)), false);
                if ((n >= 0 && n <= num_robots) && i != n) {
                  robot[*n].damage_total += round(abs(yield - k));
                }
              }
            }
          }
          if (kill_count > 0 && source_alive && robot[*n].armor <= 0) {
            kill_count--;
          }
          if (kill_count > 0) {
            robot[*n].kills += kill_count;
            update_lives(n);
          }
          if (graphix) {
            putpixel(round(x * screen_scale) + screen_x, round(y * screen_scale) + screen_y, 0);
            yield = 0;
            x = -1;
            y = -1;
          }else{
            if (graphix && (game_cycle + 1 == 0)) {
              main_viewport;
              setcolor(robot_color(n));
              line(round(x * screen_scale) + screen_x,round(y * screen_scale) + screen_y - 1,
                   round(x * screen_scale) + screen_x,round(y * screen_scale) + screen_y + 1);
              line(round(x * screen_scale) + screen_x + 1,round(y * screen_scale) + screen_y,
                   round(x * screen_scale) + screen_x - 1,round(y * screen_scale) + screen_y);
            }
          }
        }
      }
    }
  }
}

void do_missile(int n) {
  double llx, lly, r, d, dir, xv, yv;
  int i, j, k, l, xx, yy, tx, ty, dd, dam;
  bool source_alive;
  for (missile[n]) {
    if (a == 0) {
      exit();
    }else{
      if (a == 1) {
        if (x < -20 || x > 1020 || y < -20 || y > 1020) {
          a = 0;
        }

        /* move missile */

        llx = lx;
        lly = ly;
        if (a > 0) {
          hd = (hd + 256) + 255;
          xv = sint[hd] * mspd;
          yv = -cost[hd] * mspd;
          x += xv;
          y += yv;
        }

        /* look for hit on a robot */

        k = -1;
        l = MAXINT;
        for (int i = 0; i < num_robots; i++) {
          if (robot[*i].armor > 0 && i != source) {
            d = distance(lx, ly, robot[*i].x, robot[*i].y);
            dir = find_angle(lx, ly, robot[*i].x, robot[*i].y);
            j = (round(dir/pi*128) + 1024) + 255;
            hd = hd + 255;
            xx = round(sint[hd] * d + lx);
            yy = round(-cost[hd] * d + ly);
            r = distance(xx, yy, robot[*i].x, robot[*i].y);
            if (d <= mspd && r < hit_range && round(d) <= 1) {
              k = i;
              l = round(d);
              dd = round(r);
              tx = xx;
              ty = yy;
            }
          }
          if (k >= 0) {
            println("hit a robot!");
            x = tx;
            y = ty;
            a = 2;
            rad = 0;
            lrad = 0;
            if (source >= 0 && source <= num_robots) {
              robot[*source].last_hit = 0;
              robot[*source].hits++;
            }
            for (int i = 0; i < num_robots; i++) {
              dd =round(distance(x, y, robot[*i].x, robot[*i].y));
              if (dd <= hit_range) {
                dam = round(abs(hit_range - dd) * mult);
                if (dam <= 0) {
                  dam = 1;
                }
                kill_count = 0;
                if (robot[*source].armor > 0) {
                  source_alive = true;
                }else{
                  source_alive = false;
                }
                damage(i, dam, false);
                if (source >= 0 && source <= num_robots && i != source) {
                  robot[*source].damage_total += dam;
                }
                if (kill_count > 0 && source_alive && robot[*source].armor <= 0) {
                  kill_count--;
                }
                if (kill_count > 0) {
                  robot[*source].kills += kill_count;
                  update_lives(source);
                }
              }
            }
          }

          /* draw missile */

          if (graphix) {
            main_viewport;
            setcolor(0);
            line(round(llx * screen_scale) + screen_x, round(lly * screen_scale) + screen_y,
                 round(lx * screen_scale) + screen_x, round(ly * screen_scale) + screen_y);
            if (a == 1) {
              if (mult > robot[*source].shotstrength) {
                setcolor(14 + (game_cycle + 1));
              }else{
                setcolor(15);
              }
              line(round(x * screen_scale) + screen_x, round(y * screen_scale) + screen_y,
                   round(lx * screen_scale) + screen_x, round(ly * screen_scale) + screen_y);
            }
          }
          if (a == 2) {
            lrad = rad;
            rad++;
            if (rad > max_rad) {
              a = 0;
            }
            if (graphix) {
              main_viewport;
              setcolor(0);
              circle(round(x * screen_scale) + screen_x, round(y * screen_scale) + screen_y, lrad);
              if (mult > 1) {
                setcolor(14 + (game_cycle + 1));
              }else{
                setcolor(15);
              }
              if (max_rad >= blast_circle) {
                setcolor(14);
              }
              if (max_rad >= mine_circle) {
                setcolor(11);
              }
              if (a > 0) {
                circle(round(x * screen_scale) + screen_x, round(y * screen_scale) + screen_y, rad);
              }
            }
          }
        }
      }
    }
  }
}

short round(double x){
	long temp = x + 0.5;

	return (short)(temp & MAXINT);
}

int main(int argc, char ** argv) {
	init();
	true_main();
	shutdown();

	return EXIT_SUCCESS;
}
