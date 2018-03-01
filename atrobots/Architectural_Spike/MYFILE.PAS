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

{ This unit contains functions useful for handling files, including
  renaming, deleting, and accessing in high-speed variable-length
  random access. }


{$G+}{$N+}{$E+}{$X+}{$D-}
Unit myfile;

interface

uses filelib;

var
 the_name:string;
 the_eof:byte;

function  file_size(fn:string):longint;
procedure delete_file(fn:string);
procedure rename_file(fn,fn2:string);
Procedure open_filein(var h:word; fn:string);
Procedure open_fileout(var h:word; fn:string);
Procedure open_fileboth(var h:word; fn:string);
Procedure create_fileout(var h:word; fn:string);
Procedure read_file(h:word; ploc:pointer; var len:integer);
Procedure write_file(h:word; ploc:pointer; var len:integer);
procedure close_file(h:word);
Procedure file_seek(h,d:word; m:byte; var p:word);
Procedure long_seek(h:word; d:longint; m:byte; var p:longint);
Procedure read_long(h:word; ploc:pointer; var len:word);
Procedure write_long(h:word; ploc:pointer; var len:word);


implementation


function lstr(s1:string; l:integer):string;
begin
 if length(s1)<=l then lstr:=s1
 else lstr:=copy(s1,1,l);
end;

function file_size(fn:string):longint;
var
 f:file of byte;
begin
 if not exist(fn) then
  begin file_size:=-1; exit; end;
 assign(f,fn);
 reset(f);
 file_size:=filesize(f);
 close(f);
end;

procedure delete_file(fn:string);
var
 f:file of byte;
begin
 if not exist(fn) then exit;
 assign(f,fn);
 erase(f);
end;

procedure rename_file(fn,fn2:string);
var
 f:file of byte;
begin
 if not exist(fn) then exit;
 assign(f,fn);
 rename(f,fn2);
end;

Procedure open_filein(var h:word; fn:string);
var
 w:word;
begin
 the_name:=fn+#0;
 asm
  push  ds
  mov   dx,     seg the_name
  mov   ds,     dx
  mov   dx,     offset the_name
  inc   dx
  mov   ax,     3D00h;
  int   21h
  pop   ds
  mov   w,      ax
 end;
 h:=w;
end;

Procedure open_fileboth(var h:word; fn:string);
var
 w:word;
begin
 the_name:=fn+#0;
 asm
  push  ds
  mov   dx,     seg the_name
  mov   ds,     dx
  mov   dx,     offset the_name
  inc   dx
  mov   ax,     3D02h;
  int   21h
  pop   ds
  mov   w,      ax
 end;
 h:=w;
end;

Procedure open_fileout(var h:word; fn:string);
var
 w:word;
begin
 the_name:=fn+#0;
 if exist(lstr(fn,length(fn)-1)) then
 asm
  push  ds
  mov   dx,     seg the_name
  mov   ds,     dx
  mov   dx,     offset the_name
  inc   dx
  mov   ax,     3D01h;
  int   21h
  pop   ds
  mov   w,      ax
 end
 else
 asm
  push  ds
  mov   dx,     seg the_name
  mov   ds,     dx
  mov   dx,     offset the_name
  inc   dx
  mov   cx,     20h
  mov   ax,     3C00h;
  int   21h
  pop   ds
  mov   w,      ax
 end;
 h:=w;
end;

Procedure create_fileout(var h:word; fn:string);
var
 w:word;
begin
 the_name:=fn+#0;
 asm
  push  ds
  mov   dx,     seg the_name
  mov   ds,     dx
  mov   dx,     offset the_name
  inc   dx
  mov   cx,     20h
  mov   ax,     3C00h;
  int   21h
  pop   ds
  mov   w,      ax
 end;
 h:=w;
end;


Procedure read_file(h:word; ploc:pointer; var len:integer);
var
 tseg,tofs,pp,w:word;
 ll:integer;
label ok,uh_oh,alright;
begin
 tseg:=seg(ploc^);
 tofs:=ofs(ploc^);
 ll:=len; w:=0;
 asm
  push  ds
  mov   bx,     h
  mov   cx,     ll
  mov   dx,     tseg
  mov   ds,     dx
  mov   dx,     tofs
  mov   ax,     3F00h;
  int   21h
  jc    uh_oh
  jmp   alright
  uh_oh:
  mov   w,      ax
  alright:
  pop   ds
  cmp   ll,     ax
  je    ok
  mov   the_eof,1
  ok:
  mov   ll,     ax
 end;
 len:=ll;
 if w<>0 then begin writeln(' ****** ',w,' ****** '); {pausescr;} end;
