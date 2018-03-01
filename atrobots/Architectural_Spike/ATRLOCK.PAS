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

{ This program encrypts the robot programs for distribution }

{$G+}{$N+}{$E+}{$X+}{$D-}
{$M 32768,0,655360}
program atrlock;

uses stuff, filelib;

const
 locktype = 3;

var
 fn1,fn2:string;
 f1,f2:text;
 s,s1,s2,lock_code:string;
 i,j,k,lock_pos,lock_dat,this_dat:integer;

function encode(s:string):string;
var
 i,j,k:integer;
begin
 {k:=0;}
 if lock_code<>'' then
  for i:=1 to length(s) do
   begin
    inc(lock_pos); if lock_pos>length(lock_code) then lock_pos:=1;
    if ord(s[i]) in [0..31,128..255] then s[i]:=' ';
    this_dat:=ord(s[i]) and 15;
    s[i]:=char((ord(s[i]) xor (ord(lock_code[lock_pos]) xor lock_dat))+1);
    lock_dat:=this_dat;
   end;
 encode:=s;
end;

function prepare(s,s1:string):string;
var
 i,j,k,l:integer;
 s2:string;
begin
   {--remove comments--}
   if (length(s1)=0) or (s1[1]=';') then s1:=''
    else
     begin
      k:=0;
      for i:=length(s1) downto 1 do
         if s1[i]=';' then k:=i;
      if k>0 then s1:=lstr(s1,k-1);
     end;

   {--remove excess spaces--}
   s2:='';
   for i:=1 to length(s1) do
    if not (s1[i] in [' ',#8,#9,#10,',']) then s2:=s2+s1[i]
     else begin if s2<>'' then s:=s+s2+' '; s2:=''; end;

   if s2<>'' then s:=s+s2;

   prepare:=s;
end;

procedure write_line(s,s1:string);
begin
   s:=prepare(s,s1);

   {--write line!--}
   if length(s)>0 then
    begin
     s:=encode(s);
     writeln(f2,s);
    end;
end;

begin
 randomize;
 lock_pos:=0;
 lock_dat:=0;
 if (paramcount<1) or (paramcount>2) then
  begin
   writeln('Usage: ATRLOCK <robot[.at2]> [locked[.atl]]');
   halt;
  end;
 fn1:=btrim(ucase(paramstr(1)));
 if fn1=base_name(fn1) then fn1:=fn1+'.AT2';
 if not exist(fn1) then
  begin writeln('Robot "',fn1,'" not found!'); halt; end;
 if paramcount=2 then
    fn2:=btrim(ucase(paramstr(2))) else fn2:=base_name(fn1)+'.ATL';
 if fn2=base_name(fn2) then fn2:=fn2+'.ATL';
 if not valid(fn2) then
  begin writeln('Output name "',fn1,'" not valid!'); halt; end;
 if fn1=fn2 then
  begin writeln('Filenames can not be the same!'); halt; end;
 assign(f1,fn1); reset(f1);
 assign(f2,fn2); rewrite(f2);

 {--copy comment header--}
 writeln(f2,';------------------------------------------------------------------------------');
 s:='';
 while (not eof(f1)) and (s='') do
  begin
   readln(f1,s);
   s:=btrim(s);
   if s[1]=';' then begin writeln(f2,s); s:=''; end;
  end;

 {--lock header--}
 writeln(f2,';------------------------------------------------------------------------------');
 writeln(f2,'; ',no_path(base_name(fn1)),' Locked on ',date);
 writeln(f2,';------------------------------------------------------------------------------');
 lock_code:='';
 k:=random(21)+20;
 for i:=1 to k do
  lock_code:=lock_code+char(random(32)+65);
 writeln(f2,'#LOCK',locktype,' ',lock_code);

 {--decode lock-code--}
 for i:=1 to length(lock_code) do
  lock_code[i]:=char(ord(lock_code[i])-65);

 write('Encoding "',fn1,'"...');

 {--encode robot--}
 s:=btrim(s);
 if length(s)>0 then
  write_line('',ucase(s));
 while not eof(f1) do
  begin
   {--read line!--}
   readln(f1,s1); s:='';
   s1:=btrim(ucase(s1));

   {--write line!--}
   write_line(s,s1);
  end;
 writeln('Done. Used LOCK Format #',locktype,'.');
 writeln('Only ATR2 v2.08 or later can decode.');
 writeln('LOCKed robot saved as "',fn2,'"');

 close(f1); close(f2);
end.

