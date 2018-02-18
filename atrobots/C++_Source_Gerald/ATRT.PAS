(*
Copyright (c) 1999, Ed T. Toton III. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

   All advertising materials mentioning features or use of this software
   must display the following acknowledgement:

        This product includes software developed by Ed T. Toton III &
        NecroBones Enterprises.

   No modified or derivative copies or software may be distributed in the
   guise of official or original releases/versions of this software. Such
   works must contain acknowledgement that it is modified from the original.

   Neither the name of the author nor the name of the business or
   contributers may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*)

{ This program is a shell for ATR2 that runs tournaments. }

{$E+}{$N+}{$G+}{$X+}{$D-}
{$M 12288,0,0}
program AT_Robots_tournament;

uses stuff, filelib, myfile, crt, dos;

const
 progname       ='ATR-Tournament';
 version        ='1.05';
 cnotice1       ='Copyright (C) 1997, Ed T. Toton III';
 cnotice2       ='All Rights Reserved.';
 main_filename  ='ATR2';
 prog_ext       ='.EXE';
 robot_ext      ='.AT2';
 robot_ext2     ='.ATL';
 config_ext     ='.ATS';
 compile_ext    ='.CMP';
 report_ext     ='.REP';
 tourn_ext      ='.TRN';
 list_ext       ='.LST';
 result_ext     ='.RES';
 data_ext       ='.DAT';
 log_ext        ='.LOG';

 max_robots     =512;

type
 string8        =string[8];
 robot_ptr      =^string8;
 robot_rec      = record
                   name:string[8];
                   wins,matches,shots,kills,deaths,hits,damage,cycles,errors:longint;
                   locked:boolean;
                  end;

var
 tname,robot_dir:string;
 num_robots,matches:longint;
 game_limit:longint;
 robot:array[1..max_robots] of robot_rec;
 unequal_matches,yes_to_all,report_only,sound_on,registered,abort,complete:boolean;
 reg_name:string;
 reg_num:word;
 log_file:text;
 param_line:string;
 bestkiller,bestsurvivor,mostdead,triggerhappy,bestmarksman,
 mostdestructive,longestlived,mosterrors,haphazard,conservative,
 leasteffective,biggestloser,shortestlived,longestreign,newestcontender:integer;



function divide(r1,r2:real):real;
var
 r:real;
begin  {divide, checking for division by zero}
 if (r1=0) or (r2=0) then r:=0
         else r:=r1/r2;
 divide:=r;
end;

procedure chirp;
begin
 if (not sound_on) then exit;
 sound(2000);
 delay(100);
 sound(4000);
 delay(100);
 sound(1000);
 delay(100);
 sound(2000);
 delay(100);
 nosound;
end;



procedure check_registration;
var
 w:word;
 i:integer;
 f:text;
 s:String;
begin
 registered:=false;
 if exist('ATR2.REG') then
  begin
   assign(f,'ATR2.REG');
   reset(f);
   readln(f,reg_name);
   readln(f,reg_num);
   close(f);
   w:=0; s:=btrim(ucase(reg_name));
   for i:=1 to length(s) do
     inc(w,ord(s[i]));
   w:=w xor $5AA5;
   if w=reg_num then registered:=true;
  end;
end;


