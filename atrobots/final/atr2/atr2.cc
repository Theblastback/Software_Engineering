#include "Headers/filelib.h"
#include "Headers/myfile.h"
#include "Headers/atr2func.h"
#include <time.h>
#include <iostream>
#include <cstdint>

// Begin global variable/definition listings
std::string progname = "AT-Robots";
std::string version = "2.11";
std::string cnotice1 = "Copyright 1997 ""99, Ed T. Toton III";
std::string cnotice2 = "All Rights Reserved.";
std::string cnotice3 = "Copyright 2014, William \"amos\" Confer";
std::string main_filename = "atr2";
std::string robot_ext = ".AT2";
std::string locked_ext = ".ATL";
std::string config_ext = ".ATS";
std::string compile_ext = ".CMP";
std::string report_ext = ".REP";

#define PI 3.141592

#define MININT	-32768
#define MAXINT	32767

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
	int16_t	op[MAX_OP + 1];
};

typedef op_rec prog_type;


class config_rec {
public:
	int16_t scanner, weapon, armor, engine, heatsinks, shield, mines;
};

class mine_rec {
public:
	double x, y;
	int16_t detect, yield;
	bool detonate;
};

class robot_rec {
public:
	bool		is_locked;
	int16_t		mem_watch;
	double	x, y, lx, ly, xv, yv, speed, shotstrength, damageadj, speedadj, meters;

	int16_t	hd, thd, lhd, spd, tspd, armor, larmor, heat, lheat, ip, plen, scanarc,
		accuracy, shift, err, delay_left, robot_time_limit, max_time, time_left,
		lshift, arc_count, sonar_count, scanrange, last_damage, last_hit, transponder,
		shutdown, channel, lendarc, endarc, lstartarc, startarc, mines;

	int16_t	tx[MAX_ROBOT_LINES], ltx[MAX_ROBOT_LINES], ty[MAX_ROBOT_LINES], lty[MAX_ROBOT_LINES];
	int32_t	wins, trials, kills, deaths, startkills, shots_fired, match_shots, hits,
		damage_total, cycles_lived, error_count;
	robot_rec();

	config_rec	config;
	std::string	name;
	std::string	fn;
	bool	shields_up, lshields, overburn, keepshift, cooling, won;
	prog_type	code[MAX_CODE + 1];
	int16_t		ram[MAX_RAM + 1];
	mine_rec	mine[MAX_MINES + 1];
	std::ofstream	errorlog;
};

class missile_rec {
public:
	double x, y, lx, ly, mult, mspd;
	int16_t source, a, hd, rad, lrad, max_rad;
};

// Begin global variables
int32_t paramcount;
char **paramstr;

// Robot variables
int16_t num_robots;
typedef robot_rec * robot_ptr;

robot_ptr robot[MAX_ROBOTS + 5]; // Array started at -2, so I shifted it over
missile_rec missile[MAX_MISSILES + 1];

// Compiler variables
std::string	f;
int16_t	numvars, numlabels, maxcode, lock_pos, lock_dat;
std::string	varname[MAX_VARS - 1];
int16_t	varloc[MAX_VARS - 1];
std::string	labelname[MAX_VARS - 1];
int16_t	labelnum[MAX_VARS - 1];
bool	show_source, compile_only;
std::string	lock_code;

// Simulator/graphics variables
bool	bout_over;
int16_t	step_mode;	// 0 = off; for 0 < step_mode <= 9 = # of game cycles per step
int16_t	temp_mode;	// Stores previous step_mode for return to step
int16_t	step_count;	// Step counter used as break flag
bool	step_loop;	// Break flag for stepping

bool	old_shields, insane_missiles, debug_info, windoze, no_gfx, logging_errors,
timing, show_arcs;
int16_t	game_delay, time_slice, insanity, update_timer, max_gx, max_gy, stats_mode;
int	game_limit, game_cycle, matches, played, executed;

// General settings
bool	quit, report, show_cnotice;
int16_t	kill_count, report_type;


// Begin functions

// Functions that need to be declared prior to being written
void prog_error(int16_t, std::string);
void compile(int16_t, std::string);
void setscreen();
void do_robot(int16_t);
void do_mine(int16_t, int16_t);
void do_missile(int16_t);
void create_robot(int16_t, std::string);

void parse_param(std::string s) {
	// err_log << "Begin parse_param" << endl;
	std::cout << "Begin parse_param" << endl;
	fstream f;
	string fn, s1;
	bool found = false;

	s = btrim(ucase(s));
	if (s.compare("") == 0)
		return;
	std::cout << s << ")))))))))))))))))))))))))))))))" << endl;
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
	}

	else if ((s[0] == '/') || (s[0] == '-') || (s[0] == '=')) {
		s1 = rstr(s, s.length() - 1);
		switch (s1[0]) {
		case 'X':
			std::cout << "In case x ------------------------------------------------------" << endl;
			step_mode = value(rstr(s1, s1.length() - 1));
			found = true;
			if (step_mode == 0) {
				step_mode = 1;
				std::cout << "in step if ==============================" << endl;
			}
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
	}
	else if (s[1] == ';') {
		found = true;
	}
	else if ((num_robots < MAX_ROBOTS) && (s.compare("") != 0)) {
		num_robots++;
		create_robot(num_robots, s);
		found = true;
		if (num_robots == MAX_ROBOTS)
			cout << "Maximum number of robots reached." << endl;
	}
	else
		prog_error(10, "");
	;
	if (!found) {
		prog_error(8, s);
	}

	std::cout << "end parse_param" << endl;
	// err_log << "End parse_param" << endl << endl;
}


std::string operand(int16_t n, int16_t m) {
	std::cout << "Begin operand" << endl;
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

	if ((m & 8) > 0)
		s = "[" + s + "]";

	std::cout << "End operand" << endl << endl;

	return s;
}


std::string mnemonic(int16_t n, int16_t m) {
	std::cout << "Begin mnemonic" << endl;
	std::string s = cstr(n);

	if (m == 0)
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
		case 10: s = "RET"; break;
		case 11: s = "CALL"; break;
		case 12: s = "JMP"; break;
		case 13: s = "JLS"; break;
		case 14: s = "JGR"; break;
		case 15: s = "JNE"; break;
		case 16: s = "JE"; break;
		case 17: s = "SWAP"; break;
		case 18: s = "DO"; break;
		case 19: s = "LOOP"; break;
		case 20: s = "CMP"; break;
		case 21: s = "TEST"; break;
		case 22: s = "MOV"; break;
		case 23: s = "LOC"; break;
		case 24: s = "GET"; break;
		case 25: s = "PUT"; break;
		case 26: s = "INT"; break;
		case 27: s = "IPO"; break;
		case 28: s = "OPO"; break;
		case 29: s = "DELAY"; break;
		case 30: s = "PUSH"; break;
		case 31: s = "POP"; break;
		case 32: s = "ERR"; break;
		case 33: s = "INC"; break;
		case 34: s = "DEC"; break;
		case 35: s = "SHL"; break;
		case 36: s = "SHR"; break;
		case 37: s = "ROL"; break;
		case 38: s = "ROR"; break;
		case 39: s = "JZ"; break;
		case 40: s = "JNZ"; break;
		case 41: s = "JGE"; break;
		case 42: s = "JLE"; break;
		case 43: s = "SAL"; break;
		case 44: s = "SAR"; break;
		case 45: s = "NEG"; break;
		case 46: s = "JTL"; break;
		default: s = "XXX";
		}
	else
		s = operand(n, m);


	std::cout << "End mnemonic" << endl << endl;
	return s;
}


int16_t max_shown() {

	switch (stats_mode) {
	case 1:
		return 12;
	case 2:
		return 32;
	default:
		return 6;
	}
}


bool graph_check(int16_t n) {

	bool ok = true;

	if (!graphix || (n < 0) || (n > num_robots) || (n >= max_shown()))
		ok = false;

	return ok;
}

void robot_graph(int16_t n) {
	std::cout << "Begin robot_graph" << endl;
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

	std::cout << "End robot_graph" << endl;
}

void update_armor(int16_t n) {
	std::cout << "Begin update_armor" << endl;
	if (graph_check(n) && (step_mode <= 0)) {
		robot_graph(n);
		if (robot[n]->armor > 0) {
			switch (stats_mode) {
			case 1:
				bar(30, 13, 29 + robot[n]->armor, 18);
				break;
			case 2:
				bar(88, 3, 87 + (robot[n]->armor >> 2), 8);
				break;
			default:
				bar(30, 25, 29 + (robot[n]->armor), 30);
			}
		}
		setfillstyle(DARK_GRAY);
		if (robot[n]->armor < 100) {
			switch (stats_mode) {
			case 1:
				bar(30 + robot[n]->armor, 13, 29 + robot[n]->armor, 30);
				break;
			case 2:
				bar(88, 3, 87 + (robot[n]->armor >> 2), 8);
				break;
			default:
				bar(30, 25, 29 + robot[n]->armor, 30);
			}
		}
	}
	std::cout << "End update_armor" << endl << endl;
}

void update_heat(int16_t n) {
	std::cout << "Begin update heat" << endl;
	if (graph_check(n) && (step_mode <= 0)) {
		robot_graph(n);

		if (robot[n]->heat > 5) {
			switch (stats_mode) {
			case 1:
				bar(30, 23, 29 + (robot[n]->heat / 5), 28);
				break;
			case 2:
				bar(127, 3, 126 + (robot[n]->heat / 20), 8);
				break;
			default:
				bar(30, 35, 29 + (robot[n]->heat / 5), 40);
			}
		}
		setfillstyle(DARK_GRAY);
		if (robot[n]->heat < 500) {
			switch (stats_mode) {
			case 1:
				bar(30 + (robot[n]->heat / 5), 23, 129, 28);
				break;
			case 2:
				bar(127 + (robot[n]->heat / 20), 3, 151, 8);
				break;
			default:
				bar(30 + (robot[n]->heat / 5), 35, 129, 40);
			}
		}
	}
	std::cout << "End update_heat" << endl << endl;
}

void robot_error(int16_t n, int16_t i, std::string ov) {
	std::cout << "Begin robot_error" << endl;
	if (graph_check(n) && (step_mode <= 0)) {
		if (stats_mode == 0) {
			robot_graph(n);
			setfillstyle(BLACK);
			bar(66, 56, 154, 64);
			setcolor(robot_color(n));
			//outtextxy(66, 56, addrear(cstr(i), 7) + hex(i));
			// chirp();
		}
	}

	std::cout << "End robot_error" << endl << endl;
}

void update_lives(int16_t n) {
	std::cout << "Begin update_lives" << endl;
	if (graph_check(n) && (stats_mode == 0)) {
		robot_graph(n);
		setcolor(robot_color(n) - 8);
		setfillstyle(BLACK);
		bar(11, 46, 130, 53);

		//outtextxy(11, 46, "K: ");
		//outtextxy(29, 46, zero_pad(robot[n] -> kills, 4));
		//outtextxy(80, 46, "D :");
		//outtextxy(98, 46, zero_pad(robot[n] -> deaths, 4));
	}

	std::cout << "End update_lives" << endl << endl;
}

void update_cycle_window() {
	// err_log << "Begin update_cycle_window" << endl;
	if (!graphix) {
		std::cout << endl << "Match " << played << "/" << matches << " Cycle: " << zero_pad(game_cycle, 9);
	}
	else {
		viewport(480, 440, 635, 475);
		setfillstyle(BLACK);
		bar(59, 2, 154, 10);
		setcolor(LIGHT_GRAY);
		//outtextxy(75, 3, zero_pad(game_cycle, 9));
		SDL_RenderPresent(renderer_main);
	}

	// err_log << "End update_cycle_window" << endl << endl;
}


// Initialize the entire screen
void setscreen() {
	std::cout << "Begin setscreen" << endl;
	int16_t i;


	if (!graphix)
		return;
	// Window & renderer will be created in init, so no to worry here
	viewport(0, 0, 639, 479); // XXX Legitimate size of window XXX
	box(0, 0, 639, 479);

	stats_mode = 0;
	if ((num_robots >= 0) && (num_robots <= 5)) {
		stats_mode = 0;
	}
	else if ((num_robots >= 6) && (num_robots <= 11)) {
		stats_mode = 1;
	}
	else if ((num_robots >= 12) && (num_robots <= MAX_ROBOTS)) {
		stats_mode = 2;
	}
	else
		stats_mode = 0;


	// Main arena
	hole(4, 4, 475, 475);

	// Cycle window
	viewport(480, 480, 635, 475);
	hole(0, 0, 155, 45);
	setcolor(LIGHT_GRAY);
	//	//outtextxy(3, 3, "FreeMem: " + cstr(memavail);  XXX Not possible. Cannot get amount of bytes available in heap storage (that's what memavail is)
	//outtextxy(3, 13, "Cycle:    ");
	//outtextxy(3, 23, "Limit     " + zero_pad(game_limit, 9));
	//outtextxy(2, 33, "Match:    " + cstr(played) + "/" + cstr(matches));
	update_cycle_window();

	// Robot windows
	for (i = 0; i <= MAX_ROBOTS; i++) {

		if (i < max_shown()) {

			robot_graph(i);

			hole(0, 0, max_gx, max_gy);

			if (i <= num_robots) {
				setcolor(robot_color(i));
				//outtextxy(3, 2, base_name( no_path(robot[i] -> fn)) );
				switch (stats_mode) {
				case 1:
					//outtextxy(3, 12, " A:");
					//outtextxy(2, 22, " H:");
					break;
				case 2:
					setcolor(robot_color(i) & 7);
					//outtextxy(80, 2, "A");
					//outtextxy(118, 2, "H");
					break;
				default:
					//outtextxy(3, 24, " A:");
					//outtextxy(3, 34, " H:");
					break;
				}


				setcolor(robot_color(i));

				if (stats_mode <= 1) {

					//outtextxy(80, 2, "Wins: ");
					//outtextxy(122, 2, zero_pad(robot[i] -> wins, 4));
				}

				if (stats_mode == 0) {

					//outtextxy(3, 56, " Error:");
					setcolor(robot_color(i) & 7);
					//outtextxy(3, 12, robot[i] -> name);
					setcolor(DARK_GRAY);
					//outtextxy(66, 56, "None");
				}

				robot[i]->lx = 1000 - robot[i]->x;

				robot[i]->ly = 1000 - robot[i]->y;

				update_armor(i);

				update_heat(i);

				update_lives(i);

			}
			else {
				setfillstyle(DARK_GRAY); // XXX This would come out checkered. New color scheme may be needed
				bar(1, 1, max_gx - 1, max_gy - 1);
			}
		}
	}
	std::cout << "End setscreen" << endl << endl;
}