end;

Procedure write_file(h:word; ploc:pointer; var len:integer);
var
 tseg,tofs:word;
 ll:integer;
begin
 tseg:=seg(ploc^);
 tofs:=ofs(ploc^);
 ll:=len;
 asm
  push  ds
  mov   bx,     h
  mov   cx,     ll
  mov   dx,     tseg
  mov   ds,     dx
  mov   dx,     tofs
  mov   ax,     4000h;
  int   21h
  pop   ds
  mov   ll,     ax
 end;
 len:=ll;
end;

Procedure read_long(h:word; ploc:pointer; var len:word);
var
 tseg,tofs,pp,w:word;
 ll:word;
label ok,uh_oh,alright;
begin
 tseg:=seg(ploc^);
 tofs:=ofs(ploc^);
 ll:=len; w:=0;
 asm
  push  ds
  mov   bx,     h
  mov   cx,     ll
  mov   dx,     tseg
  mov   ds,     dx
  mov   dx,     tofs
  mov   ax,     3F00h;
  int   21h
  jc    uh_oh
  jmp   alright
  uh_oh:
  mov   w,      ax
  alright:
  pop   ds
  cmp   ll,     ax
  je    ok
  mov   the_eof,1
  ok:
  mov   ll,     ax
 end;
 len:=ll;
 if w<>0 then begin writeln(' ****** ',w,' ****** '); {pausescr;} end;
end;

Procedure write_long(h:word; ploc:pointer; var len:word);
var
 tseg,tofs:word;
 ll:word;
begin
 tseg:=seg(ploc^);
 tofs:=ofs(ploc^);
 ll:=len;
 asm
  push  ds
  mov   bx,     h
  mov   cx,     ll
  mov   dx,     tseg
  mov   ds,     dx
  mov   dx,     tofs
  mov   ax,     4000h;
  int   21h
  pop   ds
  mov   ll,     ax
 end;
 len:=ll;
end;

procedure close_file(h:word);
begin
 asm
  mov   ax,     3E00h
  mov   bx,     h
  int   21h
 end;
end;

Procedure file_seek(h,d:word; m:byte; var p:word);
var
 w:word;
begin
 asm
  mov   bx,     h
  mov   cx,     0
  mov   dx,     d
  mov   al,     m
  mov   ah,     42h
  int   21h
  mov   w,      ax
 end;
 p:=w;
end;

Procedure long_seek(h:word; d:longint; m:byte; var p:longint);
var
 d1,d2:word;
begin
 d1:=d shr 16;
 d2:=d and $FFFF;
 asm
  mov   bx,     h
  mov   cx,     d1
  mov   dx,     d2
  mov   al,     m
  mov   ah,     42h
  int   21h
  mov   d1,      ax
  mov   d2,      dx
 end;
 p:=d2; p:=p shl 16;
 p:=p or d1;
end;

{procedure read_long(var h:word; p:pointer; var l:longint);
var
 len:longint;
 k:integer;
begin
 l:=len;
 if len<=0 then exit;
 if len>65535 then len:=65535;
 if len<=32000 then
  begin
   k:=len;
   read_file(h,p,k);
   l:=k;
  end
 else
  begin
   k:=32000;
   read_file(h,p,k);
   if k<32000 then l:=k else
    begin
     k:=len-32000;
     read_file(h,ptr(seg(p^),ofs(p^)+32000),k);
     l:=k+32000;
    end;
  end;
end;

procedure write_long(var h:word; p:pointer; var l:longint);
var
 len:longint;
 k:integer;
begin
 l:=len;
 if len<=0 then exit;
 if len>65535 then len:=65535;
 if len<=32000 then
  begin
   k:=len;
   write_file(h,p,k);
   l:=k;
  end
 else
  begin
   k:=32000;
   write_file(h,p,k);
   if k<32000 then l:=k else
    begin
     k:=len-32000;
     write_file(h,ptr(seg(p^),ofs(p^)+32000),k);
     l:=k+32000;
    end;
  end;
end;}

begin
 the_eof:=0;
end.
