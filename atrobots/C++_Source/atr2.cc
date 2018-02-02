
#include <cstdio>	// Required for file manipulation
#include <cstdlib>

#include "Headers/filelib.h"
#include "Headers/myfile.h"
#include "Headers/at2func.h"

// These probably aren't the correct headers, but these are needed to be included
#include "SDL2"
#include "SDL2_TTF"


// Beginning of global variables

string progname		= "AT-Robots";
string version		= "2.11";
string cnotice1		= "Copyright 1997 ''99, Ed T. Toton III";
string cnotice2		= "All Rights Reserved.";
string cnotice3		= "Copyright 2014, William "Amos" Confer";
string main_filename	= "ATR2";
string robot_ext	= ".AT2";
string locked_ext	= ".ATL";
string config_ext	= ".ATS";
string compile_ext	= ".CMP";
string report_ext	= ".REP";

bool T		= true;
bool F		= false;
short minint	= -32768;

// {debugging/compiler}
bool show_code		= F;
bool compile_by_line	= F;
char max_var_len	= 16;
bool debugging_compiler = F;

// {robots}
char max_robots		= 31; 	// {starts at 0, so total is max_robots+1}
short max_code		= 1023; // {same here}
char max_op		= 3;	// {etc...}
short stack_size	= 256;   
short stack_base	= 768;
short max_ram		= 1023; // {but this does start at 0 (odd #, 2^n-1)}
short max_vars		= 256;
short max_labels	= 256;
char acceleration	= 4;
char turn_rate		= 8;
char max_vel		= 4;
short max_missiles	= 1023;
char missile_spd	= 32;
char hit_range		= 14;
char blast_radius	= 25;
char crash_range	= 8;
unsigned char max_sonar	= 250;
short com_queue		= 512;
unsigned char max_queue	= 255;
char max_config_points	= 12;
char max_mines		= 63;
char mine_blast		= 35;

// {simulator & graphics}
float screen_scale	= 0.46;
char screen_x		= 5;
char screen_y		= 5;
char robot_scale	= 6;
char default_delay	= 20;
char default_slice	= 5;
short mine_circle	= (mine_blast * screen_scale) + 1;
short blast_circle	= (blast_radius * screen_scale) + 1;
short mis_radius	= (hit_range / 2) + 1;
char max_robot_lines	= 8;

// To compensate for the viewport

// Global SDL related items
SDL_Window	*main_window;
SDL_Renderer	*main_renderer;
SDL_Color	*text_color;
Uint8		*keyboard_state;
unsigned char	keypressed;


TTF_Font	*main_font;




/* Screen overlay? Creates 50% grayscale, saved to variable gray50. Checkerboard
 Gray50 : FillPatternType = ($AA, $55, $AA, $55,
                             $AA, $55, $AA, $55);
*/

struct op_rec {
	short op[max_op];
};
 
struct prog_type {
	struct op_rec[max_code];
}

struct config_rec {
	short scanner, weapon, armor, engine, heatsinks, shield, mines;
}

struct mine_rec {
	double x, y;
	short detect, yield;
	bool detonate;
};

struct robot_rec {
	bool is_ocked;	// {used to determine wether to allow debugger}
	bool mem_watch;	// {current base of memory view for debugger}

	double x, y, lx, ly, xv, yv, speed, shotstrength, damageadj, speedadj, meters;

	short hd, thd, lhd, spd, tspd, armor, larmor, heat, lheat, ip, plen, scanarc,
		accuracy, shift, err, delay_left, robot_time_limit, max_time, time_left,
		lshift, arc_count, sonar_count, scanrange, last_damage, last_hit, transponder,
		shutdown, channel, lendarc, endarc, lstartarc, startarc, mines;

	short tx[max_robot_lines+1], ltx[max_robot_lines+1], ty[max_robot_lines+1], lty[max_robot_lines+1];

	short wins, trials, kills, deaths, startkills, shots_fired, match_shots, hits, damage_total,
		cycles_lived, error_count;

	struct config_rec config;
	string name[31];	// Was char *name[31]
	string fn;		// Was char[255]
	bool shields_up, lshields, overburn, keepshift, cooling, won;
	struct prog_type code;
	short ram[max_ram];
	struct mine_rec mine[max_mines];

	fstream errorlog;	// Was FILE * 
};

string parsetype[16];
struct robot_rec * robot_ptr	= robot_rec;

struct missile_rec {
	double x, y, lx, ly, mult, mspd;
	short source, a, hd, rad, lrad, max_rad;
};


// {--robot variables--}
short num_robots;
struct robot_ptr robot[max_robots+4]; // { a few extra pointers for luck.. :) } EDIT: I SHIFTED THE ARRAY INDEXES. IT USED TO START AT -2, NOW IT STARTS AT 0 -MZ
struct missile_rec missile[max_missiles];

// {--compiler variables--}
fstream	f;
short	numvars, numlabels, maxcode, lock_pos, lock_dat;
char	varname[max_var_len][max_vars];
short	varloc[max_vars];
char	labelname[max_var_len][max_vars];
short	labelnum[max_labels];
bool	show_source, compile_only;
string	lock_code;

// {--simulator/graphics variables--}
bool	bout_over;	//	{made global from procedure bout}
short	step_mode;	//	{0=off, for (0<step_mode<=9) = #of game cycles per step}
short	temp_mode;	//	{stores previous step_mode for return to step}
short 	step_count;	//	{step counter used as break flag}
bool	step_loop;	//	{break flag for stepping}

 bool show_debugger; // {flag for viewing debugger panel vs. robot stats}

bool	old_shields, insane_missiles, debug_info, windoze, no_gfx, logging_errors,
	timing, show_arcs;
short	game_delay, time_slice, insanity, update_timer, max_gx, max_gy, stats_mode;
short	game_limit, game_cycle;
short	matches, played, executed;

// {--general settings--}
bool	quit, report, show_cnotice;
short	kill_count, report_type;



char * operand(short n, short m) {
	string s;

	// cstr converts number to string	
	s = cstr(n);
     	/*
     	Microcode:
		0 = instruction, number, constant
		1 = variable, memory access
		2 = :label
		3 = !label (unresolved)
		4 = !label (resolved)
		8h mask = inderect addressing (enclosed in [])
     	*/
	switch ( m & 7 ) {
		case 1:
			s = "@" + s;
			break;

		case 2:
			s = ":" + s;
			break;

		case 3:
			s = "$" + s;
			break;

		case 4:
			s = "!" + s;
			break;

		default:
			s = cstr(n);
	}

	if ( (m & 8) > 0 ) then
		s = "[" + s + "]";

	return(s);
}

char * mnemonic(short n, short m) {

	string s;

	s = cstr(n);

	char *text[] { "NOP", "ADD", "OR", "AND", "SUB", "OR", "AND", "XOR", 
		"NOT", "MPY", "DIV", "MOD", "RET", "CALL", "JMP", "JLS", "JGR",
		"JNE", "JE", "SWAP", "DO", "LOOP", "CMP", "TEST", "MOV", "LOC",
		"GET", "PUT", "INT", "IPO", "OPO", "DELAY", "PUSH", "POP", "ERR",
		"INC", "DEC", "SHL", "SHR", "ROL", "ROR", "JZ", "JNZ", "JGE",
		"JLE", "SAL", "SAR", "NEG", "JTL"};



	if ( m == 0 ) {
		if ( n < 49 )
			s = text[n];
		else
			s = "XXX";
	} else
		s = operand(n, m);

	return (s);
}
/* Do not need this tyoe of assembly error checking at this point in time
procedure log_error(n,i:integer;ov:string);
	short j, k;
	string s;

	if ( !logging_errors ) 
		exit;
 with robot[n]^ do
  begin
   case i of
    01:s = 'Stack full - Too many CALLs?';
    02:s = 'Label not found. Hmmm.';
    03:s = 'Can''t assign value - Tisk tisk.';
    04:s = 'Illegal memory reference';
    05:s = 'Stack empty - Too many RETs?';
    06:s = 'Illegal instruction. How bizarre.';
    07:s = 'Return out of range - Woops!';
    08:s = 'Divide by zero';
    09:s = 'Unresolved !label. WTF?';
    10:s = 'Invalid Interrupt Call';
    11:s = 'Invalid Port Access';
    12:s = 'Com Queue empty';
    13:s = 'No mine-layer, silly.';
    14:s = 'No mines left';
    15:s = 'No shield installed - Arm the photon torpedoes instead. :-)';
    16:s = 'Invalid Microcode in instruction.';
    else s = 'Unkown error';
   end;
   writeln(errorlog,'<',i,'> ',s,' (Line #',ip,') [Cycle: ',game_cycle,', Match: ',played,'/',matches,']');
   write(errorlog,' ',mnemonic(code[ip].op[0],code[ip].op[3] and 15),'  ',
                      operand(code[ip].op[1],(code[ip].op[3] shr 4) and 15),', ',
                      operand(code[ip].op[2],(code[ip].op[3] shr 8) and 15));
   if ov<>'' then writeln(errorlog,'    (Values: ',ov,')')
             else writeln(errorlog);
   write(errorlog,' AX=',addrear(cstr(ram[65])+',',7));
   write(errorlog,' BX=',addrear(cstr(ram[66])+',',7));
   write(errorlog,' CX=',addrear(cstr(ram[67])+',',7));
   write(errorlog,' DX=',addrear(cstr(ram[68])+',',7));
   write(errorlog,' EX=',addrear(cstr(ram[69])+',',7));
   write(errorlog,' FX=',addrear(cstr(ram[70])+',',7));
   writeln(errorlog,' Flags=',ram[64]);
   write(errorlog,' AX=',addrear(hex(ram[65])+',',7));
   write(errorlog,' BX=',addrear(hex(ram[66])+',',7));
   write(errorlog,' CX=',addrear(hex(ram[67])+',',7));
   write(errorlog,' DX=',addrear(hex(ram[68])+',',7));
   write(errorlog,' EX=',addrear(hex(ram[69])+',',7));
   write(errorlog,' FX=',addrear(hex(ram[70])+',',7));
   writeln(errorlog,' Flags=',hex(ram[64]));
   writeln(errorlog);
  end;
end;
*/

short max_shown {
	switch (stats_mode) {
	case 1:
		return 12;
		break;

	case 2
		return 32;
		break;

	default:
		return 6;
	}
}

bool graph_check(short n) {
	bool ok;
	ok = true;

	if ( !graphix || (n<0) || (n > num_robots) || (n >= max_shown) )
		ok = false;

	return ok;
}

void robot_graph(short n) {
	int rgb;

	switch (stats_mode) {
		case 1:
			set_viewport(480, 4+n*35, 635, 37+n*35);
			max_gx = 155;
			max_gy = 33;
			break;

		case 2:
			set_viewport(480, 4+n*13, 635, 15+n*13);
			max_gx = 155;
			max_gy = 11;
			break;

		default:
			set_viewport(480, 4+n*70, 635, 70+n*70);
			max_gx = 155;
			max_gy = 66;
	}
	rgb = robot_color(n);
	SDL_SetRenderDrawColor((rgb>>24), ((rgb<<8)>>24), ((rgb<<16)>>24), 0xff);	
	setcolor_render(robot_color(n));
}

void update_armor(short n) {
	if (graph_check(n) && (step_mode<=0) ) {
	robot_graph(n);
	if (robot[n] -> armor > 0) {
		switch (stats_mode) {
     		case 1:
			bar(30, 13, 29 + robot[n]->armor, 18);
			break;

		case 2:
			bar(88, 3, 87 + (robot[n]->armor >> 2), 8);
			break;

		default:
			bar(30, 25, 29 + robot[n]->armor, 30);
		}
	}

	setcolor(8);
	SDL_SetRenderDrawColor(120, 120, 120, 120);

	if ( armor < 100 )
		switch (stats_mode) {
     		case 1:
			bar(30 + robot[n]->armor, 13, 129, 18);
			break;

		case 2:
			bar(88+(robot[n]->armor >> 2), 3 , 111, 8);
			break;

		default:
			bar( 30 + robot[n]->armor, 25, 129, 30);
		}
	}
}

void update_heat(short n) {
	if (graph_check(n) && (step_mode <= 0)) {
		robot_graph(n);
		if (robot[n]->heat > 5)
			switch (stats_mode)
			case 1:
				bar(30, 23, 29 + (robot[n]->heat / 5), 28);
				break;

			case 2:
				bar(127, 3, 126+(robot[n]->heat / 20), 8);
				break;

			default:
				bar(30, 35, 29+(robot[n]->heat / 5), 40);
			}

		SDL_SetRenderDrawColor(120, 120, 120, 255);

		if (robot[n]->heat < 500)
			switch (stats_mode) {
			case 1:
				bar(30+(robot[n]->heat / 5), 23, 129, 28);
				break;

			case 2:
				bar(127+(robot[n]->heat / 20), 3, 151, 8);
				break;

			default:
				bar(30+(robot[n]->heat / 5), 35, 129, 40);
			}
	}
}

/*
procedure robot_error(n,i:integer;ov:string);
begin
 if graph_check(n) {FIFI} and (step_mode<=0) {/FIFI} then
 with robot[n]^ do
  begin
   if (stats_mode=0) then
    begin
     robot_graph(n);
     setfillstyle(1,0);
     bar(66,56,154,64);
     setcolor(robot_color(n));
     outtextxy(66,56,addrear(cstr(i),7)+hex(i));
     chirp;
    end;
   if logging_errors then log_error(n,i,ov);
   inc(error_count);
  end;
end;
*/

void update_lives(short n) {
	if (graph_check(n) && (stats_mode = 0) && (step_mode <= 0) ) {
		robot_graph(n);
		setcolor_render(robot_color(n)-8);
		SDL_SetRenderDrawColor(0x0, 0x0, 0x0, 0xff); // Black

		bar(11, 46, 130, 53);
		outtextxy(11, 46,"K:");
		outtextxy(29, 46, zero_pad(robot[n]->kills,4));
		outtextxy(80, 46,"D:");
		outtextxy(98, 46, zero_pad(robot[n]->deaths,4));
	}
}

void update_cycle_window ();
	if (!graphix)
		cout << #13+ << "match " << played << "/" << matches << " Cycle: " << zero_pad(game_cycle,9);
	else {
		set_viewport(480, 440, 635, 474);		
		SDL_SetRenderDrawColor(0x0, 0x0, 0x0, 0xff); // Solid fill, black

		bar(59, 2, 154, 10);

		setcolor(7);
		outtextxy(75,03,zero_pad(game_cycle,9));

		SDL_RenderPresent();
	}
}

void setscreen() {
	short i;

	if (!graphix)
		return;

	set_viewport(0, 0, 639, 479);

	box(0,0,639,479);

	stats_mode = 0;

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

	// main arena
	hole(4, 4, 475, 475);

	// cycle window
	set_viewport(480, 430, 635, 475);

	hole(0, 0, 155, 45);

	setcolor(7);

	outtextxy(3, 3, "FreeMem: " + cstr(memavail));
	outtextxy(3, 13, "Cycle:   ");
	outtextxy(3, 23, "Limit:   " + zero_pad(game_limit,9));
	outtextxy(3, 33, "Match:   "+cstr(played)+'/'+cstr(matches));

	update_cycle_window();

	// robot windows
	for (i = 0; i <= max_robots; i++) {
		if (i < max_shown) {
    			robot_graph(i);
    			hole(0  , 0  , max_gx, max_gy);
    			if (i <= num_robots) {
      				setcolor(robot_color(i));
      				outtextxy(3  , 2  , base_name(no_path(robot[i]->fn)));
      				switch (stats_mode)
       					case 1:
          					outtextxy(3  , 12  ," A:");
          					outtextxy(3  , 22  ," H:");
						break;
         				
       					case 2:
          					setcolor(robot[i]->robot_color(i) & 7);
          					outtextxy(80  , 2  ,"A");
          					outtextxy(118  , 2  ,"H");
         					break;

       					default:
          					outtextxy(3  , 24  , " A:");
          					outtextxy(3  , 34  , " H:");
				}
      				setcolor(robot[i]->robot_color(i));
      				if (stats_mode <= 1) {
			        	outtextxy(80  , 2  , "Wins:");
        				outtextxy(122  , 2  , zero_pad(wins,4));
				}
      				if (stats_mode = 0) {
        				outtextxy(3  , 56  , " Error:");
        				setcolor(robot[i]->robot_color(i) & 7);
        				outtextxy(3  , 12  , robot[i]->name);
        				setcolor(8);
        				outtextxy(66  , 56  , "None");
				}
      				robot[i] -> lx = 1000 - robot[i] -> x;
				robot[i] -> ly = 1000 - robot[i] -> y;
      				robot[i] -> update_armor(i);
      				robot[i] -> update_heat(i);
      				robot[i] -> update_lives(i);
    			} else {
				SDL_SetRenderDrawColor(0x80, 0x80, 0x80, 0xff); // Gray50
				bar(1  , 1  , max_gx-1, max_gy-1);
			}
		}
	}
}

