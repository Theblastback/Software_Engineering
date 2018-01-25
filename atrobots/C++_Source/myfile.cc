#include "Headers/myfile.h"
#include "Headers/filelib.h"


string lstr(string s1, short l) {
	if ( s1.length() <= l )
		return(s1);
	else
		return( copy(s1,1,l) );
}

short file_size(string fn) {
	if ( !exist(fn) )
		return(-1);

	return(file_length(fn));
}

void delete_file(string fn) {
	if ( !exist(fn) )
		return;

	remove(fm.c_str());
}

void rename_file(string fn, string fn2) {
	if ( !exist(fn) )
		return;

	std::rename(f.c_str(), fn2.c_str());
}

void open_filein(unsigned short h, string fn) {
	unsigned short w;

	the_name = fn + "\0";

    /*
!!!!!!!!!!!!!!!!!!!!!
     Assembly in C++
        Registries (The variables after instruction)
        have a percent sign next to them. Other than that,
        I think it's pretty straight forward.

        https://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
         - This article helps with it.
!!!!!!!!!!!!!!!!!!!!!
*/

    __asm__("push %eds"
            "mov  %dx,      seg the_name"
            "mov  %ds,      %dx"
            "mov  %dx,      offset the_name"
            "inc  %dx"
            "mov  %ax,      %3D00h;"
            "int  %21h"
            "pop  %ds"
            "mov  w,       %ax");

	h = w;
}

void open_fileboth(unsigned short h, string fn) {
	unsigned short w;

	the_name = fn + "\0";

	__asm__("push  %ds"
		"mov   %dx,     seg the_name"
		"mov   %ds,     %dx"
		"mov   %dx,     offset the_name"
		"inc   %dx"
		"mov   %ax,     %3D02h;"
		"int  %21h"
		"pop   %ds"
		"mov   w,      %ax");

	h = w;
}

void open_fileout(unsigned short h, string fn) {
	unsigned short w;

	the_name =fn + "\0";

	if ( exist( lstr(fn, fn.length()-1) ) ) {
		__asm__("push  %ds"
			"mov   %dx,     seg the_name"
			"mov   %ds,     %dx"
			"mov   %dx,     offset the_name"
			"inc   %dx"
			"mov   %ax,     %3D01h;"
			"int   %21h"
			"pop   %ds"
			"mov   w,      %ax");

	} else {
		__asm__("push  %ds"
			"mov   %dx,     seg the_name"
			"mov   %ds,     %dx"
			"mov   %dx,     offset the_name"
			"inc   %dx"
			"mov   %cx,     %20h"
			"mov   %ax,     %3C00h;"
			"int   %21h"
			"pop   %ds"
			"mov   w,      %ax");
	}
	h = w;
}

void create_fileout(unsigned short h; string fn);
	unsigned short w;

	the_name = fn + "\0";

	__asm__("push  %ds"
		"mov   %dx,     seg the_name"
		"mov   %ds,     %dx"
		"mov   %dx,     offset the_name"
		"inc   %dx"
		"mov   %cx,     %20h"
		"mov   %ax,     %3C00h;"
		"int   %21h"
		"pop   %ds"
		"mov   w,      %ax");

	h = w;
}


void read_file(unsigned short h, void * ploc, short len) {
	unsigned short tseg, tofs, pp, w;
	short ll;
//	label ok, uh_oh, alright;

	tseg = seg(ploc *);
	tofs = ofs(ploc *);
 ll:=len; w:=0;

    __asm__("push  %ds"
            "mov   %bx,     h"
            "mov   %cx,     ll"
            "mov   %dx,     tseg"
            "mov   %ds,     %dx"
            "mov   %dx,     tofs"
            "mov   %ax,     %3F00h;"
            "int   %21h"
            "jc    uh_oh"
            "jmp   alright"
            "uh_oh:"
            "mov   w,      %ax"
            "alright:"
            "pop   %ds"
            "cmp   ll,     %ax"
            "je    ok"
            "mov   %the_eof, $1"
            "ok:"
            "mov   ll,     %ax");

 len:=ll;
 if w<>0 then begin writeln(' ****** ',w,' ****** '); {pausescr;} end;
end;

// data type word == unsigned short

Procedure write_file(h:word; ploc:pointer; var len:integer);
var
 tseg,tofs:word;
 ll:integer;
begin
 tseg:=seg(ploc^);
 tofs:=ofs(ploc^);
 ll:=len;
    __asm__("push  ds"

            "mov   %bx,     h"
            "mov   %cx,     ll"
            "mov   %dx,     tseg"
            "mov   %ds,     %dx"
            "mov   %dx,     tofs"
            "mov   %ax,     %4000h;"
            "int   %21h"
            "pop   %ds"
            "mov   ll,     %ax");
 len:=ll;
end;

Procedure read_long(h:word; ploc:pointer; var len:word);
var
 tseg,tofs,pp,w:word;
 ll:word;
begin
 tseg:=seg(ploc^);
 tofs:=ofs(ploc^);
 ll:=len; w:=0;

    __asm__("push  %ds"
            "mov   %bx,     h"
            "mov   %cx,     ll"
            "mov   %dx,     tseg"
            "mov   %ds,     %dx"
            "mov   %dx,     tofs"
            "mov   %ax,     %3F00h;"
            "int   %21h"
            "jc    uh_oh"
            "jmp   alright"
            "uh_oh:"
            "mov   %w,      %ax"
            "alright:"
            "pop   %ds"
            "cmp   ll,     %ax"
            "je    ok"
            "mov   %the_eof,$1"
            "ok:"
            "mov   ll,     %ax"
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
    __asm__("push  %ds"
            "mov   %bx,     h"
            "mov   %cx,     ll"
            "mov   %dx,     tseg"
            "mov   %ds,     %dx"
            "mov   %dx,     tofs"
            "mov   %ax,     %4000h;"
            "int   %21h"
            "pop   %ds"
            "mov   ll,     %ax");
 len:=ll;
end;

procedure close_file(h:word);
begin
    __asm__("mov   %ax,     %3E00h"
            "mov   %bx,     h"
            "int   %21h");
 end;
end;

Procedure file_seek(h,d:word; m:byte; var p:word);
var
 w:word;
begin

    __asm__("mov   %bx,     h"
            "mov   %cx,     %0"
            "mov   %dx,     d"
            "mov   %al,     m"
            "mov   %ah,     %42h"
            "int   %21h"
            "mov   w,      %ax");
 p:=w;
end;

Procedure long_seek(h:word; d:longint; m:byte; var p:longint);
var
 d1,d2:word;
begin
 d1:=d shr 16;
 d2:=d and $FFFF;

    __asm__("mov   %bx,     h"
            "mov   %cx,     d1"
            "mov   %dx,     d2"
            "mov   %al,     m"
            "mov   %ah,     %42h"
            "int   %21h"
            "mov   d1,      %ax"
            "mov   d2,      %dx");

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
