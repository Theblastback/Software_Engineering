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
void to_Uppercase(string *s) {
    transform(s->begin(), s->end(), s->begin(), (int (*)(int)) std::toupper);
}

void parse1(int n, int p, parsetype s) {
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

        if (s[i] == '') {
            opcode = 0;
            microcode = 0;
            found = true;
        }
        if (lstr(s[i], 1) == '[' && rstr(s[i], 1) == ']') {
            s[i] = cope(s[i], 2, length(s[i]) - 2);
            indirect = true;
        }

        //Labels
        if (!found && s[i][1] = '!') {
            ss = s[i];
            ss = trim(rstr(ss, length(ss) - 1));
            if (numlabels > 0) {
                for (j = 1; j <= numlabels; j++) {
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
                        if (numlabels > max_labels) {
                            prog_error(15, '');
                        } else {
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
                    if (strcmp(s[i], varname[j])) {
                        opcode = varloc[j];
                        microcode = 1;
                        found = true;
                    }
                    if (strcmp(s[i], "NOP")) {
                        opcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "ADD")) {
                        opcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "SUB")) {
                        opcode = 2;
                        found = true;
                    } else if (strcmp(s[i], "OR")) {
                        opcode = 3;
                        found = true;
                    } else if (strcmp(s[i], "AND")) {
                        opcode = 4;
                        found = true;
                    } else if (strcmp(s[i], "XOR")) {
                        opcode = 5;
                        found = true;
                    } else if (strcmp(s[i], "NOT")) {
                        opcode = 6;
                        found = true;
                    } else if (strcmp(s[i], "MPY")) {
                        opcode = 7;
                        found = true;
                    } else if (strcmp(s[i], "DIV")) {
                        opcode = 8;
                        found = true;
                    } else if (strcmp(s[i], "MOD")) {
                        opcode = 9;
                        found = true;
                    } else if (strcmp(s[i], "RET")) {
                        opcode = 10;
                        found = true;
                    } else if (strcmp(s[i], "RETURN")) {
                        opcode = 10;
                        found = true;
                    } else if (strcmp(s[i], "GSB")) {
                        opcode = 11;
                        found = true;
                    } else if (strcmp(s[i], "GOSUB")) {
                        opcode = 11;
                        found = true;
                    } else if (strcmp(s[i], "CALL")) {
                        opcode = 11;
                        found = true;
                    } else if (strcmp(s[i], "JMP")) {
                        opcode = 12;
                        found = true;
                    } else if (strcmp(s[i], "JUMP")) {
                        opcode = 12;
                        found = true;
                    } else if (strcmp(s[i], "GOTO")) {
                        opcode = 12;
                        found = true;
                    } else if (strcmp(s[i], "JLS")) {
                        opcode = 13;
                        found = true;
                    } else if (strcmp(s[i], "JB")) {
                        opcode = 13;
                        found = true;
                    } else if (strcmp(s[i], "JGR")) {
                        opcode = 14;
                        found = true;
                    } else if (strcmp(s[i], "JA")) {
                        opcode = 14;
                        found = true;
                    } else if (strcmp(s[i], "JNE")) {
                        opcode = 15;
                        found = true;
                    } else if (strcmp(s[i], "JEQ")) {
                        opcode = 16;
                        found = true;
                    } else if (strcmp(s[i], "JE")) {
                        opcode = 16;
                        found = true;
                    } else if (strcmp(s[i], "XCHG")) {
                        opcode = 17;
                        found = true;
                    } else if (strcmp(s[i], "SWAP")) {
                        opcode = 17;
                        found = true;
                    } else if (strcmp(s[i], "DO")) {
                        opcode = 18;
                        found = true;
                    } else if (strcmp(s[i], "LOOP")) {
                        opcode = 19;
                        found = true;
                    } else if (strcmp(s[i], "CMP")) {
                        opcode = 20;
                        found = true;
                    } else if (strcmp(s[i], "TEST")) {
                        opcode = 21;
                        found = true;
                    } else if (strcmp(s[i], "SET")) {
                        opcode = 22;
                        found = true;
                    } else if (strcmp(s[i], "MOV")) {
                        opcode = 22;
                        found = true;
                    } else if (strcmp(s[i], "LOC")) {
                        opcode = 23;
                        found = true;
                    } else if (strcmp(s[i], "ADDR")) {
                        opcode = 23;
                        found = true;
                    } else if (strcmp(s[i], "GET")) {
                        opcode = 24;
                        found = true;
                    } else if (strcmp(s[i], "PUT")) {
                        opcode = 25;
                        found = true;
                    } else if (strcmp(s[i], "INT")) {
                        opcode = 26;
                        found = true;
                    } else if (strcmp(s[i], "IPO")) {
                        opcode = 27;
                        found = true;
                    } else if (strcmp(s[i], "IN")) {
                        opcode = 27;
                        found = true;
                    } else if (strcmp(s[i], "OPO")) {
                        opcode = 28;
                        found = true;
                    } else if (strcmp(s[i], "OUT")) {
                        opcode = 28;
                        found = true;
                    } else if (strcmp(s[i], "DEL")) {
                        opcode = 29;
                        found = true;
                    } else if (strcmp(s[i], "DELAY")) {
                        opcode = 29;
                        found = true;
                    } else if (strcmp(s[i], "PUSH")) {
                        opcode = 30;
                        found = true;
                    } else if (strcmp(s[i], "POP")) {
                        opcode = 31;
                        found = true;
                    } else if (strcmp(s[i], "ERR")) {
                        opcode = 32;
                        found = true;
                    } else if (strcmp(s[i], "ERROR")) {
                        opcode = 32;
                        found = true;
                    } else if (strcmp(s[i], "INC")) {
                        opcode = 33;
                        found = true;
                    } else if (strcmp(s[i], "DEC")) {
                        opcode = 34;
                        found = true;
                    } else if (strcmp(s[i], "SHL")) {
                        opcode = 35;
                        found = true;
                    } else if (strcmp(s[i], "SHR")) {
                        opcode = 36;
                        found = true;
                    } else if (strcmp(s[i], "ROL")) {
                        opcode = 37;
                        found = true;
                    } else if (strcmp(s[i], "ROR")) {
                        opcode = 38;
                        found = true;
                    } else if (strcmp(s[i], "JZ")) {
                        opcode = 39;
                        found = true;
                    } else if (strcmp(s[i], "JNZ")) {
                        opcode = 40;
                        found = true;
                    } else if (strcmp(s[i], "JAE")) {
                        opcode = 41;
                        found = true;
                    } else if (strcmp(s[i], "JGE")) {
                        opcode = 41;
                        found = true;
                    } else if (strcmp(s[i], "JLE")) {
                        opcode = 42;
                        found = true;
                    } else if (strcmp(s[i], "JBE")) {
                        opcode = 42;
                        found = true;
                    } else if (strcmp(s[i], "SAL")) {
                        opcode = 43;
                        found = true;
                    } else if (strcmp(s[i], "SAR")) {
                        opcode = 44;
                        found = true;
                    } else if (strcmp(s[i], "NEG")) {
                        opcode = 45;
                        found = true;
                    } else if (strcmp(s[i], "JTL")) {
                        opcode = 46;
                        found = true;
                    }

                        //Registers
                    else if (strcmp(s[i], "COLCNT")) {
                        opcode = 8;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "METERS")) {
                        opcode = 9;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "COMBASE")) {
                        opcode = 10;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "COMEND")) {
                        opcode = 11;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "FLAGS")) {
                        opcode = 64;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "AX")) {
                        opcode = 65;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "BX")) {
                        opcode = 66;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "CX")) {
                        opcode = 67;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "DX")) {
                        opcode = 68;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "EX")) {
                        opcode = 69;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "FX")) {
                        opcode = 70;
                        microcode = 1;
                        found = true;
                    } else if (strcmp(s[i], "SP")) {
                        opcode = 71;
                        microcode = 1;
                        found = true;
                    }

                        //Constants
                    else if (strcmp(s[i], "MAXINT")) {
                        opcode = 32767;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "MININT")) {
                        opcode = -32768;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_SPEDOMETER")) {
                        opcode = 1;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_HEAT")) {
                        opcode = 2;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_COMPASS")) {
                        opcode = 3;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_TANGLE")) {
                        opcode = 4;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_TURRET_OFS")) {
                        opcode = 4;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_THEADING")) {
                        opcode = 5;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_TURRET_ABS")) {
                        opcode = 5;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_ARMOR")) {
                        opcode = 6;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_DAMAGE")) {
                        opcode = 6;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_SCAN")) {
                        opcode = 7;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_ACCURACY")) {
                        opcode = 8;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_RADAR")) {
                        opcode = 9;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_RANDOM")) {
                        opcode = 10;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_RAND")) {
                        opcode = 10;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_THROTTLE")) {
                        opcode = 11;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_TROTATE")) {
                        opcode = 12;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_OFS_TURRET")) {
                        opcode = 12;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_TAIM")) {
                        opcode = 13;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_ABS_TURRET")) {
                        opcode = 13;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_STEERING")) {
                        opcode = 14;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_WEAP")) {
                        opcode = 15;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_WEAPON")) {
                        opcode = 15;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_FIRE")) {
                        opcode = 15;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_SONAR")) {
                        opcode = 16;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_ARC")) {
                        opcode = 17;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_SCANARC")) {
                        opcode = 17;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_OVERBURN")) {
                        opcode = 18;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_TRANSPONDER")) {
                        opcode = 19;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_SHUTDOWN")) {
                        opcode = 20;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_CHANNEL")) {
                        opcode = 21;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_MINELAYER")) {
                        opcode = 22;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_MINETRIGGER")) {
                        opcode = 23;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_SHIELD")) {
                        opcode = 24;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "P_SHIELDS")) {
                        opcode = 24;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_DESTRUCT")) {
                        opcode = 0;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_RESET")) {
                        opcode = 1;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_LOCATE")) {
                        opcode = 2;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_KEEPSHIFT")) {
                        opcode = 3;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_OVERBURN")) {
                        opcode = 4;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_ID")) {
                        opcode = 5;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_TIMER")) {
                        opcode = 6;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_ANGLE")) {
                        opcode = 7;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_TID")) {
                        opcode = 8;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_TARGETID")) {
                        opcode = 8;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_TINFO")) {
                        opcode = 9;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_TARGETINFO")) {
                        opcode = 9;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_GINFO")) {
                        opcode = 10;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_GAMEINFO")) {
                        opcode = 10;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_RINFO")) {
                        opcode = 11;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_ROBOTINFO")) {
                        opcode = 11;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_COLLISIONS")) {
                        opcode = 12;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_RESETCOLCNT")) {
                        opcode = 13;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_TRANSMIT")) {
                        opcode = 14;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_RECEIVE")) {
                        opcode = 15;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_DATAREADY")) {
                        opcode = 16;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_CLEARCOM")) {
                        opcode = 17;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_KILLS")) {
                        opcode = 18;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_DEATHS")) {
                        opcode = 18;
                        microcode = 0;
                        found = true;
                    } else if (strcmp(s[i], "I_CLEARMETERS")) {
                        opcode = 19;
                        microcode = 0;
                        found = true;
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
}