void graph_mode(bool on) {
	std::cout << "Begin graph_mode" << endl;
	if (on && !graphix) {
		// Replace Graph_VGA function with sdl
		setscreen();
		graphix = true;
	}
	else if (!on && graphix) {
		// Turn graphics off
		SDL_SetRenderDrawColor(renderer_main, 0x0, 0x0, 0x0, 0xff);
		SDL_RenderClear(renderer_main);
		// Blacks out entire screen. However, to retain portability, we need the window still.
		// Without the window, key presses cannot be detected

		graphix = false;
	}

	std::cout << "End graph_mode" << endl << endl;
}


void print_code(int16_t n, int16_t p) {
	std::cout << "Begin print_code" << endl << endl;
	int16_t i;

	std::cout << (hex(p) + ": ");
	for (i = 0; i <= MAX_OP; i++)
		std::cout << zero_pad(robot[n]->code[p].op[i], 5) << " ";

	std::cout << " =  ";
	for (i = 0; i <= MAX_OP; i++)
		std::cout << hex(robot[n]->code[p].op[i]) << "h ";

	std::cout << "End print_code" << endl << endl;
}


void check_plen(int16_t plen) {
	std::cout << "Begin check_plen" << endl;
	if (plen > MAX_CODE)
		prog_error(16, "\nMaximum progrm length exceeded, (Limit: " + cstr(MAX_CODE + 1) + " compiled lines)");

	std::cout << "End check_plen" << endl << endl;
}


void robot_config(int16_t n) {
	std::cout << "Begin robot_config" << endl;

	// Doing case statements like this to reduce line count
	switch (robot[n]->config.scanner) {
	case 5: robot[n]->scanrange = 1500; break;
	case 4: robot[n]->scanrange = 1000; break;
	case 3: robot[n]->scanrange = 700; break;
	case 2: robot[n]->scanrange = 500; break;
	case 1: robot[n]->scanrange = 350; break;
	default: robot[n]->scanrange = 250;
	}

	switch (robot[n]->config.weapon) {
	case 5: robot[n]->shotstrength = 1.5; break;
	case 4: robot[n]->shotstrength = 1.35; break;
	case 3: robot[n]->shotstrength = 1.2; break;
	case 2: robot[n]->shotstrength = 1; break;
	case 1: robot[n]->shotstrength = 0.8; break;
	default: robot[n]->shotstrength = 0.5;
	}

	switch (robot[n]->config.armor) {
	case 5:
		robot[n]->damageadj = 0.66;
		robot[n]->speedadj = 0.66;
		break;
	case 4:
		robot[n]->damageadj = 0.77;
		robot[n]->speedadj = 0.75;
		break;
	case 3:
		robot[n]->damageadj = 0.83;
		robot[n]->speedadj = 0.85;
		break;
	case 2:
		robot[n]->damageadj = 1;
		robot[n]->speedadj = 1;
		break;
	case 1:
		robot[n]->damageadj = 1.5;
		robot[n]->speedadj = 1.2;
		break;
	default:
		robot[n]->damageadj = 2;
		robot[n]->speedadj = 1.33;
	}

	switch (robot[n]->config.engine) {
	case 5: robot[n]->speedadj *= 1.5; break;
	case 4: robot[n]->speedadj *= 1.35; break;
	case 3: robot[n]->speedadj *= 1.2; break;
	case 2: robot[n]->speedadj *= 1; break;
	case 1: robot[n]->speedadj *= 0.8; break;
	default: robot[n]->speedadj *= 0.5;
	}
	// Heatsinks are handled seperately
	switch (robot[n]->config.mines) {
	case 5: robot[n]->mines = 24; break;
	case 4: robot[n]->mines = 16; break;
	case 3: robot[n]->mines = 10; break;
	case 2: robot[n]->mines = 6; break;
	case 1: robot[n]->mines = 4; break;
	default:
		robot[n]->mines = 2;
		robot[n]->config.mines = 0;
	}

	robot[n]->shields_up = false;
	if ((robot[n]->config.shield < 3) || (robot[n]->config.shield > 5))
		robot[n]->config.shield = 0;
	if ((robot[n]->config.heatsinks < 0) || (robot[n]->config.heatsinks > 5))
		robot[n]->config.heatsinks = 0;

	std::cout << "End robot_config" << endl << endl;
}


void reset_software(int16_t n) {
	std::cout << "Begin reset_software" << endl;
	int16_t i;

	for (i = 0; i <= MAX_RAM; i++)
		robot[n]->ram[i] = 0;

	robot[n]->ram[71] = 768;
	robot[n]->thd = robot[n]->hd;
	robot[n]->tspd = 0;
	robot[n]->scanarc = 8;
	robot[n]->shift = 0;
	robot[n]->err = 0;
	robot[n]->overburn = false;
	robot[n]->keepshift = false;
	robot[n]->ip = 0;
	robot[n]->accuracy = 0;
	robot[n]->meters = 0;
	robot[n]->delay_left = 0;
	robot[n]->time_left = 0;
	robot[n]->shields_up = false;

	std::cout << "End reset_software" << endl << endl;
}


void reset_hardware(int16_t n) {
	std::cout << "Begin reset_hardware" << endl;
	int16_t i;
	double d, dd;

	// robot[n] -> dereference
	for (i = 0; i < MAX_ROBOT_LINES; i++) {
		robot[n]->ltx[i] = 0;
		robot[n]->tx[i] = 0;
		robot[n]->lty[i] = 0;
		robot[n]->ty[i] = 0;
	}

	do {
		robot[n]->x = rand() % 1000;
		robot[n]->y = rand() % 1000;
		dd = 1000;

		for (i = 0; i < num_robots; i++) {

			if (robot[i]->x < 0)
				robot[i]->x = 0;

			if (robot[i]->x > 1000)
				robot[i]->x = 1000;

			if (robot[i]->y < 0)
				robot[i]->y = 0;

			if (robot[i]->y > 1000)
				robot[i]->y = 1000;

			d = _distance(robot[n]->x, robot[n]->y, robot[i]->x, robot[i]->y);

			if ((robot[i]->armor > 0) && (i != n) && (d < dd))
				dd = d;

		}
	} while (dd < 32);

	for (i = 0; i < MAX_MINES; i++) {
		robot[n]->mine[i].x = 1;
		robot[n]->mine[i].y = -1;
		robot[n]->mine[i].yield = 0;
		robot[n]->mine[i].detonate = false;
		robot[n]->mine[i].detect = 0;
	}
	robot[n]->lx = -1;
	robot[n]->ly = -1;
	robot[n]->hd = rand() % 256;
	robot[n]->shift = 0;
	robot[n]->lhd = robot[n]->hd;
	robot[n]->lshift = robot[n]->shift + 1;
	robot[n]->spd = 0;
	robot[n]->speed = 0;
	robot[n]->cooling = false;
	robot[n]->armor = 100;
	robot[n]->larmor = 0;
	robot[n]->heat = 0;
	robot[n]->lheat = 0;
	robot[n]->match_shots = 0;
	robot[n]->won = false;
	robot[n]->last_damage = 0;
	robot[n]->last_hit = 0;
	robot[n]->transponder = n + 1;
	robot[n]->meters = 0;
	robot[n]->shutdown = 400;
	robot[n]->shields_up = false;
	robot[n]->channel = robot[n]->transponder;
	robot[n]->startkills = robot[n]->kills;

	robot_config(n);

	std::cout << "End reset_hardware" << endl << endl;
}

void init_robot(int16_t n) {
	std::cout << "Begin init_robot" << endl;
	int16_t i, k;

	robot[n]->wins = 0;
	robot[n]->trials = 0;
	robot[n]->kills = 0;
	robot[n]->deaths = 0;
	robot[n]->shots_fired = 0;
	robot[n]->match_shots = 0;
	robot[n]->hits = 0;
	robot[n]->damage_total = 0;
	robot[n]->cycles_lived = 0;
	robot[n]->error_count = 0;
	robot[n]->plen = 0;
	robot[n]->max_time = 0;
	robot[n]->name = "";
	robot[n]->fn = "";
	robot[n]->speed = 0;
	robot[n]->arc_count = 0;
	robot[n]->sonar_count = 0;
	robot[n]->robot_time_limit = 0;
	robot[n]->scanrange = 1500;
	robot[n]->shotstrength = 1;
	robot[n]->damageadj = 1;
	robot[n]->speedadj = 1;
	robot[n]->mines = 0;

	robot[n]->config.scanner = 5;
	robot[n]->config.weapon = 2;
	robot[n]->config.armor = 2;
	robot[n]->config.engine = 2;
	robot[n]->config.heatsinks = 1;
	robot[n]->config.shield = 0;
	robot[n]->config.mines = 0;

	for (i = 0; i <= MAX_RAM; i++)
		robot[n]->ram[i] = 0;

	robot[n]->ram[71] = 768;

	for (i = 0; i <= MAX_CODE; i++)
		for (k = 0; k <= MAX_OP; k++)
			robot[n]->code[i].op[k] = 0;

	reset_hardware(n);
	reset_software(n);

	std::cout << "End init_robot" << endl << endl;
}


void create_robot(int16_t n, std::string filename) {
	std::cout << "Begin create_robot" << endl;

	int16_t i, k;

	for (i = 0; i <= MAX_ROBOTS + 4; i++)
		robot[i] = new robot_rec();


	init_robot(n);

	filename = ucase(btrim(filename));

	if (filename.compare(base_name(filename)) == 0) {

		if (filename[0] == '?') {
			filename = filename + locked_ext;
		}
		else {
			filename = filename + robot_ext;
		}
	}

	if (filename[0] == '?') {
		// err_log << "-> Detected question mark" << endl;
		filename = rstr(filename, filename.length() - 1);
	}

	// err_log << "Prior to robot[n] -> fn being created" << endl;
	robot[n]->fn = base_name(no_path(filename));

	// err_log << "\trobot -> fn created" << endl;
	compile(n, filename);


	robot_config(n);



	k = robot[n]->config.scanner + robot[n]->config.armor + robot[n]->config.weapon +
		robot[n]->config.engine + robot[n]->config.heatsinks + robot[n]->config.shield + robot[n]->config.mines;


	if (k > MAX_CONFIG_POINTS)
		prog_error(21, cstr(k) + "/" + cstr(MAX_CONFIG_POINTS));

	std::cout << "End create_robot" << endl << endl;
}


void shutdown() {
	std::cout << "Begin shutdown" << endl;
	int16_t i;

	graph_mode(false);
	if (show_cnotice) {
		textcolor(CYAN);
		std::cout << progname << " " << version << " ";
		std::cout << cnotice1 << endl << cnotice2 << endl << cnotice3 << endl;
	}

	textcolor(LIGHT_GRAY);
	std::cout << endl;
	if (logging_errors) {
		for (i = 0; i <= num_robots; i++) {
			std::cout << "Robot error-log created: " << base_name(robot[i]->fn) << ".ERR" << endl;
			robot[i]->errorlog.close();
		}
	}

	for (i = 0; i <= MAX_ROBOTS + 4; i++)
		delete robot[i];

	SDL_DestroyRenderer(renderer_main);
	SDL_DestroyWindow(window_main);

	TTF_Quit();
	SDL_Quit();

	std::cout << "End shutdown" << endl;
	std::exit(EXIT_SUCCESS);
}


void init() {
	std::cout << "Begin init" << endl;

	int16_t i;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cerr << "ERROR: Could not init SDL: " << SDL_GetError();
		std::exit(EXIT_FAILURE);
	}


	if (TTF_Init() == -1) {
		std::cout << "ERROR: Could not init TTF: " << TTF_GetError();
		SDL_Quit();
	}

	// Load text type here *sdl_ttf critical*
	text_type = TTF_OpenFont("FUTURE.TTF", 8);

	SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_OPENGL, &window_main, &renderer_main);


	if (debugging_compiler || compile_by_line || show_code) {
		std::cout << "!!! Warning !!! Compiler Debugging enabled !!!";
		readkey();
		std::cout << endl;
	}

	step_mode = 1;
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
	//randomize();

	num_robots = -1;
	game_limit = 100000;
	game_cycle = 0;
	game_delay = DEFAULT_DELAY;
	time_slice = DEFAULT_SLICE;

	for (i = 0; i <= MAX_MISSILES; i++) {
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
	reg_num = to_string(0xffff);
	check_registration();
	std::cout << endl;
	textcolor(CYAN);
	std::cout << progname << " " << version << " ";
	std::cout << cnotice1 << endl << cnotice2 << endl;
	textcolor(LIGHT_GRAY);
	if (!registered) {
		textcolor(RED);
		std::cout << "Unregistered version" << endl;
		textcolor(LIGHT_GRAY);
	}
	else
		std::cout << "Registered to: " << reg_name;



	// delete_compile_report();

	std::string tmp;

	if (paramcount > 0) {

		for (i = 1; i <= paramcount - 1; i++) {

			tmp = paramstr[i]; // Arguement is paramstr. It is effectively argv, but global

			parse_param(btrim(ucase(tmp)));

			if (i == paramcount)
				break;
		}
	}
	else
		prog_error(5, "");


	temp_mode = step_mode;

	if (logging_errors)
		for (i = 0; i <= num_robots; i++)
			robot[i]->errorlog.open(base_name(robot[i]->fn) + ".ERR");

	if (compile_only)
		//	write_compile_report();
		if (num_robots < 1)
			prog_error(4, "");

	if (!no_gfx) // If there are graphics
		graph_mode(true);	// Start in graph mode

	if (matches > 100000)
		matches = 100000;
	if (matches < 1)
		matches = 1;
	if (game_delay > 1000)
		game_delay = 1000;
	if (game_delay < 0)
		game_delay = 0;
	if (time_slice > 100)
		time_slice = 100;
	if (time_slice < 1)
		time_slice = 1;
	if (game_limit < 0)
		game_limit = 0;
	if (game_limit > 100000)
		game_limit = 100000;
	if (maxcode < 1)
		maxcode = 1; // 2 lines of max code. 0 based
	if (maxcode > MAX_CODE)
		maxcode = MAX_CODE;

	for (i = num_robots + 1; i <= MAX_ROBOTS + 4; i++)
		robot[i] = robot[0];
	std::cout << "End init " << reg_name;
	// err_log << "End init" << endl << endl;
}