void graph_mode(bool on);
	if (on && ( (bool)!graphix)) {
		Graph_VGA();
		cleardevice();
		setscreen();
		graphix = true;
	} else
		if ( !on && (bool)graphix)
			closegraph();
			graphix = false;
}

/*
procedure prog_error(n:integer; ss:string);
var
 s:string;
begin
 graph_mode(false);
 textcolor(15);
 write('Error #',n,': ');
 case n of
  00:s = ss;(*user error*)
  01:s = 'Invalid :label - "'+ss+'", silly mortal.';
  02:s = 'Undefined identifier - "'+ss+'". A typo perhaps?';
  03:s = 'Memory access out of range - "'+ss+'"';
  04:s = 'Not enough robots for combat. Maybe we should just drive in circles.';
  05:s = 'Robot names and settings must be specified. An empty arena is no fun.';
  06:s = 'Config file not found - "'+ss+'"';
  07:s = 'Cannot access a config file from a config file - "'+ss+'"';
  08:s = 'Robot not found "'+ss+'". Perhaps you mistyped it?';
  09:s = 'Insufficient RAM to load robot: "'+ss+'"... This is not good.';
  10:s = 'Too many robots! We can only handle '+cstr(max_robots+1)+'! Blah.. limits are limits.';
  11:s = 'You already have a perfectly good #def for "'+ss+'", silly.';
  12:s = 'Variable name too long! (Max:'+cstr(max_var_len)+') "'+ss+'"';
  13:s = '!Label already defined "'+ss+'", silly.';
  14:s = 'Too many variables! (Var Limit: '+cstr(max_vars)+')';
  15:s = 'Too many !labels! (!Label Limit: '+cstr(max_labels)+')';
  16:s = 'Robot program too long! Boldly we simplify, simplify along...'+ss;
  17:s = '!Label missing error. !Label #'+ss+'.';
  18:s = '!Label out of range: '+ss;
  19:s = '!Label not found. '+ss;
  20:s = 'Invalid config option: "'+ss+'". Inventing a new device?';
  21:s = 'Robot is attempting to cheat; Too many config points ('+ss+')';
  22:s = 'Insufficient data in data statement: "'+ss+'"';
  23:s = 'Too many asterisks: "'+ss+'"';
  {FIFI}
  24:s = 'Invalid step count: "'+ss+'". 1-9 are valid conditions.';
  {/FIFI}
  25:s = '"'+ss+'"';
  else s = ss;
 end;
 writeln(s);
 writeln;
 halt;
end;


procedure print_code(n,p:integer);
var
 i:integer;
begin
  begin
   write(hex(p)+': ');
   for i = 0 to max_op do
    write(zero_pad(robot[n]^.code[p].op[i],5),' ');
   write(' =  ');
   for i = 0 to max_op do
    write(hex(robot[n]^.code[p].op[i]),'h ');
   writeln; writeln;
  end;
end;


procedure parse1(n,p:integer; s:parsetype);
var
 i,j,k,opcode,microcode:integer;
 found,indirect:boolean;
 ss:string;
begin
 with robot[n]^ do
  begin
   for i = 0 to max_op-1 do
    begin
     k = 0; found = false;
     opcode = 0;    {nop}
     microcode = 0; {instruction/constant}
     s[i] = btrim(ucase(s[i]));
     indirect = false;

     (*
     Microcode:
        0 = instruction, number, constant
        1 = variable, memory access
        2 = :label
        3 = !label (unresolved)
        4 = !label (resolved)
       8h mask = inderect addressing (enclosed in [])
     *)

     if s[i]='' then begin opcode = 0; microcode = 0; found = true; end;

     if (lstr(s[i],1)='[') and (rstr(s[i],1)=']') then
       begin
        s[i] = copy(s[i],2,length(s[i])-2);
        indirect = true;
       end;

     {!labels}
     if (not found) and (s[i][1]='!') then
      begin
       ss = s[i];
       ss = btrim(rstr(ss,length(ss)-1));
       if numlabels>0 then
        for j = 1 to numlabels do
         if ss=labelname[j] then
          begin
           found = true;
           if labelnum[j]>=0 then
             begin opcode = labelnum[j]; microcode = 4; {resolved !label} end
            else
             begin opcode = j; microcode = 3; {unresolved !label} end;
          end;
       if not found then
        begin
         inc(numlabels);
         if numlabels>max_labels then prog_error(15,'')
          else
           begin
            labelname[numlabels] = ss;
            labelnum[numlabels] = -1;
            opcode = numlabels;
            microcode = 3; {unresolved !label}
            found = true;
           end;
        end;
      end;

     {variables}
     if (numvars>0) and (not found) then
      for j = 1 to numvars do
       if s[i]=varname[j] then
        begin
         opcode = varloc[j];
         microcode = 1; {variable}
         found = true;
        end;

     {instructions}
     if s[i]='NOP'     then begin opcode = 000; found = true; end;
     if s[i]='ADD'     then begin opcode = 001; found = true; end;
     if s[i]='SUB'     then begin opcode = 002; found = true; end;
     if s[i]='OR'      then begin opcode = 003; found = true; end;
     if s[i]='AND'     then begin opcode = 004; found = true; end;
     if s[i]='XOR'     then begin opcode = 005; found = true; end;
     if s[i]='NOT'     then begin opcode = 006; found = true; end;
     if s[i]='MPY'     then begin opcode = 007; found = true; end;
     if s[i]='DIV'     then begin opcode = 008; found = true; end;
     if s[i]='MOD'     then begin opcode = 009; found = true; end;
     if s[i]='RET'     then begin opcode = 010; found = true; end;
     if s[i]='RETURN'  then begin opcode = 010; found = true; end;
     if s[i]='GSB'     then begin opcode = 011; found = true; end;
     if s[i]='GOSUB'   then begin opcode = 011; found = true; end;
     if s[i]='CALL'    then begin opcode = 011; found = true; end;
     if s[i]='JMP'     then begin opcode = 012; found = true; end;
     if s[i]='JUMP'    then begin opcode = 012; found = true; end;
     if s[i]='GOTO'    then begin opcode = 012; found = true; end;
     if s[i]='JLS'     then begin opcode = 013; found = true; end;
     if s[i]='JB'      then begin opcode = 013; found = true; end;
     if s[i]='JGR'     then begin opcode = 014; found = true; end;
     if s[i]='JA'      then begin opcode = 014; found = true; end;
     if s[i]='JNE'     then begin opcode = 015; found = true; end;
     if s[i]='JEQ'     then begin opcode = 016; found = true; end;
     if s[i]='JE'      then begin opcode = 016; found = true; end;
     if s[i]='XCHG'    then begin opcode = 017; found = true; end;
     if s[i]='SWAP'    then begin opcode = 017; found = true; end;
     if s[i]='DO'      then begin opcode = 018; found = true; end;
     if s[i]='LOOP'    then begin opcode = 019; found = true; end;
     if s[i]='CMP'     then begin opcode = 020; found = true; end;
     if s[i]='TEST'    then begin opcode = 021; found = true; end;
     if s[i]='SET'     then begin opcode = 022; found = true; end;
     if s[i]='MOV'     then begin opcode = 022; found = true; end;
     if s[i]='LOC'     then begin opcode = 023; found = true; end;
     if s[i]='ADDR'    then begin opcode = 023; found = true; end;
     if s[i]='GET'     then begin opcode = 024; found = true; end;
     if s[i]='PUT'     then begin opcode = 025; found = true; end;
     if s[i]='INT'     then begin opcode = 026; found = true; end;
     if s[i]='IPO'     then begin opcode = 027; found = true; end;
     if s[i]='IN'      then begin opcode = 027; found = true; end;
     if s[i]='OPO'     then begin opcode = 028; found = true; end;
     if s[i]='OUT'     then begin opcode = 028; found = true; end;
     if s[i]='DEL'     then begin opcode = 029; found = true; end;
     if s[i]='DELAY'   then begin opcode = 029; found = true; end;
     if s[i]='PUSH'    then begin opcode = 030; found = true; end;
     if s[i]='POP'     then begin opcode = 031; found = true; end;
     if s[i]='ERR'     then begin opcode = 032; found = true; end;
     if s[i]='ERROR'   then begin opcode = 032; found = true; end;
     if s[i]='INC'     then begin opcode = 033; found = true; end;
     if s[i]='DEC'     then begin opcode = 034; found = true; end;
     if s[i]='SHL'     then begin opcode = 035; found = true; end;
     if s[i]='SHR'     then begin opcode = 036; found = true; end;
     if s[i]='ROL'     then begin opcode = 037; found = true; end;
     if s[i]='ROR'     then begin opcode = 038; found = true; end;
     if s[i]='JZ'      then begin opcode = 039; found = true; end;
     if s[i]='JNZ'     then begin opcode = 040; found = true; end;
     if s[i]='JAE'     then begin opcode = 041; found = true; end;
     if s[i]='JGE'     then begin opcode = 041; found = true; end;
     if s[i]='JLE'     then begin opcode = 042; found = true; end;
     if s[i]='JBE'     then begin opcode = 042; found = true; end;
     if s[i]='SAL'     then begin opcode = 043; found = true; end;
     if s[i]='SAR'     then begin opcode = 044; found = true; end;
     if s[i]='NEG'     then begin opcode = 045; found = true; end;
     if s[i]='JTL'     then begin opcode = 046; found = true; end;

     {registers}
     if s[i]='COLCNT'  then begin opcode = 008; microcode = 01; found = true; end;
     if s[i]='METERS'  then begin opcode = 009; microcode = 01; found = true; end;
     if s[i]='COMBASE' then begin opcode = 010; microcode = 01; found = true; end;
     if s[i]='COMEND'  then begin opcode = 011; microcode = 01; found = true; end;
     if s[i]='FLAGS'   then begin opcode = 064; microcode = 01; found = true; end;
     if s[i]='AX'      then begin opcode = 065; microcode = 01; found = true; end;
     if s[i]='BX'      then begin opcode = 066; microcode = 01; found = true; end;
     if s[i]='CX'      then begin opcode = 067; microcode = 01; found = true; end;
     if s[i]='DX'      then begin opcode = 068; microcode = 01; found = true; end;
     if s[i]='EX'      then begin opcode = 069; microcode = 01; found = true; end;
     if s[i]='FX'      then begin opcode = 070; microcode = 01; found = true; end;
     if s[i]='SP'      then begin opcode = 071; microcode = 01; found = true; end;

     {constants}
     if s[i]='MAXINT'    then begin opcode = 32767;  microcode = 0; found = true; end;
     if s[i]='MININT'    then begin opcode = -32768; microcode = 0; found = true; end;
     if s[i]='P_SPEDOMETER'  then begin opcode = 01; microcode = 0; found = true; end;
     if s[i]='P_HEAT'        then begin opcode = 02; microcode = 0; found = true; end;
     if s[i]='P_COMPASS'     then begin opcode = 03; microcode = 0; found = true; end;
     if s[i]='P_TANGLE'      then begin opcode = 04; microcode = 0; found = true; end;
     if s[i]='P_TURRET_OFS'  then begin opcode = 04; microcode = 0; found = true; end;
     if s[i]='P_THEADING'    then begin opcode = 05; microcode = 0; found = true; end;
     if s[i]='P_TURRET_ABS'  then begin opcode = 05; microcode = 0; found = true; end;
     if s[i]='P_ARMOR'       then begin opcode = 06; microcode = 0; found = true; end;
     if s[i]='P_DAMAGE'      then begin opcode = 06; microcode = 0; found = true; end;
     if s[i]='P_SCAN'        then begin opcode = 07; microcode = 0; found = true; end;
     if s[i]='P_ACCURACY'    then begin opcode = 08; microcode = 0; found = true; end;
     if s[i]='P_RADAR'       then begin opcode = 09; microcode = 0; found = true; end;
     if s[i]='P_RANDOM'      then begin opcode = 10; microcode = 0; found = true; end;
     if s[i]='P_RAND'        then begin opcode = 10; microcode = 0; found = true; end;
     if s[i]='P_THROTTLE'    then begin opcode = 11; microcode = 0; found = true; end;
     if s[i]='P_TROTATE'     then begin opcode = 12; microcode = 0; found = true; end;
     if s[i]='P_OFS_TURRET'  then begin opcode = 12; microcode = 0; found = true; end;
     if s[i]='P_TAIM'        then begin opcode = 13; microcode = 0; found = true; end;
     if s[i]='P_ABS_TURRET'  then begin opcode = 13; microcode = 0; found = true; end;
     if s[i]='P_STEERING'    then begin opcode = 14; microcode = 0; found = true; end;
     if s[i]='P_WEAP'        then begin opcode = 15; microcode = 0; found = true; end;
     if s[i]='P_WEAPON'      then begin opcode = 15; microcode = 0; found = true; end;
     if s[i]='P_FIRE'        then begin opcode = 15; microcode = 0; found = true; end;
     if s[i]='P_SONAR'       then begin opcode = 16; microcode = 0; found = true; end;
     if s[i]='P_ARC'         then begin opcode = 17; microcode = 0; found = true; end;
     if s[i]='P_SCANARC'     then begin opcode = 17; microcode = 0; found = true; end;
     if s[i]='P_OVERBURN'    then begin opcode = 18; microcode = 0; found = true; end;
     if s[i]='P_TRANSPONDER' then begin opcode = 19; microcode = 0; found = true; end;
     if s[i]='P_SHUTDOWN'    then begin opcode = 20; microcode = 0; found = true; end;
     if s[i]='P_CHANNEL'     then begin opcode = 21; microcode = 0; found = true; end;
     if s[i]='P_MINELAYER'   then begin opcode = 22; microcode = 0; found = true; end;
     if s[i]='P_MINETRIGGER' then begin opcode = 23; microcode = 0; found = true; end;
     if s[i]='P_SHIELD'      then begin opcode = 24; microcode = 0; found = true; end;
     if s[i]='P_SHIELDS'     then begin opcode = 24; microcode = 0; found = true; end;
     if s[i]='I_DESTRUCT'    then begin opcode = 00; microcode = 0; found = true; end;
     if s[i]='I_RESET'       then begin opcode = 01; microcode = 0; found = true; end;
     if s[i]='I_LOCATE'      then begin opcode = 02; microcode = 0; found = true; end;
     if s[i]='I_KEEPSHIFT'   then begin opcode = 03; microcode = 0; found = true; end;
     if s[i]='I_OVERBURN'    then begin opcode = 04; microcode = 0; found = true; end;
     if s[i]='I_ID'          then begin opcode = 05; microcode = 0; found = true; end;
     if s[i]='I_TIMER'       then begin opcode = 06; microcode = 0; found = true; end;
     if s[i]='I_ANGLE'       then begin opcode = 07; microcode = 0; found = true; end;
     if s[i]='I_TID'         then begin opcode = 08; microcode = 0; found = true; end;
     if s[i]='I_TARGETID'    then begin opcode = 08; microcode = 0; found = true; end;
     if s[i]='I_TINFO'       then begin opcode = 09; microcode = 0; found = true; end;
     if s[i]='I_TARGETINFO'  then begin opcode = 09; microcode = 0; found = true; end;
     if s[i]='I_GINFO'       then begin opcode = 10; microcode = 0; found = true; end;
     if s[i]='I_GAMEINFO'    then begin opcode = 10; microcode = 0; found = true; end;
     if s[i]='I_RINFO'       then begin opcode = 11; microcode = 0; found = true; end;
     if s[i]='I_ROBOTINFO'   then begin opcode = 11; microcode = 0; found = true; end;
     if s[i]='I_COLLISIONS'  then begin opcode = 12; microcode = 0; found = true; end;
     if s[i]='I_RESETCOLCNT' then begin opcode = 13; microcode = 0; found = true; end;
     if s[i]='I_TRANSMIT'    then begin opcode = 14; microcode = 0; found = true; end;
     if s[i]='I_RECEIVE'     then begin opcode = 15; microcode = 0; found = true; end;
     if s[i]='I_DATAREADY'   then begin opcode = 16; microcode = 0; found = true; end;
     if s[i]='I_CLEARCOM'    then begin opcode = 17; microcode = 0; found = true; end;
     if s[i]='I_KILLS'       then begin opcode = 18; microcode = 0; found = true; end;
     if s[i]='I_DEATHS'      then begin opcode = 18; microcode = 0; found = true; end;
     if s[i]='I_CLEARMETERS' then begin opcode = 19; microcode = 0; found = true; end;

     {memory addresses}
     if (not found) and (s[i][1]='@') and (s[i][2] in ['0'..'9'])  then
      begin
       opcode = str2int(rstr(s[i],length(s[i])-1));
       if (opcode<0) or (opcode>(max_ram+1)+(((max_code+1) shl 3)-1)) then
          prog_error(3,s[i]);
       microcode = 1; {variable}
       found = true;
      end;

     {numbers}
     if (not found) and (s[i][1] in ['0'..'9','-'])  then
      begin
       opcode = str2int(s[i]);
       found = true;
      end;

     if found then
      begin
       code[p].op[i] = opcode;
       if indirect then microcode = microcode or 8;
       code[p].op[max_op] = code[p].op[max_op] or (microcode shl (i*4));
      end
     else if s[i]<>'' then prog_error(2,s[i]);
    end;
  end;
 if show_code then print_code(n,p);
 if compile_by_line then readkey;
end;

void check_plen(short plen) {
	if (plen > maxcode) then
		prog_error(16, #13#10+'Maximum program length exceeded, (Limit: '+cstr(maxcode+1)+' compiled lines)');
}



procedure compile(n:integer;filename:string);
var
 pp:parsetype;
 s,s1,s2,s3,orig_s,msg:string;
 i,j,k,l,linecount,mask,locktype:integer;
 ss:array[0..max_op] of string[16];
 c,lc:char;
begin
 lock_code = '';
 lock_pos = 0;
 locktype = 0;
 lock_dat = 0;
 if not exist(filename) then prog_error(8,filename);
 textcolor(robot_color(n));
 writeln('Compiling robot #',n+1,': ',filename);
 with robot[n]^ do
  begin
   {FIFI}
   is_locked = false; {assume unlocked robot}
   {/FIFI}
   textcolor(robot_color(n));
   numvars = 0;
   numlabels = 0;
   for k = 0 to max_code do
    for i = 0 to max_op do
     code[k].op[i] = 0;
   plen = 0;
   assign(f,filename);
   reset(f);
   s = '';
   linecount = 0;

   {--first pass, compile--}
   while (not eof(f)) and (s<>'#END') {and (plen<=maxcode)} do
    begin
     readln(f,s);
     inc(linecount);
     if locktype<3 then lock_pos = 0;
     if lock_code<>'' then
      for i = 1 to length(s) do
       begin
        inc(lock_pos); if lock_pos>length (lock_code) then lock_pos = 1;
        case locktype of
         3:s[i] = char((ord(s[i])-1) xor (ord(lock_code[lock_pos]) xor lock_dat));
         2:s[i] = char(ord(s[i]) xor (ord(lock_code[lock_pos]) xor 1));
         else s[i] = char(ord(s[i]) xor ord(lock_code[lock_pos]));
        end;
        lock_dat = ord(s[i]) and 15;
       end;
     s = btrim(s);
     orig_s = s;
     for i = 1 to length(s) do
      if s[i] in [#0..#32,',',#128..#255] then s[i] = ' ';
     if show_source and ((lock_code='') or debugging_compiler) then
        writeln(zero_pad(linecount,3)+':'+zero_pad(plen,3)+' ',s);
     if debugging_compiler then
        begin if readkey=#27 then halt; end;
     {-remove comments-}
     k = 0;
     for i = length(s) downto 1 do
         if s[i]=';' then k = i;
     if k>0 then s = lstr(s,k-1);
     s = btrim(ucase(s));
     for i = 0 to max_op do pp[i] = '';
     if (length(s)>0) and (s[1]<>';') then
      begin
       case s[1] of
        '#':begin (*  Compiler Directives  *)
             s1 = ucase(btrim(rstr(s,length(s)-1)));
             msg = btrim(rstr(orig_s,length(orig_s)-5));
             k = 0;
             for i = 1 to length(s1) do
              if (k=0) and (s1[i]=' ') then k = i;
             dec(k);
             if k>1 then
              begin
               s2 = lstr(s1,k);
               s3 = ucase(btrim(rstr(s1,length(s1)-k)));
               k = 0;
               if numvars>0 then
                for i = 1 to numvars do
                 if s3=varname[i] then k = i;
               if (s2='DEF') and (numvars<max_vars) then
                begin
                 if length(s3)>max_var_len then prog_error(12,s3)
                  else
                 if k>0 then prog_error(11,s3)
                  else
                   begin
                    inc(numvars);
                    if numvars>max_vars then prog_error(14,'')
                     else begin
                           varname[numvars] = s3;
                           varloc [numvars] = 127+numvars;
                          end;
                   end;
                end
               else if (lstr(s2,4)='LOCK') then
                begin
                 {FIFI}
                 is_locked = true; {this robot is locked}
                 {/FIFI}
                 if length(s2)>4 then locktype = value(rstr(s2,length(s2)-4));
                 lock_code = btrim(ucase(s3));
                 writeln('Robot is of LOCKed format from this point forward. [',locktype,']');
                 {writeln('Using key: "',lock_code,'"');}
                 for i = 1 to length(lock_code) do
                  lock_code[i] = char(ord(lock_code[i])-65);
                end
               else if (s2='MSG') then name = msg
               else if (s2='TIME') then
                begin
                 robot_time_limit = value(s3);
                 if robot_time_limit<0 then robot_time_limit = 0;
                end
               else if (s2='CONFIG') then
                begin
                 if (lstr(s3,8)='SCANNER=') then config.scanner = 
                     value(rstr(s3,length(s3)-8))
                 else if (lstr(s3,7)='SHIELD=') then config.shield = 
                         value(rstr(s3,length(s3)-7))
                 else if (lstr(s3,7)='WEAPON=') then config.weapon = 
                         value(rstr(s3,length(s3)-7))
                 else if (lstr(s3,6)='ARMOR=') then config.armor = 
                         value(rstr(s3,length(s3)-6))
                 else if (lstr(s3,7)='ENGINE=') then config.engine = 
                         value(rstr(s3,length(s3)-7))
                 else if (lstr(s3,10)='HEATSINKS=') then config.heatsinks = 
                         value(rstr(s3,length(s3)-10))
                 else if (lstr(s3,6)='MINES=') then config.mines = 
                         value(rstr(s3,length(s3)-6))
                 else prog_error(20,s3);
                 with config do
                   begin
                     if scanner<0 then scanner = 0; if scanner>5 then scanner = 5;
                     if shield<0 then shield = 0;   if shield>5 then shield = 5;
                     if weapon<0 then weapon = 0;   if weapon>5 then weapon = 5;
                     if armor<0 then armor = 0;     if armor>5 then armor = 5;
                     if engine<0 then engine = 0;   if engine>5 then engine = 5;
                     if heatsinks<0 then heatsinks = 0; if heatsinks>5 then heatsinks = 5;
                     if mines<0 then mines = 0;     if mines>5 then mines = 5;
                   end;
                end
               else writeln('Warning: unknown directive "'+s2+'"');
              end;
            end;
        '*':begin  (*  Inline Pre-Compiled Machine Code  *)
             check_plen(plen);
             for i = 0 to max_op do pp[i] = '';
             for i = 2 to length(s) do
              if s[i]='*' then prog_error(23,s);
             k = 0; i = 1; s1 = '';
             if length(s)<=2 then prog_error(22,s);
             while (i<length(s)) and (k<=max_op) do
              begin
               inc(i);
               if ord(s[i]) in [33..41,43..127] then pp[k] = pp[k]+s[i]
                 else if (ord(s[i]) in [0..32,128..255]) and
                         (ord(s[i-1]) in [33..41,43..127]) then inc(k);
              end;
             for i = 0 to max_op do
              code[plen].op[i] = str2int(pp[i]);
             inc(plen);
            end;
        ':':begin  (*  :labels  *)
             check_plen(plen);
             s1 = rstr(s,length(s)-1);
             for i = 1 to length(s1) do
              if not (s1[i] in ['0'..'9']) then
                 prog_error(1,s);
             code[plen].op[0] = str2int(s1);
             code[plen].op[max_op] = 2;
             if show_code then print_code(n,plen);
             inc(plen);
            end;
        '!':begin (*  !labels  *)
             check_plen(plen);
             s1 = btrim(rstr(s,length(s)-1));
             k = 0;
             for i = length(s1) downto 1 do
                 if s1[i] in [';',#8,#9,#10,' ',','] then k = i;
             if k>0 then s1 = lstr(s1,k-1);
             k = 0;
             for i = 1 to numlabels do
              if (labelname[i]=s1) then
               begin
                if (labelnum[i]>=0) then prog_error(13,'"!'+s1+'" ('+cstr(labelnum[i])+')');
                k = i;
               end;
             if (k=0) then
              begin
               inc(numlabels);
               if numlabels>max_labels then prog_error(15,'');
               k = numlabels;
              end;
             labelname[k] = s1;
             labelnum [k] = plen;
            end;
        else begin  (*  Instructions/Numbers  *)
              check_plen(plen);
              {-parse instruction-}
              {-remove comments-}
              k = 0;
              for i = length(s) downto 1 do
                  if s[i]=';' then k = i;
              if k>0 then s = lstr(s,k-1);
              {-setup variables for parsing-}
              k = 0; for j = 0 to max_op do pp[j] = '';
              for j = 1 to length(s) do
               begin
                c = s[j];
                if (not (c in [' ',#8,#9,#10,','])) and (k<=max_op) then pp[k] = pp[k]+c
                   else if not (lc in [' ',#8,#9,#10,',']) then k = k+1;
                lc = c;
               end;
              parse1(n,plen,pp);
              inc(plen);
             end;
       end;
      end;
    end;
   close(f);
   {-Add our implied NOP if there's room. This was originally to make sure
     no one tries using an empty robot program, kinda pointless otherwise-}
   if plen<=maxcode then
    begin
     for i = 0 to max_op do pp[i] = '';
     pp[0] = 'NOP';
     parse1(n,plen,pp);
    end
   else
    dec(plen); 


   {--second pass, resolving !labels--}
   if numlabels>0 then
    for i = 0 to plen do
     for j = 0 to max_op-1 do
      if code[i].op[max_op] shr (j*4)=3 {unresolved !label} then
       begin
        k = code[i].op[j];
        if (k>0) and (k<=numlabels) then
         begin
          l = labelnum[k];
          if (l<0) then prog_error(19,'"!'+labelname[k]+'" ('+cstr(l)+')');
          if (l<0) or (l>maxcode) then prog_error(18,'"!'+labelname[k]+'" ('+cstr(l)+')')
           else
            begin
             code[i].op[j] = l;
             mask = not($F shl (j*4));
             code[i].op[max_op] = (code[i].op[max_op] and mask) or (4 shl (j*4));
                                  {resolved !label}
            end;
         end
        else prog_error(17,cstr(k));
       end;
  end;
 textcolor(7);
end;
*/


