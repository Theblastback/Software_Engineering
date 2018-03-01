unit atr2func;

(*
Copyright (c) 1999, Ed T. Toton III. All rights reserved.

Bug fixes and additional additions Copyright (c) 2014, William "Amos" Confer

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

Interface

uses dos, crt, filelib, myfile, graph;

var
 delay_per_sec:longint;
 registered,graphix,sound_on:boolean;
 reg_name:string;
 reg_num:word;
 sint,cost:array[0..255] of real;

procedure textxy(x,y:integer; s:string);
procedure coltextxy(x,y:integer; s:string; c:byte);
function hexnum(num:byte):char;
function hexb(num:byte):string;
function hex(num:word):string;
function valuer(i:string):real;
function value(i:string):longint;
function cstrr(i:real):string;
function cstr(i:longint):string;
function zero_pad(n,l:longint):string;
function zero_pads(s:string; l:longint):string;
Function addfront(b:string;l:integer): string;
Function addrear(b:string;l:integer): string;
Function ucase(s:string):string;
Function lcase(s:string):string;
Function space(i:byte):string;
Function repchar(c:char; i:byte):string;
function ltrim(s1:string):string;
function rtrim(s1:string):string;
function btrim(s1:string):string;
function lstr(s1:string; l:integer):string;
function rstr(s1:string; l:integer):string;
Procedure FlushKey;
procedure calibrate_timing;
procedure time_delay(n:integer); {n=milliseconds}
procedure check_registration;
function rol(n,k:integer):integer;
function ror(n,k:integer):integer;
function sal(n,k:integer):integer;
function sar(n,k:integer):integer;
procedure viewport(x1,y1,x2,y2:integer);
procedure main_viewport;
procedure make_tables;
function robot_color(n:integer):integer;
procedure box(x1,y1,x2,y2:integer);
procedure hole(x1,y1,x2,y2:integer);
procedure chirp;
procedure click;
function hex2int(s:String):integer;
function str2int(s:String):integer;
function distance(x1,y1,x2,y2:real):real;
function find_angle(xx,yy,tx,ty:real):real;
function find_anglei(xx,yy,tx,ty:real):integer;
{FIFI}
function bin(n:integer):string;
function decimal(num,length:integer):string;
{/FIFI}


Implementation

procedure textxy(x,y:integer; s:string);
begin
 setfillstyle(1,0);
 bar(x,y,x+length(s)*8,y+7);
 outtextxy(x,y,s);
end;

procedure coltextxy(x,y:integer; s:string; c:byte);
begin
 setcolor(c);
 textxy(x,y,s);
end;

function hexnum(num:byte):char;
begin
 case num of
  0 : hexnum:='0';
  1 : hexnum:='1';
  2 : hexnum:='2';
  3 : hexnum:='3';
  4 : hexnum:='4';
  5 : hexnum:='5';
  6 : hexnum:='6';
  7 : hexnum:='7';
  8 : hexnum:='8';
  9 : hexnum:='9';
 10 : hexnum:='A';
 11 : hexnum:='B';
 12 : hexnum:='C';
 13 : hexnum:='D';
 14 : hexnum:='E';
 15 : hexnum:='F';
 else hexnum:='X';
 end; {case}
end;

function hexb(num:byte):string;
begin
 hexb:=hexnum(num shr 4)+hexnum(num and 15);
end;

function hex(num:word):string;
begin
 hex:=hexb(num shr 8)+hexb(num and 255);
end;


function valuer(i:string):real;
var
 s:real;
 n:integer;
begin
 val(i,s,n);
 if (n>0) then s:=0;
 valuer:=s;
end;

function value(i:string):longint;
var
 s:longint;
 n:integer;
begin
 val(i,s,n);
 if (n>0) then s:=0;
 value:=s;
end;

function cstrr(i:real):string;
var
 s1:string[255];
begin
 str(i,s1);
 cstrr:=s1;
end;

function cstr(i:longint):string;
var
 s1:string[255];
begin
 str(i,s1);
 cstr:=s1;
end;

function zero_pad(n,l:longint):string;
var
 s:string;
begin
 s:=cstr(n);
 while length(s)<l do
  s:='0'+s;
 zero_pad:=s;
end;

function zero_pads(s:string;l:longint):string;
var
 s1:string;
begin
 s1:=s;
 while length(s1)<l do
  s1:='0'+s1;
 zero_pads:=s1;
end;


Function addfront(b:string;l:integer): string;
Begin
  while length(b)< l do
    b := ' '+b;
  addfront := b;
End;

Function addrear(b:string;l:integer): string;
Begin
  while length(b)< l do
    b := b+' ';
  addrear := b;
End;

Function ucase(s:string):string;
var
 i:integer;
begin
 if length(s)>=1 then
  begin
   for i:=1 to length(s) do
    s[i]:=upcase(s[i]);
  end;
 ucase:=s;
end;

Function lcase(s:string):string;
var
 i:integer;
begin
 if length(s)>=1 then
  begin
   for i:=1 to length(s) do
    if (ord(s[i])>=65) and (ord(s[i])<=90) then s[i]:=chr(ord(s[i])+32);
  end;
 lcase:=s;
end;


Function space(i:byte):string;
var
 s:string[255];
 k:integer;
begin
 s:='';
 if i>0 then for k:=1 to i do s:=s+' ';
 space:=s;
end;

Function repchar(c:char; i:byte):string;
var
 s:string[255];
 k:integer;
begin
 s:='';
 if i>0 then
  for k:=1 to i do
   begin
    s:=s+c;
   end;
 repchar:=s;
end;


function ltrim(s1:string):string;
var
 i:integer;
begin
 while (length(s1)>0) and ((copy(s1,1,1)=' ') or (copy(s1,1,1)=#8) or (copy(s1,1,1)=#9)) do
  begin
   s1:=copy(s1,2,length(s1)-1);
  end;
 ltrim:=s1;
end;

function rtrim(s1:string):string;
var
 i:integer;
begin
 while (length(s1)>0) and ((copy(s1,length(s1),1)=' ') or (copy(s1,length(s1),1)=#8)
         or (copy(s1,length(s1),1)=#9)) do
  begin
   s1:=copy(s1,1,length(s1)-1);
  end;
 rtrim:=s1;
end;

function btrim(s1:string):string;
begin
 btrim:=ltrim(rtrim(s1));
end;

function lstr(s1:string; l:integer):string;
begin
 if length(s1)<=l then lstr:=s1
 else lstr:=copy(s1,1,l);
end;

function rstr(s1:string; l:integer):string;
begin
 if length(s1)<=l then rstr:=s1
 else rstr:=copy(s1,length(s1)-l+1,l);
end;

Procedure FlushKey;              { Clears any key strokes in the key-  }
                                 { board buffer so a couple of key     }
var                              { presses don't race you through program. }
  Regs : Registers;

begin
  Regs.AH := $01;  { AH=1: Check for keystroke }
  Intr($16,regs); { Interupt $16: Keyboard services}
  IF (regs.Flags and $0040) = 0 then { if chars in buffer }
    REPEAT
      Regs.AH := 0;
      Intr($16,Regs);
      Regs.AH := $01;
      Intr($16,Regs);
    Until (regs.flags and $0040) <> 0;
end;


procedure calibrate_timing;
var
 i,k:longint;
begin
 delay_per_sec:=0;
 k:=mem[0:$46C];
 repeat until k<>mem[0:$46C];
 k:=mem[0:$46C];
 repeat delay(1); inc(delay_per_sec); until k<>mem[0:$46C];
 delay_per_sec:=round(delay_per_sec*18.2);
end;

procedure time_delay(n:integer); {n=milliseconds}
var
 i,l:longint;
begin
 if delay_per_sec=0 then calibrate_timing;
 l:=round(n/1000*delay_per_sec);
 for i:=1 to l do delay(1);
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


function rol(n,k:integer):integer;
begin
 asm
  cld
  mov   cx,     k
  rep   rol     n,      1
 end;
 rol:=n;
end;

function ror(n,k:integer):integer;
begin
 asm
  cld
  mov   cx,     k
  rep   ror     n,      1
 end;
 ror:=n;
end;

function sal(n,k:integer):integer;
begin
 asm
  cld
  mov   cx,     k
  cmp   cx,     0
  jle   @2
@1:
  sal   n,      1
  loop  @1
@2:
 end;
 sal:=n;
end;

function sar(n,k:integer):integer;
begin
 asm
  cld
  mov   cx,     k
  cmp   cx,     0
  jle   @2
@1:
  sar   n,      1
  loop  @1
@2:
 end;
 sar:=n;
end;

procedure viewport(x1,y1,x2,y2:integer);
begin
 if not graphix then exit;
 setviewport(x1,y1,x2,y2,true);
end;

procedure main_viewport;
begin
 viewport(5,5,474,474); {470x470}
end;

procedure make_tables;
var
 i,j,k:integer;
begin
 for i:=0 to 255 do
  begin
   sint[i]:=sin(i/128*pi);
   cost[i]:=cos(i/128*pi);
  end;
end;

function robot_color(n:integer):integer;
var
 k:integer;
begin
 k:=7;
 case n mod 14 of
  0:k:=10;
  1:k:=12;
  2:k:=09;
  3:k:=11;
  4:k:=13;
  5:k:=14;
  6:k:=7;
  7:k:=6;
  8:k:=2;
  9:k:=4;
  10:k:=1;
  11:k:=3;
  12:k:=5;
  13:k:=15;
  else k:=15;
 end;
 robot_color:=k;
end;


procedure box(x1,y1,x2,y2:integer);
var
 i:integer;
begin
 if not graphix then exit;
 if x2<x1 then begin i:=x1; x1:=x2; x2:=i; end;
 if y2<y1 then begin i:=y1; y1:=y2; y2:=i; end;
 setfillstyle(1,7);
 setcolor(7);
 bar(x1,y1,x2,y2);
 setcolor(15);
 line(x1,y1,x2-1,y1);
 line(x1,y1,x1,y2-1);
 setcolor(8);
 line(x1+1,y2,x2,y2);
 line(x2,y1+1,x2,y2);
end;

procedure hole(x1,y1,x2,y2:integer);
var
 i:integer;
begin
 if not graphix then exit;
 if x2<x1 then begin i:=x1; x1:=x2; x2:=i; end;
 if y2<y1 then begin i:=y1; y1:=y2; y2:=i; end;
 setfillstyle(1,0);
 setcolor(0);
 bar(x1,y1,x2,y2);
 setcolor(8);
 line(x1,y1,x2-1,y1);
 line(x1,y1,x1,y2-1);
 setcolor(15);
 line(x1+1,y2,x2,y2);
 line(x2,y1+1,x2,y2);
 putpixel(x1,y2,7);
 putpixel(x2,y1,7);
end;

procedure chirp;
begin
 if (not sound_on) or (not graphix) then exit;
 sound(250);
 delay(10);
 sound(1000);
 delay(10);
 sound(500);
 delay(10);
 sound(1000);
 delay(10);
 nosound;
end;

procedure click;
begin
 if (not sound_on) or (not graphix) then exit;
 sound(250);
 delay(3);
 sound(1000);
 delay(3);
 sound(500);
 delay(3);
 nosound;
end;

function hex2int(s:String):integer;
var
 w:word;
 i,j:integer;
begin
 i:=0; w:=0;
 while i<length(s) do
  begin
   inc(i);
   case s[i] of
    '0':begin w:=(w shl 4) or $0; end;
    '1':begin w:=(w shl 4) or $1; end;
    '2':begin w:=(w shl 4) or $2; end;
    '3':begin w:=(w shl 4) or $3; end;
    '4':begin w:=(w shl 4) or $4; end;
    '5':begin w:=(w shl 4) or $5; end;
    '6':begin w:=(w shl 4) or $6; end;
    '7':begin w:=(w shl 4) or $7; end;
    '8':begin w:=(w shl 4) or $8; end;
    '9':begin w:=(w shl 4) or $9; end;
    'A':begin w:=(w shl 4) or $A; end;
    'B':begin w:=(w shl 4) or $B; end;
    'C':begin w:=(w shl 4) or $C; end;
    'D':begin w:=(w shl 4) or $D; end;
    'E':begin w:=(w shl 4) or $E; end;
    'F':begin w:=(w shl 4) or $F; end;
    else i:=length(s);
   end;
  end;
 hex2int:=w;
end;

function str2int(s:string):integer;
var
 i,j,k:longint;
 neg:boolean;
begin
 neg:=false;
 s:=btrim(ucase(s));
 if s='' then k:=0 else
  begin
   if s[1]='-' then begin neg:=true; s:=rstr(s,length(s)-1); end;
   k:=0;
   if lstr(s,2)='0X' then
     k:=hex2int(rstr(s,length(s)-2))
   else if rstr(s,1)='H' then
     k:=hex2int(lstr(s,length(s)-1))
   else k:=value(s);
   if neg then k:=0-k;
  end;
 str2int:=k;
end;

function distance(x1,y1,x2,y2:real):real;
begin
 distance:=abs(sqrt(sqr(y1-y2)+sqr(x1-x2)));
end;

function find_angle(xx,yy,tx,ty:real):real;
var
 i,j,k,v,z:integer;
 q:real;
begin
 q:=0;
    v:=abs(round(tx-xx));
    if v=0 then      {v:=0.001;}
     begin
      if (tx=xx) and (ty>yy) then q:=pi;
      if (tx=xx) and (ty<yy) then q:=0;
     end
    else
     begin
      z:=abs(round(ty-yy));
      q:=abs(arctan(z/v));
      if (tx>xx) and (ty>yy) then q:=pi/2+q;
      if (tx>xx) and (ty<yy) then q:=pi/2-q;
      if (tx<xx) and (ty<yy) then q:=pi+pi/2+q;
      if (tx<xx) and (ty>yy) then q:=pi+pi/2-q;
      if (tx=xx) and (ty>yy) then q:=pi/2;
      if (tx=xx) and (ty<yy) then q:=0;
      if (tx<xx) and (ty=yy) then q:=pi+pi/2;
      if (tx>xx) and (ty=yy) then q:=pi/2;
     end;
 find_angle:=q;
end;

function find_anglei(xx,yy,tx,ty:real):integer;
var
 i:integer;
begin
 i:=round(find_angle(xx,yy,tx,ty)/pi*128+256);
 while (i<0) do
   inc(i,256);
 i:=i and 255;
 find_anglei:=i;
end;

{FIFI}
function bin(n:integer):string;
var
 i:integer;
 bin_string:string;
begin
 bin_string:='';
 for i:=0 to 15 do
  begin
   if (n mod 2) = 0 then
    bin_string:= '0' + bin_string
   else bin_string:= '1' + bin_string;
   n:=n div 2;
  end;
 bin:=bin_string;
end;
{/FIFI}

{FIFI}
function decimal(num,length:integer):string;
   {this can also be acheived by zero_pad(num,length);}
var
 dec_string:string;
 i:integer;
begin
 dec_string:='';
 for i:=1 to length do
  begin
   dec_string:=chr((num mod 10)+48) + dec_string;
   num:=num div 10;
  end;
 decimal:=dec_string;
end;
{/FIFI}

end.