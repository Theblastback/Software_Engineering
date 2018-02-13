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

	// robot[n] -> dereference
	for ( i = 0; i <= MAX_ROBOT_LINES; i++ ) {
		robot[n] -> ltx[i] = 0;
		robot[n] -> tx[i] = 0;
		robot[n] -> lty[i] = 0;
		robot[n] -> ty[i] = 0;
	}

	do {
		robot[n] -> x = rand() % 1000;
		robot[n] -> y = rand() % 1000;
		robot[n] -> dd = 1000;
		for ( i = 0; i <= NUM_ROBOTS; i++ ) {
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

	for ( i = 0; i <= MAX_MINES; i++ ) {
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


// delete_compile_report(

// write_compile_report(


// parse_param(


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
	insanity = false;
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

	if ( !no_gfx )
		graph_mode(true);

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