void draw_robot(int16_t n) {
	std::cout << "Begin draw robot" << endl;
	int16_t i, t;
	double xx, yy;

	if ((n < 0) || (n > num_robots))
		return;

	if (robot[n]->x > 1000)
		robot[n]->x = 1000;
	if (robot[n]->y > 1000)
		robot[n]->y = 1000;
	if (robot[n]->x < 0)
		robot[n]->x = 0;
	if (robot[n]->y < 0)
		robot[n]->y = 0;

	// Set up for draw
	xx = (robot[n]->x * SCREEN_SCALE) + SCREEN_X;

	yy = (robot[n]->y * SCREEN_SCALE) + SCREEN_Y;

	robot[n]->hd = (robot[n]->hd + 1024) & 255;

	robot[n]->tx[0] = int((xx + sint[robot[n]->hd] * 5) + 0.5);

	robot[n]->ty[0] = int ((yy - cost[robot[n]->hd] * 5) + 0.5);

	robot[n]->tx[1] = int ((xx + sint[(robot[n]->hd + 0x68) & 255] * ROBOT_SCALE) + 0.5);
	robot[n]->ty[1] = int ((yy - cost[(robot[n]->hd + 0x68) & 255] * ROBOT_SCALE) + 0.5);

	robot[n]->tx[2] = int((xx + sint[(robot[n]->hd + 0x98) & 255] * ROBOT_SCALE) + 0.5);
	robot[n]->ty[2] = int((yy - cost[(robot[n]->hd + 0x98) & 255] * ROBOT_SCALE) + 0.5);

	t = (robot[n]->hd + (robot[n]->shift & 255) + 1024) & 255;

	robot[n]->tx[3] = int ( xx + 0.5);
	robot[n]->ty[3] = int (yy + 0.5);


	robot[n]->tx[4] = int((xx + sint[t] * ROBOT_SCALE * 0.8) + 0.5);
	robot[n]->ty[4] = int((yy - cost[t] * ROBOT_SCALE * 0.8) + 0.5);


	robot[n]->tx[5] = int((xx + sint[(t + robot[n]->scanarc + 1024) & 255] * robot[n]->scanrange * SCREEN_SCALE) + 0.5);
	robot[n]->ty[5] = int((yy - cost[(t + robot[n]->scanarc + 1024) & 255] * robot[n]->scanrange * SCREEN_SCALE) + 0.5);

	robot[n]->tx[6] = int((xx + sint[(t - robot[n]->scanarc + 1024) & 255] * robot[n]->scanrange * SCREEN_SCALE) + 0.5);
	robot[n]->ty[6] = int((yy - cost[(t - robot[n]->scanarc + 1024) & 255] * robot[n]->scanrange * SCREEN_SCALE) + 0.5);


	robot[n]->startarc = int((((265 - ((t + robot[n]->scanarc) & 255)) / 256 * 360) + 90) + 0.5);
	robot[n]->endarc = int((((265 - ((t - robot[n]->scanarc) & 255)) / 256 * 360) + 90) + 0.5);

	if (graphix) {
		main_viewport();
		setcolor(0);
		if (robot[n]->lshields)
			circle(robot[n]->ltx[3], robot[n]->lty[3], ROBOT_SCALE);

		if (robot[n]->arc_count > 0) {
			line(robot[n]->ltx[3], robot[n]->lty[3], robot[n]->ltx[5], robot[n]->lty[5]);
			line(robot[n]->ltx[3], robot[n]->lty[3], robot[n]->ltx[6], robot[n]->lty[6]);
			if (robot[n]->scanrange < 1500)
				arc(robot[n]->ltx[3], robot[n]->lty[3], robot[n]->lstartarc, robot[n]->lendarc, (robot[n]->scanrange * SCREEN_SCALE) + 0.5);
		}

		if (robot[n]->sonar_count > 0)
			circle(robot[n]->ltx[3], robot[n]->lty[3], (robot[n]->scanrange * SCREEN_SCALE) + 0.5);

		if (robot[n]->armor > 0) { // Only erases bot if bot is alive
			line(robot[n]->ltx[0], robot[n]->lty[0], robot[n]->ltx[1], robot[n]->lty[1]);
			line(robot[n]->ltx[1], robot[n]->lty[1], robot[n]->ltx[2], robot[n]->lty[2]);
			line(robot[n]->ltx[2], robot[n]->lty[2], robot[n]->ltx[0], robot[n]->lty[0]);
			line(robot[n]->ltx[3], robot[n]->lty[3], robot[n]->ltx[4], robot[n]->lty[4]);
		}
	}

	if (robot[n]->armor > 0) { // If bot is alive, redraw
		if (robot[n]->arc_count > 0)
			(robot[n]->arc_count)--;
		if (robot[n]->sonar_count > 0)
			(robot[n]->sonar_count)--;
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

	robot[n]->lx = robot[n]->x;
	robot[n]->ly = robot[n]->y;
	robot[n]->lhd = robot[n]->hd;
	robot[n]->lshift = robot[n]->shift;
	robot[n]->lshields = robot[n]->shields_up;

	for (i = 0; i < MAX_ROBOT_LINES; i++) {
		robot[n]->ltx[i] = robot[n]->tx[i];
		robot[n]->lty[i] = robot[n]->ty[i];
	}

	robot[n]->lstartarc = robot[n]->startarc;
	robot[n]->lendarc = robot[n]->endarc;

	std::cout << "End draw_robot" << endl << endl;
}


// The direct memory access from ram has been replaced with a psuedo ram array
int16_t get_from_ram(int16_t n, int16_t i, int16_t j) {
	// err_log << "Begin get_from_ram" << endl;
	int16_t  k, l;

	if ((i < 0) || (i >(MAX_RAM) +(((MAX_CODE + 1) << 3) - 1))) {
		k = 0;
		robot_error(n, 4, cstr(i));
	}
	else {
		if (i < MAX_RAM)
			k = robot[n]->ram[i];
		else {
			l = i - MAX_RAM - 1;
			k = robot[n]->code[l << 2].op[l & 3];
		}
	}

	// err_log << "End get_from_ram" << endl << endl;

	return k;
}


int16_t get_val(int16_t n, int16_t c, int16_t o) {
	// err_log << "Begin get_val" << endl;
	int16_t i, j, k;

	k = 0;
	j = (robot[n]->code[c].op[MAX_OP] << (4 * 0)) & 15;
	i = robot[n]->code[c].op[o];

	if ((j & 7) == 1)
		k = get_from_ram(n, i, j);
	else
		k = i;
	if ((j & 8) > 0)
		k = get_from_ram(n, k, j);

	// err_log << "End get_val" << endl << endl;
	return k;
}

void put_val(int16_t n, int16_t c, int16_t o, int16_t v) {
	// err_log << "Begin put_val" << endl;
	int16_t i, j;
	i = 0; j = 0;

	j = (robot[n]->code[c].op[MAX_OP] >> (4 * o)) & 15;
	i = robot[n]->code[c].op[o];
	if ((j & 7) == 1) {
		if ((i < 0) || (i > MAX_RAM))
			robot_error(n, 4, cstr(i));
		else {
			if ((j & 8) > 0) {
				i = robot[n]->ram[i];
				if ((i < 0) || (i > MAX_RAM))
					robot_error(n, 4, cstr(i));
				else
					robot[n]->ram[i] = v;
			}
			else
				robot[n]->ram[i] = v;
		}
	}
	else
		robot_error(n, 3, "");

	// err_log << "End put_val" << endl << endl;
}

void push(int16_t n, int16_t v) {
	// err_log << "Begin push" << endl;
	if ((robot[n]->ram[71] >= STACK_BASE) && (robot[n]->ram[71] < (STACK_BASE + STACK_SIZE))) {
		robot[n]->ram[robot[n]->ram[71]] = v;
		(robot[n]->ram[71])++;
	}
	else
		robot_error(n, 1, cstr(robot[n]->ram[71]));

	// err_log << "End push" << endl << endl;
}

int16_t pop(int16_t n) {
	// err_log << "Begin pop" << endl;
	int16_t k;
	if ((robot[n]->ram[71] > STACK_BASE) && (robot[n]->ram[71] <= (STACK_BASE + STACK_SIZE))) {
		(robot[n]->ram[71])--;
		k = robot[n]->ram[robot[n]->ram[71]];
	}
	else
		robot_error(n, 5, cstr(robot[n]->ram[71]));

	// err_log << "End pop" << endl << endl;
	return k;
}


int16_t find_label(int16_t n, int16_t l, int16_t m) {
	// err_log << "Begin find_label" << endl;
	int16_t i, j, k;

	k = -1;
	if (m == 3)
		robot_error(n, 9, "");
	else
		if (m == 4)
			k = l;
		else
			for (i = robot[n]->plen; i >= 0; i--) {
				j = robot[n]->code[i].op[MAX_OP] & 15;
				if ((j == 2) && (robot[n]->code[i].op[0] == l))
					k = i;
			}

	// err_log << "End find_label" << endl << endl;
	return k;
}


void init_mine(int16_t n, int16_t detectrange, int16_t size) {
	// err_log << "Begin init_mine" << endl;
	int16_t i, k;

	k = -1;
	for (i = 0; i <= MAX_MINES; i++) {
		if (((robot[n]->mine[i].x < 0) || (robot[n]->mine[i].x > 1000) || (robot[n]->mine[i].y < 0) || (robot[n]->mine[i].y > 1000) ||
			(robot[n]->mine[i].yield <= 0)) && (k < 0)) {
			k = i;
		}
	}
	if (k >= 0) {
		robot[n]->mine[k].x = robot[n]->x;
		robot[n]->mine[k].y = robot[n]->y;
		robot[n]->mine[k].detect = detectrange;
		robot[n]->mine[k].yield = size;
		robot[n]->mine[k].detonate = false;
		// click(); XXX Sound function. Uncomment later
	}

	// err_log << "End init_mine" << endl << endl;
}

int16_t count_missiles() {
	// err_log << "Begin count_missiles" << endl;
	int16_t i, k;

	k = 0;
	for (i = 0; i <= MAX_MISSILES; i++) {
		if (missile[i].a > 0)
			k++;
	}

	// err_log << "End count_missiles" << endl << endl;
	return k;
}


void init_missile(double xx, double yy, double xxv, double yyx, int16_t dir, int16_t s, int16_t blast, bool ob) {
	// err_log << "Begin init_missile" << endl;
	int16_t i, k;
	double m;

	k = -1;
	// click(); XXX Sound function. Uncomment later
	for (i = MAX_MISSILES; i >= 0; i--)
		if (missile[i].a == 0)
			k = i;

	if (k >= 0) {
		missile[k].source = s;
		missile[k].x = xx;
		missile[k].lx = missile[k].x;
		missile[k].y = yy;
		missile[k].ly = missile[k].y;
		missile[k].rad = 0;
		missile[k].lrad = 0;

		if (ob)
			missile[k].mult = 1.25;
		else
			missile[k].mult = 1;

		if (blast > 0) {
			missile[k].max_rad = blast;
			missile[k].a = 2;
		}
		else {
			if ((s >= 0) && (s <= num_robots))
				missile[k].mult = missile[k].mult * (robot[s]->shotstrength);

			m = missile[k].mult;
			if (ob)
				m = m + 0.25;

			missile[k].mspd = MISSILE_SPD * missile[k].mult;
			if (insane_missiles)
				missile[k].mspd = 100 + (50 * insanity) * missile[k].mult;
			if ((s >= 0) && (s <= num_robots)) {
				robot[s]-> heat += int((20 * m));
				(robot[s]->shots_fired)++;
				(robot[s]->match_shots)++;
			}
		}
		missile[k].a = 1;
		missile[k].hd = dir;
		missile[k].max_rad = MIS_RADIUS;

		/* XXX Debug code here

		*/
	}

	// err_log << "End init_missile" << endl << endl;
}

void damage(int16_t n, int16_t d, bool physical) {
	// err_log << "Begin damage" << endl;
	int16_t i, k, h, dd;
	double m;

	if ((n < 0) || (n > num_robots) || (robot[n]->armor <= 0))
		return;
	if (robot[n]->config.shield < 3)
		robot[n]->shields_up = false;

	h = 0;
	if ((robot[n]->shields_up) && (!physical)) {
		dd = d;
		if ((old_shields) && (robot[n]->config.shield >= 3)) {
			d = 0;
			h = 0;
		}
		else {
			switch (robot[n]->config.shield) {
			case 3:
				d = int ((dd * 2 / 3) + 0.5);
				if (d < 1)
					d = 1;
				h = int ((dd * 2 / 3) + 0.5);
				break;
			case 4:
				h = dd / 2;
				d = dd - h;
				break;
			case 5:
				d = int((dd * 1 / 3) + 0.5);
				if (d < 1)
					d = 1;
				h = int ((dd * 1 / 3) + 0.5);
				if (h < 1)
					h = 1;
				break;
			}
		}
	}
	if (d < 0)
		d = 0;
	/* XXX Debug code here


	*/
	if (d > 0) {
		d = int ((d * robot[n]->damageadj) + 0.5);
		if (d < 1)
			d = 1;
	}
	robot[n]->armor -= d;
	robot[n]->heat += h;
	robot[n]->last_damage = 0;

	if (robot[n]->armor <= 0) {
		robot[n]->armor = 0;
		update_armor(n);
		robot[n]->heat = 500;
		update_heat(n);

		robot[n]->armor = 0;
		kill_count++;
		(robot[n]->deaths)++;
		update_lives(n);
		if (graphix && timing)
			time_delay(10);

		draw_robot(n);
		robot[n]->heat = 0;
		update_heat(n);
		init_missile(robot[n]->x, robot[n]->y, 0, 0, 0, n, BLAST_CIRCLE, false);
		if (robot[n]->overburn)
			m = 1.3;
		else
			m = 1;

		for (i = 0; i <= num_robots; i++) {
			if ((i != n) && (robot[i]->armor > 0)) {
				k = int((_distance(robot[n]->x, robot[n]->y, robot[i]->x, robot[i]->y)) + 0.5);
				if (k < BLAST_RADIUS)
					damage(i, (abs(BLAST_RADIUS - k) * m) + 0.5, false);
			}
		}
	}
	// err_log << "End damage" << endl;
}


int16_t scan(int16_t n) {
	// err_log << "Begin scan" << endl;
	double r, d, acc;
	int16_t dir, range, i, j, k, nn, xx, yy, sign;

	nn = -1;
	range = MAXINT;
	if ((n < 0) || (n > num_robots))
		return 0;

	if (robot[n]->scanarc < 0)
		robot[n]->scanarc = 0;

	robot[n]->accuracy = 0;
	nn = -1;
	dir = (robot[n]->shift + robot[n]->hd) & 255;
	// XXX Debug code here

	for (i = 0; i <= num_robots; i++) {
		if ((i != n) && (robot[i]->armor > 0)) {
			j = find_anglei(robot[n]->x, robot[n]->y, robot[i]->x, robot[i]->y);
			d = _distance(robot[n]->x, robot[n]->y, robot[i]->x, robot[i]->y);
			k = int (d + 0.5);
			if ((k < range) && (k <= robot[n]->scanrange) && ((abs(j - dir) <= abs(robot[n]->scanarc)) || (abs(j - dir) >= (256 - abs(robot[n]->scanarc))))) {

				dir = (dir + 1024) & 255;
				xx = int ((sint[dir] * d + robot[n]->x) + 0.5);
				yy = int (((-1 * cost[dir])* d + robot[n]->y) + 0.5);
				r = _distance(xx, yy, robot[i]->x, robot[i]->y);
				// XXX Debug code here
				if ((robot[n]->scanarc > 0) || (r < (HIT_RANGE - 2))) {
					range = k;
					robot[n]->accuracy = 0;
					if (robot[n]->scanarc > 0) {
						j = (j + 1024) & 255;
						dir = (dir + 1024) & 255;
						if (j < dir)
							sign = -1;
						if (j > dir)
							sign = 1;
						if ((j > 190) && (dir < 66)) {
							dir = dir + 256;
							sign = -1;
						}
						if ((dir > 190) && (j < 66)) {
							j = j + 256;
							sign = 1;
						}
						acc = abs(j - dir) / robot[n]->scanarc * 2;
						if (sign < 0)
							robot[n]->accuracy = -1 * abs((int)(acc + 0.5));
						else
							robot[n]->accuracy = abs((int)(acc + 0.5));

						if (robot[n]->accuracy > 2)
							robot[n]->accuracy = 2;
						if (robot[n]->accuracy < -2)
							robot[n]->accuracy = -2;
					}
					// XXX Debug code here

				}
			}
		}
		if ((nn >= 0) && (nn <= num_robots)) {
			robot[n]->ram[5] = robot[nn]->transponder;
			robot[n]->ram[6] = (robot[nn]->hd - (robot[n]->hd + robot[n]->shift) + 1024) & 255;
			robot[n]->ram[7] = robot[nn]->spd;
			robot[n]->ram[13] = int ((robot[nn]->speed * 100) + 0.5);
		}
	}

	// err_log << "End scan" << endl << endl;
	return range;
}


void com_transmit(int16_t n, int16_t chan, int16_t data) {
	// err_log << "Begin com_transmit" << endl;
	int16_t i;

	for (i = 0; i <= num_robots; i++) {
		if ((robot[i]->armor > 0) && (i != n) && (robot[i]->channel == chan)) {
			if ((robot[i]->ram[10] < 0) || (robot[i]->ram[10] > MAX_QUEUE))
				robot[i]->ram[10] = 0;
			if ((robot[i]->ram[11] < 0) || (robot[i]->ram[11] > MAX_QUEUE))
				robot[i]->ram[11] = 0;
			robot[i]->ram[robot[i]->ram[11] + COM_QUEUE] = data;
			(robot[i]->ram[11])++;
			if (robot[i]->ram[11] > MAX_QUEUE)
				robot[i]->ram[11] = 0;
			if (robot[i]->ram[11] == robot[i]->ram[10])
				(robot[i]->ram[10])++;
			if (robot[i]->ram[10] > MAX_QUEUE)
				robot[i]->ram[10] = 0;
		}
	}
	// err_log << "End com_trasmit" << endl << endl;
}

int16_t com_receive(int16_t i) {
	// err_log << "Begin com_receive" << endl;
	int16_t k;

	if (robot[i]->ram[10] != robot[i]->ram[11]) {
		if ((robot[i]->ram[10] < 0) || (robot[i]->ram[10] > MAX_QUEUE))
			robot[i]->ram[10] = 0;
		if ((robot[i]->ram[11] < 0) || (robot[i]->ram[11] > MAX_QUEUE))
			robot[i]->ram[11] = 0;

		k = robot[i]->ram[robot[i]->ram[10] + COM_QUEUE];
		(robot[i]->ram[10])++;

		if (robot[i]->ram[10] > MAX_QUEUE)
			robot[i]->ram[10] = 0;
	}
	else
		robot_error(i, 12, "");

	// err_log << "End com_receive" << endl << endl;
	return k;
}

int16_t in_port(int16_t n, int16_t p, int16_t *time_used) {
	// err_log << "Begin in_port" << endl;
	int16_t v, i, j, k, l, nn;

	v = 0;

	switch (p) {
	case 1: v = robot[n]->spd; break;
	case 2: v = robot[n]->heat; break;
	case 3: v = robot[n]->hd; break;
	case 4: v = robot[n]->shift; break;
	case 5: v = (robot[n]->shift + robot[n]->hd) & 255; break;
	case 6: v = robot[n]->armor;
	case 7:
		v = scan(n);
		*time_used += 1;
		if (show_arcs)
			robot[n]->arc_count = 2;
		break;
	case 8:
		v = robot[n]->accuracy;
		*time_used += 1;
		break;
	case 9: // This got mixed with case 16. Check again
		nn = -1;
		*time_used += 3;
		k = MAXINT;
		for (i = 0; i <= num_robots; i++) {
			j = int( _distance(robot[n]->x, robot[n]->y, robot[i]->x, robot[i]->y) + 0.5);
			if ((n != j) && (j < k) && (j < MAX_SONAR) && (robot[i]->armor > 0)) {
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
		if (show_arcs)
			robot[n]->sonar_count = 2;

		*time_used += 40;
		l = -1;
		k = MAXINT;

		for (i = 0; i <= MAX_ROBOTS; i++) {
			j = int( _distance(robot[n]->x, robot[n]->y, robot[i]->x, robot[i]->y) + 0.5);
			if ((n != j) && (j < k) && (j < MAX_SONAR) && (robot[i]->armor > 0)) {
				k = j;
				l = i;
				nn = i;
			}
		}

		if (l > 0) {
			v = (int)(((find_angle(robot[n]->x, robot[n]->y, robot[i]->x, robot[i]->y) / PI * 128 + 1024 + (rand() % 66)) - 32) + 0.5) & 255;
		}
		else
			v = MININT;
		if ((nn >= 0) && (n <= num_robots))
			robot[n]->ram[5] = robot[nn]->transponder;
		break;
	case 17: v = robot[n]->scanarc; break;
	case 18:
		if (robot[n]->overburn)
			v = 1;
		else
			v = 0;
		break;
	case 19: v = robot[n]->transponder; break;
	case 20: v = robot[n]->shutdown; break;
	case 21: v = robot[n]->channel; break;
	case 22: v = robot[n]->mines; break;
	case 23:
		if (robot[n]->config.mines >= 0) {
			k = 0;
			for (i = 0; i <= MAX_MINES; i++) {
				if ((robot[n]->mine[i].x >= 0) && (robot[n]->mine[i].x <= 1000) &&
					(robot[n]->mine[i].y >= 0) && (robot[n]->mine[i].y <= 1000) &&
					(robot[n]->mine[i].yield > 0)) {

					k++;
				}
			}
			v = k;
		}
		else
			v = 0;
		break;
	case 24:
		if (robot[n]->config.shield > 0) {
			if (robot[n]->shields_up)
				v = 1;
			else
				v = 0;
		}
		else {
			v = 0;
			robot[n]->shields_up = false;
		}
		break;
	default:
		robot_error(n, 11, cstr(p));
	}

	// err_log << "End in_port" << endl << endl;

	return v;
}


void out_port(int16_t n, int16_t p, int16_t v, int16_t *time_used) {
	// err_log << "Begin out_port" << endl;
	int16_t i;

	switch (p) {
	case 11: robot[n]->tspd = v; break;
	case 12: robot[n]->shift = (robot[n]->shift + v + 1024) & 255; break;
	case 13: robot[n]->shift = (v + 1024) & 255; break;
	case 14: robot[n]->thd = (robot[n]->thd + v + 1024) & 255; break;
	case 15:
		*time_used += 3;
		if (v > 4)
			v = 4;
		else if (v < -4)
			v = -4;
		init_missile(robot[n]->x, robot[n]->y, robot[n]->xv, robot[n]->yv, (robot[n]->hd + robot[n]->shift + v) & 255, n, 0, robot[n]->overburn);
		break;
	case 17: robot[n]->scanarc = v; break;
	case 18:
		if (v == 0)
			robot[n]->overburn = false;
		else
			robot[n]->overburn = true;
		break;
	case 19: robot[n]->transponder = v; break;
	case 20: robot[n]->shutdown = v; break;
	case 21: robot[n]->channel = v; break;
	case 22:
		if (robot[n]->config.mines >= 0) {
			if (robot[n]->mines > 0) {
				init_mine(n, v, MINE_BLAST);
				robot[n]->mines -= 1;
			}
			else
				robot_error(n, 14, "");
		}
		else
			robot_error(n, 13, "");
		break;
	case 23:
		if (robot[n]->config.mines >= 0) {
			for (i = 0; i <= MAX_MINES; i++)
				robot[n]->mine[i].detonate = true;
		}
		else
			robot_error(n, 13, "");
		break;
	case 24:
		if (robot[n]->config.shield >= 3) {
			if (v == 0)
				robot[n]->shields_up = false;
			else
				robot[n]->shields_up = true;
		}
		else {
			robot[n]->shields_up = false;
			robot_error(n, 15, "");
		}
		break;
	default:
		robot_error(n, 11, cstr(p));
	}
	if (robot[n]->scanarc > 64)
		robot[n]->scanarc = 64;
	else if (robot[n]->scanarc < 0)
		robot[n]->scanarc = 0;

	// err_log << "End out_port" << endl << endl;
}


void call_int(int16_t n, int16_t int_num, int16_t * time_used) {
	// err_log << "Begin call_int" << endl;
	int16_t i, j, k;

	switch (int_num) {
	case 0: damage(n, 1000, true); break;
	case 1:
		reset_software(n);
		*time_used = 10;
		break;
	case 2:
		*time_used = 5;
		robot[n]->ram[69] = int16_t(robot[n]->x + 0.5); // double to signed int16_t may cause problems
		robot[n]->ram[70] = int (robot[n]->y + 0.5);
		break;
	case 3:
		*time_used = 2;
		if (robot[n]->ram[65] == 0)
			robot[n]->keepshift = false;
		else
			robot[n]->keepshift = true;
		robot[n]->ram[70] = robot[n]->shift & 255;
		break;
	case 4:
		if (robot[n]->ram[65] == 0)
			robot[n]->overburn = false;
		else
			robot[n]->overburn = true;
		break;
	case 5:
		*time_used = 2;
		robot[n]->ram[70] = robot[n]->transponder;
		break;
	case 6:
		*time_used = 2;
		robot[n]->ram[69] = game_cycle >> 16;
		robot[n]->ram[70] = game_cycle & 65535;
		break;
	case 7:
		j = robot[n]->ram[69];
		k = robot[n]->ram[70];
		if (j < 0)
			j = 0;
		else if (j > 1000)
			j = 1000;
		if (k < 0)
			k = 0;
		else if (k > 1000)
			k = 1000;

		robot[n]->ram[65] = (int16_t)((find_angle((int16_t)robot[n]->x + 0.5, (int16_t)robot[n]->y + 0.5, j, k) / PI * 128 + 256) + 0.5) & 255;
		*time_used = 32;
		break;
	case 8:
		robot[n]->ram[70] = robot[n]->ram[5];
		*time_used = 1;
		break;
	case 9:
		robot[n]->ram[69] = robot[n]->ram[6];
		robot[n]->ram[70] = robot[n]->ram[7];
		break;
	case 10:
		k = 0;
		for (i = 0; i <= num_robots; i++)
			if (robot[i]->armor > 0)
				k++;

		robot[n]->ram[68] = k;
		robot[n]->ram[69] = played;
		robot[n]->ram[70] = matches;
		*time_used = 4;
		break;
	case 11:
		robot[n]->ram[68] = int ((robot[n]->speed * 100) + 0.5);
		robot[n]->ram[69] = robot[n]->last_damage;
		robot[n]->ram[70] = robot[n]->last_hit;
		*time_used = 5;
		break;
	case 12:
		robot[n]->ram[70] = robot[n]->ram[8];
		*time_used = 1;
		break;
	case 13:
		robot[n]->ram[8] = 0;
		*time_used = 1;
		break;
	case 14:
		com_transmit(n, robot[n]->channel, robot[n]->ram[65]);
		*time_used = 1;
		break;
	case 15:
		if (robot[n]->ram[10] != robot[n]->ram[11])
			robot[n]->ram[70] = com_receive(n);
		else
			robot_error(n, 12, "");
		*time_used = 1;
		break;
	case 16:
		if (robot[n]->ram[11] >= robot[n]->ram[10])
			robot[n]->ram[70] = robot[n]->ram[11] - robot[n]->ram[10];
		else
			robot[n]->ram[70] = MAX_QUEUE + 1 - robot[n]->ram[10] + robot[n]->ram[11];

		*time_used = 1;
		break;
	case 17:
		robot[n]->ram[10] = 0;
		robot[n]->ram[11] = 0;
		*time_used = 1;
		break;
	case 18:
		robot[n]->ram[68] = robot[n]->kills;
		robot[n]->ram[69] = robot[n]->kills - robot[n]->startkills;
		robot[n]->ram[70] = robot[n]->deaths;
		*time_used = 3;
		break;
	case 19:
		robot[n]->ram[9] = 0;
		robot[n]->meters = 0;
		break;
	default:
		robot_error(n, 10, cstr(int_num));
	}

	// err_log << "End call_int" << endl << endl;
}


void jump(int16_t n, int16_t o, bool * inc_ip) {
	// err_log << "Begin jump" << endl;
	int16_t loc;

	loc = find_label(n, get_val(n, robot[n]->ip, o), robot[n]->code[robot[n]->ip].op[MAX_OP] >> (o * 4));
	if ((loc >= 0) && (loc <= robot[n]->plen)) {
		*inc_ip = false;
		robot[n]->ip = loc;
	}
	else
		robot_error(n, 2, cstr(loc));

	// err_log << "End jump" << endl << endl;
}

// A bunch of debug related functions go here


bool gameover() {
	// err_log << "gameover" << endl;
	int16_t n, k;

	if ((game_cycle >= game_limit) && (game_limit > 0))
		return true;

	if ((game_cycle & 31) == 0) {
		k = 0;
		for (n = 0; n <= num_robots; n++)
			if (robot[n]->armor > 0)
				k++;

		if (k <= 1)
			return true;
		else
			return false;

	}
	else
		return false;
}


void toggle_graphix() {
	// err_log << "Begin toggle_grapix" << endl;
	graph_mode(!graphix);
	if (graphix == false) {
		textcolor(7);
		std::cout << "Match " << played << "/" << matches << ", Battle in progress&&" << endl << endl;
	}
	else
		setscreen();

	// err_log << "End toggle_graphix" << endl << endl;
}

bool invalid_microcode(int16_t n, int16_t ip) {
	std::cout << "Start Invalid Microde" << endl;
	int cunt = 0;
	// err_log << "Begin invalid_microcode" << endl;
	bool invalid;
	int16_t i, k;

	for (i = 0; i <= 2; i++) {
		k = (robot[n]->code[ip].op[MAX_OP] >> (i << 2)) & 7;


		if ((k < 0) || (k > 4)) {
			invalid = true;

		}
		else
			invalid = false;

		if (i == 2)
			break;
		std::cout << "still in for loop " << "i= " << i << endl;

	}
	std::cout << "End Invalid Microde " << cunt << endl;

	cunt++;
	// err_log << "End invalid_microcode" << endl << endl;
	return invalid;
}

void process_keypress(char c) {
	// err_log << "Begin process_keypress" << endl;
	// Should probably replace these case statements with SDL's scancodes

	switch (c) {
	case 'C': calibrate_timing(); break;
	case 'T': timing = !timing;; break;
	case 'A': show_arcs = !show_arcs; break;
	case 'S':
	case 'Q':
		if (sound_on)
			//	chirp();
			sound_on = !sound_on;
		if (sound_on)
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

	// err_log << "End proccess_keypress" << endl << endl;
}


std::string victor_string(int16_t k, int16_t n) {
	// err_log << "Begin victor_string" << endl;
	std::string s = "";

	if (k == 1)
		s = "Robot #" + cstr(n + 1) + " (" + robot[n]->fn + ") wins!";
	if (k == 0)
		s = "Simultaneos destruction, match is a tie.";
	if (k > 1)
		s = "No clear victor, match is a tie.";


	// err_log << "End victor_string" << endl << endl;
	return s;
}


void show_statistics() {
	// err_log << "Begin show_statistics" << endl;
	int16_t i, j, k, n, sx, sy;

	if (windoze == false)
		return;

	if (graphix == true) { // Display results in window
		sx = 24;
		sy = 93 - num_robots * 3;
		viewport(0, 0, 639, 479);
		box(sx, sy, sx + 591, sy + 102 + num_robots * 12);
		hole(sx + 4, sy + 4, sx + 587, sy + 98 + num_robots * 12);
		setfillstyle(1); // 50/50 checkering

		bar(sx + 5, sy + 5, sx + 586, sy + 97 + num_robots * 12);
		setcolor(WHITE);
		//outtextxy(sx + 16, sy + 20, "Robot            Scored   Wins   Matches   Armor   Kills   Deaths    Shots");
		//outtextxy(sx + 16, sy + 30, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

		n = -1;
		k = 0;
		for (i = 0; i <= num_robots; i++) {
			if ((robot[n]->armor > 0) || (robot[n]->won)) {
				k++;
				n = i;
			}
		}

		for (i = 0; i <= num_robots; i++) {
			setcolor(robot_color(i));
			if ((k == 1) && (n == i))
				j = 1;
			else
				j = 0;
			/* XXX XXX
			outtextxy(sx + 16, sy + 42 + i * 12, addfront(cstr(i + 1), 2) +
			" - " + addrear(robot[n] -> fn, 15) + cstr(j) +
			addfront(cstr(robot[n] -> wins), 8) + addfront(cstr(robot[n] -> trials), 8) +
			addfront(cstr(robot[n] -> armor) + "%", 9) + addfront(cstr(robot[n] -> kills), 7) +
			addfront(cstr(robot[n] -> deaths), 8) + addfront(cstr(robot[n] -> match_shots), 9));
			*/
		}

		setcolor(WHITE);
		//outtextxy(sx + 16, sy + 64 + num_robots * 12, victor_string(k, n));

		if (windoze) {
			//outtextxy(sx + 16, sy + 76 +num_robots * 12, "Press any key to continue&&");
			readkey();
		}
	}
	else { // No graphics; Display results on commandline/terminal
		textcolor(WHITE);
		std::cout << endl << space(79) << endl;
		std::cout << "Match " << played << "/" << matches << " results:" << endl << endl;
		std::cout << "Robot            Scored   Wins   Matches   Armor   Kills   Deaths    Shots" << endl;
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		n = -1;
		k = 0;

		for (i = 0; i <= num_robots; i++) {
			if ((robot[n]->armor > 0) || (robot[n]->won)) {
				k++;
				n = i;
			}
		}

		for (i = 0; i <= num_robots; i++) {
			textcolor(robot_color(i));
			if ((k == 1) && (n == i))
				j = 1;
			else
				j = 0;

			std::cout << addfront(cstr(i + 1), 2) << " - " << addrear(robot[n]->fn, 15) << cstr(j) <<
				addfront(cstr(robot[n]->wins), 8) << addfront(cstr(robot[n]->trials), 8) <<
				addfront(cstr(robot[n]->armor) + "%", 9) << addfront(cstr(robot[n]->kills), 7) <<
				addfront(cstr(robot[n]->deaths), 8) << addfront(cstr(robot[n]->match_shots), 9) << endl;

		}
		textcolor(WHITE);
		std::cout << endl << victor_string(k, n) << endl << endl;
	}
	// err_log << "End show_statistics" << endl << endl;
}


void score_robots() {
	// err_log << "Begin score_robots" << endl;
	int16_t i, k, n;
	k = 0;


	for (i = 0; i <= num_robots; i++) {
		robot[i]->trials += 1;
		if (robot[i]->armor > 0) {
			k++;
			n = i;
		}
	}
	if ((k == 1) && (n >= 0)) {
		robot[n]->wins += 1;
		robot[n]->won = true;
	}

	// err_log << "End score_robots" << endl;
}

void init_bout() {
	// err_log << "Begin init_bout" << endl;
	int16_t i;

	game_cycle = 0;

	for (i = 0; i <= MAX_MISSILES; i++) {
		missile[i].a = 0;
		missile[i].source = -1;
		missile[i].x = 0;
		missile[i].y = 0;
		missile[i].lx = 0;
		missile[i].ly = 0;
		missile[i].mult = 1;
	}
	for (i = 0; i <= num_robots; i++) {
		robot[i]->mem_watch = 128;
		reset_hardware(i);
		reset_software(i);
	}

	if (graphix)
		setscreen();
	if (graphix && (step_mode > 0))
		//init_debug_window; XXX Debug function here

		if (graphix == false)
			textcolor(LIGHT_GRAY);
	// err_log << "End init_bout" << endl << endl;
}


// Primary function that runs the games.
void bout() {
	// err_log << "Begin bout" << endl;
	std::cout << "Begin Bout" << endl;
	int16_t i, k;
	unsigned char c;

	if (quit)
		return;

	played++;
	init_bout();
	bout_over = false;

	if (step_mode == 0)
		step_loop = false;
	else
		step_loop = true;

	step_count = -1; // will be set to 0 upon first iteration of do while loop
	if (graphix && (step_mode > 0))
		for (i = 0; i <= num_robots; i++)
			draw_robot(i);

	// Begin start of main loop
	std::cout << "start of main loop bout" << endl;
	do {
		std::cout << "in do loop" << endl;
		game_cycle++;
		std::cout << game_cycle << endl;
		for (i = 0; i <= num_robots; i++) {
			std::cout << "robot armor" << robot[i]->armor << endl;
			if (robot[i]->armor > 0) {
				std::cout << "number of robots" << num_robots << endl;
				std::cout << "robot number =" << i << endl;
				do_robot(i);

			}


			if (i == 10)
				break;
		}
		std::cout << "past do robot" << endl;
		for (i = 0; i <= MAX_MISSILES; i++)
			if (missile[i].a > 0)
				do_missile(i);
		;
		for (i = 0; i <= num_robots; i++)
			for (k = 0; k <= MAX_MINES; k++)
				if (robot[i]->mine[k].yield > 0)
					do_mine(i, k);


		if (graphix && timing)
			time_delay(game_delay);


		if (keypressed())
			c = toupper(readkey());
		else
			c = 255;

		switch (c) {
		case 'X':
			if (!robot[0]->is_locked) {
				if (graphix == false)
					toggle_graphix();

				if (robot[0]->armor > 0) {
					if (temp_mode > 0)
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
				else if ((game_delay >= 30) && (game_delay <= 39))
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
			if (game_delay > 0) {
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



		if (game_delay < 0)
			game_delay = 0;
		else if (game_delay > 100)
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


		if (!graphix)
			k = 100;

		if (graphix) {
			if (((game_cycle % k) == 0) || (game_cycle == 10))
				update_cycle_window();
			else {
				if (update_timer != get_seconds_past_hour() >> 1)
					update_cycle_window();

				update_timer = get_seconds_past_hour() >> 1;
			}
		}
	} while (!quit || !gameover() || !bout_over);
	std::cout << "end of main do bout" << endl;

	update_cycle_window();

	score_robots();
	show_statistics();
	// err_log << "End bout" << endl << endl;
	std::cout << "end bout" << endl;
}


void write_report() {
	std::cout << "Begin write_report" << endl;
	int16_t i;
	fstream f;

	f.open(main_filename + report_ext, fstream::out);

	f << (num_robots + 1) << endl;
	for (i = 0; i <= num_robots; i++) {
		switch (report_type) {
		case 2:
			f << robot[i]->wins << " " << robot[i]->trials << " " << robot[i]->kills << " " << robot[i]->deaths << " " << robot[i]->fn << " " << endl;
			break;
		case 3:
			f << robot[i]->wins << " " << robot[i]->trials << " " << robot[i]->kills << " " << robot[i]->deaths << " " << robot[i]->armor <<
				" " << robot[i]->heat << " " << robot[i]->shots_fired << " " << robot[i]->fn << " " << endl;
			break;
		case 4:
			f << robot[i]->wins << " " << robot[i]->trials << " " << robot[i]->kills << " " << robot[i]->deaths << " " << robot[i]->armor <<
				" " << robot[i]->heat << " " << robot[i]->shots_fired << " " << robot[i]->hits << " " << robot[i]->damage_total <<
				" " << robot[i]->cycles_lived << " " << robot[i]->error_count << " " << robot[i]->fn << " " << endl;
			break;
		default:
			f << robot[i]->wins << " " << robot[i]->trials << " " << robot[i]->fn << " " << endl;
		}
	}

	f.close();
	std::cout << "End write_report" << endl << endl;
}


void begin_window() {
	std::cout << "Begin begin_window" << endl;
	std::string s;

	if (!graphix || !windoze)
		return;


	setscreen();

	viewport(0, 0, 639, 479);
	box(100, 150, 539, 200);
	hole(105, 155, 534, 195);

	setfillstyle(1); // Checkered

					 //bar(105, 155, 534, 195);

	setcolor(15);
	s = "Press any key to begin!";
	//outtextxy(320 - ((s.length() << 3) >> 1), 172, s);

	readkey();

	setscreen();

	std::cout << "End begin_window" << endl;
}

void true_main() {
	// err_log << "Begin true_main" << endl;
	std::cout << "Begin true main" << endl;
	int16_t i, k, n, w;

	if (graphix) {
		begin_window();
	}


	if (matches > 0)
		for (i = 1; i <= matches; i++) {

			bout();
			std::cout << "past bout" << endl;
		}


	if (graphix == false)
		std::cout << endl;


	if (quit)
		return;

	// Calculates overall statistics
	if (matches > 1) {
		std::cout << endl << endl;

		graph_mode(false);
		textcolor(WHITE);
		std::cout << "Bout complete! (" << matches << ")" << endl << endl;

		k = 0;
		w = 0;

		for (i = 0; i <= num_robots; i++) {
			if (robot[i]->wins == w)
				k++;
			if (robot[i]->wins > w) {
				k = 1;
				n = i;
				w = robot[i]->wins;
			}
		}

		std::cout << "Robot            Wins   Matches   Kills   Deaths    Shots" << endl;
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

		for (i = 0; i <= num_robots; i++) {
			textcolor(robot_color(i));
			std::cout << addfront(cstr(i + 1), 2) << " - " << addrear(robot[i]->fn, 8) <<
				addfront(cstr(robot[i]->wins), 7) << addfront(cstr(robot[i]->trials), 8) <<
				addfront(cstr(robot[n]->kills), 8) << addfront(cstr(robot[n]->deaths), 8) <<
				addfront(cstr(robot[n]->shots_fired), 9) << endl;
		}

		textcolor(WHITE);
		std::cout << endl;
		if (k == 1) {
			std::cout << "Robot #" << to_string(n + 1) << " (" << robot[n]->fn << ") wins the bout! (score :" << to_string(w) << "/" << to_string(matches) << endl;
		}
		else
			std::cout << "There is no clear victor!" << endl;

		std::cout << endl;
	}
	else if (graphix) {
		graph_mode(false);
		show_statistics();
	}

	if (report)
		write_report();

	// err_log << "End true_main" << endl << endl;
}


void execute_instruction(int16_t n) {
	// err_log << "Begin execute_instruction" << endl;
	std::cout << "Begin Execute instruction" << endl;
	int16_t i, j, k;
	int16_t time_used, loc;
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
	//std::cout << "past this wierd invalid microde" << endl;
	std::cout << step_mode << graphix << n;
	if (graphix && (step_mode > 0) && (n == 0)) {
		std::cout << "in grpahix loop" << endl;
		step_count++;
		update_cycle_window();
		//update_debug_window();
		if (((step_count % step_mode) == 0))
			step_loop = true;
		else
			step_loop = false;
		std::cout << "step loop = " << step_loop << endl;
		int loopadoop = 0;
		int keypressnum = 0;
		while (step_loop && (!(quit | gameover() | bout_over))) {

			if (loopadoop < 1) {
				std::cout << "Waiting for keypress" << loopadoop << endl;
				loopadoop++;
			}
			if (keypressed()) {
				keypressnum++;
				std::cout << "KEYPRESS " << keypressnum << endl;

				// Loop does nothing until key is pressed
				c = toupper(readkey());
				std::cout << c << endl;
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
					std::cout << "entered number 1-9 case" << endl;
					step_mode = value(to_string(c));
					std::cout << "step_mode= " << step_mode << endl;
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
							//outtextxy(35, 212 + (10 * i), decimal(robot[n]->mem_watch + i, 4) + " :");
							robot[n]->mem_watch -= 1;
						// update_debug_memory();
					}
					break;
				case '+':
				case '=':
					if (robot[n]->mem_watch < 1014) {
						setcolor(BLACK);
						for (i = 0; i <= 9; i++)
							//outtextxy(35, 212 + (10 * i), decimal(robot[n]->mem_watch + i, 4) + " :");
							robot[n]->mem_watch += 1;
						// update_debug_memory();
					}
					break;
				case '[':
				case '{':
					if (robot[n]->mem_watch > 0) {
						setcolor(BLACK);
						for (i = 0; i <= 9; i++)
							//outtextxy(35, 212 + (10 * i), decimal(robot[n]->mem_watch + i, 4) + " :");
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
							//outtextxy(35, 212 + (10 * i), decimal(robot[n]->mem_watch + i, 4) + " :");
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
			if (loopadoop < 1) {
				std::cout << " pastWaiting for keypress" << endl;
				loopadoop++;
			}
		}

		if (!(((robot[n]->code[robot[n]->ip].op[MAX_OP] & 7) == 0) || (robot[n]->code[robot[n]->ip].op[MAX_OP] == 1))) {
			time_used = 0;
		}
		else {
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
				}
				else
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
				}
				else robot_error(n, 4, cstr(k));
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
				}
				else
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
	// err_log << "End execute_instruction" << endl << endl;
	std::cout << "end execute instruction" << endl;
}

void do_robot(int16_t n) {
	// err_log << "Begin do_robot" << endl;
	std::cout << "begin do robot" << endl;
	int16_t i, k, tthd;
	double heat_mult, ttx, tty; //change made since variable eventually needs decimal

	if (n < 0 || n > num_robots)
		return;
	if (robot[n]->armor <= 0)
		return;
	robot[n]->time_left = time_slice;
	std::cout << "time left: " << robot[n]->time_left << endl << "time limit: " << robot[n]->robot_time_limit << endl << "max time: " << robot[n]->max_time << endl;
	if (robot[n]->time_left > robot[n]->robot_time_limit && robot[n]->robot_time_limit > 0)
		robot[n]->time_left = robot[n]->robot_time_limit;
	if (robot[n]->time_left>robot[n]->max_time && robot[n]->max_time > 0)
		robot[n]->time_left = robot[n]->max_time;
	executed = 0;
	//execute timeslice
	std::cout << "time left: " << robot[n]->time_left << endl << "cooling: " << robot[n]->cooling << endl << "executed: " << executed << endl << "time_slice : " << time_slice << endl << "robot armor: " << robot[n]->armor << endl;
	while (robot[n]->time_left > 0 && !robot[n]->cooling && executed < 20 + time_slice && robot[n]->armor > 0) {

		if (robot[n]->delay_left < 0) {

			robot[n]->delay_left = 0;
		}
		if (robot[n]->delay_left > 0) {

			robot[n]->delay_left--;
			robot[n]->time_left--;
		}
		if (robot[n]->time_left >= 0 && robot[n]->delay_left == 0) {
			std::cout << "time left: " << robot[n]->time_left << endl;
			std::cout << "delay_left: " << robot[n]->delay_left << endl;

			execute_instruction(n);

		}
		std::cout << "checkpint 1" << endl;
		if (robot[n]->heat >= robot[n]->shutdown) {
			robot[n]->cooling = true;
			robot[n]->shields_up = false;
		}
		if (robot[n]->heat >= 500) {
			damage(n, 1000, true);
		}
		executed++;

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
	if (robot[n]->shields_up && ((game_cycle & 3) == 0))
		robot[n]->heat++;

	if (!robot[n]->shields_up) {
		if (robot[n]->heat > 0)
			switch (robot[n]->config.heatsinks) {
			case 5:
				if ((game_cycle & 1) == 0)
					robot[n]->heat--;
				break;

			case 4:
				if ((game_cycle % 3) == 0)
					robot[n]->heat--;
				break;

			case 3:
				if ((game_cycle & 3) == 0)
					robot[n]->heat--;
				break;

			case 2:
				if ((game_cycle & 7) == 0)
					robot[n]->heat--;
				break;

			case 1: // Do nothing. Just needed to no trigger default
				break;

			default:
				if ((game_cycle & 3) == 0)
					robot[n]->heat++;
			}

		if (robot[n]->overburn && ((game_cycle % 3) == 0))
			robot[n]->heat++;
		if (robot[n]->heat > 0)
			robot[n]->heat--;
		if ((robot[n]->heat > 0) && ((game_cycle & 7) == 0) && (abs(robot[n]->tspd) <= 25))
			robot[n]->heat--;
		if ((robot[n]->heat <= (robot[n]->shutdown - 50)) || (robot[n]->heat <= 0))
			robot[n]->cooling = false;
	}

	if (robot[n]->cooling)
		robot[n]->tspd = 0;

	heat_mult = 1;
	if ((robot[n]->heat >= 80) && (robot[n]->heat <= 99))
		heat_mult = 0.98;
	else if ((robot[n]->heat >= 100) && (robot[n]->heat <= 149))
		heat_mult = 0.95;
	else if ((robot[n]->heat >= 150) && (robot[n]->heat <= 199))
		heat_mult = 0.85;
	else if ((robot[n]->heat >= 200) && (robot[n]->heat <= 249))
		heat_mult = 0.75;
	else if ((robot[n]->heat >= 250) && (robot[n]->heat <= MAXINT))
		heat_mult = 0.50;

	if (robot[n]->overburn)
		heat_mult = heat_mult * 1.3;
	if ((robot[n]->heat >= 475) && ((game_cycle & 3) == 0))
		damage(n, 1, true);
	if ((robot[n]->heat >= 450) && ((game_cycle & 7) == 0))
		damage(n, 1, true);
	if ((robot[n]->heat >= 400) && ((game_cycle & 15) == 0))
		damage(n, 1, true);
	if ((robot[n]->heat >= 350) && ((game_cycle & 31) == 0))
		damage(n, 1, true);
	if ((robot[n]->heat >= 300) && ((game_cycle & 63) == 0))
		damage(n, 1, true);

	//update robot in physical world
	//acceleration
	if (abs(robot[n]->spd - robot[n]->tspd) <= ACCELERATION)
		robot[n]->spd = robot[n]->tspd;
	else {
		if (robot[n]->tspd > robot[n]->spd) {
			robot[n]->spd += ACCELERATION;
		}
		else {
			robot[n]->spd -= ACCELERATION;
		}
	}

	//Turn Robot
	tthd = robot[n]->hd + robot[n]->shift;
	if (abs(robot[n]->hd - robot[n]->thd) <= TURN_RATE || (abs(robot[n]->hd - robot[n]->thd) >= 256 - TURN_RATE))
		robot[n]->hd = robot[n]->thd;
	else if (robot[n]->hd != robot[n]->thd) {
		k = 0;
		if (((robot[n]->thd > robot[n]->hd) && (abs(robot[n]->hd - robot[n]->thd) <= 128)) ||
			((robot[n]->thd < robot[n]->hd) && (abs(robot[n]->hd - robot[n]->thd) >= 128)))
			k = 1;
		if (k == 1)
			robot[n]->hd = (robot[n]->hd + TURN_RATE) & 255;
		else
			robot[n]->hd = (robot[n]->hd + 256 - TURN_RATE) & 255;
	}

	robot[n]->hd = robot[n]->hd & 255;
	if (robot[n]->keepshift)
		robot[n]->shift = (tthd - robot[n]->hd + 1024) & 255;

	//Move Robot
	robot[n]->spd = 100;
	robot[n]->speed = double(robot[n]->spd / 100 * (MAX_VEL * heat_mult * robot[n]->speedadj));
	std::cout << "Speed variable defenitions" << endl;
	std::cout << "Robot spd: " << robot[n]->spd << endl << "max_vel: " << MAX_VEL << endl << "heat_mult" << heat_mult << endl << "Speedadj: " << robot[n]->speedadj << endl;
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
		tty = robot[n]->y;
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
			ttx = robot[n]->x;
			tty = robot[n]->y;
			robot[i]->tspd = 0;
			robot[i]->spd = 0;
			robot[n]->ram[8]++;
			robot[i]->ram[8]++;
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
	robot[n]->ram[9] = (int)trunc(robot[n]->meters);
	robot[n]->x = ttx;
	robot[n]->y = tty;

	//test of robots out values
	std::cout << "test of values going into draw robots" << endl;
	std::cout << "robot hd: " << robot[n]->hd << endl << "robot thd: " << robot[n]->thd << endl;
	std::cout << "robot speed: " << robot[n]->speed << endl << "robot shift: " << robot[n]->shift << endl;
	std::cout << "robot xv: " << robot[n]->xv << endl << "robot yv: " << robot[n]->yv << endl;



	//draw robot
	if (robot[n]->armor < 0)
		robot[n]->armor = 0;
	if (robot[n]->heat < 0)
		robot[n]->heat = 0;
	if (graphix) {
		if (robot[n]->armor != robot[n]->larmor)
			update_armor(n);
		if (robot[n]->heat / 5 != (int)robot[n]->lheat / 5)
			update_heat(n);
		std::cout << "**********************************" << endl;
		std::cout << "Value of ttx: " << ttx << endl;
		std::cout << "Value of tty: " << tty << endl;
		draw_robot(n);
	}

	robot[n]->lheat = robot[n]->heat;
	robot[n]->larmor = robot[n]->armor;

	robot[n]->cycles_lived++;
	std::cout << "End do_robot" << endl << endl;
}

void do_mine(int16_t n, int16_t m) {
	// err_log << "Begin do_mine" << endl;
	int16_t i, k;
	double d;
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
					k = int( _distance(_.x, _.y, robot[i]->x, robot[i]->y) + 0.5);
					if (k < _.yield) {
						damage(i, int16_t(abs(_.yield - k) + 0.5), false);
						if ((n >= 0) && (n <= num_robots) && (i != n))
							robot[n]->damage_total += int (abs(_.yield - k) + 0.5);
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
		}
		else { // Begin drawing mine
			if (graphix && ((game_cycle & 1) == 0)) {
				main_viewport();
				setcolor(robot_color(n));
				line((_.x*SCREEN_SCALE + 0.5) + SCREEN_X, (_.y*SCREEN_SCALE + 0.5) + SCREEN_Y - 1, (_.x*SCREEN_SCALE + 0.5) + SCREEN_X, (_.y*SCREEN_SCALE + 0.5) + SCREEN_Y + 1);
				line((_.x*SCREEN_SCALE + 0.5) + SCREEN_X + 1, (_.y*SCREEN_SCALE + 0.5) + SCREEN_Y, (_.x*SCREEN_SCALE + 0.5) + SCREEN_X - 1, (_.y*SCREEN_SCALE + 0.5) + SCREEN_Y);
			}
		}
	}
	// err_log << "End do_mine" << endl << endl;
}

void do_missile(int16_t n) {
	// err_log << "Begin do_missile" << endl;
	double llx, lly, r, d, xv, yv;
	int16_t i, k, l, xx, yy, tx, ty, dd, dam;
	bool source_alive;


	// Created specifically because I'm lazy
	auto _ = missile[n];

	if (missile[n].a == 0)
		return;

	if (missile[n].a == 1) {
		// Look for colision with walls
		if ((missile[n].x < -2) || (missile[n].x > 1020) || (missile[n].y < -20) || (missile[n].y > 1020))
			missile[n].a = 0;

		llx = _.lx;
		lly = _.ly;
		_.lx = _.x;
		_.ly = _.y;

		if (_.a > 0) {
			_.hd = (_.hd + 256) & 255;
			xv = sint[_.hd] * _.mspd;
			yv = cost[_.hd] * _.mspd;
			_.x = _.x + xv;
			_.y = _.y + yv;
		}

		// Check if robot was hit
		k = -1;
		l = MAXINT;
		for (i = 0; i <= num_robots; i++) {
			if ((robot[i]->armor > 0) && (i != _.source)) {
				d = _distance(_.lx, _.ly, robot[i]->x, robot[i]->y);
				_.hd = _.hd & 0xff;
				xx =  int ((sint[_.hd] * d + _.lx) + 0.5);
				yy = int ((cost[_.hd] * d + _.ly) + 0.5);
				r = _distance(xx, yy, robot[i]->x, robot[i]->y);
				if ((d <= _.mspd) && (r < HIT_RANGE) && ((d + 0.5) <= l)) {
					k = i;
					l = int (d + 0.5);
					dd = int (r + 0.5);
					tx = xx;
					ty = yy;
				}
			}
			// Hit a robot
			if (k >= 0) {
				_.x = tx;
				_.y = ty;
				_.a = 2;
				_.rad = 0;
				_.lrad = 0;
				if ((_.source >= 0) && (_.source <= num_robots)) {
					robot[_.source]->last_hit = 0;
					robot[_.source]->hits++;
				}

				for (i = 0; i <= num_robots; i++) {
					dd = int ((_distance(_.x, _.y, robot[i]->x, robot[i]->y)) + 0.5);
					if (dd <= HIT_RANGE) {
						dam = int ((abs(HIT_RANGE - dd) * _.mult) + 0.5);
						if (dam <= 0)
							dam = 1;

						kill_count = 0;
						if ((robot[_.source]->armor > 0)) {
							source_alive = true;
						}
						else
							source_alive = false;

						damage(i, dam, false);
						if ((_.source >= 0) && (_.source <= num_robots) && (i != _.source))
							robot[_.source]->damage_total += dam;

						if ((kill_count > 0) && source_alive && (robot[_.source]->armor <= 0))
							kill_count--;

						if (kill_count > 0) {
							robot[_.source]->kills += kill_count;
							update_lives(_.source);
						}
					}
				}
			}
		}
		// Draw missile
		if (graphix) {
			main_viewport();
			setcolor(BLACK);
			line(int16_t(llx * SCREEN_SCALE + 0.5) + SCREEN_X, int16_t(lly * SCREEN_SCALE + 0.5) + SCREEN_Y,
				int16_t(_.ly * SCREEN_SCALE + 0.5) + SCREEN_X, int16_t(_.ly * SCREEN_SCALE + 0.5) + SCREEN_Y);

			if (_.a == 1) {
				if (_.mult > robot[_.source]->shotstrength)
					setcolor(14 + (game_cycle & 1));
				else
					setcolor(WHITE);

				line(int16_t(_.x + SCREEN_SCALE + 0.5) + SCREEN_X, int16_t(_.y * SCREEN_SCALE + 0.5) + SCREEN_Y,
					int16_t(_.lx * SCREEN_SCALE + 0.5) + SCREEN_X, int16_t(_.ly * SCREEN_SCALE + 0.5) + SCREEN_Y);
			}
		}
	}
	if (_.a == 2) {
		_.lrad = _.rad;
		_.rad++;
		if (_.rad > _.max_rad)
			_.a = 0;

		if (graphix) {
			main_viewport();
			setcolor(BLACK);
			circle(int16_t(_.x * SCREEN_SCALE + 0.5) + SCREEN_X, int16_t(_.y * SCREEN_SCALE + 0.5) + SCREEN_Y, _.lrad);
			if (_.mult > 1)
				setcolor(14 + (game_cycle & 1));
			else
				setcolor(WHITE);

			if (_.max_rad >= BLAST_CIRCLE)
				setcolor(14);
			if (_.max_rad >= MINE_CIRCLE)
				setcolor(11);

			if (_.a > 0)
				circle(int16_t(_.x * SCREEN_SCALE + 0.5) + SCREEN_X, int16_t(_.y * SCREEN_SCALE + 0.5) + SCREEN_Y, _.lrad);
		}
	}

	// err_log << "End do_missile" << endl << endl;
}


void log_error(int16_t i, int16_t n, std::string ov) {
	// err_log << "Begin log_error" << endl;
	std::string s;

	if (!logging_errors) {
		return;
	}

	switch (i) {
	case 1:
		s = "Stack full - Too many CALLs?";
		break;

	case 2:
		s = "Label not found. Hmmm.";
		break;

	case 3:
		s = "Can't assign value - Tisk tisk.";
		break;

	case 4:
		s = "Illegal memory reference";
		break;

	case 5:
		s = "Stack empty - Too many RETs?";
		break;

	case 6:
		s = "Illegal instruction. How bizarre.";
		break;

	case 7:
		s = "Return out of range - Woops!";
		break;

	case 8:
		s = "Divide by zero";
		break;

	case 9:
		s = "Unresolved !label. WTF?";
		break;

	case 10:
		s = "Invalid Interrupt Call";
		break;

	case 11:
		s = "Invalid Port Access";
		break;

	case 12:
		s = "Com Queue empty";
		break;

	case 13:
		s = "No mine-layer, silly.";
		break;

	case 14:
		s = "No mines left";
		break;

	case 15:
		s = "No shield installed - Arm the photon torpedoes instead. :/";
		break;

	case 16:
		s = "Invalid Microcode in instruction.";
		break;

	default:
		s = "Unknown error.";
	}
	robot[n]->errorlog
		<< " <" << i << "> " << s << " (Line #" << robot[n]->ip << ") [Cycle: " << to_string(game_cycle) << ", Match: " << to_string(played) << "/" << to_string(matches) <<
		"]" << endl;
	robot[n]->errorlog << " " << mnemonic(robot[n]->code[robot[n]->ip].op[0], robot[n]->code[robot[n]->ip].op[3] & 15) << "  " <<
		operand(robot[n]->code[robot[n]->ip].op[1], (robot[n]->code[robot[n]->ip].op[3] >> 4) & 15) << ", " <<
		operand(robot[n]->code[robot[n]->ip].op[2], (robot[n]->code[robot[n]->ip].op[3] >> 8) & 15) << endl;
	if (ov.compare("") == 0)
		robot[n]->errorlog << "    (Values: " << ov << ")" << endl;
	else
		robot[n]->errorlog << " ";

	robot[n]->errorlog << " AX=" << addrear(cstr(robot[n]->ram[65]) + ",", 7) << endl;
	robot[n]->errorlog << " BX=" << addrear(cstr(robot[n]->ram[66]) + ",", 7) << endl;
	robot[n]->errorlog << " CX=" << addrear(cstr(robot[n]->ram[67]) + ",", 7) << endl;
	robot[n]->errorlog << " DX=" << addrear(cstr(robot[n]->ram[68]) + ",", 7) << endl;
	robot[n]->errorlog << " EX=" << addrear(cstr(robot[n]->ram[69]) + ",", 7) << endl;
	robot[n]->errorlog << " FX=" << addrear(cstr(robot[n]->ram[70]) + ",", 7) << endl;
	robot[n]->errorlog << " Flags = " << robot[n]->ram[64] << endl;
	robot[n]->errorlog << " AX=" << addrear(hex(robot[n]->ram[65]) + ",", 7) << endl;
	robot[n]->errorlog << " BX=" << addrear(hex(robot[n]->ram[66]) + ",", 7) << endl;
	robot[n]->errorlog << " CX=" << addrear(hex(robot[n]->ram[67]) + ",", 7) << endl;
	robot[n]->errorlog << " DX=" << addrear(hex(robot[n]->ram[68]) + ",", 7) << endl;
	robot[n]->errorlog << " EX=" << addrear(hex(robot[n]->ram[69]) + ",", 7) << endl;
	robot[n]->errorlog << " FX=" << addrear(hex(robot[n]->ram[70]) + ",", 7) << endl;
	robot[n]->errorlog << " Flags = " << hex(robot[n]->ram[64]) << endl;

	// err_log << "End log_error" << endl << endl;

	return;
}

void prog_error(int16_t n, std::string ss) {
	// err_log << "Begin prog_error" << endl;
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

	// err_log << "End prog_error" << endl << endl;
	exit(EXIT_FAILURE);

}


void parse1(int32_t n, int32_t p, string * s) {
	// err_log << "Begin parse1" << endl;
	int32_t i, j, opcode, microcode;
	bool found, indirect;
	std::string ss;

	for (i = 0; i <= MAX_OP - 1; i++) {
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

		// err_log << "\tChecking for braces" << endl;
		if (!(lstr(s[i], 1).compare("[")) && !(rstr(s[i], 1).compare("]"))) {
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
						}
						else {
							opcode = j; //unresolved label
							microcode = 3;
						}
					}
					if (!found) {
						numlabels++;
						if (numlabels > MAX_LABELS) {
							prog_error(15, "");
						}
						else {
							labelname[numlabels] = ss;
							labelnum[numlabels] = -1;
							opcode = numlabels;
							microcode = 3; //unresolved label
							found = true;
						}
					}
				}
			}

			if ((numvars > 0) && !found) {
				for (j = 1; j <= numvars; j++) {
					if (!s[i].compare(varname[j])) {
						opcode = varloc[j];
						microcode = 1;
						found = true;
					}
					if (!s[i].compare("NOP")) {
						opcode = 0;
						found = true;
					}
					else if (!s[i].compare("ADD")) {
						opcode = 1;
						found = true;
					}
					else if (!s[i].compare("SUB")) {
						opcode = 2;
						found = true;
					}
					else if (!s[i].compare("OR")) {
						opcode = 3;
						found = true;
					}
					else if (!s[i].compare("AND")) {
						opcode = 4;
						found = true;
					}
					else if (!s[i].compare("XOR")) {
						opcode = 5;
						found = true;
					}
					else if (!s[i].compare("NOT")) {
						opcode = 6;
						found = true;
					}
					else if (!s[i].compare("MPY")) {
						opcode = 7;
						found = true;
					}
					else if (!s[i].compare("DIV")) {
						opcode = 8;
						found = true;
					}
					else if (!s[i].compare("MOD")) {
						opcode = 9;
						found = true;
					}
					else if (!s[i].compare("RET")) {
						opcode = 10;
						found = true;
					}
					else if (!s[i].compare("RETURN")) {
						opcode = 10;
						found = true;
					}
					else if (!s[i].compare("GSB")) {
						opcode = 11;
						found = true;
					}
					else if (!s[i].compare("GOSUB")) {
						opcode = 11;
						found = true;
					}
					else if (!s[i].compare("CALL")) {
						opcode = 11;
						found = true;
					}
					else if (!s[i].compare("JMP")) {
						opcode = 12;
						found = true;
					}
					else if (!s[i].compare("JUMP")) {
						opcode = 12;
						found = true;
					}
					else if (!s[i].compare("GOTO")) {
						opcode = 12;
						found = true;
					}
					else if (!s[i].compare("JLS")) {
						opcode = 13;
						found = true;
					}
					else if (!s[i].compare("JB")) {
						opcode = 13;
						found = true;
					}
					else if (!s[i].compare("JGR")) {
						opcode = 14;
						found = true;
					}
					else if (!s[i].compare("JA")) {
						opcode = 14;
						found = true;
					}
					else if (!s[i].compare("JNE")) {
						opcode = 15;
						found = true;
					}
					else if (!s[i].compare("JEQ")) {
						opcode = 16;
						found = true;
					}
					else if (!s[i].compare("JE")) {
						opcode = 16;
						found = true;
					}
					else if (!s[i].compare("XCHG")) {
						opcode = 17;
						found = true;
					}
					else if (!s[i].compare("SWAP")) {
						opcode = 17;
						found = true;
					}
					else if (!s[i].compare("DO")) {
						opcode = 18;
						found = true;
					}
					else if (!s[i].compare("LOOP")) {
						opcode = 19;
						found = true;
					}
					else if (!s[i].compare("CMP")) {
						opcode = 20;
						found = true;
					}
					else if (!s[i].compare("TEST")) {
						opcode = 21;
						found = true;
					}
					else if (!s[i].compare("SET")) {
						opcode = 22;
						found = true;
					}
					else if (!s[i].compare("MOV")) {
						opcode = 22;
						found = true;
					}
					else if (!s[i].compare("LOC")) {
						opcode = 23;
						found = true;
					}
					else if (!s[i].compare("ADDR")) {
						opcode = 23;
						found = true;
					}
					else if (!s[i].compare("GET")) {
						opcode = 24;
						found = true;
					}
					else if (!s[i].compare("PUT")) {
						opcode = 25;
						found = true;
					}
					else if (!s[i].compare("INT")) {
						opcode = 26;
						found = true;
					}
					else if (!s[i].compare("IPO")) {
						opcode = 27;
						found = true;
					}
					else if (!s[i].compare("IN")) {
						opcode = 27;
						found = true;
					}
					else if (!s[i].compare("OPO")) {
						opcode = 28;
						found = true;
					}
					else if (!s[i].compare("OUT")) {
						opcode = 28;
						found = true;
					}
					else if (!s[i].compare("DEL")) {
						opcode = 29;
						found = true;
					}
					else if (!s[i].compare("DELAY")) {
						opcode = 29;
						found = true;
					}
					else if (!s[i].compare("PUSH")) {
						opcode = 30;
						found = true;
					}
					else if (!s[i].compare("POP")) {
						opcode = 31;
						found = true;
					}
					else if (!s[i].compare("ERR")) {
						opcode = 32;
						found = true;
					}
					else if (!s[i].compare("ERROR")) {
						opcode = 32;
						found = true;
					}
					else if (!s[i].compare("INC")) {
						opcode = 33;
						found = true;
					}
					else if (!s[i].compare("DEC")) {
						opcode = 34;
						found = true;
					}
					else if (!s[i].compare("SHL")) {
						opcode = 35;
						found = true;
					}
					else if (!s[i].compare("SHR")) {
						opcode = 36;
						found = true;
					}
					else if (!s[i].compare("ROL")) {
						opcode = 37;
						found = true;
					}
					else if (!s[i].compare("ROR")) {
						opcode = 38;
						found = true;
					}
					else if (!s[i].compare("JZ")) {
						opcode = 39;
						found = true;
					}
					else if (!s[i].compare("JNZ")) {
						opcode = 40;
						found = true;
					}
					else if (!s[i].compare("JAE")) {
						opcode = 41;
						found = true;
					}
					else if (!s[i].compare("JGE")) {
						opcode = 41;
						found = true;
					}
					else if (!s[i].compare("JLE")) {
						opcode = 42;
						found = true;
					}
					else if (!s[i].compare("JBE")) {
						opcode = 42;
						found = true;
					}
					else if (!s[i].compare("SAL")) {
						opcode = 43;
						found = true;
					}
					else if (!s[i].compare("SAR")) {
						opcode = 44;
						found = true;
					}
					else if (!s[i].compare("NEG")) {
						opcode = 45;
						found = true;
					}
					else if (!s[i].compare("JTL")) {
						opcode = 46;
						found = true;
					}

					//Registers
					else if (!s[i].compare("COLCNT")) {
						opcode = 8;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("METERS")) {
						opcode = 9;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("COMBASE")) {
						opcode = 10;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("COMEND")) {
						opcode = 11;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("FLAGS")) {
						opcode = 64;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("AX")) {
						opcode = 65;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("BX")) {
						opcode = 66;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("CX")) {
						opcode = 67;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("DX")) {
						opcode = 68;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("EX")) {
						opcode = 69;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("FX")) {
						opcode = 70;
						microcode = 1;
						found = true;
					}
					else if (!s[i].compare("SP")) {
						opcode = 71;
						microcode = 1;
						found = true;
					}

					//Constants
					else if (!s[i].compare("MAXINT")) {
						opcode = 32767;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("MININT")) {
						opcode = -32768;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_SPEDOMETER")) {
						opcode = 1;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_HEAT")) {
						opcode = 2;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_COMPASS")) {
						opcode = 3;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_TANGLE")) {
						opcode = 4;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_TURRET_OFS")) {
						opcode = 4;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_THEADING")) {
						opcode = 5;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_TURRET_ABS")) {
						opcode = 5;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_ARMOR")) {
						opcode = 6;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_DAMAGE")) {
						opcode = 6;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_SCAN")) {
						opcode = 7;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_ACCURACY")) {
						opcode = 8;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_RADAR")) {
						opcode = 9;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_RANDOM")) {
						opcode = 10;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_RAND")) {
						opcode = 10;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_THROTTLE")) {
						opcode = 11;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_TROTATE")) {
						opcode = 12;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_OFS_TURRET")) {
						opcode = 12;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_TAIM")) {
						opcode = 13;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_ABS_TURRET")) {
						opcode = 13;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_STEERING")) {
						opcode = 14;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_WEAP")) {
						opcode = 15;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_WEAPON")) {
						opcode = 15;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_FIRE")) {
						opcode = 15;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_SONAR")) {
						opcode = 16;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_ARC")) {
						opcode = 17;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_SCANARC")) {
						opcode = 17;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_OVERBURN")) {
						opcode = 18;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_TRANSPONDER")) {
						opcode = 19;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_SHUTDOWN")) {
						opcode = 20;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_CHANNEL")) {
						opcode = 21;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_MINELAYER")) {
						opcode = 22;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_MINETRIGGER")) {
						opcode = 23;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_SHIELD")) {
						opcode = 24;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("P_SHIELDS")) {
						opcode = 24;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_DESTRUCT")) {
						opcode = 0;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_RESET")) {
						opcode = 1;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_LOCATE")) {
						opcode = 2;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_KEEPSHIFT")) {
						opcode = 3;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_OVERBURN")) {
						opcode = 4;
						microcode = 0;
						found = true;
					}
					if (!s[i].compare("I_ID")) {
						opcode = 5;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_TIMER")) {
						opcode = 6;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_ANGLE")) {
						opcode = 7;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_TID")) {
						opcode = 8;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_TARGETID")) {
						opcode = 8;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_TINFO")) {
						opcode = 9;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_TARGETINFO")) {
						opcode = 9;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_GINFO")) {
						opcode = 10;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_GAMEINFO")) {
						opcode = 10;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_RINFO")) {
						opcode = 11;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_ROBOTINFO")) {
						opcode = 11;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_COLLISIONS")) {
						opcode = 12;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_RESETCOLCNT")) {
						opcode = 13;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_TRANSMIT")) {
						opcode = 14;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_RECEIVE")) {
						opcode = 15;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_DATAREADY")) {
						opcode = 16;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_CLEARCOM")) {
						opcode = 17;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_KILLS")) {
						opcode = 18;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_DEATHS")) {
						opcode = 18;
						microcode = 0;
						found = true;
					}
					else if (!s[i].compare("I_CLEARMETERS")) {
						opcode = 19;
						microcode = 0;
						found = true;
					}

					//Memory addresses
					if (!found && (s[i][0] == '@') && ((s[i][1] >= '0') && (s[i][1] <= '9'))) {
						opcode = str2int((rstr(s[i], (uint16_t)s[i].length() - 1)));
						if (opcode < 0 || opcode >(MAX_RAM + 1) + (((MAX_CODE + 1) << 3) - 1)) {
							prog_error(3, s[i]);
							microcode = 1;
							found = true;
						}
					}

					//Numbers
					if (!found && ((s[i][0] == '-') || ((s[i][0] >= '0') && (s[i][0] <= '9')))) {
						opcode = str2int(s[i]);
						found = true;
					}

					if (found) {
						robot[n]->code[p].op[i] = opcode;
						if (indirect) {
							microcode = microcode | 8;
						}

						robot[n]->code[p].op[MAX_OP] = robot[n]->code[p].op[MAX_OP] | (microcode << (i * 4));
					}
					else if (s[i].compare(""))
						prog_error(2, s[i]);
				}
			}
			if (show_code)
				print_code(n, p);
			if (compile_by_line)
				readkey();
		}
	}

	// err_log << "End parse1" << endl << endl;
}

void compile(int16_t n, std::string filename) {
	// err_log << "Begin compile" << endl;
	std::cout << "begin compile" << endl;
	fstream f;
	string pp[MAX_OP + 1];
	std::string s, s1, s2, s3, orig_s, msg;
	int16_t i, j, k, l, linecount, mask, locktype;
	std::string ss[MAX_OP];
	char c, lc;

	lock_code = "";
	lock_pos = 0;
	locktype = 0;
	lock_dat = 0;
	//Needs to be a filestream in the main function

	if (!exist(filename)) {

		prog_error(8, filename);
	}

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
			for (i = 0; (uint16_t)i < s.length(); i++) {
				lock_pos++;
				if ((uint16_t)lock_pos > lock_code.length())
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

		for (i = 0; (uint16_t)i < s.length(); i++) {
			if (((s[i] >= 0) && (s[i] <= 32)) || ((s[i] >= 128) && (s[i] <= 255)))
				s.insert(i, " ");
		}

		if (show_source && ((!lock_code.compare("")) || debugging_compiler))
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
				for (i = 0; (uint16_t)i < s1.length(); i++)
					if ((k == 0) && (s1[i] == ' '))
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
					if (!s2.compare("DEF") && numvars < MAX_VARS) {
						if (s3.length() > MAX_VAR_LEN) {
							prog_error(14, "");
						}
						else {
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
					}
					else if (!lstr(s2, 4).compare("LOCK")) {
						robot[n]->is_locked = true;
						if (s2.length() > 4)
							locktype = value(rstr(s2, s2.length() - 4));
						lock_code = btrim(ucase(s3));
						std::cout << "Robot is of LOCKed format from this point32_tforward. [" << locktype << "]" << endl;
						for (i = 1; (uint16_t)i < lock_code.length(); i++) {
							lock_code[i] = char(lock_code[i] - 65);
						}
					}
					else if (!s2.compare("MSG"))
						robot[n]->name = msg;
					else if (!s2.compare("TIME")) {
						robot[n]->robot_time_limit = value(s3);
						if (robot[n]->robot_time_limit < 0)
							robot[n]->robot_time_limit = 0;

					}
					else if (!s2.compare("CONFIG")) {
						if (!lstr(s3, 8).compare("SCANNER="))
							robot[n]->config.scanner = value(rstr(s3, s3.length() - 8));

						else if (!lstr(s3, 7).compare("SHIELD="))
							robot[n]->config.shield = value(rstr(s3, s3.length() - 7));
						else if (!lstr(s3, 7).compare("WEAPON="))
							robot[n]->config.weapon = value(rstr(s3, s3.length() - 7));
						else if (!lstr(s3, 6).compare("ARMOR="))
							robot[n]->config.armor = value(rstr(s3, s3.length() - 6));
						else if (!lstr(s3, 7).compare("ENGINE="))
							robot[n]->config.engine = value(rstr(s3, s3.length() - 7));
						else if (!lstr(s3, 10).compare("HEATSINKS="))
							robot[n]->config.heatsinks = value(rstr(s3, s3.length() - 10));
						else if (!lstr(s3, 6).compare("MINES="))
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
					}
					else
						std::cout << "WARNING: unknown directive '" << s2 << "' " << endl;

				}
				break;
			case '*':
				check_plen(robot[n]->plen);
				for (i = 0; i < MAX_OP; i++)
					pp[i] = "";
				for (i = 1; (uint16_t)i < s.length(); i++)
					if (s[i] == '*')
						prog_error(23, s);
				k = 0;
				i = 1;
				s1 = "";
				if (s.length() <= 2)
					prog_error(23, s);
				while (((uint16_t)i < s.length()) && (k <= MAX_OP)) {
					i++;
					if (((s[i] >= 33) && (s[i] <= 41)) || ((s[i] >= 43) && (s[i] <= 127)))
						pp[k] = pp[k] + s[i];
					else if ((((s[i] >= 0) && (s[i] <= 32)) || ((s[i] >= 128) && (s[i] <= 255))) &&
						(((s[i - 1] >= 33) && (s[i - 1] <= 41)) || ((s[i - 1] >= 43) && (s[i - 1] <= 127))))
					{
						k++;
					}
				}
				for (i = 0; i <= MAX_OP; i++)
					robot[n]->code[robot[n]->plen].op[i] = value(pp[i]);
				robot[n]->plen++;
				break;
			case ':':
				check_plen(robot[n]->plen);
				s1 = rstr(s, s.length() - 1);
				for (i = 0; (uint16_t)i <= s1.length(); i++)
					if (!((s1[i] >= '0') && (s[i] <= '9')))
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
				for (i = s1.length() - 1; i >= 0; i--) {
					if ((s1[i] == ';') || ((s1[i] >= 8) && (s1[i] <= 10)) || (s1[i] == ' ') || (s1[i] == '.'))
						k = i;
				}
				if (k > 0)
					s1 = lstr(s1, k - 1);
				k = 0;
				for (i = 0; i <= numlabels; i++) {
					if (!labelname[i].compare(s1.c_str())) {
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
				for (i = s.length() - 1; i >= 0; i--)
					if (s[i] == ';')
						k = i;
				if (k > 0)
					s = lstr(s, k - 1);
				//setup variables for parsing
				k = 0;
				for (j = 0; j <= MAX_OP; j++)
					pp[j] = "";
				for (j = 0; (uint16_t)j < s.length(); j++) {
					c = s[j];
					if ((!((c == '[') || (c == 8) || (c == 9) || (c == 10) || (c == '.'))) && k <= MAX_OP)
						pp[k] = pp[k] + c;
					else if (!((lc == ' ') || (lc == '8') || (lc == '9') || (lc == 10) || (lc == '.'))) {
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
		}
		else {
			robot[n]->plen--;
		}

		//second pass, resolving !labels
		if (numlabels > 0) {
			for (i = 0; i <= robot[n]->plen; i++) {
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
					}
					else {
						prog_error(17, cstr(k));
					}
				}
			}
		}
	}
	textcolor(7);

	// err_log << "End compile" << endl << endl;
}

int main(int argc, char ** argv) {
	paramcount = argc;
	paramstr = argv;


	std::cout << "start init" << endl;
	init();
	std::cout << "ened init" << endl;
	std::cout << "start true main" << endl;
	true_main();
	std::cout << "end true main" << endl;
	shutdown();

	return EXIT_SUCCESS;
}

// Just to dynamically allocate new memory (SUPER out of place, to avoid an error)
robot_rec::robot_rec() {
}