void robot_config(short n) {
	short i, j, k;
begin
 with robot[n]^ do
  begin
   case config.scanner of
    5:scanrange = 1500;
    4:scanrange = 1000;
    3:scanrange = 700;
    2:scanrange = 500;
    1:scanrange = 350;
    else scanrange = 250;
   end;
   case config.weapon of
    5:shotstrength = 1.5;
    4:shotstrength = 1.35;
    3:shotstrength = 1.2;
    2:shotstrength = 1;
    1:shotstrength = 0.8;
    else shotstrength = 0.5;
   end;
   case config.armor of
    5:begin damageadj = 0.66; speedadj = 0.66; end;
    4:begin damageadj = 0.77; speedadj = 0.75; end;
    3:begin damageadj = 0.83; speedadj = 0.85; end;
    2:begin damageadj = 1; speedadj = 1; end;
    1:begin damageadj = 1.5; speedadj = 1.2; end;
    else begin damageadj = 2; speedadj = 1.33; end;
   end;
   case config.engine of
    5:speedadj = speedadj*1.5;
    4:speedadj = speedadj*1.35;
    3:speedadj = speedadj*1.2;
    2:speedadj = speedadj*1;
    1:speedadj = speedadj*0.8;
    else speedadj = speedadj*0.5;
   end;
   {heatsinks are handled seperately}
   case config.mines of
    5:mines = 24;
    4:mines = 16;
    3:mines = 10;
    2:mines = 6;
    1:mines = 4;
    else begin mines = 2; config.mines = 0; end;
   end;
   shields_up = false;
   if (config.shield<3) or (config.shield>5) then config.shield = 0;
   if (config.heatsinks<0) or (config.heatsinks>5) then config.heatsinks = 0;
  end;
end;

procedure reset_software(n:integer);
var
 i:integer;
begin
 with robot[n]^ do
  begin
   for i = 0 to max_ram do ram[i] = 0;
   ram[71]  =  768;
   thd = hd;  tspd = 0;
   scanarc = 8; shift = 0; err = 0;
   overburn = false; keepshift = false;
   ip = 0; accuracy = 0;
   meters = 0;
   delay_left = 0; time_left = 0;
   shields_up = false;
  end;
end;

procedure reset_hardware(n:integer);
var
 i:integer;
 d,dd:real;
begin
 with robot[n]^ do
  begin
   for i = 1 to max_robot_lines do
    begin ltx[i] = 0; tx[i] = 0; lty[i] = 0; ty[i] = 0; end;
   repeat
    x = random(1000); y = random(1000);
    dd = 1000;
    for i = 0 to num_robots do
     begin
      if robot[i]^.x<0 then robot[i]^.x = 0;
      if robot[i]^.x>1000 then robot[i]^.x = 1000;
      if robot[i]^.y<0 then robot[i]^.y = 0;
      if robot[i]^.y>1000 then robot[i]^.y = 1000;
      d = distance(x,y,robot[i]^.x,robot[i]^.y);
      if (robot[i]^.armor>0) and (i<>n) and (d<dd) then dd = d;
     end;
   until dd>32;
   for i = 0 to max_mines do
    with mine[i] do
     begin x = -1; y = -1; yield = 0; detonate = false; detect = 0; end;
   lx = -1; ly = -1;
   hd = random(256); shift = 0;
   lhd = hd+1; lshift = shift+1;
   spd = 0; speed = 0;
   cooling = false;
   armor = 100; larmor = 0;
   heat = 0; lheat = 1;
   match_shots = 0;
   won = false;
   last_damage = 0;
   last_hit = 0;
   transponder = n+1;
   meters = 0;
   shutdown = 400;
   shields_up = false;
   channel = transponder;
   startkills = kills;
   robot_config(n);
  end;
end;

procedure init_robot(n:integer);
var
 i,j,k,l:integer;
begin
 with robot[n]^ do
  begin
   wins = 0; trials = 0; kills = 0; deaths = 0; shots_fired = 0; match_shots = 0;
   hits = 0; damage_total = 0; cycles_lived = 0; error_count = 0;
   plen = 0; max_time = 0;
   name = ''; fn = '';
   speed = 0;
   arc_count = 0;
   sonar_count = 0;
   robot_time_limit = 0;
   scanrange = 1500;
   shotstrength = 1;
   damageadj = 1;
   speedadj = 1;
   mines = 0;
   with config do
    begin
     scanner = 5;
     weapon = 2;
     armor = 2;
     engine = 2;
     heatsinks = 1;
     shield = 0;
     mines = 0;
    end;
   for i = 0 to max_ram do ram[i] = 0;
   ram[71]  =  768;
   for i = 0 to max_code do
    for k = 0 to max_op do
     code[i].op[k] = 0;
   reset_hardware(n);
   reset_software(n);
  end;
end;

procedure create_robot(n:integer; filename:string);
var
 i,j,k:integer;
begin
 if maxavail<sizeof(robot_rec) then prog_error(9,base_name(no_path(filename)));
 new(robot[n]);
 with robot[n]^ do
  begin
   init_robot(n);
   filename = ucase(btrim(filename));
   if filename=base_name(filename) then
    begin
     if filename[1]='?' then filename = filename+locked_ext
                        else filename = filename+robot_ext;
    end;
   if filename[1]='?' then filename = rstr(filename,length(filename)-1);
   fn = base_name(no_path(filename));
   compile(n,filename);
   robot_config(n);
   with config do
    begin
     k = scanner+armor+weapon+engine+heatsinks+shield+mines;
     if (k)>max_config_points then
        prog_error(21,cstr(k)+'/'+cstr(max_config_points));
    end;
  end;
end;

void shutdown() {
	short i, j, k;

	graph_mode(false);
	if (show_cnotice) {
		// textcolor(3);
		cout << progname << " " << version << " ");
		cout << cnotice1 << endl;
		cout << cnotice2 << endl;
		cout << cnotice3 << endl;
	}

	textcolor(7);
	if (!registered)
		textcolor(4);
		cout << "Unregistered version << endl;
	else
		cout << "Registered to: " << reg_name << endl;

	textcolor(7);
	cout << endl;
	if (logging_errors)
		for (i = 0; i <= num_robots; i++) {
		cout << "Robot error-log created: " << base_name(robot[i] -> fn) + ".ERR");
		if (robot[i] -> errorlog.good())
			robot[i] -> errorlog.close();
		}
	exit(EXIT_SUCCESS);
}