void compile(int n, string filename) {
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
    if (!filestream)
        prog_error(8, filename);
    textcolor(robot_color(n));
    cout << "Compiling robot #", n + 1, ": ", filename << endl;

    robot[n]->is_locked = false;
    textcolor(robot_color(n));
    numvars = 0;
    numlabels = 0;
    for (k = 0; k <= max_code; k++) {
        for (i = 0; i <= max_op; i++) {
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

    //TODO robot[n] in front of all robot variables.

    //First pass, compile
    while (!feof(f) && s != "#END") {
        //readln(f, s);
        linecount++;
        if (locktype < 3)
            lock_pos = 0;
        if (lock_code != '')
            for (i = 1; i <= strlen(s); i++) {
                lock_pos++;
                if (lock_pos > strlen(lock_code))
                    lock_pos = 1;
                switch (locktype) {
                    case 3:
                        s[i] = char((ord(s[i]) - 1) xor (ord(lock_code[lock_pos]) xor lock_dat));
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
        for (i = 1; i <= strlen(s); i++) {
            //if s[i] in [#0..#32,',',#128..#255] then s[i]:=' ';
        }
        if (show_source && ((lock_code = '') || debugging_compiler))
            cout << zero_pas(linecount, 3) + ":" + zero_pad(robot[n]->plen, 3) + ' ', s << endl;
        if(debugging_compiler)
            if(readkey == 27)
                halt();
        k = 0;
        for(i = s.length(); s >= 1; s--)
            if(s[i] == ';')
                k = i;
        if(k > 0)
            s = lstr(s, k-1);
        s = trim(to_Uppercase(s));
        for(i = 0; i <= max_op; i++){
            pp[i] = '';
        }
        if(s.length() > 0 && s[1] != ';'){
            switch(s[1]){
                case '#':
                    s1 = to_Uppercase(trim(rstr(s,s.length()-1)));
                    msg = trim(rstr(orig_s, orig_s.length()-5));
                    k = 0;
                    for(i = 1; i <= s1.length(); i++)
                        if(k == 0 && s1[i] = ' ')
                            k = i;
                    k--;
                    if(k > 1) {
                        s2 = lstr(s1, k);
                        s3 = to_Uppercase(trim(rstr(s1, s1.length() - k)));
                        k = 0;
                        if(numvars > 0)
                            for(i = 1; i <= numvars; i++)
                                if(!strcmp(s3, varname[i]))
                                    k = i;
                        if(!strcmp(s2, "DEF") && numvars < max_vars){
                            if(s3.length() > max_var_len) {
                                prog_error(14, '');
                            }else{
                                if(k > 0)
                                    prog_error(11, s3);
                                else {
                                    numvars++;
                                    if(numvars > max_vars)
                                        prog_error(14, '');
                                    else{
                                        varname[numvars] = s3;
                                        varloc[numvars] = 127 + numvars;
                                    }

                                }

                            }
                        }
                        else if(!strcmp(lstr(s2, 4), "LOCK")){
                            robot[n]->is_locked = true;
                            if(s2.length > 4)
                                locktype = value(rstr(s2, length(s2)-4));
                            lock_code = trim(to_Uppercase(s3));
                            cout<< "Robot is of LOCKed format from this point forward. [",locktype,"]"<<endl;
                            for(i = 1; i <= lock_code.length(); i++){
                                lock_code[i] = char(ord(lock_code[i])- 65);
                            }
                        }else if(!strcmp(s2, "MSG"))
                            robot[n]->name = msg;
                        else if(!strcmp(s2, "TIME")){
                            robot[n]->robot_time_limit = value(s3);
                            if(robot[n]->robot_time_limit < 0)
                                robot[n]->robot_time_limit = 0;
                        }else if(!strcmp(s2, "CONFIG")){
                            if(!strcmp(lstr(s3,8), "SCANNER="))
                                robot[n]->config.scanner = value(rstr(s3, s3.length()-8));
                            else if(!strcmp(lstr(s3,7), "SHIELD="))
                                robot[n]->config.shield = value(rstr(s3, s3.length()-7));
                            else if(!strcmp(lstr(s3,7), "WEAPON="))
                                robot[n]->config.weapon = value(rstr(s3, s3.length()-7));
                            else if(!strcmp(lstr(s3,6), "ARMOR="))
                                robot[n]->config.armor = value(rstr(s3, s3.length()-6));
                            else if(!strcmp(lstr(s3,7), "ENGINE="))
                                robot[n]->config.engine = value(rstr(s3, s3.length()-7));
                            else if(!strcmp(lstr(s3,10), "HEATSINKS="))
                                robot[n]->config.heatsinks = value(rstr(s3, s3.length()-10));
                            else if(!strcmp(lstr(s3,6), "MINES="))
                                robot[n]->config.mines = value(rstr(s3, s3.length()-6));
                            else
                                prog_error(20, s3);

                            if(robot[n]->config.scanner < 0)
                                robot[n]->conifg.scanner = 0;
                            if(robot[n]->config.scanner > 5)
                                robot[n]->config.scanner = 5;

                            if(robot[n]->config.shield < 0)
                                robot[n]->conifg.shield = 0;
                            if(robot[n]->config.shield > 5)
                                robot[n]->config.shield = 5;

                            if(robot[n]->config.weapon < 0)
                                robot[n]->conifg.weapon = 0;
                            if(robot[n]->config.weapon > 5)
                                robot[n]->config.weapon = 5;

                            if(robot[n]->config.armor < 0)
                                robot[n]->conifg.armor = 0;
                            if(robot[n]->config.armor > 5)
                                robot[n]->config.armor = 5;

                            if(robot[n]->config.engine < 0)
                                robot[n]->conifg.engine = 0;
                            if(robot[n]->config.engine > 5)
                                robot[n]->config.engine = 5;

                            if(robot[n]->config.heatsinks < 0)
                                robot[n]->conifg.heatsinks = 0;
                            if(robot[n]->config.heatsinks > 5)
                                robot[n]->config.heatsinks = 5;

                            if(robot[n]->config.mines < 0)
                                robot[n]->conifg.mines = 0;
                            if(robot[n]->config.mines > 5)
                                robot[n]->config.mines = 5;
                        }
                        else
                            cout<< "WARNING: unknown directive '", s2, "' "<<endl;

                    }
                    break;
                case '*':
                    check_plen(robot[n]->plen);
                    for(i = 0; i <= max_op; i++)
                        pp[i] = '';
                    for(i = 2; i <= s.length(); i++)
                        if(s[i] == '*')
                            prog_error(23,s);
                    k = 0;
                    i = 1;
                    s1 = '';
                    if(s.length() <= 2)
                        prog_error(23, s);
                    while(i < length(s) && k <= max_op){
                        i++;
                        /*
                         * if ord(s[i]) in [33..41,43..127] then pp[k]:=pp[k]+s[i]
                            else if (ord(s[i]) in [0..32,128..255]) and
                            (ord(s[i-1]) in [33..41,43..127]) then inc(k);
                         */
                    }
                    for(i = 0; i <= max_op; i++)
                        robot[n]->code[robot[n]->plen].op[i] = value(pp[i]);
                    robot[n]->plen++;
                    break;
                case ':':
                    check_plen(robot[n]->plen);
                    s1 = rstr(s, s.length()-1);
                    for(i = 1; i <= s1.length())
                        //if not (s1[i] in ['0'..'9']) then
                            prog_error(1, s);
                    robot[n]->code[robot[n]->plen].op[0] = value(s1);
                    robot[n]->code[robot[n]->plen].op[max_op] = 2;
                    if(show_code)
                        print_code(n, robot[n]->plen);
                    robot[n]->plen++;
                    break;
                case '!':
                    check_plen(robot[n]->plen);
                    s1 = trim(rstr(s, s.length()-1));
                    k = 0;
                    for(i = s1.length(); i >= 1; i--) {
                        //in [';',#8,#9,#10,' ',','] then k:=i;
                    }
                    if(k > 0)
                        s1 = lstr(s1, k-1);
                    k = 0;
                    for(i = 1; i <= numlabels; i++){
                        if(!strcmp(labelname[i], s1)){
                            if(labelnum[i] >= 0)
                                prog_error(13, "!", s1, "(", cstr(labelnum[i]),")");
                            k = i;
                        }
                        if(k == 0){
                            numlabels++;
                            if(numlabels > max_labels)
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
                    for(i = s.length(); i >= 1; i--)
                        if(s[i] == ';')
                            k = i;
                    if(k > 0)
                        s = lstr(s, k-1);
                    //setup variables for parsing
                    k = 0;
                    for(j = 0; j <= max_op; j++)
                        pp[j] = '';
                    for(j = 1; j <= s.length(); j++){
                        c = s[j];
                        if(/*not (c in [' ',#8,#9,#10,','])) &&*/ k <= max_op)
                            pp[k] = pp[k]+c;
                        else if (/*lc in [' ',#8,#9,#10,','])*/){
                            k = k+1;
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
        if(robot[n]->plen <= maxcode) {
            for (i = 0; i <= max_op; i++)
                pp[i] = "";
            pp[0] = "NOP";
            parse1(n, robot[n]->plen, pp);
        }else{
            robot[n]->plen--;
        }

        //second pass, resolving !labels
        if(numlabels > 0){
            for(i = 0; i <= plen; i++){
                for(j = 0; j <= max_op-1; j++){
                    if(robot[n]->code[i].op[max_op] >> (j*4) == 3){
                        k = robot[n]->code[i].op[j];
                        if(k > 0 && k <= numlabels){
                            l = labelnum[k];
                            if(l<0)
                                prog_error(19,'"!'+labelname[k]+'" ('+cstr(l)+')');
                            if(l<0 || l > maxcode)
                                prog_error(18, '"!'+labelname[k]+'" ('+cstr(l)+')');
                            else{
                                robot[n]->code[i].op[j] = l;
                                mask = !(std::hex(F) << (j*4));
                                robot[n]->code[i].op[max_op] = (robot[n]->code[i].op[max_op] & mask) or (4 << (j*4));
                            }
                        }
                    }else{
                        prog_error(17, cstr(k));
                    }
                }
            }
        }
    }
    textcolor(7);
}

void do_robot(int n){
    int i, j, k, l, tthd, heat_mult, ttx, tty;
    if(n < 0 || n > num_robots)
        return EXIT_SUCCESS;
    if(robot[n]->armor <= 0)
        return EXIT_SUCCESS;
    robot[n]->time_left = time_slice;
    if(robot[n]->time_left > robot[n]->robot_time_limit && robot[n]->robot_time_limit > 0)
        robot[n]->time_left = robot[n]->robot_time_limit;
    if(robot[n]->time_left>robot[n]->max_time && robot[n]->max_time > 0)
        robot[n]->time_left = robot[n]->max_time;
    robot[n]->executed = 0;
    //execute timeslice
    while(robot[n]->time_left > 0 && !robot[n]->cooling && robot[n]->executed < 20+time_slice && robot[n]->armor > 0){
        if(robot[n]->delay_left < 0)
            robot[n]->delay_left = 0;
        if(robot[n]->delay_left > 0) {
            robot[n]->delay_left--;
            robot[n]->time_left--;
        }
        if(robot[n]->time_left >= 0 && robot[n]->delay_left == 0)
            execute_instruction(n);
        if(robot[n]->heat >= robot[n]->shutdown){
            robot[n]->cooling = true;
            robot[n]->shields_up = false;
        }
        if(robot[n]->heat >= 500){
            damage(n, 1000, true);
        }
    }
    robot[n]->thd = (robot[n]->thd + 1024) and 255;
    robot[n]->hd = (robot[n]->hd + 1024) and 255;
    robot[n]->shift = (robot[n]->shift + 1024) and 255;
    if(robot[n]->tspd < -75)
        robot[n]->tspd = -75;
    if(robot[n]->tspd > 100)
        robot[n]->tspd = 100;
    if(robot[n]->spd < -75)
        robot[n]->spd = -75;
    if(robot[n]->spd > 100)
        robot[n]->spd = 100;
    if(robot[n]->heat < 0)
        robot[n]->heat = 0;
    if(robot[n]->last_damage < robot[n]->maxint)
        robot[n]->last_damage++;
    if(robot[n]->last_hit < robot[n]->maxint)
        robot[n]->last_hit++;

    //update heat
    if(robot[n]->shields_up && (game_cycle and 3 == 0))
        robot[n]->heat++;
    if(!robot[n]->shields_up){
        if(robot[n]->heat > 0)
            switch(robot[n]->config.heatsinks){
                case 5:
                    if(game_cycle and 1 == 0)
                        robot[n]->heat--;
                    break;
                case 4:
                    if(game_cycle % 3 == 0)
                        robot[n]->heat--;
                    break;
                case 3:
                    if(game_cycle and 3 == 0)
                        robot[n]->heat--;
                    break;
                case 2:
                    if(game_cycle and 7 == 0)
                        robot[n]->heat--;
                    break;
                case 1:
                    break;
                default:
                    if(game_cycle and 3 == 0)
                        robot[n]->heat++;
            }
        if(robot[n]->overburn && (game_cycle % 3 == 0))
            robot[n]->heat++;
        if(robot[n]->head > 0)
            robot[n]->heat--;
        if(robot[n]->heat > 0 && (game_cycle and 7 == 0) && (abs(robot[n]->tspd) <= 25))
            robot[n]->heat--;
        if(robot[n]->heat <= robot[n]->shutdown-50 || (robot[n]->heat <= 0))
            robot[n]->cooling = false;
    }
    if(robot[n]->cooling)
        robot[n]->tspd = 0;
    heat_mult = 1;
    switch(robot[n]->heat){
        case 80..99:
            heat_mult = 0.98;
            break;
        case 100..149:
            heat_mult = 0.95;
            break;
        case 150..199:
            heat_mult = 0.85;
            break;
        case 200..249:
            heat_mult = 0.75;
            break;
        case 250..maxint:
            heat_mult = 0.50;
    }
    if(robot[n]->overburn)
        heat_mult = heat_mult * 1.3;
    if(robot[n]->heat >= 475 && (game_cycle and 03 == 0))
        damage(n, 1, true);
    else if(robot[n]->heat >= 450 && (game_cycle and 07 == 0))
        damage(n, 1, true);
    else if(robot[n]->heat >= 400 && (game_cycle and 15 == 0))
        damage(n, 1, true);
    else if(robot[n]->heat >= 350 && (game_cycle and 31 == 0))
        damage(n, 1, true);
    else if(robot[n]->heat >= 300 && (game_cycle and 63 == 0))
        damage(n, 1, true);

    //update robot in physical world
    //acceleration
    if(abs(robot[n]->spd-robot[n]->tspd) <= robot[n]->acceleration)
        robot[n]->spd = robot[n]->tspd;
    else{
        if(robot[n]->tspd > robot[n]->spd) {
            robot[n]->spd++;
            robot[n]->acceleration++;
        }else{
            robot[n]->spd--;
            robot[n]->acceleration--;
        }
    }
    tthd = robot[n]->hd+robot[n]->shift;
    if(abs(robot[n]->hd-robot[n]->thd) <= robot[n]->turn_rate || (abs(robot[n]->hd-robot[n]->thd) >= 256-robot[n]->turn_rate))
        robot[n]->hd = robot[n]->thd;
    else if(robot[n]->hd != robot[n]->thd){
        k = 0;
        if((robot[n]->thd > robot[n]->hd) && (abs(robot[n]->hd-robot[n]->thd) <= 128) ||
                (robot[n]->thd < robot[n]->hd) && (abs(robot[n]->hd-robot[n]->thd) >= 128))
            k = 1;
        if(k = 1)
            robot[n]->hd = (robot[n]->hd + robot[n]->turn_rate) and 255;
        else
            robot[n]->hd = (robot[n]->hd + 256 - robot[n]->turn_rate) and 255;
    }
    robot[n]->hd = robot[n]->hd and 255;
    if(robot[n]->keepshift)
        robot[n]->shift = (tthd - robot[n]->hd + 1024) and 255;
    robot[n]->speed = robot[n]->spd/100*(max_vel*heat_mult*robot[n]->speedadj);
    robot[n]->xv = sint[robot[n]->hd]*robot[n]->speed;
    robot[n]->yv = -cost[robot[n]->hd]*robot[n]->speed;
    if(robot[n]->hd == 0 || robot[n]->hd == 128)
        robot[n]->xv = 0;
    if(robot[n]->hd == 64 || robot[n]->hd == 192)
        robot[n]->yv = 0;
    if(robot[n]->xv != 0)
        robot[n]->ttx = robot[n]->x + robot[n]->xv;
    else
        robot[n]->ttx = robot[n]->x;
    if(robot[n]->yv != 0)
        robot[n]->tty = robot[n]->y + robot[n]->yv;
    else
        robot[n]->tty = y;
    if(robot[n]->ttx < 0 || (robot[n]->tty < 0) || (robot[n]->ttx > 1000) || (robot[n]->tty > 1000)){
        robot[n]->ram[8]++;
        robot[n]->tspd = 0;
        if(abs(robot[n]->speed) >= max_vel/2)
            damage(n, 1, true);
        robot[n]->spd = 0;
    }
    for(i = 0; i <= num_robots; i++)
        if(i != n && robot[i]->armor > 0 && distance(robot[n]->ttx, robot[n]->tty, robot[i]->x, robot[i]->y) < crash_range){
            robot[n]->tspd = 0;
            robot[n]->spd = 0;
            robot[n]->ttx = x;
            robot[n]->tty = y;
            robot[i]->tspd = 0;
            robot[i]->spd = 0;
            robot[n]->ram[8]++;
            robot[i]->ram[8];
            if(abs(robot[n]->speed) >= max_vel/2){
                damage(n, 1, true);
                damage(i, 1, true);
            }
        }
    if(robot[n]->ttx < 0)
        robot[n]->ttx = 0;
    if(robot[n]->tty < 0)
        robot[n]->tty = 0;
    if(robot[n]->ttx > 1000)
        robot[n]->ttx = 1000;
    if(robot[n]->tty > 1000)
        robot[n]->tty = 1000;
    robot[n]->meters = robot[n]->meters + distance(robot[n]->x, robot[n]->y, robot[n]->ttx, robot[n]->tty);
    if(robot[n]->meters > maxint)
        robot[n]->meters = robot[n]->meters-maxint;
    robot[n]->ram[9] = trunc(robot[n]->meters);
    robot[n]->x = robot[n]->ttx;
    robot[n]->y = robot[n]->tty;

    //draw robot
    if(robot[n]->armor < 0)
        robot[n]->armor = 0;
    if(robot[n]->heat < 0)
        robot[n]->heat = 0;
    if(graphix){
        if(robot[n]->armor != robot[n]->larmor)
            update_armor(n);
        if(robot[n]->heat/5 != robot[n]->lheat/5)
            update_heat(n);
        draw_robot(n);
    }
    robot[n]->lheat = robot[n]->heat;
    robot[n]->larmor = robot[n]->armor;

    robot[n]->cycles_lives++;
}

int main() {
    log_error(4);
}
