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
}

struct op_rec prog_type[MAX_CODE];

struct config_rec {
	short scanner, weapon, armor, engine, heatsinks, shield, mines;
}

struct mine_rec {
	double x, y;
	short detect, yield;
	bool detonate;
}

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


