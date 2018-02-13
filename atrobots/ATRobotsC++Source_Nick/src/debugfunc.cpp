#include <iostream>
#include <string>

using namespace std;

bool logging_errors = true;

ofstream log;

/*
 * Logs errors to the console for debugging robots.
 */
void log_error(int i, int n, string ov){
    log.open("errlog.txt");
    log<< endl;
    log<< endl;
    if(!logging_errors)
        log<< "Turn on debugging mode to error check your code!"<<endl;
    switch(i){
    case 1:
        log<< "Stack full - Too many CALLs?"<<endl;
        break;

    case 2:
        log<< "Label not found. Hmmm."<<endl;
        break;

    case 3:
        log<< "Can't assign value - Tisk tisk."<<endl;
        break;

    case 4:
        log<< "Illegal memory reference"<<endl;
        break;

    case 5:
        log<< "Stack empty - Too many RETs?"<<endl;
        break;

    case 6:
        log<< "Illegal instruction. How bizarre."<<endl;
        break;

    case 7:
        log<< "Return out of range - Woops!"<<endl;
        break;

    case 8:
        log<< "Divide by zero"<<endl;
        break;

    case 9:
        log<< "Unresolved !label. WTF?"<<endl;
        break;

    case 10:
        log<< "Invalid Interrupt Call"<<endl;
        break;

    case 11:
        log<< "Invalid Port Access"<<endl;
        break;

    case 12:
        log<< "Com Queue empty"<<endl;
        break;

    case 13:
        log<< "No mine-layer, silly."<<endl;
        break;

    case 14:
        log<< "No mines left"<<endl;
        break;

    case 15:
        log<< "No shield installed - Arm the photon torpedoes instead. :/"<<endl;
        break;

    case 16:
        log<< "Invalid Microcode in instruction."<<endl;
        break;

    default:
        log<< "Unknown error."<<endl;
    }
    log<< endl;
    log<< endl;
    log<< " <", i, "> ", s, " (Line #",robot[n]->ip,") [Cycle: ", robot[n]->game_cycle,", Match: ",robot[n]->played, "/", robot[n]->matches,"]"<<endl;
    log<< " ", mneonic(robot[n]->code[robot[n]->ip].op[0], robot[n]->code[robot[n]->ip].op[3] & 15),"  ",
                operand(robot[n]->code[robot[n]->ip].op[1], (robot[n]->code[robot[n]->ip].op[3] >> 4) & 15), ", ",
                operand(robot[n]->code[robot[n]->ip].op[2], (robot[n]->code[robot[n]->ip].op[3] >> 8) & 15)<<endl;
    if(ov != '')
        log<< "    (Values: ", ov, ")"<<endl;
    else
        log<< " ";
    log<< " AX=", addrear(cstr(robot[n]->ram[65])+",", 7)<<endl;
    log<< " BX=", addrear(cstr(robot[n]->ram[66])+",", 7)<<endl;
    log<< " CX=", addrear(cstr(robot[n]->ram[67])+",", 7)<<endl;
    log<< " DX=", addrear(cstr(robot[n]->ram[68])+",", 7)<<endl;
    log<< " EX=", addrear(cstr(robot[n]->ram[69])+",", 7)<<endl;
    log<< " FX=", addrear(cstr(robot[n]->ram[70])+",", 7)<<endl;
    log<< " Flags = ", robot[n]->ram[64]<<endl;
    log<< " AX=", addrear(hex(robot[n]->ram[65])+",", 7)<<endl;
    log<< " BX=", addrear(hex(robot[n]->ram[66])+",", 7)<<endl;
    log<< " CX=", addrear(hex(robot[n]->ram[67])+",", 7)<<endl;
    log<< " DX=", addrear(hex(robot[n]->ram[68])+",", 7)<<endl;
    log<< " EX=", addrear(hex(robot[n]->ram[69])+",", 7)<<endl;
    log<< " FX=", addrear(hex(robot[n]->ram[70])+",", 7)<<endl;
    log<< " Flags = ", hex(robot[n]->ram[64])<<endl;
    log.close();
    return;
}

void prog_error(int n, string ss){
    graph_mode(false);
    log.open("errlog.txt");
    log<< endl;
    log<< endl;
    switch (n){
        case 0:
            log<< ss;
            break;
        case 1:
            log<< "Invalid :label - "+ss+", silly mortal.";
            break;
        case 2:
            log<< "Undefined identifier - "+ss+". A typo perhaps?";
            break;
        case 3:
            log<< "Memory access out of range - "+ss+"";
            break;
        case 4:
            log<< "Not enough robots for combat. Maybe we should just drive in circles.";
            break;
        case 5:
            log<< "Robot names and settings must be specified. An empty arena is no fun.";
            break;
        case 6:
            log<< "Config file not found - "+ss+"";
            break;
        case 7:
            log<< "Cannot access a config file from a config file - "+ss+"";
            break;
        case 8:
            log<< "Robot not found "+ss+". Perhaps you mistyped it?";
            break;
        case 9:
            log<< "Insufficient RAM to load robot: "+ss+"... This is not good.";
            break;
        case 10:
            log<< "Too many robots! We can only handle "+cstr(max_robots+1)+"! Blah.. limits are limits.";
            break;
        case 11:
            log<< "You already have a perfectly good #def for "+ss+", silly.";
            break;
        case 12:
            log<< "Variable name too long! (Max:"+cstr(max_var_len)+") "+ss+"";
            break;
        case 13:
            log<< "!Label already defined "+ss+", silly.";
            break;
        case 14:
            log<< "Too many variables! (Var Limit: "+cstr(max_vars)+")";
            break;
        case 15:
            log<< "Too many !labels! (!Label Limit: "+cstr(max_labels)+")";
            break;
        case 16:
            log<< "Robot program too long! Boldly we simplify, simplify along..."+ss;
            break;
        case 17:
            log<< "!Label missing error. !Label #"+ss+".";
            break;
        case 18:
            log<< "!Label out of range: "+ss+"";
            break;
        case 19:
            log<< "!Label not found. "+ss+"";
            break;
        case 20:
            log<< "Invalid config option: "+ss+". Inventing a new device?";
            break;
        case 21:
            log<< "Robot is attempting to cheat; Too many config points ("+ss+")";
            break;
        case 22:
            log<< "Insufficient data in data statement: "+ss+"";
            break;
        case 23:
            log<< "Too many asterisks: "+ss+"";
            break;
        case 24:
            log<< "Invalid step count: "+ss+". 1-9 are valid conditions.";
            break;
        case 25:
            log<< "'"+ss+"'";
            break;
        default:
            log<< ss;
    }
    log.close();
    return;

}

int main() {
    log_error(4);
}
