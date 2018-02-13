#include <iostream>
#include <string>

using namespace std;

bool logging_errors = true;

void log_error(int i){
    if(!logging_errors)
        cout<< "Turn on debugging mode to error check your code!"<<endl;
    //with robot[n]^ do
    switch(i){
    case 1:
        cout<< "Stack full - Too many CALLs?"<<endl;
        break;

    case 2:
        cout<< "Label not found. Hmmm."<<endl;
        break;

    case 3:
        cout<< "Can't assign value - Tisk tisk."<<endl;
        break;

    case 4:
        cout<< "Illegal memory reference"<<endl;
        break;

    case 5:
        cout<< "Stack empty - Too many RETs?"<<endl;
        break;

    case 6:
        cout<< "Illegal instruction. How bizarre."<<endl;
        break;

    case 7:
        cout<< "Return out of range - Woops!"<<endl;
        break;

    case 8:
        cout<< "Divide by zero"<<endl;
        break;

    case 9:
        cout<< "Unresolved !label. WTF?"<<endl;
        break;

    case 10:
        cout<< "Invalid Interrupt Call"<<endl;
        break;

    case 11:
        cout<< "Invalid Port Access"<<endl;
        break;

    case 12:
        cout<< "Com Queue empty"<<endl;
        break;

    case 13:
        cout<< "No mine-layer, silly."<<endl;
        break;

    case 14:
        cout<< "No mines left"<<endl;
        break;

    case 15:
        cout<< "No shield installed - Arm the photon torpedoes instead. :/"<<endl;
        break;

    case 16:
        cout<< "Invalid Microcode in instruction."<<endl;
        break;

    default:
        cout<< "Unknown error."<<endl;
    }
    return;
}
/*
*procedure log_error(n,i:integer;ov:string);
var
 j,k:integer;
 s:string;
begin
 if not logging_errors then exit;
 with robot[n]^ do
  begin
   case i of
    01:s:='Stack full - Too many CALLs?';
    02:s:='Label not found. Hmmm.';
    03:s:='Can''t assign value - Tisk tisk.';
    04:s:='Illegal memory reference';
    05:s:='Stack empty - Too many RETs?';
    06:s:='Illegal instruction. How bizarre.';
    07:s:='Return out of range - Woops!';
    08:s:='Divide by zero';
    09:s:='Unresolved !label. WTF?';
    10:s:='Invalid Interrupt Call';
    11:s:='Invalid Port Access';
    12:s:='Com Queue empty';
    13:s:='No mine-layer, silly.';
    14:s:='No mines left';
    15:s:='No shield installed - Arm the photon torpedoes instead. :-)';
    16:s:='Invalid Microcode in instruction.';
    else s:='Unkown error';
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

int main()
{
    log_error(4);
}
