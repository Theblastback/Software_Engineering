//Seperate file for execute_instruction function

void execute_instruction ( int n){
  int i, j, k;
  int time_used, loc;
  bool inc_ip;
  char c;

  //Update System Variables
  ram [000] = tspd;
  ram [001] = thd;
  ram [002] = shift;
  ram [003] = accuracy;

  time_used = 1;
  inc_ip = true;
  loc = 0;
  if (ip > plen || ip < 0) {
    ip = 0;
  }

  if (invalid_microcode(n,ip)){
    time_used = 1;
    robot_error (n, 16, hex(code[ip].op[max_op]));
  }

  if (graphix && step_mode > 0 && n == 0){
    step_count++;
    update_cycle_window;
    update_debug_window;

    if(step_count % step_mode = 0) {
      step_loop = true;
    }
    else{
      step_loop = false;
    }

    while (step_loop && !(quit || gameover || bout_over)){
      if (keypressed){
        //with robot[0]^ do
         upcase(readkey);
         switch (c){

         case 'X':
         temp_mode = step_mode;
         step_mode = 0;
         step_loop = false;
         close_debug_window;
         break;

         case ' ':
         step_count = 0;
         step_loop = false;
         break;

         case 1 ... 9:
         step_mode =  value(c); //figure out value
         step_count = 0;
         step_loop = false;
         break;

         case 0:
         step_mode = 10;
         step_count = 0;
         step_loop = false;
         break;

         case '-': //check these for set color, outtext and dec
         if ( robot[n] -> mem_watch > 0){
           setcolor (0);
           for (int i = 0; i <= 9; i++){
              outtextxy(035,212+(10*i),decimal((robot[n] -> mem_watch+i),4) + ' :')
           }
           robot[n] -> mem_watch--;
           update_debug_memory;
         }
         break;
         case '_':
         if (robot[n] -> mem_watch > 0){
           setcolor (0);
           for (int i = 0; i <= 9; i++){
              outtextxy(035,212+(10*i),decimal((robot[n] -> mem_watch+i),4) + ' :')
              robot[n] -> mem_watch--;
              update_debug_memory;
           }
         }
         break;

         case '+':
         if (robot[n] -> mem_watch < 1014){
           setcolor(0);
           for (int i = 0; i <= 9; i++){
             outtextxy(035,212+(10*i),decimal((robot[n] -> mem_watch+i),4) + ' :');
           }
           robot[n] -> mem_watch++;
           update_debug_memory;
         }
         break;
         case '=':
         if (robot[n] -> mem_watch < 1014){
           setcolor(0);
           for (int i = 0; i <= 9; i++){
             outtextxy(035,212+(10*i),decimal((robot[n] -> mem_watch+i),4) + ' :');
           }
           robot[n] -> mem_watch++;
           update_debug_memory;
         }
         break;

         case '[':
         if(robot[n] -> mem_watch > 0){
           setcolor(0);
           for (int i = 0; i <= 9; i++){
             outtextxy(035,212+(10*i),decimal((robot[n] -> mem_watch+i),4) + ' :');
           }
           robot[n] -> mem_watch -= 10;
           if (robot[n] -> mem_watch < 0){
             robot[n] -> mem_watch = 0;
           }
           update_debug_memory
         }
         break;
         case '{':
         if(robot[n] -> mem_watch > 0){
           setcolor(0);
           for (int i = 0; i <= 9; i++){
             outtextxy(035,212+(10*i),decimal((robot[n] -> mem_watch+i),4) + ' :');
           }
           robot[n] -> mem_watch -= 10;
           if (robot[n] -> mem_watch < 0){
             robot[n] -> mem_watch = 0;
           }
           update_debug_memory
         }
         break;

         case ']':
         if (robot[n] -> mem_watch < 1014){
           setcolor(0);
           for (int i = 0l i <= 9; i++){
             outtextxy(035,212+(10*i), decimal((robot[n] -> mem_watch+i), 4) + ' :');
           }
           robot[n] -> mem_watch += 10;
           if (robot[n] -> mem_watch > 1014){
             robot[n] -> mem_watch = 1014;
           }
           update_debug_memory;
         }
         break;
         case '}':
         if (robot[n] -> mem_watch < 1014){
           setcolor(0);
           for (int i = 0l i <= 9; i++){
             outtextxy(035,212+(10*i), decimal((robot[n] -> mem_watch+i), 4) + ' :');
           }
           robot[n] -> mem_watch += 10;
           if (robot[n] -> mem_watch > 1014){
             robot[n] -> mem_watch = 1014;
           }
           update_debug_memory;
         }
         break;

         case 'G':
         toggle_graphix;
         temp_mode = step_mode;
         step_mode = 0;
         step_loop = false;
         break;
       }
     }
     else process_keypress(c);
   }
  }

  if (! ((code[robot[n] -> ip].op[max_op] && 7) in [0,1])){
    time_used = 0;
  }
  else {
    switch (get_val(n,robot[n] -> ip,0)){ //assembly?
      case 0: executed++; break; //NOP
      case 1: //ADD
        put_val(n,robot[n] -> ip,1,get_val(n, robot[n] -> ip,1) + get_val(n,robot[n] -> ip,2));
        executed++;
        break;
      case 2: //SUB
        put_val(n,robot[n] -> ip,1,get_val(n,robot[n] -> ip,1) - get_val(n,robot[n] -> ip,2));
        executed++;
        break;
      case 3://OR
        put_val(n,robot[n] -> ip,1,get_val(n,robot[n] -> ip,1) | get_val(n,robot[n] -> ip,2));
        executed++;
        break;
      case 4: //AND
        put_val(n,robot[n] -> ip,1,get_val(n,robot[n] -> ip,1) & get_val(n,robot[n] -> ip,2));
        executed ++;
        break;
      case 5: //XOR
        put_val(n,robot[n] -> ip,1,get_val(n,robot[n] -> ip,1) ^ get_val(n,robot[n] -> ip,2));
        executed++;
        break;
      case 6: //NOT
        put_val(n,robot[n] -> ip,1,!(get_val(n,robot[n] -> ip,1)));
        executed++;
        break;
      case 7: //MPY
        put_val(n,ip,1,get_val(n,ip,1)*get_val(n,ip,2));
        time_used = 10;
        executed++;
        break;
      case 8: //DIV
        j = get_val(n,ip,2);
        if (j != 0){
          put_val(n,ip,1,get_val(n,ip,1) / j);
        }
        else{
          robot_error(n,8,"");
        }
        time_used = 10;
        executed++;
        }
        break;
      case 9: //MOD
        j = get_val(n,ip,2);
        if (j != 0){
          put_val(n,ip,1,get_val(n,ip,1) % j);
        }
        else {
          robot_error(n,8,"");
        }
        time_used = 10;
        executed++;
        break;
      case 10: //RET
        ip = pop(n);
        if (ip < 0 || ip > plen){
          robot_error(n,7,cstr(ip));
        }
        executed++;
        break;
      case 11: //GSB
        loc - find_label(get_val(n,ip,1),code[ip].op[max_op] >> (1*4))
        if (loc >= 0){
          push(n,ip);
          inc_ip = false;
          ip = loc;
        }
        else {
          robot_error(n,2,cstr(get_val(n,i,1)));
        }
        executed++;
        break;
      case 12: //JMP
        jump(n,1,inc_ip);
        executed++;
        break;
      case 13: //JLS,JB
        if (ram[64] && 2>0){
          jump(n,1,inc_ip);
        }
        time_used = 0;
        executed++;
        break;
      case 14: //JGR,JA
        if (ram[64] && 4>0){
          jump(n,1,inc_ip);
        }
        time_used = 0;
        executed++;
        break;
      case 15: //JNE
        if (ram[64] and 1 = 0){
          jump(n,1,inc_ip);
        }
        time_used = 0;
        executed++;
        break;
      case 16: // JEQ,JE
        if (ram[64] && 1 > 0){
          jump (n,1,inc_ip);
        }
        time_used = 0;
        executed++;
        break;
      case 17://SWAP, XCHG
        ram[4] = get_val(n,ip,1);
        put_val(n,ip,1,get_val(n,ip,2));
        put_val(n,ip,2,ram[4]);
        time_used = 3;
        executed++;
        break;
      case 18: //DO
        ram[67]:=get_val(n,ip,1);
        executed++;
        break;
      case 19: //loop
        ram[67]--;
        if (ram[67] > 0){
          jump (n,1,inc_ip);
        }
        executed++;
        break;
      case 20: //CMP
        k = get_val(n,ip,1) - get_val(n,ip,2);
        ram [64] = ram[64] & $FFF0
        if (k=0){
          ram[64] = ram [64] | 1;
        }
        if (k<0){
          ram[64] = ram [64] | 2;
        }
        if (k>0){
          ram[64] = ram [64] | 4;
        }
        if (get_val(n,ip,2) = 0 && k=0){
          ram[64] = ram[64] | 8;
        }
        executed++;
        break;
      case 21: //TEST
        k = get_val(n,ip,1) & get_val(n,ip,2);
        ram [64] = ram[64] & $FFF0
        if (k = get_val(n,ip,2)){
          ram[64] =  ram[64] | 1;
        }
        if (k = 0){
          ram[64] = ram[64] | 8;
        }
        executed++;
        break;
      case 22: //MOV, SET
        put_val(n,ip,1,get_val(n,ip,2));
        executed++;
        break;
      case 23: //LOC
        put_val(n,ip,1,code[ip].op[2]);
        time_used = 2;
        executed++;
        break;
      case 24: //GET
        k = get_val(n,ip,2);
        if (k >=0 && l <= MAX_RAM){
          put_val (n,ip,1,ram[k])
        }
        else {
          if (k > MAX_RAM && k <= (MAX_RAM + 1) + (((MAX_CODE + 1) shl 3)-1)){
            j = k - MAX_RAM - 1;
            put_val(n,ip,1,code[j shr 2].op[j & 3]);
          }
          else{
            robot_error(n,4,cstr(k));
          }
          time_used = 2;
          executed++;
        }
        break;
      case 25: //PUT
        k= get_val(n,ip,2);
        if (k>=0 and k<=MAX_RAM){
            ram[k]= get_val(n,ip,1)
          }
          else {
            robot_error(n,4,cstr(k));
          }
        time_used = 2;
        executed++;
        break;
      case 26: //INT
        call_int(n,get_val(n,ip,1),time_used);
        executed++;
        break;
      case 27: //IPO,IN
        time_used = 4;
        put_val(n,ip,2,in_port(n,get_val(n,ip,1),time_used));
        executed++;
        break;
      case 28: //OPO,OUT
        out_port(n,get_val(n,ip,1),get_val(n,ip,2),time_used);
        executed++;
        break;
      case 29: //DEL,DELAY
        time_used = get_val(n,ip,1);
        executed++;
        break;
      case 30: //PUSH
        push(n,get_val(n,ip,1));
        executed++;
        break;
      case 31: //POP
        put_val(n,ip,1,pop(n));
        executed++;
        break;
      case 32: //ERR
        robot_error(n,get_val(n,ip,1),"");
        time_used = 0;
        executed++;
        break;
      case 33: //INC
        put_val(n,ip,1,get_val(n,ip,1)+1);
        executed++;
        break;
      case 34: //DEC
        put_val(n,ip,1,get_val(n,ip,1)-1);
        executed++;
        break;
      case 35: //SHL
        put_val(n,ip,1,get_val(n,ip,1) shl get_val(n,ip,2));
        executed++;
        break;
      case 36: //SHR
        put_val(n,ip,1,get_val(n,ip,1) shr get_val(n,ip,2));
        executed++;
        break;
      case 37: //ROL
        put_val(n,ip,1,rol(get_val(n,ip,1),get_val(n,ip,2)));
        break;
      case 38: //ROR
        put_val(n,ip,1,ror(get_val(n,ip,1),get_val(n,ip,2)));
        executed++;
        break;
      case 39: //JZ
        time_used = 0;
        if (ram[64] && 8 > 0){
          jump(n,1,inc_ip);
        }
        executed++;
        break;
      case 40: //JNZ
        time_used=0
        if (ram[64] && 8 = 0){
          jump(n,1,inc_ip);
        }
        executed++;
        break;
      case 41: //JBE,JLE
        if ((ram[64] && 1 > 0) || (ram[64] && 4 > 0)){
          jump(n,1,inc_ip);
        }
        time_used = 0;
        executed++;
        break;
      case 42: //JBE,JLE
        if ((ram[64] && 1 > 0) || (ram[64] && 2 > 0)){
          jump(n,1,inc_ip);
        }
        time_used = 0;
        executed++;
        break;
      case 43: //SAL
        put_val(n,ip,1,sal(get_val(n,ip,1),get_val(n,ip,2)));
        executed++;
        break;
      case 44: //SAR
        put_val(n,ip,1,sar(get_val(n,ip,1),get_val(n,ip,2)));
        executed++;
        break;
      case 45: //NEG
        put_val(n,ip,1,0-get_val(n,ip,1));
        executed++;
        break;
      case 46: //JTL
        loc = get_val(n,ip,1);
        if (loc >= 0 && loc <=plen){
          inc_ip = false;
          ip = loc;
        }
        else {
          robot_error(n,2,cstr(loc));
        }
        break;
    else {
      robot_error(n,6,"");
     }
    }

  delay_left++;
  time_used++;
  if (inc_ip){
    ip++;
  }
  if (graphix && n = 0 && step_mode > 0){
    update_debug_window
  }
}
