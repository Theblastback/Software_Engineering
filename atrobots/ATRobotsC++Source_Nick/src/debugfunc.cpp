#include <iostream>
#include <string>

using namespace std;

bool logging_errors = true;

ofstream log;

/*
 * Logs errors to the console for debugging robots.
 */
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

void prog_error(int n, string ss) {
    graph_mode(false);
    log.open("errlog.txt");
    log << endl;
    log << endl;
    switch (n) {
        case 0:
            log << ss;
            break;
        case 1:
            log << "Invalid :label - " + ss + ", silly mortal.";
            break;
        case 2:
            log << "Undefined identifier - " + ss + ". A typo perhaps?";
            break;
        case 3:
            log << "Memory access out of range - " + ss + "";
            break;
        case 4:
            log << "Not enough robots for combat. Maybe we should just drive in circles.";
            break;
        case 5:
            log << "Robot names and settings must be specified. An empty arena is no fun.";
            break;
        case 6:
            log << "Config file not found - " + ss + "";
            break;
        case 7:
            log << "Cannot access a config file from a config file - " + ss + "";
            break;
        case 8:
            log << "Robot not found " + ss + ". Perhaps you mistyped it?";
            break;
        case 9:
            log << "Insufficient RAM to load robot: " + ss + "... This is not good.";
            break;
        case 10:
            log << "Too many robots! We can only handle " + cstr(max_robots + 1) + "! Blah.. limits are limits.";
            break;
        case 11:
            log << "You already have a perfectly good #def for " + ss + ", silly.";
            break;
        case 12:
            log << "Variable name too long! (Max:" + cstr(max_var_len) + ") " + ss + "";
            break;
        case 13:
            log << "!Label already defined " + ss + ", silly.";
            break;
        case 14:
            log << "Too many variables! (Var Limit: " + cstr(max_vars) + ")";
            break;
        case 15:
            log << "Too many !labels! (!Label Limit: " + cstr(max_labels) + ")";
            break;
        case 16:
            log << "Robot program too long! Boldly we simplify, simplify along..." + ss;
            break;
        case 17:
            log << "!Label missing error. !Label #" + ss + ".";
            break;
        case 18:
            log << "!Label out of range: " + ss + "";
            break;
        case 19:
            log << "!Label not found. " + ss + "";
            break;
        case 20:
            log << "Invalid config option: " + ss + ". Inventing a new device?";
            break;
        case 21:
            log << "Robot is attempting to cheat; Too many config points (" + ss + ")";
            break;
        case 22:
            log << "Insufficient data in data statement: " + ss + "";
            break;
        case 23:
            log << "Too many asterisks: " + ss + "";
            break;
        case 24:
            log << "Invalid step count: " + ss + ". 1-9 are valid conditions.";
            break;
        case 25:
            log << "'" + ss + "'";
            break;
        default:
            log << ss;
    }
    log.close();
    return;

}

/*
From ATRLOCK, just using as a place holder for now.
*/
 void to_Uppercase (string * s){
    transform(s -> begin(), s -> end(), s -> begin(), (int (*)(int))std::toupper);
 }