procedure delete_compile_report;
begin
 if exist(main_filename+compile_ext) then
   delete_file(main_filename+compile_ext);
end;

procedure write_compile_report;
var
 f:text;
 i,j,k:integer;
begin
 assign(f,main_filename+compile_ext);
 rewrite(f);
 writeln(f,num_robots+1);
 for i = 0 to num_robots do
  with robot[i]^ do
   writeln(f,fn);
 close(f);
 textcolor(15);
 writeln;
 writeln('All compiles successful!');
 writeln;
 shutdown;
end;


procedure parse_param(s:String);
var
 f:text;
 fn,s1:string;
 found:boolean;
begin
 found = false;
 s = btrim(ucase(s));
 if s='' then exit;
 if s[1]='#' then
  begin
   fn = rstr(s,length(s)-1);
   if fn=base_name(fn) then fn = fn+config_ext;
   if not exist(fn) then prog_error(6,fn);
   assign(f,fn); reset(f);
   while not eof(f) do
    begin
     readln(f,s1);
     s1 = ucase(btrim(s1));
     if s1[1]='#' then prog_error(7,s1)
        else parse_param(s1);
    end;
   close(f);
   found = true;
  end
 else if s[1] in ['/','-','='] then
  begin
   s1 = rstr(s,length(s)-1);
   {FIFI}
   if s1[1]='X' then
    begin
     step_mode = value(rstr(s1,length(s1)-1));
     found = true;
     if step_mode=0 then step_mode = 1;
     if (step_mode<1) or (step_mode>9) then prog_error(24,rstr(s1,length(s1)-1));
    end;
   {/FIFI}
   if s1[1]='D' then begin game_delay = value(rstr(s1,length(s1)-1)); found = true; end;
   if s1[1]='T' then begin time_slice = value(rstr(s1,length(s1)-1)); found = true; end;
   if s1[1]='L' then begin game_limit = value(rstr(s1,length(s1)-1))*1000; found = true; end;
   if s1[1]='Q' then begin sound_on = false; found = true; end;
   if s1[1]='M' then begin matches = value(rstr(s1,length(s1)-1)); found = true; end;
   if s1[1]='S' then begin show_source = false; found = true; end;
   if s1[1]='G' then begin no_gfx = true; found = true; end;
   if s1[1]='R' then begin report = true; found = true;
                     if (length(s1)>1) then
                        report_type = value(rstr(s1,length(s1)-1)); end;
   if s1[1]='C' then begin compile_only = true; found = true; end;
   if s1[1]='^' then begin show_cnotice = false; found = true; end;
   if s1[1]='A' then begin show_arcs = true; found = true; end;
   if s1[1]='W' then begin windoze = false; found = true; end;
   if s1[1]='$' then begin debug_info = true; found = true; end;
   if s1[1]='#' then begin maxcode = value(rstr(s1,length(s1)-1))-1; found = true; end;
   if s1[1]='!' then begin insane_missiles = true; if (length(s1)>1) then
                     insanity = value(rstr(s1,length(s1)-1)); found = true; end;
   if s1[1]='@' then begin old_shields = true; found = true; end;
   if s1[1]='E' then begin logging_errors = true; found = true; end;
   if insanity<0 then insanity = 0;
   if insanity>15 then insanity = 15;
  end
 else if s[1]=';' then found = true
 else if (num_robots<max_robots) and (s<>'') then
  begin
   inc(num_robots);
   create_robot(num_robots,s);
   found = true;
   if num_robots=max_robots then writeln('Maximum number of robots reached.');
  end
   else prog_error(10,'');
 if not found then prog_error(8,s);
end;


void init() {
	short i;
	if ( debugging_compiler | compile_by_line | show_code ) {
		fprintf(stderr, "!!! Warning !!! Compiler Debugging enabled !!!\n")
		flushkey;
		readkey;
		writeln;
	}

	if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO)) { // Only triggers when sdl fails to initialize
		fprintf(stderr, "ERROR: SDL Failed to initialize.\n %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if (TTF_Init() == -1) {
		fprintf(stderr, "ERROR: Could not initialize TTF.\n%s\n", TTF_GetError());
		exit(EXIT_FAILURE);
	}

	viewport = (SDL_Rect *) malloc(sizeof(SDL_Rect));

	
	step_mode = 0; // stepping disabled
	logging_errors = false;
	stats_mode = 0;
	insane_missiles = false;
	insanity = 0;
	delay_per_sec = 0;
	windoze = true;
	graphix = false;
	no_gfx = false;
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
	randomize();
	num_robots = -1;
	game_limit = 100000;
	game_cycle = 0;
	game_delay = default_delay;
	time_slice = default_slice;

	for ( i = 0; i <=max_missiles; i++) {
 		missile -> a = 0;
		missile -> source = -1;
		missile -> x = 0;
		missile -> y = 0;
		missile -> lx = 0;
		missile -> ly = 0;
		missile -> mult = 1;
	}

	registered = false;
	reg_name = "Unregistered";
	reg_num = ~0U;
	check_registration();

	cout << endl;
	// textcolor(3); Cyan text
	fprintf(stdout, "progname, %f \n", version); // unsure what type of variable version is
	fprintf(stdout, "%d\n", cnotice1);
	fprintf(stdout, "%d\n", cnotice2);
	textcolor(7);

	if (!registered ) {
		// textcolor(4); Red text
		cout << "Unregistered version\n";
	} else
		cout << "Registered to: " << reg_name << endl;

	// textcolor(7); Set text color to light gray

	cout << endl;

	delete_compile_report();

	if (paramcount > 0)
		for (i = 1; i <= paramcount; i++)
			parse_param( btrim( ucase( paramstr(i)) ) );
	else
		prog_error( 5, "");

	temp_mode = step_mode; // store initial step_mode

	if (logging_errors) {
		for (i = 0; i <= num_robots; i++) {
			if (robot[i] -> errorlog.good())
				robot[i] -> errorlog.close;

			robot[i] -> errorlog.open(base_name(robot[i] -> fn)+".ERR", fstream::out);
		}
	}

	if ( compile_only )
		write_compile_report();

	if (num_robots < 1)
		prog_error(4,"");

	if ( !no_gfx )
		graph_mode(true);

	// fix ups
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
		maxcode = 1; // 0 based, so actually 2 lines

	if (maxcode > max_code)
		maxcode = max_code;

	// Just to avoid floating pointers
	for (i = num_robots+1; i <= max_robots+2; i++)
		robot[i] = robot[0];

	robot[-1] = robot[0];
	robot[-2] = robot[0];

	if ( !graphix) {
		fprintf(stdout, "Freemem: %d\n", memavail);
		fprintf(stdout, "\n");
	}
}

procedure draw_robot(n:integer);
var
 i,t:integer;
 xx,yy:real;