procedure pause;
begin
 if yes_to_all then exit;
 write('[PAUSE]');
 flushkey;
 if readkey=#27 then halt;
 write(#13,'        ',#13);
end;

function cstrr(i:real; k:integer):string;
var
 s1:string[255];
begin
 str(i:0:k,s1);
 cstrr:=s1;
end;

procedure shutdown;
begin
 if not report_only then
  begin
   writeln; textcolor(3);
   write  (progname,' ',version,' ');
   writeln(cnotice1);
   writeln(cnotice2);
   textcolor(7);
   writeln;
  end;
 halt;
end;

procedure init;
var
 f:text;
 fn:string;
 i,j,k:integer;
begin

 registered:=true;
 reg_name:='Unregistered';
 reg_num:=$FFFF;
 {check_registration;}

 if not registered then
  begin
   textcolor(30);
   write('UNREGISTERED');
   textcolor(14);
   write(' - ');
   textcolor(12);
   writeln('You must register AT-Robots first.');
   textcolor(7);
   writeln;
   halt;
  end;

 tname:=''; robot_dir:='';
 if paramcount<1 then
  begin
   writeln('Usage:  ATRT <tournament name>');
   writeln; halt;
  end;
 tname:=base_name(ucase(btrim(paramstr(1))));
 yes_to_all:=false;
 if paramcount>1 then
  for i:=2 to paramcount do
   begin
    if (ucase(btrim(paramstr(i)))='/Y') or
       (ucase(btrim(paramstr(i)))='-Y') then yes_to_all:=true;
   end;
 sound_on:=false;
 report_only:=false;
 if tname[1]='/' then
  begin
   report_only:=true;
   tname:=base_name(rstr(tname,length(tname)-1));
   writeln('Formatting a report only, using "',tname+report_ext,'"');
   if not exist(tname+report_ext) then
    begin writeln('Report not found!'); halt; end;
  end else begin
   fn:=tname+tourn_ext;
   if not exist(fn) then
    begin
     writeln('Tournament file "',fn,'" not found!');
     writeln; halt;
    end;
   assign(f,fn); reset(f);
   readln(f,robot_dir);
   readln(f,matches);
   readln(f,game_limit);
   readln(f,i);
   readln(f,param_line);
   if i>0 then begin sound_on:=true; writeln('Chirping is on!'); chirp; end
          else sound_on:=false;
   close(f);
   if (rstr(robot_dir,1)<>'\') and (robot_dir<>'') then
      robot_dir:=robot_dir+'\';
   fn:=robot_dir+no_path(fn);
   if not valid(fn) then
    begin
     writeln('Robot directory is invalid or does not exist!');
     writeln('(',robot_dir,')'); writeln; halt;
    end;
  end;
 if matches<1 then matches:=1;
 if matches>1000 then matches:=1000;
 if game_limit<0 then game_limit:=0;
 if game_limit>16000 then game_limit:=16000;

 for i:=1 to max_robots do
  with robot[i] do
   begin name:=''; wins:=0; matches:=0; locked:=false; kills:=0; deaths:=0;
   shots:=0; hits:=0; errors:=0; cycles:=0;
   damage:=0; end;

 textcolor(7);
 if not report_only then
  begin
   writeln; textcolor(3);
   write  (progname,' ',version,' ');
   writeln(cnotice1);
   writeln(cnotice2);
   textcolor(7);
   writeln;
 end;
end;

procedure count_robots;
var
 sr:searchrec;
 i,j,k:integer;
 c:char;
begin
 k:=0;
 findfirst(robot_dir+'*'+robot_ext,archive,sr);
 while doserror=0 do
  begin
   inc(k);
   findnext(sr);
  end;
 findfirst(robot_dir+'*'+robot_ext2,archive,sr);
 while doserror=0 do
  begin
   inc(k);
   findnext(sr);
  end;
 j:=0;
 for i:=k-1 downto 1 do j:=j+i;
 if k<=1 then
  begin
   writeln('Robots found: ',k);
   writeln('There must be at least 2 robots to compete!');
   writeln; halt;
  end;
 writeln(k,' robots found. That means ',j,' one-on-one pairings of');
 writeln(matches,' matches each, for a total of ',j*matches,' matches.');
 writeln;
 writeln('Before matches begin, all robots will undergo a test-compile to');
 writeln('verify their validity as a robot program. Any failed compiles');
 writeln('will be disqualified from this list of competing robots.');
 writeln;
 write('Proceed? [Y/N] ');
 if yes_to_all then c:='Y' else
    repeat c:=upcase(readkey); until c in ['Y','N'];
 writeln(c); writeln;
 if c='N' then shutdown;
end;

function test_compile(fn:string):boolean;
var
 ok:boolean;
begin
 ok:=false;
 delete_file(main_filename+compile_ext);
 swapvectors;
 exec(main_filename+prog_ext,param_line+' /S /C '+fn);
 swapvectors;
 if exist(main_filename+compile_ext) then ok:=true;
 test_compile:=ok;
end;

procedure make_robot_list;
var
 sr:searchrec;
 i,j,k:integer;
 f:text;
begin
 count_robots;
 num_robots:=0;
 k:=0;
 findfirst(robot_dir+'*'+robot_ext,archive,sr);
 while doserror=0 do
  begin
   if test_compile(robot_dir+base_name(sr.name)) then
    begin
     inc(num_robots);
     robot[num_robots].name:=base_name(sr.name);
     robot[num_robots].wins:=0;
     robot[num_robots].kills:=0;
     robot[num_robots].deaths:=0;
     robot[num_robots].shots:=0;
     robot[num_robots].matches:=0;
     robot[num_robots].locked:=false;
     robot[num_robots].hits:=0;
     robot[num_robots].damage:=0;
     robot[num_robots].cycles:=0;
     robot[num_robots].errors:=0;
    end;
   findnext(sr);
  end;
 findfirst(robot_dir+'*'+robot_ext2,archive,sr);
 while doserror=0 do
  begin
   if test_compile('?'+robot_dir+base_name(sr.name)) then
    begin
     inc(num_robots);
     robot[num_robots].name:=base_name(sr.name);
     robot[num_robots].wins:=0;
     robot[num_robots].kills:=0;
     robot[num_robots].deaths:=0;
     robot[num_robots].shots:=0;
     robot[num_robots].matches:=0;
     robot[num_robots].locked:=true;
     robot[num_robots].hits:=0;
     robot[num_robots].damage:=0;
     robot[num_robots].cycles:=0;
     robot[num_robots].errors:=0;
    end;
   findnext(sr);
  end;
 assign(f,tname+list_ext);
 rewrite(f);
 writeln(f,num_robots);
 for i:=1 to num_robots do
  if robot[i].locked then
    writeln(f,robot[i].name+robot_ext2)
   else
    writeln(f,robot[i].name+robot_ext);
 close(f);
end;

procedure read_robot_list;
var
 i,j,k:integer;
 f:text;
 s:string;
begin
 num_robots:=0;
 assign(f,tname+list_ext);
 reset(f);
 readln(f,num_robots);
 for i:=1 to num_robots do
  begin
   readln(f,s);
   robot[i].name:=btrim(base_name(ucase(s)));
   if ucase(btrim(exten(s)))=robot_ext2 then
      robot[i].locked:=true else robot[i].locked:=false;
   robot[i].wins:=0;
   robot[i].matches:=0;
   robot[i].shots:=0;
   robot[i].kills:=0;
   robot[i].deaths:=0;
   robot[i].cycles:=0;
   robot[i].damage:=0;
   robot[i].hits:=0;
   robot[i].errors:=0;
  end;
 close(f);
 writeln('Robot list read.');
end;

procedure swap_robots(i,k:integer);
var
 r:robot_rec;
begin
 r:=robot[i];
 robot[i]:=robot[k];
 robot[k]:=r;
end;

function higher_score(i,k:integer):boolean;
var
 needswap:boolean;
 score1,score2:real;
begin
 needswap:=false;

 if robot[i].matches=0 then score1:=0
    else score1:=robot[i].wins/robot[i].matches;
 if robot[k].matches=0 then score2:=0
    else score2:=robot[k].wins/robot[k].matches;
 (* Sort first by score, then kills, deaths, damage, longevity, errors, then name *)
 if (score1<score2) then
    needswap:=true;
 if (score1=score2) and (robot[i].matches>0) and (robot[k].matches>0) then
  begin
   if (divide(robot[i].kills,robot[i].matches)<divide(robot[k].kills,robot[k].matches)) then
      needswap:=true;
   if (divide(robot[i].kills,robot[i].matches)=divide(robot[k].kills,robot[k].matches)) then
    begin
     if (divide(robot[i].deaths,robot[i].matches)>divide(robot[k].deaths,robot[k].matches)) then
        needswap:=true;
     if (divide(robot[i].deaths,robot[i].matches)=divide(robot[k].deaths,robot[k].matches)) then
      begin
       if (divide(robot[i].damage,robot[i].matches)<divide(robot[k].damage,robot[k].matches)) then
          needswap:=true;
       if (divide(robot[i].damage,robot[i].matches)=divide(robot[k].damage,robot[k].matches)) then
        begin
         if (divide(robot[i].cycles,robot[i].matches)<divide(robot[k].cycles,robot[k].matches)) then
            needswap:=true;
         if (divide(robot[i].cycles,robot[i].matches)=divide(robot[k].cycles,robot[k].matches)) then
          begin
           if (divide(robot[i].errors,robot[i].matches)>divide(robot[k].errors,robot[k].matches)) then
              needswap:=true;
           if (divide(robot[i].errors,robot[i].matches)=divide(robot[k].errors,robot[k].matches)) then
            begin
             if (robot[i].name<robot[k].name) then
                needswap:=true;
            end;
          end;
        end;
      end;
    end;
  end;
 higher_score:=needswap;
end;

procedure rank_robots;
var
 i,k:integer;
begin
 for i:=1 to num_robots do
  for k:=i to num_robots do
   begin
    if (i<>k) then
     begin
      if (higher_score(i,k)) then
         swap_robots(i,k);
     end;
   end;
 bestkiller:=1; bestsurvivor:=1; mostdead:=1; triggerhappy:=1;
 bestmarksman:=1; mostdestructive:=1; longestlived:=1; mosterrors:=1;
 haphazard:=1; conservative:=1; leasteffective:=1; biggestloser:=1;
 shortestlived:=1; newestcontender:=1; longestreign:=1; unequal_matches:=false;
 for i:=1 to num_robots do
  with robot[i] do
   begin
    if divide(wins,matches)>divide(robot[bestsurvivor].wins,robot[bestsurvivor].matches) then bestsurvivor:=i;
    if divide(wins,matches)<=divide(robot[biggestloser].wins,robot[biggestloser].matches) then biggestloser:=i;
    if divide(kills ,matches)>divide(robot[bestkiller].kills,robot[bestkiller].matches) then bestkiller:=i;
    if divide(deaths,matches)>divide(robot[mostdead].deaths,robot[mostdead].matches) then mostdead:=i;
    if divide(shots ,matches)>divide(robot[triggerhappy].shots,robot[triggerhappy].matches) then triggerhappy:=i;
    if divide(shots ,matches)<=divide(robot[conservative].shots,robot[conservative].matches) then conservative:=i;
    if divide(hits,shots)>divide(robot[bestmarksman].hits,robot[bestmarksman].shots) then bestmarksman:=i;
    if (divide(hits,shots)<=divide(robot[haphazard].hits,robot[haphazard].shots)) and (shots>0) then haphazard:=i;
    if divide(damage,matches)>divide(robot[mostdestructive].damage,robot[mostdestructive].matches) then mostdestructive:=i;
    if divide(damage,matches)<=divide(robot[leasteffective].damage,robot[leasteffective].matches) then leasteffective:=i;
    if divide(cycles,matches)>divide(robot[longestlived].cycles,robot[longestlived].matches) then longestlived:=i;
    if divide(cycles,matches)<=divide(robot[shortestlived].cycles,robot[shortestlived].matches) then shortestlived:=i;
    if divide(errors,matches)>divide(robot[mosterrors].errors,robot[mosterrors].matches) then mosterrors:=i;
    if matches>robot[longestreign].matches then begin longestreign:=i; unequal_matches:=true; end;
    if matches<robot[newestcontender].matches then begin newestcontender:=i; unequal_matches:=true; end;
   end;
end;

procedure write_results;
var
 f,f2,f3:text;
 i,j,k:integer;
 fn,fn2,s,e:string;
 r,score:real;
 pairings:longint;
begin
 fn:=tname+result_ext;
 fn2:=tname+'.HTM';
 pairings:=0;
 for i:=num_robots-1 downto 1 do pairings:=pairings+i;
 assign(f,fn); rewrite(f);
 assign(f2,fn2); rewrite(f2);
 assign(f3,'ATRT'+report_ext); rewrite(f3); writeln(f3,num_robots);
 {--RES file header--}
 writeln(f,'Tournament results:   (',base_name(tname),')');
 writeln(f);
 writeln(f,'Number of robots:         ',addfront(cstr(num_robots),5));
 if not report_only then
  begin
   writeln(f,'Matches per pairing:      ',addfront(cstr(matches),5));
   writeln(f,'Number of pairings:       ',addfront(cstr(pairings),5));
   writeln(f,'Total matches:            ',addfront(cstr(pairings*matches),5));
  end;
 writeln(f,'-------------------------------');
 if not report_only then
  begin
   writeln(f,'Total pairings per robot: ',addfront(cstr((num_robots-1)),5));
   writeln(f,'Total matches per robot:  ',addfront(cstr((num_robots-1)*matches),5));
  end;
 writeln(f);
 s:=time; s:=lstr(s,length(s)-5);
 writeln(f,'Results completed: ',s,', ',date);
 writeln(f);
 {--HTML file header--}
 writeln(f2,'<HTML>');
 writeln(f2,'<HEAD>');
 writeln(f2,'<TITLE>AT-Robots Tournament Results (',base_name(tname),')</TITLE>');
 writeln(f2,'</HEAD>'); writeln(f2);
 writeln(f2,'<BODY>');
 writeln(f2,'<BODY BGCOLOR="#C0C0C0" TEXT="#000000" LINK="#0000FF" VLINK="#000080">');
 writeln(f2,'<A NAME = "top"></A>');
 writeln(f2,'<H2 align=center>AT-Robots Tournament Results</H2>');
 writeln(f2,'<H2 align=center>(',base_name(tname),')</H2><BR>');
 writeln(f2);
 writeln(f2,'<BR>');
 writeln(f2,'<CENTER><TABLE align=center border=3>');
 writeln(f2,'<TR border=1><TD>Number of robots: </TD><TD align=right>',addrear(cstr(num_robots),5),'</TD>');
 if not report_only then
  begin
   writeln(f2,'<TR border=1><TD>Matches per pairing: </TD><TD align=right>',addrear(cstr(matches),5),'</TD>');
   writeln(f2,'<TR border=1><TD>Number of pairings:  </TD><TD align=right>',addrear(cstr(pairings),5),'</TD>');
   writeln(f2,'<TR border=1><TD>Total matches:    </TD><TD align=right>',addrear(cstr(pairings*matches),5),'</TD>');
   writeln(f2,'</TABLE></CENTER>');
   writeln(f2,'<CENTER><TABLE align=center border=3>');
   writeln(f2,'<TR border=1><TD>Total pairings per robot:  </TD><TD align=right>',addrear(cstr((num_robots-1)),5),'</TD>');
   writeln(f2,'<TR border=1><TD>Total matches per robot:</TD><TD align=right>',addrear(cstr((num_robots-1)*matches),5),
              '</TD>');
  end;
 writeln(f2,'</TABLE></CENTER>');
 writeln(f2);
 writeln(f2,'<BR>');
 writeln(f2,'<CENTER><TABLE align=center border=3>');
 s:=time; s:=lstr(s,length(s)-5);
 writeln(f2,'<TD align=center>Results completed: ',s,', ',date,'</TD>');
 writeln(f2,'</TABLE></CENTER>');
 writeln(f2,'<BR>');


 {--RES best/worst list--}
 writeln(f,'Best Survivor:      ',addrear(base_name(robot[bestsurvivor].name),9),
           '(',cstrr(divide(robot[bestsurvivor].wins,robot[bestsurvivor].matches)*100,3),'% wins)');
 writeln(f,'Biggest Loser:      ',addrear(base_name(robot[biggestloser].name),9),
           '(',cstrr(divide(robot[biggestloser].wins,robot[biggestloser].matches)*100,3),'% wins)');
 writeln(f,'Best Killer:        ',addrear(base_name(robot[bestkiller].name),9),
           '(',cstrr(divide(robot[bestkiller].kills,robot[bestkiller].matches),3),' kills/match)');
 writeln(f,'Most Dead:          ',addrear(base_name(robot[mostdead].name),9),
           '(',cstrr(divide(robot[mostdead].deaths,robot[mostdead].matches),3),' deaths/match)');
 writeln(f,'Most Trigger-Happy: ',addrear(base_name(robot[triggerhappy].name),9),
           '(',cstrr(divide(robot[triggerhappy].shots,robot[triggerhappy].matches),2),' shots/match)');
 writeln(f,'Most Conservative:  ',addrear(base_name(robot[conservative].name),9),
           '(',cstrr(divide(robot[conservative].shots,robot[conservative].matches),2),' shots/match)');
 writeln(f,'Best Marksman:      ',addrear(base_name(robot[bestmarksman].name),9),
           '(',cstrr(divide(robot[bestmarksman].hits,robot[bestmarksman].shots)*100,2),'% hit)');
 if robot[haphazard].shots>0 then
 writeln(f,'Most Haphazard:     ',addrear(base_name(robot[haphazard].name),9),
           '(',cstrr(divide(robot[haphazard].hits,robot[haphazard].shots)*100,2),'% hit)');
 writeln(f,'Most Destructive:   ',addrear(base_name(robot[mostdestructive].name),9),
           '(',cstrr(divide(robot[mostdestructive].damage,robot[mostdestructive].matches),2),' damage/match)');
 writeln(f,'Least Effective:    ',addrear(base_name(robot[leasteffective].name),9),
           '(',cstrr(divide(robot[leasteffective].damage,robot[leasteffective].matches),2),' damage/match)');
 writeln(f,'Longest Lived:      ',addrear(base_name(robot[longestlived].name),9),
           '(',cstrr(divide(robot[longestlived].cycles,robot[longestlived].matches),2),' cycles/match)');
 writeln(f,'Shortest Lived:     ',addrear(base_name(robot[shortestlived].name),9),
           '(',cstrr(divide(robot[shortestlived].cycles,robot[shortestlived].matches),2),' cycles/match)');
 if unequal_matches then
  begin
   writeln(f,'Longest Reign:      ',addrear(base_name(robot[longestreign].name),9),
             '(',cstr(robot[longestreign].matches),' matches)');
   writeln(f,'Newest Contender:   ',addrear(base_name(robot[newestcontender].name),9),
             '(',cstr(robot[newestcontender].matches),' matches)');
  end;
 if robot[mosterrors].errors>0 then
 writeln(f,'Most Error-Prone:   ',addrear(base_name(robot[mosterrors].name),9),
           '(',cstrr(divide(robot[mosterrors].errors,robot[mosterrors].matches),3),' errors/match)');
 {--HTML best/worst list--} {err}
 writeln(f2,'<P><CENTER><TABLE border=3 cellpadding=3 cellspacing=0>');
 writeln(f2,'<TR><TD>Best Survivor:      </TD><TD align=right>',base_name(robot[bestsurvivor].name),'    </TD>',
          '<TD>[',cstrr(divide(robot[bestsurvivor].wins,robot[bestsurvivor].matches)*100,3),'% wins]</TD></TR>');
 writeln(f2,'<TR><TD>Biggest Loser:      </TD><TD align=right>',base_name(robot[biggestloser].name),'    </TD>',
          '<TD>[',cstrr(divide(robot[biggestloser].wins,robot[biggestloser].matches)*100,3),'% wins]</TD></TR>');
 writeln(f2,'<TR><TD>Best Killer:        </TD><TD align=right>',base_name(robot[bestkiller].name),'      </TD>',
          '<TD>[',cstrr(divide(robot[bestkiller].kills,robot[bestkiller].matches),3),' kills/match]</TD></TR>');
 writeln(f2,'<TR><TD>Most Dead:          </TD><TD align=right>',base_name(robot[mostdead].name),'        </TD>',
          '<TD>[',cstrr(divide(robot[mostdead].deaths,robot[mostdead].matches),3),' deaths/match]</TD></TR>');
 writeln(f2,'<TR><TD>Most Trigger-Happy: </TD><TD align=right>',base_name(robot[triggerhappy].name),'    </TD>',
          '<TD>[',cstrr(divide(robot[triggerhappy].shots,robot[triggerhappy].matches),2),' shots/match]</TD></TR>');
 writeln(f2,'<TR><TD>Most Conservative:  </TD><TD align=right>',base_name(robot[conservative].name),'    </TD>',
          '<TD>[',cstrr(divide(robot[conservative].shots,robot[conservative].matches),2),' shots/match]</TD></TR>');
 writeln(f2,'<TR><TD>Best Marksman:      </TD><TD align=right>',base_name(robot[bestmarksman].name),'    </TD>',
          '<TD>[',cstrr(divide(robot[bestmarksman].hits,robot[bestmarksman].shots)*100,2),'% hit]</TD></TR>');
 if robot[haphazard].shots>0 then
 writeln(f2,'<TR><TD>Most Haphazard:     </TD><TD align=right>',base_name(robot[haphazard].name),'    </TD>',
          '<TD>[',cstrr(divide(robot[haphazard].hits,robot[haphazard].shots)*100,2),'% hit]</TD></TR>');
 writeln(f2,'<TR><TD>Most Destructive:   </TD><TD align=right>',base_name(robot[mostdestructive].name),' </TD>',
          '<TD>[',cstrr(divide(robot[mostdestructive].damage,robot[mostdestructive].matches),2),' damage/match]</TD></TR>');
 writeln(f2,'<TR><TD>Least Effective:    </TD><TD align=right>',base_name(robot[leasteffective].name),' </TD>',
          '<TD>[',cstrr(divide(robot[leasteffective].damage,robot[leasteffective].matches),2),' damage/match]</TD></TR>');
 writeln(f2,'<TR><TD>Longest Lived:      </TD><TD align=right>',base_name(robot[longestlived].name),'    </TD>',
          '<TD>[',cstrr(divide(robot[longestlived].cycles,robot[longestlived].matches),2),' cycles/match]</TD></TR>');
 writeln(f2,'<TR><TD>Shortest Lived:      </TD><TD align=right>',base_name(robot[shortestlived].name),'    </TD>',
          '<TD>[',cstrr(divide(robot[shortestlived].cycles,robot[shortestlived].matches),2),' cycles/match]</TD></TR>');
 if unequal_matches then
  begin
   writeln(f2,'<TR><TD>Longest Reign:      </TD><TD align=right>',base_name(robot[longestreign].name),'    </TD>',
            '<TD>[',cstr(robot[longestreign].matches),' matches]</TD></TR>');
   writeln(f2,'<TR><TD>Newset Contender:    </TD><TD align=right>',base_name(robot[newestcontender].name),'    </TD>',
            '<TD>[',cstr(robot[newestcontender].matches),' matches]</TD></TR>');
  end;
 if robot[mosterrors].errors>0 then
 writeln(f2,'<TR><TD>Most Error-Prone:   </TD><TD align=right>',base_name(robot[mosterrors].name),'      </TD>',
          '<TD>[',cstrr(divide(robot[mosterrors].errors,robot[mosterrors].matches),3),' errors/match]</TD></TR>');
 writeln(f2,'</TABLE></CENTER><P>');

 {--RES top of table--}
 writeln(f);
 writeln(f,'Rank  Robot           Score     Wins/Matches  Kills/Deaths     Hits/Shots');
 writeln(f,'~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~');
 {--HTML top of table--}
 writeln(f2,'<BR>');
 writeln(f2,'<CENTER><TABLE align=center border=5 cellspacing=0>');
 writeln(f2,'<TR border=1><TD align=center>Num:</TD><TD align=center>Rank</TD><TD align=center>Robot</TD>',
            '<TD align=center>== Score ==</TD><TD align=center>Wins/Matches</TD>',
            '<TD align=center>Hits/Shots</TD><TD align=center>Kills/Deaths</TD></TR>');
 writeln(f2);


 k:=0; score:=0;
 for i:=1 to num_robots do
  with robot[i] do
   begin
    if matches>0 then r:=wins/matches*100 else r:=0;
    if score<>r then begin score:=r; inc(k); end;
    if locked then e:=robot_ext2 else e:=robot_ext;
    writeln(f,addfront(cstr(k),4)+'  '+
              addrear(name+e,12)+addfront(cstrr(r,2)+'%',10)+
              addfront(cstr(wins)+'/'+cstr(matches),16)+
              addfront(cstr(kills)+'/'+cstr(deaths),14)+
              addfront(cstr(hits)+'/'+cstr(shots),15));
    writeln(f2,'<TR border=2><TD align=right>',
              addfront(cstr(i),4),'</TD><TD align=right>',
              addfront(cstr(k),4),'</TD><TD>',
              addrear(name+e,12),'</TD>');
    writeln(f2,'    <TD align=right>',
              addfront(cstrr(r,2)+'%',13),'</TD><TD align=right>',
              addfront(cstr(wins)+'/'+cstr(matches),10),'</TD>');
    writeln(f2,'    <TD align=right>',
              addfront(cstr(hits)+'/'+cstr(shots),13),'</TD><TD align=right>',
              addfront(cstr(kills)+'/'+cstr(deaths),10),'</TD></TR>');
    writeln(f2);
    writeln(f3,wins,' ',matches,' ',kills,' ',deaths,' 0 0 ',shots,' ',
               hits,' ',damage,' ',cycles,' ',errors,' ',name);
   end;


 {--RES file ender--}
 writeln(f);
 writeln(f,'Generated by ',progname,' ',version);
 writeln(f,cnotice1);
 writeln(f,cnotice2);
 {--HTML file ender--}
 writeln(f2,'</TABLE></CENTER>');
 writeln(f2);
 writeln(f2,'<BR>');
 writeln(f2,'<CENTER><P align=center>');
 if not report_only then
   writeln(f2,'<A href="',lcase(no_path(base_name(tname))),
              '.log">Click here for log</A>');
 writeln(f2,'</P></CENTER>');
 writeln(f2);
 writeln(f2,'<BR><BR><HR>');
 writeln(f2,'<CENTER>');
 writeln(f2,'Generated by ',progname,' ',version,'<BR>');
 writeln(f2,cnotice1,'<BR>');
 writeln(f2,cnotice2,'<BR>');
 writeln(f2,'</CENTER>');
 writeln(f2,'</BODY>');
 writeln(f2,'</HTML>');
 close(f);
 close(f2);
 close(f3);
 writeln('Results written to "',fn,'"');
 writeln('HTML copy written to "',fn2,'"');
end;

procedure list_from_report;
var
 i,j,k,l:integer;
 f:text;
 s,fn:string;
begin {err}
 fn:=tname+report_ext;
 if not exist(fn) then exit;
 assign(f,fn);
 reset(f);
 readln(f,num_robots);
 for i:=1 to num_robots do
  with robot[i] do
   begin
    read(f,wins);
    read(f,matches);
    read(f,kills);
    read(f,deaths);
    read(f,j);
    read(f,k);
    read(f,shots);
    read(f,hits);
    read(f,damage);
    read(f,cycles);
    read(f,errors);
    readln(f,s);
    name:=btrim(ucase(s));
    {writeln(name);}
   end;
 close(f);
end;

procedure read_report(fn,rn1,rn2:string);
var
 f:text;
 i,j,k,n,armor,heat:integer;
 r:robot_rec;
 s:string;
begin
 writeln(log_file);
 rn1:=base_name(no_path(rn1));
 rn2:=base_name(no_path(rn2));
 writeln(log_file,addrear(rn1,8),' vs ',rn2,':');
 assign(f,fn); reset(f);
 readln(f,k);
 for i:=1 to k do
  with r do
   begin
    read(f,wins);
    read(f,matches);
    read(f,kills);
    read(f,deaths);
    read(f,armor);
    read(f,heat);
    read(f,shots);
    read(f,hits);
    read(f,damage);
    read(f,cycles);
    read(f,errors);
    readln(f,s);
    name:=btrim(ucase(s));
    n:=0;
    for j:=1 to num_robots do
     if ucase(btrim(robot[j].name))=ucase(btrim(name)) then n:=j;
    if n>0 then
     begin
      inc(robot[n].wins,wins);
      inc(robot[n].matches,matches);
      inc(robot[n].kills,kills);
      inc(robot[n].deaths,deaths);
      inc(robot[n].shots,shots);
      inc(robot[n].hits,hits);
      inc(robot[n].damage,damage);
      inc(robot[n].cycles,cycles);
      inc(robot[n].errors,errors);
      writeln(log_file,addrear(name,9),': ',wins,'/',matches,'   [Kills: ',kills,',  Deaths: ',deaths,',  Shots: ',shots,']');
     end;
    if n=0 then writeln(name,' not found!');
   end;
 close(f);
 writeln('Report read.');
end;

procedure do_matches;
var
 n1,n2,i,j,k:integer;
 c:char;
 f:text;
 s,rn1,rn2:string;
 pairings:longint;
begin
 n1:=1; n2:=1;
 for i:=1 to num_robots do
  with robot[i] do
   begin wins:=0; matches:=0; end;

 if exist(tname+data_ext) then
  writeln('Incomplete tournament found! Tournament will resume!');

 pairings:=0;
 for i:=num_robots-1 downto 1 do pairings:=pairings+i;

 writeln;
 writeln('Robots: ',num_robots,',  Matches per pairing: ',matches);
 writeln('pairings:  ',pairings,',  Total matches: ',matches*pairings);
 writeln; writeln('The matches are about to begin...');
 write('Proceed? [Y/N] ');
 if yes_to_all then c:='Y' else
    repeat c:=upcase(readkey); until c in ['Y','N'];
 writeln(c); writeln;
 if c='N' then shutdown;

 delete_file(tname+list_ext);

 if exist(tname+data_ext) then
  begin
   {--must read data to resume--}
   assign(f,tname+data_ext); reset(f);
   readln(f,n1);
   readln(f,n2);
   readln(f,num_robots);
   for i:=1 to num_robots do
    with robot[i] do
     begin
      read(f,wins);
      read(f,matches);
      readln(f,s);
      name:=ucase(btrim(s));
     end;
   close(f);
  end
 else
  delete_file(tname+log_ext);

 assign(log_file,tname+log_ext);
 if exist(tname+log_ext) then append(log_file) else rewrite(log_file);

 writeln(log_file,'Tournament started: ',time,' ',date);
 writeln(log_file);

 abort:=false;
 while (n1<num_robots) and (not abort) do
  begin
   while (n2<=num_robots) and (not abort) do
    begin
     if n1<>n2 then
      begin
       writeln(repchar('-',79));
       writeln(robot[n1].name,' vs ',robot[n2].name,':');
       {pause;}
       delete_file(main_filename+report_ext);
       swapvectors;
       if robot[n1].locked then rn1:=robot[n1].name+robot_ext2 else rn1:=robot[n1].name;
       if robot[n2].locked then rn2:=robot[n2].name+robot_ext2 else rn2:=robot[n2].name;
       exec(main_filename+prog_ext,param_line+' /R4 /L'+cstr(game_limit)
                                  +' /M'+cstr(matches)+' '
                                  +robot_dir+rn1+' '
                                  +robot_dir+rn2);
       swapvectors;
       if not exist(main_filename+report_ext) then abort:=true
        else read_report(main_filename+report_ext,robot[n1].name,robot[n2].name);
       if sound_on then chirp;
      end;
     if not abort then inc(n2);
    end;
   if not abort then begin inc(n1); n2:=n1; end;
  end;
 if abort then
  begin
   writeln('Tournament aborted!');
   {--must write data for future continuation--}
   assign(f,tname+data_ext); rewrite(f);
   writeln(f,n1);
   writeln(f,n2);
   writeln(f,num_robots);
   for i:=1 to num_robots do
    with robot[i] do
     writeln(f,wins,' ',matches,' ',name);
   close(f);
  end else delete_file(tname+data_ext);
 writeln(log_file);
 writeln(log_file,'Tournament stopped: ',time,' ',date);
 close(log_file);
 if (n1>=num_robots) or (n2>num_robots) then complete:=true else complete:=false;
 if complete then writeln('Tournament complete!');
end;

procedure main;
begin
 if exist(tname+result_ext) then
  begin
   writeln('Result file already exists! Tournament already completed!');
   writeln;
  end
 else
  begin
   abort:=false;
   if not report_only then
    begin
     if not exist(tname+list_ext) then
        make_robot_list else read_robot_list;
     do_matches;
    end else begin
     list_from_report;
     complete:=true;
    end;
   if (not abort) and (complete) then
    begin
     rank_robots;
     write_results;
    end;
  end;
end;

begin
 init;
 main;
 shutdown;
end.