void parse1(int n, int p, parsetype s){
    int i, j, k, opcode, microcode;
    bool found, indirect;
    string ss;

    for (i = 0; i <= max_op - 1; i++) {
        k = 0;
        found = false;
        opcode = 0;
        microcode = 0;
        s[i] = trim(to_Uppercase(s[i]));
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

     if(s[i] == ''){
        opcode = 0;
        microcode = 0;
        found = true;
     }
     if(lstr(s[i], 1) == '[' && rstr(s[i], 1) == ']'){
        s[i] = cope(s[i], 2, length(s[i])-2);
        indirect = true;
     }

     //Labels
     if(!found && s[i][1] = '!'){
        ss = s[i];
        ss = trim(rstr(ss, length(ss)-1));
        if(numlabels > 0){
            for(j = 1; j <= numlabels; j++){
                if(ss == labelname[j]){
                    found = true;
                    if(labelnum[j] >= 0){
                        opcode = labelnum[j]; //resolved label
                        microcode = 4;
                    }else{
                    opcode = j; //unresolved label
                    microcode = 3;
                    }
                }
                if(!found){
                    numlabels++;
                    if(numlabels > max_labels){
                        prog_error(15, '');
                    }else{
                        labelname[numblabels] = ss;
                        labelnum[numlabels] = -1;
                        opcode = numlabels;
                        microcode = 3; //unresolved label
                        found = true;
                    }
                }
            }
        }

        if (numvars > 0 && !found) {
            for (j = 1; j <= numvars; j++) {
                if (s[i] == varname[j]) {
                    opcode = varloc[j];
                    microcode = 1;
                    found = true;
                }
                //Instructions
                switch (s[i]) {
                    case "NOP":
                        opcode = 0;
                        found = true;
                        break;
                    case "ADD":
                        opcode = 1;
                        found = true;
                        break;
                    case "SUB":
                        opcode = 2;
                        found = true;
                        break;
                    case "OR":
                        opcode = 3;
                        found = true;
                        break;
                    case "AND":
                        opcode = 4;
                        found = true;
                        break;
                    case "XOR":
                        opcode = 5;
                        found = true;
                        break;
                    case "NOT":
                        opcode = 6;
                        found = true;
                        break;
                    case "MPY":
                        opcode = 7;
                        found = true;
                        break;
                    case "DIV":
                        opcode = 8;
                        found = true;
                        break;
                    case "MOD":
                        opcode = 9;
                        found = true;
                        break;
                    case "RET":
                        opcode = 10;
                        found = true;
                        break;
                    case "RETURN":
                        opcode = 10;
                        found = true;
                        break;
                    case "GSB":
                        opcode = 11;
                        found = true;
                        break;
                    case "GOSUB":
                        opcode = 11;
                        found = true;
                        break;
                    case "CALL":
                        opcode = 11;
                        found = true;
                        break;
                    case "JMP":
                        opcode = 12;
                        found = true;
                        break;
                    case "JUMP":
                        opcode = 12;
                        found = true;
                        break;
                    case "GOTO":
                        opcode = 12;
                        found = true;
                        break;
                    case "JLS":
                        opcode = 13;
                        found = true;
                        break;
                    case "JB":
                        opcode = 13;
                        found = true;
                        break;
                    case "JGR":
                        opcode = 14;
                        found = true;
                        break;
                    case "JA":
                        opcode = 14;
                        found = true;
                        break;
                    case "JNE":
                        opcode = 15;
                        found = true;
                        break;
                    case "JEQ":
                        opcode = 16;
                        found = true;
                        break;
                    case "JE":
                        opcode = 16;
                        found = true;
                        break;
                    case "XCHG":
                        opcode = 17;
                        found = true;
                        break;
                    case "SWAP":
                        opcode = 17;
                        found = true;
                        break;
                    case "DO":
                        opcode = 18;
                        found = true;
                        break;
                    case "LOOP":
                        opcode = 19;
                        found = true;
                        break;
                    case "CMP":
                        opcode = 20;
                        found = true;
                        break;
                    case "TEST":
                        opcode = 21;
                        found = true;
                        break;
                    case "SET":
                        opcode = 22;
                        found = true;
                        break;
                    case "MOV":
                        opcode = 22;
                        found = true;
                        break;
                    case "LOC":
                        opcode = 23;
                        found = true;
                        break;
                    case "ADDR":
                        opcode = 23;
                        found = true;
                        break;
                    case "GET":
                        opcode = 24;
                        found = true;
                        break;
                    case "PUT":
                        opcode = 25;
                        found = true;
                        break;
                    case "INT":
                        opcode = 26;
                        found = true;
                        break;
                    case "IPO":
                        opcode = 27;
                        found = true;
                        break;
                    case "IN":
                        opcode = 27;
                        found = true;
                        break;
                    case "OPO":
                        opcode = 28;
                        found = true;
                        break;
                    case "OUT":
                        opcode = 28;
                        found = true;
                        break;
                    case "DEL":
                        opcode = 29;
                        found = true;
                        break;
                    case "DELAY":
                        opcode = 29;
                        found = true;
                        break;
                    case "PUSH":
                        opcode = 30;
                        found = true;
                        break;
                    case "POP":
                        opcode = 31;
                        found = true;
                        break;
                    case "ERR":
                        opcode = 32;
                        found = true;
                        break;
                    case "ERROR":
                        opcode = 32;
                        found = true;
                        break;
                    case "INC":
                        opcode = 33;
                        found = true;
                        break;
                    case "DEC":
                        opcode = 34;
                        found = true;
                        break;
                    case "SHL":
                        opcode = 35;
                        found = true;
                        break;
                    case "SHR":
                        opcode = 36;
                        found = true;
                        break;
                    case "ROL":
                        opcode = 37;
                        found = true;
                        break;
                    case "ROR":
                        opcode = 38;
                        found = true;
                        break;
                    case "JZ":
                        opcode = 39;
                        found = true;
                        break;
                    case "JNZ":
                        opcode = 40;
                        found = true;
                        break;
                    case "JAE":
                        opcode = 41;
                        found = true;
                        break;
                    case "JGE":
                        opcode = 41;
                        found = true;
                        break;
                    case "JLE":
                        opcode = 42;
                        found = true;
                        break;
                    case "JBE":
                        opcode = 42;
                        found = true;
                        break;
                    case "SAL":
                        opcode = 43;
                        found = true;
                        break;
                    case "SAR":
                        opcode = 44;
                        found = true;
                        break;
                    case "NEG":
                        opcode = 45;
                        found = true;
                        break;
                    case "JTL":
                        opcode = 46;
                        found = true;
                        break;

                        //Registers
                    case "COLCNT":
                        opcode = 8;
                        microcode = 1;
                        found = true;
                        break;
                    case "METERS":
                        opcode = 9;
                        microcode = 1;
                        found = true;
                        break;
                    case "COMBASE":
                        opcode = 10;
                        microcode = 1;
                        found = true;
                        break;
                    case "COMEND":
                        opcode = 11;
                        microcode = 1;
                        found = true;
                        break;
                    case "FLAGS":
                        opcode = 64;
                        microcode = 1;
                        found = true;
                        break;
                    case "AX":
                        opcode = 65;
                        microcode = 1;
                        found = true;
                        break;
                    case "BX":
                        opcode = 66;
                        microcode = 1;
                        found = true;
                        break;
                    case "CX":
                        opcode = 67;
                        microcode = 1;
                        found = true;
                        break;
                    case "DX":
                        opcode = 68;
                        microcode = 1;
                        found = true;
                        break;
                    case "EX":
                        opcode = 69;
                        microcode = 1;
                        found = true;
                        break;
                    case "FX":
                        opcode = 70;
                        microcode = 1;
                        found = true;
                        break;
                    case "SP":
                        opcode = 71;
                        microcode = 1;
                        found = true;
                        break;

                        //Constants
                    case "MAXINT":
                        opcode = 32767;
                        microcode = 0;
                        found = true;
                        break;
                    case "MININT":
                        opcode = -32768;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_SPEDOMETER":
                        opcode = 1;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_HEAT":
                        opcode = 2;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_COMPASS":
                        opcode = 3;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_TANGLE":
                        opcode = 4;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_TURRET_OFS":
                        opcode = 4;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_THEADING":
                        opcode = 5;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_TURRET_ABS":
                        opcode = 5;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_ARMOR":
                        opcode = 6;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_DAMAGE":
                        opcode = 6;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_SCAN":
                        opcode = 7;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_ACCURACY":
                        opcode = 8;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_RADAR":
                        opcode = 9;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_RANDOM":
                        opcode = 10;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_RAND":
                        opcode = 10;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_THROTTLE":
                        opcode = 11;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_TROTATE":
                        opcode = 12;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_OFS_TURRET":
                        opcode = 12;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_TAIM":
                        opcode = 13;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_ABS_TURRET":
                        opcode = 13;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_STEERING":
                        opcode = 14;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_WEAP":
                        opcode = 15;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_WEAPON":
                        opcode = 15;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_FIRE":
                        opcode = 15;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_SONAR":
                        opcode = 16;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_ARC":
                        opcode = 17;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_SCANARC":
                        opcode = 17;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_OVERBURN":
                        opcode = 18;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_TRANSPONDER":
                        opcode = 19;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_SHUTDOWN":
                        opcode = 20;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_CHANNEL":
                        opcode = 21;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_MINELAYER":
                        opcode = 22;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_MINETRIGGER":
                        opcode = 23;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_SHIELD":
                        opcode = 24;
                        microcode = 0;
                        found = true;
                        break;
                    case "P_SHIELDS":
                        opcode = 24;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_DESTRUCT":
                        opcode = 0;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_RESET":
                        opcode = 1;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_LOCATE":
                        opcode = 2;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_KEEPSHIFT":
                        opcode = 3;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_OVERBURN":
                        opcode = 4;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_ID":
                        opcode = 5;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_TIMER":
                        opcode = 6;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_ANGLE":
                        opcode = 7;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_TID":
                        opcode = 8;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_TARGETID":
                        opcode = 8;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_TINFO":
                        opcode = 9;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_TARGETINFO":
                        opcode = 9;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_GINFO":
                        opcode = 10;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_GAMEINFO":
                        opcode = 10;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_RINFO":
                        opcode = 11;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_ROBOTINFO":
                        opcode = 11;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_COLLISIONS":
                        opcode = 12;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_RESETCOLCNT":
                        opcode = 13;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_TRANSMIT":
                        opcode = 14;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_RECEIVE":
                        opcode = 15;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_DATAREADY":
                        opcode = 16;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_CLEARCOM":
                        opcode = 17;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_KILLS":
                        opcode = 18;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_DEATHS":
                        opcode = 18;
                        microcode = 0;
                        found = true;
                        break;
                    case "I_CLEARMETERS":
                        opcode = 19;
                        microcode = 0;
                        found = true;
                        break;
                }

                //Memory addresses
                if (!found && s[i][1] == '@' && (atoi(s[i][2]) >= 0 && atoi(s[i][2] <= 9))) {
                    opcode = atoi(rstr(s[i], strlen(s[i]) - 1));
                    if (opcode < 0 || opcode > (max_ram + 1) + (((max_code + 1) << 3) - 1)) {
                        prog_error(3, s[i]);
                        microcode = 1;
                        found = true;
                    }
                }

                //Numbers
                if (!found && (atoi(s[i][1]) >= 0 && atoi(s[i][1] <= 9)) || s[i][1] == '-') {
                    opcode = atoi(s[i]);
                    found = true;
                }

                if (found) {
                    robot[n]->code[p].op[i] = opcode;
                    if (indirect) {
                        microcode = microcode | 8;
                    }
                    robot[n]->code[p].op[max_op] = robot[n]->code[p].op[max_op] | (microcode << (i * 4));
                } else if (s[i] != '')
                    prog_error(2, s[i]);
            }
        }
        if (show_code)
            print_code(n, p);
        if (compile_by_line)
            readkey;
    }
}

void compile(int n, string filename){
    parsetype pp;
    string s, s1, s2, s3, orig_s, msg;
    int i, j, k, l, linecount, mask, locktype;
    string ss[max_op];
    char c, lc;

    lock_code = '';
    lock_pos = 0;
    locktype = 0;
    lock_dat = 0;
    //Needs to be a filestream in the main function
    if(!filestream)
        prog_error(8, filename);
    textcolor(robot_color(n));
    cout<< "Compiling robot #", n+1, ": ", filename<<endl;

    robot[n]->is_locked = false;
    textcolor(robot_color(n));
    numvars = 0;
    numlabels = 0;
    for(k = 0; k <= max_code; k++){
        for(i = 0; i <= max_op; i++){
            robot[n]->code[k].op[i] = 0;
        }
    }
    robot[n]->plen = 0;
    /*
     * ????
     * assign(f,filename)
     * reset(f)
     * ????
     */
    s = '';
    linecount = 0;

    //First pass, compile
    while(!feof(f) && s != "#END"){
        //readln(f, s);
        linecount++;
        if(locktype < 3)
            lock_pos = 0;
        if(lock_code != '')
            for(i = 1; i <= strlen(s); i++){
                lock_pos++;
                if(lock_pos > strlen(lock_code))
                    lock_pos = 1;
                switch(locktype){
                    case 3:
                        s[i] = char((ord(s[i])-1) xor (ord(lock_code[lock_pos]) xor lock_dat));
                        break;
                    case 2:
                        s[i] = char(ord(s[i]) xor (ord(lock_code[lock_pos]) xor 1));
                        break;
                    default:
                        lock_dat = char(ord(s[i]) xor ord(lock_code[lock_pos]));
                }
                lock_dat = ord(s[i]) and 15;
            }
        s = trim(s);
        orig_s = s;
        for(i = 1; i <= strlen(s); i++){
            //if s[i] in [#0..#32,',',#128..#255] then s[i]:=' ';
        }
        if(show_source && ((lock_code='') || debugging_compiler))
            cout<< zero_pas(linecount, 3) + ":" + zero_pad(robot[n]->plen, 3)+' ', s << endl;
    }

}

/*
 * {--first pass, compile--}
   while (not eof(f)) and (s<>'#END') {and (plen<=maxcode)} do
    begin
     readln(f,s);
     inc(linecount);
     if locktype<3 then lock_pos:=0;
     if lock_code<>'' then
      for i:=1 to length(s) do
       begin
        inc(lock_pos); if lock_pos>length (lock_code) then lock_pos:=1;
        case locktype of
         3:s[i]:=char((ord(s[i])-1) xor (ord(lock_code[lock_pos]) xor lock_dat));
         2:s[i]:=char(ord(s[i]) xor (ord(lock_code[lock_pos]) xor 1));
         else s[i]:=char(ord(s[i]) xor ord(lock_code[lock_pos]));
        end;
        lock_dat:=ord(s[i]) and 15;
       end;
     s:=btrim(s);
     orig_s:=s;
     for i:=1 to length(s) do
      if s[i] in [#0..#32,',',#128..#255] then s[i]:=' ';
     if show_source and ((lock_code='') or debugging_compiler) then
        writeln(zero_pad(linecount,3)+':'+zero_pad(plen,3)+' ',s);
     if debugging_compiler then
        begin if readkey=#27 then halt; end;
     {-remove comments-}
     k:=0;
     for i:=length(s) downto 1 do
         if s[i]=';' then k:=i;
     if k>0 then s:=lstr(s,k-1);
     s:=btrim(ucase(s));
     for i:=0 to max_op do pp[i]:='';
     if (length(s)>0) and (s[1]<>';') then
 */

int main() {
    log_error(4);
}