begin
 if not (n in [0..num_robots]) then exit;
 with robot[n]^ do
  begin
   if x>1000 then x = 1000;
   if y>1000 then y = 1000;
   if x<0 then x = 0;
   if y<0 then y = 0;
   {if (lhd=hd) and (lx=x) and (ly=y) then exit;}

   {--set up for erase--}
   {xx = lx*screen_scale; yy = ly*screen_scale;
   lhd = (lhd+1024) and 255;
   ltx[1] = round(xx+sint[lhd]*robot_scale);
   lty[1] = round(yy-cost[lhd]*robot_scale);
   ltx[2] = round(xx+sint[(lhd+$68) and 255]*robot_scale);
   lty[2] = round(yy-cost[(lhd+$68) and 255]*robot_scale);
   ltx[3] = round(xx+sint[(lhd+$98) and 255]*robot_scale);
   lty[3] = round(yy-cost[(lhd+$98) and 255]*robot_scale);
   t = (lhd+(lshift and 255)+1024) and 255;
   ltx[4] = round(xx+sint[t and 255]*robot_scale);
   lty[4] = round(yy-cost[t and 255]*robot_scale);}

   {--set up for draw--}
   xx = x*screen_scale+screen_x; yy = y*screen_scale+screen_y;
   hd = (hd+1024) and 255;
   tx[1] = round(xx+sint[hd]*5);
   ty[1] = round(yy-cost[hd]*5);
   tx[2] = round(xx+sint[(hd+$68) and 255]*robot_scale);
   ty[2] = round(yy-cost[(hd+$68) and 255]*robot_scale);
   tx[3] = round(xx+sint[(hd+$98) and 255]*robot_scale);
   ty[3] = round(yy-cost[(hd+$98) and 255]*robot_scale);
   t = (hd+(shift and 255)+1024) and 255;
   tx[4] = round(xx); ty[4] = round(yy);
   tx[5] = round(xx+sint[t]*robot_scale*0.8);
   ty[5] = round(yy-cost[t]*robot_scale*0.8);
   tx[6] = round(xx+sint[(t+scanarc+1024) and 255]*scanrange*screen_scale);
   ty[6] = round(yy-cost[(t+scanarc+1024) and 255]*scanrange*screen_scale);
   tx[7] = round(xx+sint[(t-scanarc+1024) and 255]*scanrange*screen_scale);
   ty[7] = round(yy-cost[(t-scanarc+1024) and 255]*scanrange*screen_scale);
   startarc = round(((256-((t+scanarc+1024) and 255))/256*360)+90);
   endarc = round(((256-((t-scanarc+1024) and 255))/256*360)+90);


   if graphix then
    begin
     main_viewport;
     {--erase--}
     setcolor(0);
     if lshields then
        circle(ltx[4],lty[4],robot_scale);
     if (arc_count>0) then
      begin
       line(ltx[4],lty[4],ltx[6],lty[6]);
       line(ltx[4],lty[4],ltx[7],lty[7]);
       if scanrange<1500 then
          arc(ltx[4],lty[4],lstartarc,lendarc,round(scanrange*screen_scale));
      end;
     if (sonar_count>0) then
      begin
       circle(ltx[4],lty[4],round(max_sonar*screen_scale));
      end;
     if armor>0 then 
      begin {only erase body if still alive, that way we leave a "corpse" when dead}
       line(ltx[1],lty[1],ltx[2],lty[2]);
       line(ltx[2],lty[2],ltx[3],lty[3]);
       line(ltx[3],lty[3],ltx[1],lty[1]);
       line(ltx[4],lty[4],ltx[5],lty[5]);
      end;
    end;
   if armor>0 then {If we're still alive we need to redraw}
    begin
     if arc_count>0   then dec(arc_count);
     if sonar_count>0 then dec(sonar_count);
     if graphix then
      begin
       {--draw--}
       setcolor(robot_color(n) and 7);
       if shields_up then
        circle(tx[4],ty[4],robot_scale);
       setcolor(robot_color(n));
       line(tx[1],ty[1],tx[2],ty[2]);
       line(tx[2],ty[2],tx[3],ty[3]);
       line(tx[3],ty[3],tx[1],ty[1]);
       setcolor(7);
       line(tx[4],ty[4],tx[5],ty[5]);
       setcolor(8);
       if show_arcs and (arc_count>0) then
        begin
         line(tx[4],ty[4],tx[6],ty[6]);
         line(tx[4],ty[4],tx[7],ty[7]);
         if scanrange<1500 then
            arc(tx[4],ty[4],startarc,endarc,round(scanrange*screen_scale));
        end;
       if show_arcs and (sonar_count>0) then
          circle(tx[4],ty[4],round(max_sonar*screen_scale));
     end;
     lx = x; ly = y; lhd = hd; lshift = shift; lshields = shields_up;
     for i = 1 to max_robot_lines do
      begin ltx[i] = tx[i]; lty[i] = ty[i]; end;
     lstartarc = startarc; lendarc = endarc;
    end;
  end;
end;

function get_from_ram(n,i,j:integer):integer;
 var k,l:integer;
begin
 with robot[n]^ do
  begin
   if (i<0) or (i>(max_ram+1)+(((max_code+1) shl 3)-1)) then
     begin k = 0; robot_error(n,4,cstr(i)); end
    else
     begin
      if i<=max_ram then k = ram[i]
       else
        begin
         l = i-max_ram-1;
         k = code[l shr 2].op[l and 3];
        end;
     end;
  end;
 get_from_ram = k;
end;

function get_val(n,c,o:integer):integer;
var
 i,j,k,l:integer;
begin
 k = 0;
 with robot[n]^ do
  begin
   j = (code[c].op[max_op] shr (4*o)) and 15;
   i = code[c].op[o];
   if (j and 7)=1 then
    begin
     k = get_from_ram(n,i,j);
    end else k = i;
   if (j and 8)>0 then
    k = get_from_ram(n,k,j);
  end;
 get_val = k;
end;

procedure put_val(n,c,o,v:integer);
var
 i,j,k:integer;
begin
 k = 0; i = 0; j = 0;
 with robot[n]^ do
  begin
   j = (code[c].op[max_op] shr (4*o)) and 15;
   i = code[c].op[o];
   if (j and 7)=1 then
    begin
     if (i<0) or (i>max_ram) then
       robot_error(n,4,cstr(i))
      else
       if (j and 8)>0 then
        begin
         i = ram[i];
         if (i<0) or (i>max_ram) then
          robot_error(n,4,cstr(i))
         else ram[i] = v;
        end
       else ram[i] = v;
    end
   else
    robot_error(n,3,'');
  end;
end;

procedure push(n,v:integer);
begin
 with robot[n]^ do
  begin
   if (ram[71] >= stack_base) and (ram[71] < (stack_base + stack_size)) then
    begin
     ram[ram[71]]  =  v;
     inc(ram[71]);
    end
   else robot_error(n,1,cstr(ram[71]));
  end;
end;

function pop(n:integer):integer;
var
 k:integer;
begin
 with robot[n]^ do
  begin
   if (ram[71] > stack_base) and (ram[71] <= (stack_base + stack_size)) then
    begin
     dec(ram[71]);
     k  =  ram[ram[71]];
    end
   else robot_error(n,5,cstr(ram[71]));
  end;
 pop = k;
end;

function find_label(n,l,m:integer):integer;
var
 i,j,k:integer;
begin
 k = -1;
 with robot[n]^ do
  begin
   if m=3 then robot_error(n,9,'')
    else
   if m=4 then k = l
    else
   for i = plen downto 0 do
    begin
     j = code[i].op[max_op] and 15;
     if (j=2) and (code[i].op[0]=l) then k = i;
    end;
  end;
 find_label = k;
end;


procedure init_mine(n,detectrange,size:integer);
var
 i,j,k:integer;
begin
 with robot[n]^ do
  begin
   k = -1;
   for i = 0 to max_mines do
    if ((mine[i].x<0) or (mine[i].x>1000) or (mine[i].y<0) or (mine[i].y>1000)
       or (mine[i].yield<=0)) and (k<0) then k = i;
   if k>=0 then
    begin
     mine[k].x = x;
     mine[k].y = y;
     mine[k].detect = detectrange;
     mine[k].yield = size;
     mine[k].detonate = false;
     click;
    end;
  end;
end;

function count_missiles:integer;
var
 i,k:integer;
begin
 k = 0;
 for i = 0 to max_missiles do
  if missile[i].a>0 then inc(k);
 count_missiles = k;
end;

procedure init_missile(xx,yy,xxv,yyv:real; dir,s,blast:integer; ob:boolean);
var
 i,j,k:integer;
 m:real;
 sound:boolean;
begin
 k = -1; click;
 for i = max_missiles downto 0 do
  if missile[i].a=0 then k = i;
 if k>=0 then
  with missile[k] do
   begin
    source = s;
    x = xx; lx = x;
    y = yy; ly = y;
    rad = 0; lrad = 0;
    if ob then mult = 1.25 else mult = 1;
    if blast>0 then
      begin max_rad = blast; a = 2; end
     else
      begin
       if (s>=0) and (s<=num_robots) then
          mult = mult*(robot[s]^.shotstrength);
       m = mult;
       if ob then m = m+0.25;
       mspd = missile_spd*mult;
       if insane_missiles then mspd = 100+(50*insanity)*mult;
       if (s>=0) and (s<=num_robots) then
         begin
          inc(robot[s]^.heat,round(20*m));
          inc(robot[s]^.shots_fired);
          inc(robot[s]^.match_shots);
         end;
       a = 1; hd = dir;
       max_rad = mis_radius;
       if debug_info then
         begin writeln(#13,zero_pad(game_cycle,5),' F ',s,': hd=',hd,'           ');
         repeat until keypressed; flushkey; end;
      end;
   end;
 (*The following was to see if teh missile array is big enough*)
  {else
   begin
    sound = sound_on;
    sound_on = true;
    chirp;
    sound_on = sound;
   end;
 setfillstyle(1,0);
 setcolor(robot_color(k));
 bar(5,5,37,12);
 outtextxy(5,5,cstr(count_missiles));}
end;

procedure damage(n,d:integer; physical:boolean);
var
 i,k,h,dd:integer;
 m:real;
begin
 if (n<0) or (n>num_robots) or (robot[n]^.armor<=0) then exit;
 if robot[n]^.config.shield<3 then robot[n]^.shields_up = false;
 with robot[n]^ do
  begin
   h = 0;
   if (shields_up) and (not physical) then
    begin
     dd = d;
     if (old_shields) and (config.shield>=3) then begin d = 0; h = 0; end
      else case config.shield of
       3:begin d = round(dd*2/3); if d<1 then d = 1; h = round(dd*2/3); end;
       4:begin h = trunc(dd/2); d = dd-h; end;
       5:begin d = round(dd*1/3); if d<1 then d = 1;
               h = round(dd*1/3); if h<1 then h = 1; end;
      end;
    end;
   if d<0 then d = 0;
   if debug_info then
    begin writeln(#13,zero_pad(game_cycle,5),' D ',n,': ',armor,'-',d,'=',armor-d,'           ');
    repeat until keypressed; flushkey; end;
   if d>0 then
    begin d = round(d*damageadj); if d<1 then d = 1; end;
   dec(armor,d);
   inc(heat,h);
   last_damage = 0;
   if armor<=0 then
    begin
     armor = 0;
     update_armor(n);
     heat = 500;
     update_heat(n);
     armor = 0;
     inc(kill_count);
     inc(deaths);
     update_lives(n);
     if graphix and timing then time_delay(10);
     draw_robot(n);
     heat = 0;
     update_heat(n);
     init_missile(x,y,0,0,0,n,blast_circle,false);
     if overburn then m = 1.3 else m = 1;
     for i = 0 to num_robots do
      if (i<>n) and (robot[i]^.armor>0) then
       begin
        k = round(distance(x,y,robot[i]^.x,robot[i]^.y));
        if k<blast_radius then
         damage(i,round(abs(blast_radius-k)*m),false);
       end;
    end;
  end;
end;

function scan(n:integer):integer;
var
 r,d,acc:real;
 dir,range,i,j,k,l,nn,xx,yy,sign:integer;
begin
 nn = -1;
 range = maxint;
 if not (n in [0..num_robots]) then exit;
 with robot[n]^ do
  begin
   if scanarc<0 then scanarc = 0;
   accuracy = 0; nn = -1;
   dir = (shift+hd) and 255;
   if debug_info then writeln('<SCAN Arc=',scanarc,', Dir=',dir,'>');
   for i = 0 to num_robots do
    if (i<>n) and (robot[i]^.armor>0) then
     begin
      j = find_anglei(x,y,robot[i]^.x,robot[i]^.y);
      d = distance(x,y,robot[i]^.x,robot[i]^.y);
      k = round(d);
      if (k<range) and (k<=scanrange) and ((abs(j-dir)<=abs(scanarc)) or (abs(j-dir)>=256-abs(scanarc))) then
       begin
        dir = (dir+1024) and 255;
        xx = round( sint[dir]*d+x);
        yy = round(-cost[dir]*d+y);
        r = distance(xx,yy,robot[i]^.x,robot[i]^.y);
        if debug_info then begin
          writeln('SCAN HIT! Scan X,Y: ',round(xx),',',round(yy),'  Robot X,Y: ',
                   round(robot[i]^.x),',',round(robot[i]^.y),'  Dist=',round(r));
          repeat until keypressed; flushkey; end;
        if (scanarc>0) or (r<hit_range-2) then
         begin
          range = k;
          accuracy = 0;
          if scanarc>0 then
           begin
            j = (j+1024) and 255; dir = (dir+1024) and 255;
            if (j<dir) then sign = -1;
            if (j>dir) then sign =  1;
            if (j>190) and (dir<66) then begin dir = dir+256; sign = -1; end;
            if (dir>190) and (j<66) then begin   j =   j+256; sign =  1; end;
            acc = abs(j-dir)/scanarc*2;
            if sign<0 then accuracy = -abs(round(acc))
                      else accuracy = abs(round(acc));
            if accuracy>2 then accuracy = 2;
            if accuracy<-2 then accuracy = -2;
           end;
          nn = i;
          if debug_info then
           begin writeln(#13,zero_pad(game_cycle,5),' S ',n,': nn=',nn,', range=',range,', acc=',accuracy,'           ');
           repeat until keypressed; flushkey; end;
         end;
       end;
     end;
   if nn in [0..num_robots] then
    begin
     ram[5] = robot[nn]^.transponder;
     ram[6] = (robot[nn]^.hd-(hd+shift)+1024) and 255;
     ram[7] = robot[nn]^.spd;
     ram[13] = round(robot[nn]^.speed*100);
    end;
  end;
 scan = range;
end;

procedure com_transmit(n,chan,data:integer);
var
 i,j,k:integer;
begin
 for i = 0 to num_robots do
  with robot[i]^ do
   if (armor>0) and (i<>n) and (channel=chan) then
    begin
     if (ram[10]<0) or (ram[10]>max_queue) then ram[10] = 0;
     if (ram[11]<0) or (ram[11]>max_queue) then ram[11] = 0;
     ram[ram[11]+com_queue] = data;
     inc(ram[11]);
     if (ram[11]>max_queue) then ram[11] = 0;
     if (ram[11]=ram[10]) then inc(ram[10]);
     if (ram[10]>max_queue) then ram[10] = 0;
    end;
end;

function com_receive(n:integer):integer;
var
 i,j,k:integer;
begin
 k = 0;
 with robot[n]^ do
  begin
   if (ram[10]<>ram[11]) then
    begin
     if (ram[10]<0) or (ram[10]>max_queue) then ram[10] = 0;
     if (ram[11]<0) or (ram[11]>max_queue) then ram[11] = 0;
     k = ram[ram[10]+com_queue];
     inc(ram[10]);
     if (ram[10]>max_queue) then ram[10] = 0;
    end
   else robot_error(n,12,'');
  end;
 com_receive = k;
end;

function in_port(n,p:integer; var time_used:integer):integer;
var
 v,i,j,k,l,nn:integer;
begin
 v = 0;
 with robot[n]^ do
  begin
   case p of
    01:v = spd;
    02:v = heat;
    03:v = hd;
    04:v = shift;
    05:v = (shift+hd) and 255;
    06:v = armor;
    07:begin v = scan(n); inc(time_used); if show_arcs then arc_count = 2; end;
    08:begin v = accuracy; inc(time_used); end;
    09:begin
        nn = -1;
        inc(time_used,3); k = maxint; nn = -1;
        for i = 0 to num_robots do
         begin
          j = round(distance(x,y,robot[i]^.x,robot[i]^.y));
          if (n<>i) and (j<k) and (robot[i]^.armor>0) then begin k = j; nn = i; end;
         end;
        v = k;
        if nn in [0..num_robots] then
           ram[5] = robot[nn]^.transponder;
       end;
    10:v = random(65535)+random(2);
    16:begin
        nn = -1;
        if show_arcs then sonar_count = 2;
        inc(time_used,40);
        l = -1; k = maxint; nn = -1;
        for i = 0 to num_robots do
         begin
          j = round(distance(x,y,robot[i]^.x,robot[i]^.y));
          if (n<>i) and (j<k) and (j<max_sonar) and (robot[i]^.armor>0) then
             begin k = j; l = i; nn = i; end;
         end;
        if l>=0 then v = (round(find_angle(x,y,robot[l]^.x,robot[l]^.y)/pi*128+
                              1024+random(65)-32) and 255)
                else v = minint;
        if nn in [0..num_robots] then
           ram[5] = robot[nn]^.transponder;
       end;
    17:v = scanarc;
    18:if overburn then v = 1 else v = 0;
    19:v = transponder;
    20:v = shutdown;
    21:v = channel;
    22:v = mines;
    23:if config.mines>=0 then begin
        k = 0;
        for i = 0 to max_mines do
         with mine[i] do
          if (x>=0) and (x<=1000) and (y>=0) and (y<=1000) and (yield>0) then inc(k);
        v = k;
       end else v = 0;
    24:if config.shield>0 then
        begin if shields_up then v = 1 else v = 0;
        end else begin v = 0; shields_up = false; end;
    else robot_error(n,11,cstr(p));
   end;
  end;
 in_port = v;
end;

procedure out_port(n,p,v:integer; var time_used:integer);
var
 i:integer;
begin
 with robot[n]^ do
  begin
   case p of
    11:tspd = v;
    12:shift = (shift+v+1024) and 255;
    13:shift = (v+1024) and 255;
    14:thd = (thd+v+1024) and 255;
    15:begin
        inc(time_used,3);
        if v>4 then v = 4;
        if v<-4 then v = -4;
        init_missile(x,y,xv,yv,(hd+shift+v) and 255,n,0,overburn);
       end;
    17:scanarc = v;
    18:if v=0 then overburn = false else overburn = true;
    19:transponder = v;
    20:shutdown = v;
    21:channel = v;
    22:if config.mines>=0 then
        begin if mines>0 then begin init_mine(n,v,mine_blast); dec(mines); end else robot_error(n,14,'');
        end else robot_error(n,13,'');
    23:if config.mines>=0 then
        begin for i = 0 to max_mines do mine[i].detonate = true;
        end else robot_error(n,13,'');
    24:if config.shield>=3 then
        begin if v=0 then shields_up = false else shields_up = true;
        end else begin shields_up = false; robot_error(n,15,''); end;
    else robot_error(n,11,cstr(p));
   end;
   if scanarc>64 then scanarc = 64; if scanarc<0 then scanarc = 0;
  end;
end;

procedure call_int(n,int_num:integer; var time_used:integer);
var
 i,j,k:integer;
begin
 with robot[n]^ do
  begin
   case int_num of
    00:damage(n,1000,true);
    01:begin
        reset_software(n);
        time_used = 10;
       end;
    02:begin
        time_used = 5;
        ram[69] = round(x);
        ram[70] = round(y);
       end;
    03:begin
        time_used = 2;
        if ram[65]=0 then keepshift = false else keepshift = true;
        ram[70] = shift and 255;
       end;
    04:begin
        if ram[65]=0 then overburn = false else overburn = true;
       end;
    05:begin
        time_used = 2;
        ram[70] = transponder;
       end;
    06:begin
        time_used = 2;
        ram[69] = game_cycle shr 16;
        ram[70] = game_cycle and 65535;
       end;
    07:begin
        j = ram[69]; k = ram[70];
        if j<0 then j = 0; if j>1000 then j = 1000;
        if k<0 then k = 0; if k>1000 then k = 1000;
        ram[65] = round(find_angle(round(x),round(y),j,k)/pi*128+256) and 255;
        time_used = 32;
       end;
    08:begin
        ram[70] = ram[5];
        time_used = 1;
       end;
    09:begin
        ram[69] = ram[6];
        ram[70] = ram[7];
        time_used = 2;
       end;
    10:begin
        k = 0;
        for i = 0 to num_robots do
         if robot[i]^.armor>0 then inc(k);
        ram[68] = k;
        ram[69] = played;
        ram[70] = matches;
        time_used = 4;
       end;
    11:begin
        ram[68] = round(speed*100);
        ram[69] = last_damage;
        ram[70] = last_hit;
        time_used = 5;
       end;
    12:begin
        ram[70] = ram[8];
        time_used = 1;
       end;
    13:begin
        ram[8] = 0;
        time_used = 1;
       end;
    14:begin
        com_transmit(n,channel,ram[65]);
        time_used = 1;
       end;
    15:begin
        if (ram[10]<>ram[11]) then
         ram[70] = com_receive(n)
        else
         robot_error(n,12,'');
        time_used = 1;
       end;
    16:begin
        if (ram[11]>=ram[10]) then
         ram[70] = ram[11]-ram[10]
        else
         begin
          ram[70] = max_queue+1-ram[10]+ram[11];
         end;
        time_used = 1;
       end;
    17:begin
        ram[10] = 0;
        ram[11] = 0;
        time_used = 1;
       end;
    18:begin
        ram[68] = kills;
        ram[69] = kills-startkills;
        ram[70] = deaths;
        time_used = 3;
       end;
    19:begin ram[9] = 0; meters = 0; end;
    else robot_error(n,10,cstr(int_num));
   end;
  end;
end;

procedure jump(n,o:integer; var inc_ip:boolean);
var
 i,j,k,l,loc:integer;
begin
 with robot[n]^ do
  begin
   loc = find_label(n,get_val(n,ip,o),code[ip].op[max_op] shr (o*4));
   if (loc>=0) and (loc<=plen) then begin inc_ip = false; ip = loc; end
                               else robot_error(n,2,cstr(loc));
  end;
end;

{FIFI}
procedure update_debug_bars;
begin
 if graphix and (step_mode>0) then with robot[0]^ do
  begin
   {debugger window}
   viewport(480,4,635,410);

   {armour}
   setfillstyle(1,robot_color(0));
   if armor>0 then
    bar(88,03,87+(armor shr 2),08);
   setfillstyle(1,8);
   if armor<100 then
    bar(88+(armor shr 2),03,111,08);

   {heat}
   setfillstyle(1,robot_color(0));
   if heat>5 then
    bar(127,03,126+(heat div 20),08);
   setfillstyle(1,8);
   if heat<500 then
    bar(127+(heat div 20),03,151,08);
  end;
end;
{/FIFI}

{FIFI}
procedure update_debug_system;
var
 i:integer;
begin
 if graphix and (step_mode>0) then with robot[0]^ do
  begin
   {debugger window}
   viewport(480,4,635,410);
   setfillstyle(1,0);
   {for i = 0 to 6 do
    begin
     bar(039,32+(i*10),70,39+(i*10));
     bar(110,32+(i*10),141,39+(i*10));
    end;} {Commented out the black bars, and switched to 'textxy' -Bones}
   setcolor(robot_color(0));
   textxy(039,32,hex(ram[0])); textxy(110,32,hex(ram[7]));
   textxy(039,42,hex(ram[1])); textxy(110,42,hex(ram[8]));
   textxy(039,52,hex(ram[2])); textxy(110,52,hex(ram[9]));
   textxy(039,62,hex(ram[3])); textxy(110,62,hex(ram[10]));
   textxy(039,72,hex(ram[4])); textxy(110,72,hex(ram[11]));
   textxy(039,82,hex(ram[5])); textxy(110,82,hex(ram[12]));
   textxy(039,92,hex(ram[6])); textxy(110,92,hex(ram[13]));
  end;
end;
{/FIFI}

{FIFI}
procedure update_debug_registers;
var
 i:integer;
begin
 if graphix and (step_mode>0) then with robot[0]^ do
  begin
   {debugger window}
   viewport(480,4,635,410);
   setfillstyle(1,0);
   {for i = 0 to 4 do
    begin
     bar(039,122+(i*10),70,129+(i*10));
     bar(110,122+(i*10),141,129+(i*10));
    end;} {Commented out the black bars, and switched to 'textxy' -Bones}
   setcolor(robot_color(0));
   textxy(039,122,hex(ram[65])); textxy(110,122,hex(ram[69]));
   textxy(039,132,hex(ram[66])); textxy(110,132,hex(ram[70]));
   textxy(039,142,hex(ram[67]));
   textxy(039,152,hex(ram[68])); textxy(110,152,hex(ram[71]));
  end;
end;
{/FIFI}


{FIFI}
procedure update_debug_flags;
var
bin_string:string;
begin
 if graphix and (step_mode>0) then with robot[0]^ do
  begin
   {debugger window}
   viewport(480,4,635,410);
   bin_string = bin(ram[64]);
   if bin_string[13]='1' then setcolor(robot_color(0)) else setcolor(robot_color(8));
   outtextxy(111,172,'Z');
   if bin_string[14]='1' then setcolor(robot_color(0)) else setcolor(robot_color(8));
   outtextxy(119,172,'G');
   if bin_string[15]='1' then setcolor(robot_color(0)) else setcolor(robot_color(8));
   outtextxy(127,172,'L');
   if bin_string[16]='1' then setcolor(robot_color(0)) else setcolor(robot_color(8));
   outtextxy(135,172,'E');
   {setfillstyle(1,0);
   bar(015,182,142,189);
   setcolor(robot_color(0));}
   {Commented out the black bars, and switched to 'textxy' -Bones}
   textxy(015,182,bin(ram[64]));
  end;
end;
{/FIFI}

{FIFI}
procedure update_debug_memory;
var
 i:integer;
begin
 if graphix and (step_mode>0) then with robot[0]^ do
  begin
   {debugger window}
   viewport(480,4,635,410);
   {setfillstyle(1,0);
   for i = 0 to 9 do
    bar(091,212+(i*10),123,222+(i*10));}
    {Commented out the black bars, and switched to 'textxy' -Bones}
   setcolor(robot_color(0));
   for i = 0 to 9 do
    if ((mem_watch+i)>=0) and ((mem_watch+i)<=1023) then
     begin
      textxy(035,212+(10*i),decimal((mem_watch+i),4) + ' :');
      textxy(091,212+(10*i),hex(ram[mem_watch+i]));
     end;
  end;
end;
{/FIFI}

{FIFI}
procedure update_debug_code;
var
 i:integer;
begin
 if graphix and (step_mode>0) then with robot[0]^ do
  begin
   {debugger window}
   viewport(480,4,635,410);
   {code}
   setfillstyle(1,0);
   for i = 0 to 6 do bar(003,332+(i*10),152,340+(i*10));
   setcolor(7);
   for i = 0 to 6 do
    if (ip+i-3)>=0 then
     begin
      if i=3 then setcolor(robot_color(0));
      textxy(003,332+(i*10),decimal(ip+i-3,4)+': '
            +mnemonic(code[ip+i-3].op[0],code[ip+i-3].op[3] and 15)+' '
            +operand(code[ip+i-3].op[1],(code[ip+i-3].op[3] shr 4) and 15)+','
            +operand(code[ip+i-3].op[2],(code[ip+i-3].op[3] shr 8) and 15));
      if i=3 then setcolor(7);
     end;
  end;
end;
{/FIFI}


{FIFI}
procedure update_debug_window;
begin
 if graphix and (step_mode>0) then
  begin

   {armour + heat}
   update_debug_bars;

   {system variables}
   update_debug_system;

   {registers}
   update_debug_registers;

   {flag register}
   update_debug_flags;

   {memory}
   update_debug_memory;

   {code}
   update_debug_code;

  end;
end;
{/FIFI}

{FIFI}
procedure init_debug_window;
var i:integer;
begin
 if graphix then
 begin

 {debugger window}
 viewport(480,4,635,430);
 setfillstyle(1,7);
 bar(0,0,155,422);
 viewport(480,4,635,410);
 hole(0,0,155,402);

 with robot[0]^ do
  begin
   setcolor(robot_color(0));
   outtextxy(003,02,base_name(no_path(fn)));

   {armour}
   outtextxy(080,02,'A');
   setfillstyle(1,robot_color(0));

   {heat}
   outtextxy(118,02,'H');
   setfillstyle(1,robot_color(0));

   {system variables}
   setcolor(robot_color(8));
   outtextxy(007,22,'System:');
   setcolor(robot_color(0));
   outtextxy(015,32,'00:      07:');
   outtextxy(015,42,'01:      08:');
   outtextxy(015,52,'02:      09:');
   outtextxy(015,62,'03:      10:');
   outtextxy(015,72,'04:      11:');
   outtextxy(015,82,'05:      12:');
   outtextxy(015,92,'06:      13:');

   {registers}
   setcolor(robot_color(8));
   outtextxy(007,112,'Registers:');
   setcolor(robot_color(0));
   outtextxy(015,122,'ax:      ex:');
   outtextxy(015,132,'bx:      fx:');
   outtextxy(015,142,'cx:');
   outtextxy(015,152,'dx:      sp:');

   {flag register}
   setcolor(robot_color(8));
   outtextxy(007,172,'Flags:');

   {memory}
   setcolor(robot_color(8));
   outtextxy(007,202,'Memory:');
   setcolor(7);
   outtextxy(103,202,'-+,[]');
   setcolor(robot_color(0));
   for i = 0 to 9 do
    outtextxy(035,212+(10*i),decimal((mem_watch+i),4) + ' :');

   {code}
   setcolor(robot_color(8));
   outtextxy(007,322,'Code:');

  end;
 update_debug_window;

 end;
end;
{/FIFI}

{FIFI}
procedure close_debug_window;
var i:integer;
begin
 if graphix then
 begin

 stats_mode = 0;
 case num_robots of
  0..5:stats_mode = 0;
  6..11:stats_mode = 1;
  12..max_robots:stats_mode = 2;
  else stats_mode = 0;
 end;

 viewport(480,4,635,430);
 setfillstyle(1,7);
 bar(0,0,155,426);
{ update_cycle_window;}

 {--robot windows--}
 for i = 0 to max_robots do
  if i<max_shown then with robot[i]^ do
   begin
    robot_graph(i);
    hole(0,0,max_gx,max_gy);
    if i<=num_robots then
     begin
      setcolor(robot_color(i));
      outtextxy(003,02,base_name(no_path(fn)));
      case stats_mode of
       1:begin
          outtextxy(003,12,' A:');
          outtextxy(003,22,' H:');
         end;
       2:begin
          setcolor(robot_color(i) and 7);
          outtextxy(080,02,'A');
          outtextxy(118,02,'H');
         end;
       else begin
          outtextxy(003,24,' A:');
          outtextxy(003,34,' H:');
         end;
      end;
      setcolor(robot_color(i));
      if stats_mode<=1 then
       begin
        outtextxy(080,02,'Wins:');
        outtextxy(122,02,zero_pad(wins,4));
       end;
      if stats_mode=0 then
       begin
        outtextxy(003,56,' Error:');
        setcolor(robot_color(i) and 7);
        outtextxy(003,12,name);
        setcolor(8);
        outtextxy(66,56,'None');
       end;
      lx = 1000-x; ly = 1000-y;
      update_armor(i);
      update_heat(i);
      update_lives(i);
     end
    else
     begin
      setfillpattern(gray50,8);
      bar(1,1,max_gx-1,max_gy-1);
     end;
   end;

 end;
end;
{/FIFI}

function gameover:boolean;
var
 n,i,j,k,l:integer;
begin
 if (game_cycle>=game_limit) and (game_limit>0) then
     begin gameover = true; exit; end;
 if game_cycle and 31=0 then
  begin
   k = 0;
   for n = 0 to num_robots do
    if robot[n]^.armor>0 then inc(k);
   if k<=1 then gameover = true
           else gameover = false;
  end else gameover = false;
end;

procedure toggle_graphix;
begin
 graph_mode(not graphix);
 if not graphix then
  begin
   textcolor(7);
   writeln('Match ',played,'/',matches,', Battle in progress...');
   writeln;
  end
 else setscreen;
end;

function invalid_microcode(n,ip:integer):boolean;
var
 invalid:boolean;
 i,j,k:integer;
begin
 invalid = false;
 for i = 0 to 2 do
  begin
   k = (robot[n]^.code[ip].op[max_op] shr (i shl 2)) and 7;
   if not (k in [0,1,2,4]) then invalid = true;
  end;
 invalid_microcode = invalid;
end;

procedure process_keypress(c:char);
begin
 case c of
   'C':calibrate_timing;
   'T':timing = not timing;
   'A':show_arcs = not show_arcs;
   'S','Q':begin
            if sound_on then chirp;
            sound_on = not sound_on;
            if sound_on then chirp;
           end;
   '$':debug_info = not debug_info;
   'W':windoze = not windoze;
    #8:bout_over = true;
   #27:begin quit = true; step_loop = false; end;
 end;
end;

procedure execute_instruction(n:integer);
var
 i,j,k:integer;
 time_used,loc:integer;
 inc_ip:boolean;
 {FIFI}
 c:char;
 {/FIFI}
begin
 with robot[n]^ do
  begin
   {--update system variables--}
   ram[000] = tspd;
   ram[001] = thd;
   ram[002] = shift;
   ram[003] = accuracy;

   time_used = 1; inc_ip = true; loc = 0;
   if (ip>plen) or (ip<0) then ip = 0;
   if invalid_microcode(n,ip) then
      begin time_used = 1; robot_error(n,16,hex(code[ip].op[max_op])); end
    else

  {FIFI}
  if graphix and (step_mode>0) and (n=0) then  {if stepping enabled...}
   begin
    inc(step_count);
    update_cycle_window;
    update_debug_window;
    if (step_count mod step_mode)=0 then step_loop = true else step_loop = false;
    while step_loop and (not(quit or gameover or bout_over)) do
     if keypressed then with robot[0]^ do
      begin
       c = upcase(readkey);
       case c of
        'X':begin
             temp_mode = step_mode;
             step_mode = 0;
             step_loop = false;
             close_debug_window;
            end;
        ' ':begin step_count = 0; step_loop = false; end;
        '1'..'9':begin step_mode = value(c); step_count = 0; step_loop = false; end;
        '0':begin step_mode = 10; step_count = 0; step_loop = false; end;
        '-','_':if mem_watch>0 then
                 begin
                  setcolor(0);
                  for i = 0 to 9 do
                   outtextxy(035,212+(10*i),decimal((mem_watch+i),4) + ' :');
                  dec(mem_watch);
                  update_debug_memory;
                 end;
        '+','=':if mem_watch<1014 then
                 begin
                  setcolor(0);
                  for i = 0 to 9 do
                   outtextxy(035,212+(10*i),decimal((mem_watch+i),4) + ' :');
                  inc(mem_watch);
                  update_debug_memory;
                 end;
        '[','{':if mem_watch>0 then
                 begin
                  setcolor(0);
                  for i = 0 to 9 do
                   outtextxy(035,212+(10*i),decimal((mem_watch+i),4) + ' :');
                  dec(mem_watch,10);
                  if mem_watch<0 then mem_watch = 0;
                  update_debug_memory;
                 end;
        ']','}':if mem_watch<1014 then
                 begin
                  setcolor(0);
                  for i = 0 to 9 do
                   outtextxy(035,212+(10*i),decimal((mem_watch+i),4) + ' :');
                  inc(mem_watch,10);
                  if mem_watch>1014 then mem_watch = 1014;
                  update_debug_memory;
                 end;
        'G':begin toggle_graphix; temp_mode = step_mode; step_mode = 0; step_loop = false; end;
        else process_keypress(c);
       end;
      end;
   end;
  {/FIFI}

   if (not ((code[ip].op[max_op] and 7) in [0,1])) then
      begin time_used = 0; end
    else
   case get_val(n,ip,0) of
    00:begin (*NOP*)
        inc(executed);
       end;
    01:begin (*ADD*)
        put_val(n,ip,1,get_val(n,ip,1)+get_val(n,ip,2));
        inc(executed);
       end;
    02:begin (*SUB*)
        put_val(n,ip,1,get_val(n,ip,1)-get_val(n,ip,2));
        inc(executed);
       end;
    03:begin (*OR*)
        put_val(n,ip,1,get_val(n,ip,1) or get_val(n,ip,2));
        inc(executed);
       end;
    04:begin (*AND*)
        put_val(n,ip,1,get_val(n,ip,1) and get_val(n,ip,2));
        inc(executed);
       end;
    05:begin (*XOR*)
        put_val(n,ip,1,get_val(n,ip,1) xor get_val(n,ip,2));
        inc(executed);
       end;
    06:begin (*NOT*)
        put_val(n,ip,1,not(get_val(n,ip,1)));
        inc(executed);
       end;
    07:begin (*MPY*)
        put_val(n,ip,1,get_val(n,ip,1)*get_val(n,ip,2));
        time_used = 10;
        inc(executed);
       end;
    08:begin (*DIV*)
        j = get_val(n,ip,2);
        if j<>0 then put_val(n,ip,1,get_val(n,ip,1) div j)
                else robot_error(n,8,'');
        time_used = 10;
        inc(executed);
       end;
    09:begin (*MOD*)
        j = get_val(n,ip,2);
        if j<>0 then put_val(n,ip,1,get_val(n,ip,1) mod j)
                else robot_error(n,8,'');
        time_used = 10;
        inc(executed);
       end;
    10:begin (*RET*)
        ip = pop(n);
        if (ip<0) or (ip>plen) then robot_error(n,7,cstr(ip));
        inc(executed);
       end;
    11:begin (*GSB*)
        loc = find_label(n,get_val(n,ip,1),code[ip].op[max_op] shr (1*4));
        if loc>=0 then begin push(n,ip); inc_ip = false; ip = loc; end
                  else robot_error(n,2,cstr(get_val(n,ip,1)));
        inc(executed);
       end;
    12:begin (*JMP*)
        jump(n,1,inc_ip);
        inc(executed);
       end;
    13:begin (*JLS,JB*)
        if ram[64] and 2>0 then
           jump(n,1,inc_ip);
        time_used = 0;
        inc(executed);
       end;
    14:begin (*JGR,JA*)
        if ram[64] and 4>0 then
           jump(n,1,inc_ip);
        time_used = 0;
        inc(executed);
       end;
    15:begin (*JNE*)
        if ram[64] and 1=0 then
           jump(n,1,inc_ip);
        time_used = 0;
        inc(executed);
       end;
    16:begin (*JEQ,JE*)
        if ram[64] and 1>0 then
           jump(n,1,inc_ip);
        time_used = 0;
        inc(executed);
       end;
    17:begin (*SWAP, XCHG*)
        ram[4] = get_val(n,ip,1);
        put_val(n,ip,1,get_val(n,ip,2));
        put_val(n,ip,2,ram[4]);
        time_used = 3;
        inc(executed);
       end;
    18:begin (*DO*)
        ram[67] = get_val(n,ip,1);
        inc(executed);
       end;
    19:begin (*LOOP*)
        dec(ram[67]);
        if ram[67]>0 then
           jump(n,1,inc_ip);
        inc(executed);
       end;
    20:begin (*CMP*)
        k = get_val(n,ip,1)-get_val(n,ip,2);
        ram[64] = ram[64] and $FFF0;
        if k=0 then ram[64] = ram[64] or 1;
        if k<0 then ram[64] = ram[64] or 2;
        if k>0 then ram[64] = ram[64] or 4;
        if (get_val(n,ip,2)=0) and (k=0) then
                    ram[64] = ram[64] or 8;
        inc(executed);
       end;
    21:begin (*TEST*)
        k = get_val(n,ip,1) and get_val(n,ip,2);
        ram[64] = ram[64] and $FFF0;
        if k=get_val(n,ip,2) then ram[64] = ram[64] or 1;
        if k=0               then ram[64] = ram[64] or 8;
        inc(executed);
       end;
    22:begin (*MOV, SET*)
        put_val(n,ip,1,get_val(n,ip,2));
        inc(executed);
       end;
    23:begin (*LOC*)
        put_val(n,ip,1,code[ip].op[2]);
        time_used = 2;
        inc(executed);
       end;
    24:begin (*GET*)
        k = get_val(n,ip,2);
        if (k>=0) and (k<=max_ram) then
            put_val(n,ip,1,ram[k])
          else if (k>max_ram) and (k<=(Max_ram+1)+(((max_code+1) shl 3)-1)) then
           begin
            j = k-max_ram-1;
            put_val(n,ip,1,code[j shr 2].op[j and 3]);
           end
          else robot_error(n,4,cstr(k));
        time_used = 2;
        inc(executed);
       end;
    25:begin (*PUT*)
        k = get_val(n,ip,2);
        if (k>=0) and (k<=max_ram) then
            ram[k] = get_val(n,ip,1)
          else robot_error(n,4,cstr(k));
        time_used = 2;
        inc(executed);
       end;
    26:begin (*INT*)
        call_int(n,get_val(n,ip,1),time_used);
        inc(executed);
       end;
    27:begin (*IPO,IN*)
        time_used = 4;
        put_val(n,ip,2,in_port(n,get_val(n,ip,1),time_used));
        inc(executed);
       end;
    28:begin (*OPO,OUT*)
        time_used = 4;
        out_port(n,get_val(n,ip,1),get_val(n,ip,2),time_used);
        inc(executed);
       end;
    29:begin (*DEL,DELAY*)
        time_used = get_val(n,ip,1);
        inc(executed);
       end;
    30:begin (*PUSH*)
        push(n,get_val(n,ip,1));
        inc(executed);
       end;
    31:begin (*POP*)
        put_val(n,ip,1,pop(n));
        inc(executed);
       end;
    32:begin (*ERR*)
        robot_error(n,get_val(n,ip,1),'');
        time_used = 0;
        inc(executed);
       end;
    33:begin (*INC*)
        put_val(n,ip,1,get_val(n,ip,1)+1);
        inc(executed);
       end;
    34:begin (*DEC*)
        put_val(n,ip,1,get_val(n,ip,1)-1);
        inc(executed);
       end;
    35:begin (*SHL*)
        put_val(n,ip,1,get_val(n,ip,1) shl get_val(n,ip,2));
        inc(executed);
       end;
    36:begin (*SHR*)
        put_val(n,ip,1,get_val(n,ip,1) shr get_val(n,ip,2));
        inc(executed);
       end;
    37:begin (*ROL*)
        put_val(n,ip,1,rol(get_val(n,ip,1),get_val(n,ip,2)));
        inc(executed);
       end;
    38:begin (*ROR*)
        put_val(n,ip,1,ror(get_val(n,ip,1),get_val(n,ip,2)));
        inc(executed);
       end;
    39:begin (*JZ*)
        time_used = 0;
        if ram[64] and 8>0 then
           jump(n,1,inc_ip);
        inc(executed);
       end;
    40:begin (*JNZ*)
        time_used = 0;
        if ram[64] and 8=0 then
           jump(n,1,inc_ip);
        inc(executed);
       end;
    41:begin (*JAE,JGE*)
        if (ram[64] and 1>0) or (ram[64] and 4>0) then
           jump(n,1,inc_ip);
        time_used = 0;
        inc(executed);
       end;
    42:begin (*JBE,JLE*)
        if (ram[64] and 1>0) or (ram[64] and 2>0) then
           jump(n,1,inc_ip);
        time_used = 0;
        inc(executed);
       end;
    43:begin (*SAL*)
        put_val(n,ip,1,sal(get_val(n,ip,1),get_val(n,ip,2)));
        inc(executed);
       end;
    44:begin (*SAR*)
        put_val(n,ip,1,sar(get_val(n,ip,1),get_val(n,ip,2)));
        inc(executed);
       end;
    45:begin (*NEG*)
        put_val(n,ip,1,0-get_val(n,ip,1));
        inc(executed);
       end;
    46:begin (*JTL*)
        loc = get_val(n,ip,1);
        if (loc>=0) and (loc<=plen) then begin inc_ip = false; ip = loc; end
                                    else robot_error(n,2,cstr(loc));
       end;
    else begin robot_error(n,6,''); end;
   end;
   inc(delay_left,time_used);
   if inc_ip then inc(ip);
   {FIFI}
   if graphix and (n=0) and (step_mode>0) then update_debug_window;
   {/FIFI}
  end;
end;

procedure do_robot(n:integer);
var
 i,j,k,l,tthd:integer;
 heat_mult,ttx,tty:real;
begin
 if (n<0) or (n>num_robots) then exit;
 with robot[n]^ do
  begin
   if armor<=0 then exit;

   {--time slice--}
   time_left = time_slice;
   if (time_left>robot_time_limit) and (robot_time_limit>0) then
      time_left = robot_time_limit;
   if (time_left>max_time) and (max_time>0) then time_left = max_time;
   executed = 0;

   {--execute timeslice--}
    while (time_left>0) and (not cooling) and (executed<20+time_slice) and (armor>0) do
     begin
      if delay_left<0 then delay_left = 0;
      if (delay_left>0) then begin dec(delay_left); dec(time_left); end;
      if (time_left>=0) and (delay_left=0) then
        execute_instruction(n);
      if heat>=shutdown then begin cooling = true; shields_up = false; end;
      if heat>=500 then damage(n,1000,true);
     end;

   {--fix up variables--}
   thd = (thd+1024) and 255;
   hd = (hd+1024) and 255;
   shift = (shift+1024) and 255;
   if tspd<-75 then tspd = -75;
   if tspd>100 then tspd = 100;
   if spd<-75 then spd = -75;
   if spd>100 then spd = 100;
   if heat<0 then heat = 0;
   if last_damage<maxint then inc(last_damage);
   if last_hit<maxint then inc(last_hit);

   {--update heat--}
   if shields_up and (game_cycle and 3=0) then inc(heat);
   if not shields_up then
    begin
     if heat>0 then
      case config.heatsinks of  (*heat adjustments*)
       5:if game_cycle and 1=0 then dec(heat);
       4:if game_cycle mod 3=0 then dec(heat);
       3:if game_cycle and 3=0 then dec(heat);
       2:if game_cycle and 7=0 then dec(heat);
       1:begin (*Do nothing, no adjustment needed*) end;
       else if game_cycle and 3=0 then inc(heat);
      end;
     if overburn and (game_cycle mod 3=0) then inc(heat);
     if (heat>0) then dec(heat);
     if (heat>0) and (game_cycle and 7=0) and (abs(tspd)<=25) then dec(heat);
     if (heat<=shutdown-50) or (heat<=0) then cooling = false;
    end;
   if cooling then begin tspd = 0; end;
   heat_mult = 1;
   case heat of
    080..099:heat_mult = 0.98;
    100..149:heat_mult = 0.95;
    150..199:heat_mult = 0.85;
    200..249:heat_mult = 0.75;
    250..maxint:heat_mult = 0.50;
   end;
   if overburn then heat_mult = heat_mult*1.30;
   if (heat>=475) and (game_cycle and 03=0) then damage(n,1,true) else
   if (heat>=450) and (game_cycle and 07=0) then damage(n,1,true) else
   if (heat>=400) and (game_cycle and 15=0) then damage(n,1,true) else
   if (heat>=350) and (game_cycle and 31=0) then damage(n,1,true) else
   if (heat>=300) and (game_cycle and 63=0) then damage(n,1,true);

   {--update robot in physical world--}
   {-acceleration-}
   if abs(spd-tspd)<=acceleration then spd = tspd
      else begin
            if tspd>spd then inc(spd,acceleration)
                        else dec(spd,acceleration);
           end;
   {-turning-}
   tthd = hd+shift;
   if (abs(hd-thd)<=turn_rate) or (abs(hd-thd)>=256-turn_rate) then
      hd = thd
    else if hd<>thd then
     begin
      k = 0;
      if ((thd>hd) and (abs(hd-thd)<=128)) or
         ((thd<hd) and (abs(hd-thd)>=128)) then k = 1;
      if k=1 then hd = (hd+turn_rate) and 255
             else hd = (hd+256-turn_rate) and 255;
     end;
   hd = hd and 255;
   if keepshift then shift = (tthd-hd+1024) and 255;
   {-moving-}
   speed = spd/100*(max_vel*heat_mult*speedadj);
   xv =  sint[hd]*speed;
   yv = -cost[hd]*speed;
   if (hd=0) or (hd=128) then xv = 0;
   if (hd=64) or (hd=192) then yv = 0;
   if xv<>0 then ttx = x+xv else ttx = x;
   if yv<>0 then tty = y+yv else tty = y;
   if (ttx<0) or (tty<0) or (ttx>1000) or (tty>1000) then
      begin inc(ram[8]); tspd = 0; if abs(speed)>=max_vel/2 then damage(n,1,true); spd = 0; {ttx = x; tty = y;} end;
   for i = 0 to num_robots do
    if (i<>n) and (robot[i]^.armor>0) and (distance(ttx,tty,robot[i]^.x,robot[i]^.y)<crash_range)
       then begin
             tspd = 0; spd = 0; ttx = x; tty = y;
             robot[i]^.tspd = 0; robot[i]^.spd = 0;
             inc(ram[8]); inc(robot[i]^.ram[8]);
             if abs(speed)>=max_vel/2 then begin damage(n,1,true); damage(i,1,true); end;
            end;
   if ttx<0 then ttx = 0;
   if tty<0 then tty = 0;
   if ttx>1000 then ttx = 1000;
   if tty>1000 then tty = 1000;
   meters = meters+distance(x,y,ttx,tty);
   if meters>=maxint then meters = meters-maxint;
   ram[9] = trunc(meters);
   x = ttx; y = tty;


   {--draw robot--}
   if armor<0 then armor = 0;
   if heat<0 then heat = 0;
   if graphix then
    begin
     if armor<>larmor then update_armor(n);
     if heat div 5<>lheat div 5 then update_heat(n);
     draw_robot(n);
    end;
   lheat = heat; larmor = armor;

   {--Cycle complete, inc counter--}
   inc(cycles_lived);
  end;
end;

procedure do_mine(n,m:integer);
var
 i,j,k,l:integer;
 d,r:real;
 source_alive:boolean;
begin
 with robot[n]^.mine[m] do
  if (x>=0) and (x<=1000) and (y>=0) and (y<=1000) and (yield>0) then
   begin
    for i = 0 to num_robots do
     if (robot[i]^.armor>0) and (i<>n) then
      begin
       d = distance(x,y,robot[i]^.x,robot[i]^.y);
       if d<=detect then detonate = true;
      end;
    if detonate then
     begin
      init_missile(x,y,0,0,0,n,mine_circle,false);
      kill_count = 0;
      if (robot[n]^.armor>0) then source_alive = true else source_alive = false;
      for i = 0 to num_robots do
       if {(i<>n) and} (robot[i]^.armor>0) then
        begin
         k = round(distance(x,y,robot[i]^.x,robot[i]^.y));
         if k<yield then
          begin
           damage(i,round(abs(yield-k)),false);
           if (n in [0..num_robots]) and (i<>n) then
              inc(robot[n]^.damage_total,round(abs(yield-k)));
          end;
        end;
       if (kill_count>0) and (source_alive) and (robot[n]^.armor<=0) then dec(kill_count);
       if kill_count>0 then
        begin
         inc(robot[n]^.kills,kill_count);
         update_lives(n);
        end;
      if graphix then
       putpixel(round(x*screen_scale)+screen_x,round(y*screen_scale)+screen_y,0);
      yield = 0; x = -1; y = -1;
     end
    else
     begin
      (* Draw mine *)
      if graphix and (game_cycle and 1=0) then
       begin
        main_viewport;
        setcolor(robot_color(n));
        line(round(x*screen_scale)+screen_x,round(y*screen_scale)+screen_y-1,
             round(x*screen_scale)+screen_x,round(y*screen_scale)+screen_y+1);
        line(round(x*screen_scale)+screen_x+1,round(y*screen_scale)+screen_y,
             round(x*screen_scale)+screen_x-1,round(y*screen_scale)+screen_y);
       end;
     end;
   end;
end;

procedure do_missile(n:integer);
var
 llx,lly,r,d,dir,xv,yv:real;
 i,j,k,l,xx,yy,tx,ty,dd,dam:integer;
 source_alive:boolean;
begin
 with missile[n] do
  if a=0 then exit else
  begin
   if a=1 then
    begin
     {--Look for collision w/ wall--}
     if (x<-20) or (x>1020) or (y<-20) or (y>1020) then a = 0;

     {--move missile--}
     llx = lx; lly = ly;
     lx = x; ly = y;
     if a>0 then
      begin
       hd = (hd+256) and 255;
       xv =  sint[hd]*mspd;
       yv = -cost[hd]*mspd;
       x = x+xv;
       y = y+yv;
      end;

     {--look for hit on a robot--}
     k = -1; l = maxint;
     for i = 0 to num_robots do
      if (robot[i]^.armor>0) and (i<>source) then
       begin
        d = distance(lx,ly,robot[i]^.x,robot[i]^.y);
        {dir = find_angle(lx,ly,robot[i]^.x,robot[i]^.y);
        j = (round(dir/pi*128)+1024) and 255;}
        hd = hd and 255;
        xx = round( sint[hd]*d+lx);
        yy = round(-cost[hd]*d+ly);
        r = distance(xx,yy,robot[i]^.x,robot[i]^.y);
        if (d<=mspd) and (r<hit_range) and (round(d)<=l) then
         begin k = i; l = round(d); dd = round(r); tx = xx; ty = yy; end;
       end;
     if k>=0 then  (*hit a robot!*)
      begin
       x = tx; y = ty; a = 2; rad = 0; lrad = 0;
       if source in [0..num_robots] then
        begin
         robot[source]^.last_hit = 0;
         inc(robot[source]^.hits);
        end;
       for i = 0 to num_robots do
        begin
         dd = round(distance(x,y,robot[i]^.x,robot[i]^.y));
         if dd<=hit_range then
           begin
            dam = round(abs(hit_range-dd)*mult);
            if dam<=0 then dam = 1;
            kill_count = 0;
            if (robot[source]^.armor>0) then source_alive = true else source_alive = false;
            damage(i,dam,false);
            if (source in [0..num_robots]) and (i<>source) then
              inc(robot[source]^.damage_total,dam);
            if (kill_count>0) and (source_alive) and (robot[source]^.armor<=0) then dec(kill_count);
            if kill_count>0 then
             begin
              inc(robot[source]^.kills,kill_count);
              update_lives(source);
             end;
           end;
        end;
      end;

     {--draw missile--}
     if graphix then
      begin
       main_viewport;
       setcolor(0);
       line(round(llx*screen_scale)+screen_x,round(lly*screen_scale)+screen_y,
            round(lx*screen_scale)+screen_x,round(ly*screen_scale)+screen_y);
       if a=1 then
        begin
         if mult>robot[source]^.shotstrength then setcolor(14+(game_cycle and 1)) else setcolor(15);
         line(round(x*screen_scale)+screen_x,round(y*screen_scale)+screen_y,
              round(lx*screen_scale)+screen_x,round(ly*screen_scale)+screen_y);
        end;
      end;
    end;
   if (a=2) then
    begin
     lrad = rad;
     inc(rad);
     if rad>max_rad then a = 0;
     if graphix then
      begin
       main_viewport;
       setcolor(0);
       circle(round(x*screen_scale)+screen_x,round(y*screen_scale)+screen_y,lrad);
       if mult>1 then setcolor(14+(game_cycle and 1)) else setcolor(15);
       if max_rad>=blast_circle then setcolor(14);
       if max_rad>=mine_circle then setcolor(11);
       if a>0 then
        circle(round(x*screen_scale)+screen_x,round(y*screen_scale)+screen_y,rad);
      end;
    end;
  end;
end;

function victor_string(k,n:integer):string;
var
 s:string;
begin
 s = '';
 if k=1 then s = 'Robot #'+cstr(n+1)+' ('+robot[n]^.fn+') wins!';
 if k=0 then s = 'Simultaneous destruction, match is a tie.';
 if k>1 then s = 'No clear victor, match is a tie.';
 victor_string = s;
end;

procedure show_statistics;
var
 i,j,k,n,sx,sy:integer;
begin
 if not windoze then exit;
 if graphix then
  begin
   sx = 24; sy = 93-num_robots*3;
   viewport(0,0,639,479);
   box(sx+000,sy,sx+591,sy+102+num_robots*12);
   hole(sx+004,sy+004,sx+587,sy+098+num_robots*12);
   setfillpattern(gray50,1);
   bar(sx+005,sy+005,sx+586,sy+097+num_robots*12);
   setcolor(15);
   outtextxy(sx+016,sy+020,'Robot            Scored   Wins  Matches  Armor  Kills  Deaths    Shots');
   outtextxy(sx+016,sy+030,'~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~');
   n = -1; k = 0;
   for i = 0 to num_robots do
    with robot[i]^ do
     if (armor>0) or (won) then begin inc(k); n = i; end;
   for i = 0 to num_robots do
    with robot[i]^ do
     begin
      setcolor(robot_color(i));
      if (k=1) and (n=i) then j = 1 else j = 0;
      outtextxy(sx+016,sy+042+i*12,addfront(cstr(i+1),2)+' - '+addrear(fn,15)+cstr(j)
               +addfront(cstr(wins),8)+addfront(cstr(trials),8)
               +addfront(cstr(armor)+'%',9)+addfront(cstr(kills),7)
               +addfront(cstr(deaths),8)+addfront(cstr(match_shots),9));
     end;
   setcolor(15);
   outtextxy(sx+016,sy+064+num_robots*12,victor_string(k,n));
   if windoze then
    begin
     outtextxy(sx+016,sy+076+num_robots*12,'Press any key to continue...');
     flushkey; readkey;
    end;
  end
 else
  begin
   textcolor(15);
   write(#13+space(79)+#13);
   writeln('Match ',played,'/',matches,' results:');
   writeln;
   writeln('Robot            Scored   Wins  Matches  Armor  Kills  Deaths    Shots');
   writeln('~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~');
   n = -1; k = 0;
   for i = 0 to num_robots do
    with robot[i]^ do
     if (armor>0) or (won) then begin inc(k); n = i; end;
   for i = 0 to num_robots do
    with robot[i]^ do
     begin
      textcolor(robot_color(i));
      if (k=1) and (n=i) then j = 1 else j = 0;
      writeln(addfront(cstr(i+1),2)+' - '+addrear(fn,15)+cstr(j)
             +addfront(cstr(wins),8)+addfront(cstr(trials),8)
             +addfront(cstr(armor)+'%',9)+addfront(cstr(kills),7)
             +addfront(cstr(deaths),8)+addfront(cstr(match_shots),9));
     end;
   textcolor(15);
   writeln;
   writeln(victor_string(k,n));
   writeln;
   {writeln('Press any key to continue...'); readkey;}
  end;
end;

procedure score_robots;
var
 n,i,j,k,l:integer;
begin
 k = 0; n = -1;
 for i = 0 to num_robots do
  begin
   inc(robot[i]^.trials);
   if robot[i]^.armor>0 then begin inc(k); n = i; end;
  end;
 if (k=1) and (n>=0) then
  with robot[n]^ do
   begin inc(wins); won = true; end;
end;

procedure init_bout;
var
 i,j,k:integer;
begin
 game_cycle = 0;
 for i = 0 to max_missiles do
  with missile[i] do
   begin a = 0; source = -1; x = 0; y = 0; lx = 0; ly = 0; mult = 1; end;
 for i = 0 to num_robots do
  with robot[i]^ do
   begin
    {FIFI}
    mem_watch = 128;
    {/FIFI}
    reset_hardware(i);
    reset_software(i);
   end;
 if graphix then setscreen;
 {FIFI}
 if graphix and (step_mode>0) then init_debug_window;
 {/FIFI}
 if not graphix then
  begin
   textcolor(7);
   {writeln(#13+'Match ',played,'/',matches,', Battle in progress...');
   writeln;}
  end;
end;

void bout() {
	short i, j, k;
	char c;
	short timer;
	short n; // loop count for mem_watch

	if (quit)
		return;

	played++;

	init_bout();
	bout_over = false;

	if (step_mode = 0)
		step_loop = false;
	 else 
		step_loop = true; // set/clear loop flag

	step_count = -1; // will be set to 0 at first iteration of REPEAT

	if (graphix && (step_mode>0))
		for (i = 0; i <= num_robots; i++)
			draw_robot(i);

	do {
		game_cycle++;
		for (i = 0; i <= num_robots; i++) {
			if (robot[i] -> armor > 0)
				do_robot(i);

		for (i = 0; i <= max_missiles; i++) 
			if (missile[i].a > 0)
				do_missile(i);

		for (i = 0; i <= num_robots; i++)
			for (k = 0; k <= max_mines; k++)
				if (robot[i] -> mine[k].yield > 0)
					do_mine(i,k);

		if (graphix && timing) 
			SDL_Delay(game_delay);

 
		if ( keypressed )
			c = readkey
		else 
			c = 255;

		switch (c) {

		case 'X': 
			if ( !robot[0] -> is_locked )
				if ( !graphix)
					toggle_graphix();

				if ( robot[0] -> armor > 0 )
					if ( temp_mode > 0)
						step_mode = temp_mode
					else
						step_mode = 1;

					step_count = -1;
					init_debug_window();
				}
			}
			break;

		case '+':
		case '=':
			if ( game_delay < 100 )
				switch (game_delay) { // This range notation for this switch case is only supported in gcc and clang
				case 0 ... 4:
					game_delay = 5;
					break;

				case 5 ... 9:
					game_delay = 10;
					break;

				case 10 ... 14:
					game_delay = 15;
					break;

				case 15 ... 19:
					game_delay = 20;
					break;

				case 20 ... 29:
					game_delay = 30;
					break;

				case 30 ... 39:
					game_delay = 40;
					break;

				case 40 ... 49:
					game_delay = 50;
					break;

				case 50 ... 59:
					game_delay = 60;
					break;

				case 60 ... 74:
					game_delay = 75;
					break;

				case 75 ... 100:
					game_delay = 100;
					break;
				}
			break;

		case '-':
		case '_':
			if (game_delay > 0)
				switch (game_delay){ 
				case 0 ... 5:
					game_delay = 0;
					break;

				case 6 ... 10:
					game_delay = 5;
					break;

				case 11 ... 15:
					game_delay = 10;
					break;

				case 16 ... 20:
					game_delay = 15;
					break;

				case 21 ... 30:
					game_delay = 20;
					break;

				case 31 ... 40:
					game_delay = 30;
					break;

				case 41 ... 50:
					game_delay = 40;
					break;

				case 51 ... 60:
					game_delay = 50;
					break;

				case 61 ... 75:
					game_delay = 60;
					break;

				case 76 ... 100:
					game_delay = 75;
					break;
				}
			break;

		case 'G':
			toggle_graphix();
			break;

		default:
			process_keypress(c);
		}

		flushkey();

		if (game_delay < 0)
			game_delay = 0;

		if (game_delay > 100)
			game_delay = 100;
 
		switch (game_delay) {
		case 0 ... 1:
			k = 100;
			break;

		case 2 ... 5:
			k = 50;
			break;

		case 2 6 ... 10:
			k = 25;
			break;
	
		case 11 ... 25:
			k = 20;
			break;

		case 26 ... 40:
			k = 10;
			break;

		case 41 ... 70:
			k = 5;
			break;

		case 71 ... maxint:
			k = 1;
			break;

		default:
			k = 10;
		}

		if ( !graphix)
			k = 100;

		if (graphix) {
			if ( ((game_cycle % k) == 0) || ( game_cycle == 10) )
			update_cycle_window();
		} else {
/*			if ( update_timer != (mem[0:$46C] >> 1) )
				update_cycle_window();

			update_timer = mem[0:$46C] >> 1;
	mem[0:$46C] is a direct memory access. The important thing is that update timer gets set to the raw memory access.


*/		}
	} while (!quit || !gameover || !bout_over);

	update_cycle_window();

	score_robots();
	show_statistics();
)

procedure write_report;
var
 i,j,k:integer;
 f:text;
begin
 assign(f,main_filename+report_ext);
 rewrite(f);
 writeln(f,num_robots+1);
 for i = 0 to num_robots do
  with robot[i]^ do
   case report_type of
    2:writeln(f,wins,' ',trials,' ',kills,' ',deaths,' ',fn,' ');
    3:writeln(f,wins,' ',trials,' ',kills,' ',deaths,' ',armor,' ',
                heat,' ',shots_fired,' ',fn,' ');
    4:writeln(f,wins,' ',trials,' ',kills,' ',deaths,' ',armor,' ',
                heat,' ',shots_fired,' ',hits,' ',damage_total,' ',
                cycles_lived,' ',error_count,' ',fn,' ');
    else writeln(f,wins,' ',trials,' ',fn,' ');
   end;
 close(f);
end;

void begin_window {
	string s;
	int temp_w, temp_h;

	if (!graphix || !windoze)
		exit(EXIT_FAILURE);

	if (SDL_CreateWindowAndRenderer(639, 479, SDL_WINDOW_INPUT_GRABBED|SDL_WINDOW_RESIZEABLE, &main_window, &main_renderer)) { // Only true if window/renderer cannot be initialized
		fprintf(stderr, "ERROR: Cannot create window/renderer. \n%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	box(100,150,539,200);	// Located in atr2func
	hole(105,155,534,195);	// Located in atr2func

//	setfillpattern(gray50,1); // Not sure how to fill this in
	bar(105,155,534,195);

	setcolor(15);
	s = 'Press any key to begin!';


	// Create surface of text
	SDL_Surface *tmp_surf = TTF_RenderText_Solid(main_font, s.c_str(), main_color);
	SDL_Texture *tmp_text = SDL_CreateTextureFromSurface(main_renderer, tmp_surf);
	SDL_DestroySurface(tmp_surf);

	// Grab length and width of text
	SDL_QueryTexture(tmp_text, NULL, NULL, &temp_w, &temp_h);

	SDL_Rect * temp_rect = (SDL_Rect *) malloc(sizeof(SDL_Rect));
	temp_rect -> x = 320 - ((s.length() << 3) >> 1);
	temp_rect -> y = 172;
	temp_rect -> w = temp_w;
	temp_rect -> h = temp_h;

	SDL_RenderCopy(main_renderer, tmp_text, NULL, temp_rect);
	SDL_RenderPresent();

	SDL_DestroyTexture(tmp_text);
	free(temp_rext);

	readkey();

// setscreen; setscreen refreshes the screen
}


// Main loop for game
void main() {
	short i, j, k, l, n, w;

	if ( graphix )
		begin_window();

	if (matches > 0) {
		for (i=1; i <= matches; i++) {
			bout(); // Main battle function
		}
	}

	if ( !graphix )
		cout << endl;

	if ( quit() )
		exit(EXIT_FAILURE);

	if (matches > 1) { // {--Calculate overall statistics--}
		cout << endl << endl; // Gotta keep the code one for one, unfortunately. I think this just prints to the line. Without parameters, perhaps it's just a new line?
		graph_mode(FALSE);
		// textcolor(15); 		// Need to switch this function with something from SDL_ttf
		cout << "Bout complete! (" << matches << " matches)" << endl;
		cout << endl;

		k = 0; w = 0;
		for (i = 0; i <= num_robots; i++) {
			if (robot[i] -> wins = w) then 
				inc(k);

			if (robot[i] -> wins > w) {
				k = 1;
				n = i;
				w = wins;
			}
		}

		cout << "Robot           Wins  Matches  Kills  Deaths    Shots" << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

		for (i = 0; i <= num_robots; i++) {
		with robot[i]^ do
			begin
			textcolor(robot_color(i));

			// addfront returns string
			cout << addfront(cstr(i+1),2) << " - " << addrear(robot[i]->fn,8) << addfront(cstr(robot[i] -> wins),7) << addfront(cstr(robot[i] -> trials),8) <<
			addfront(cstr(robot[i] -> kills),8)addfront(cstr(robot[i] -> deaths),8) << addfront(cstr(robot[i]shots_fired),9)");
			end;
		}

		// textcolor(15);
		cout << endl;
		if (k = 1)
			cout << "Robot #" << (n+1) << " (" << robot[n] -> fn << " ) wins the bout! (score: " << w << "/" << matches << endl;
		else
			cout << "There is no clear victor!" << endl;

		cout << endl;
	}

	else if ( graphix ) {
		// {matches=1 also}
		graph_mode(FALSE);
		show_statistics();
	}

	if ( report )
		write_report();
}

void readkey() {
	SDL_Event e;
	short	index = 0;


	Beginning:
	while(1) {
		SDL_PollEvent(&e);
		if ( e.type == SDL_KEYDOWN )
			break;

		SDL_Delay(1);
	}

	keyboard_state = SDL_GetKeyboardState(NULL);

	while ((keyboard_state[index] ^ index) && (index < 30))
		index++;

	if (index == 30)
		goto Beginning;

	keypressed = index + 62;
}

void flushkey() {
	keypressed = 0;
}

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

// Main function for entire program
int main(int argc, char **argv) {
	init();
	main();
	shutdown();

	return EXIT_SUCCESS;
}
