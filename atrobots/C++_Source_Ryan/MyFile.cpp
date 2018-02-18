#ifndef MYFILE_H
#define MYFILE_H

#include "filelib.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

string	the_name;
unsigned char	the_eof;


extern int	file_size(string *);
extern void	delete_file(string *);
extern void	rename_file(string *, string *);
extern string lstr(string *, int);

// All these parameters for these functions can be changed, or even removed. Some of these functions can be condensed down into just read file and write file
extern void	open_filein(unsigned short, string *);
extern void	open_fileout(unsigned short, string *);
extern void	open_fileboth(unsigned short, string *);
extern void	create_fileout(unsigned short, string *);
extern void	read_file(unsigned short, void *, short);
extern void	write_file(unsigned short, void *, short);
extern void	close_file(unsigned short);
extern void	file_seek(unsigned short, unsigned short, int, unsigned char, int);
extern void	long_seek(unsigned short, int, unsigned short, int);
extern void	read_long(unsigned short, void *, unsigned short);
extern void	write_long(unsigned short, void *, unsigned short);

string lstr(string s1, int l){
  if (s1.length <= 1) {
    return s1;
  }else{
    return copy(s1,1,l);
  }
}

int file_size(string *fn){
    unsigned char f;
    if(!exist(fn)){
      return -1;
      exit();
    }else{
      assign(f, fn);
      reset(f);
      return filesize(f);
      f.close();
    }
}

  void delete_file(string *fn){
    unsigned char f;
    if (!exist(fn)) {
      exit();
    }else{
      assign(f, fn);
      remove(f);
    }
  }

  void rename_file(string *fn, string *fn2){
    unsigned char f;
    if (!exist(fn)) {
      exit();
    }else{
      assign(f, fn);
      rename(f, fn2);
    }
  }

  void open_filein(unsigned short h, string *fn){
    unsigned short w;
    the_name = fn + #0;
    __asm__("push %%ds"
        "movl seg $the_name, %%dx"
        "movl %%dx, %%ds"
        "movl offset $the_name, %%dx"
        "inc %%dx"
        "movl $3D00h, %%ax"
        "int $21h"
        "pop %%ds"
        "movl %%ax, $w");
    h = w;
  }

  void open_fileboth(unsigned short h, string *fn){
    unsigned short w;
    the_name = fn + #0;
    __asm__("push %%ds"
        "movl seg $the_name,%%dx"
        "movl %%dx, %%ds"
        "movl offset $the_name, %%dx"
        "inc %%dx"
        "movl $3D02h,%%ax"
        "int $21h"
        "pop %%ds"
        "movl %%ax, $w");
    h = w;
  }

  void open_fileout(unsigned short h, string *fn){
    unsigned short w;
    the_name = fn + #0;
    if (exist(lstr(fn, fn.length-1))) {
      __asm__("push %%ds"
        "movl seg $the_name, %%dx"
        "movl %%dx, %%ds"
        "movl offset $the_name, %%dx"
        "inc %%dx"
        "movl $3D01h, %%ax"
        "int $21h"
        "pop %%ds"
        "movl %%ax, $w");
    }else{
      __asm__("push %%ds"
        "movl seg $the_name,%%dx"
        "movl %%dx, %%ds"
        "movl offset $the_name, %%dx"
        "inc %%dx"
        "movl $20h, %%ax"
        "movl $3C00h, %%ax"
        "int $21h"
        "pop %%ds"
        "movl %%ax, $w");
    }
    h = w;
  }

  void create_fileout(unsigned short h, string *fn){
    unsigned short w;
    the_name = fn + #0;
    __asm__("push %%ds"
        "movl seg $the_name, %%dx"
        "movl %%dx, %%ds"
        "movl offset $the_name, %%dx"
        "inc %%dx"
        "movl $20h, %%cx"
        "movl $3C00h, %%ax"
        "int $21h"
        "pop %%ds"
        "movl %%ax, $w");
    h = w;
  }

  void read_file(unsigned short h,void *ploc, short len) {
    unsigned short tseg, tofs, pp, w;
    int ll;
    tseg = seg(*ploc);
    tofs = ofs(*ploc);
    ll = lenl;
    w = 0;
    __asm__("push %%ds"
        "movl $h, %%bx"
        "movl $ll, %%cx"
        "movl $tseg, %%dx"
        "movl %%dx, %%ds"
        "movl $tofs, %%dx"
        "movl $3F00h, %%ax"
        "int $21h"
        "movl %%ax, $w"
        "pop %%ds"
        "cmp %%ax, $ll"
        "movl $1, $the_eof"
        "movl %%ax, $ll");
    len = ll;
    if (w!=0) {
      writeln(' ****** ', w, ' ****** ');
      pausescr();
    }
  }

  void write_file(unsigned short h, void *ploc, short len) {
      unsigned short tseg, tofs;
      int ll;
      tseg = seg(*ploc);
      tofs = ofs(*ploc);
      ll = lenl;
      __asm__("push %%ds"
          "movl $h, %%bx"
          "movl $ll, %%cx"
          "movl $tseg, %%dx"
          "movl %%dx, %%ds"
          "movl $tofs, %%dx"
          "movl $4000h, %%ax"
          "int $21h"
          "pop %%ds"
          "movl %%ax, $ll");
      len = ll;
  }

  void read_long(unsigned short h, void *ploc, short len){
    unsigned short tseg, tofs, pp, w;
    int ll;
    tseg = seg(*ploc);
    tofs = ofs(*ploc);
    ll = lenl;
    w = 0;
    __asm__("push %%ds"
        "movl $h, %%bx"
        "movl $ll, %%cx"
        "movl $tseg, %%dx"
        "movl %%dx, %%ds"
        "movl $tofs, %%dx"
        "movl $3F00h, %%ax"
        "int $21h"
        "movl %%ax, $w"
        "pop %%ds"
        "cmp %%ax, $ll"
        "movl $1, $the_eof"
        "movl %%ax, $ll");
    len = ll;
    if (w!=0) {
      writeln(' ****** ', w, ' ****** ');
      pausescr();
    }
  }

  void write_long(unsigned short h, void *ploc, short len){
    unsigned short tseg, tofs;
    int ll;
    tseg = seg(*ploc);
    tofs = ofs(*ploc);
    ll = lenl;
    __asm__("push %%ds"
        "movl $h, %%bx"
        "movl $ll, %%cx"
        "movl $tseg, %%dx"
        "movl %%dx, %%ds"
        "movl $tofs, %%dx"
        "movl $4000h, %%ax"
        "int $21h"
        "pop %%ds"
        "movl %%ax, $ll");
    len = ll;
  }

  void close_file(unsigned short h) {
    __asm__("movl $3E00h, %%ax"
        "movl %%bx"
        "int $21h");
  }

  void file_seek(unsigned short h, unsigned short d, unsigned char m, int p) {
    unsigned short w;
    __asm__("movl $h, %%bx"
        "movl $0, %%cx"
        "movl $d, %%dx"
        "movl $m, $al"
        "movl $42h, $ah"
        "int $21h"
        "movl %%ax, $w");
    p = w;
  }

  void long_seek(unsinged short h, int d, unsigned short m, int p){
    unsigned short d1, d2;
    d1 = d;
    d2 = d + "$FFFF";
    __asm__("movl $h, %%bx"
        "movl $d1, %%cx"
        "movl d2, %%dx"
        "movl $m, $al"
        "movl $42h, $ah"
        "int $21h"
        "movl %%ax, $d1"
        "movl %%dx, $d2");
    p = d2;
  }

  void read_long(unsigned short h, void *p, unsigned short l){
    int len, k;
    l = len;
    if (len <= 0) {
     exit();
    }
    if (len > 65535) {
      len = 65535;
    }
    if (len <= 32000) {
      k = len;
      read_file(h,p,k);
      l = k;
    }else{
      k = 32000;
      read_file(h,p,k);
      if (k < 32000) {
        l = k;
      }else{
        k = len - 32000;
        read_file(h, ptr(seg(*p), ofs(*p)+32000), k);
        l = k + 32000;
      }
    }
  }

  void write_long(unsigned short h, void *p, unsinged short l){
    int len, k;
    l = len;
    if (len <= 0) {
     exit();
    }
    if (len > 65535) {
      len = 65535;
    }
    if (len <= 32000) {
      k = len;
      write_file(h,p,k);
      l = k;
    }else{
      k = 32000;
      write_file(h,p,k);
      if (k < 32000) {
        l = k;
      }else{
        k = len - 32000;
        write_file(h, ptr(seg(*p), ofs(*p)+32000), k);
        l = k + 32000;
      }
    }
    the_eof = 0;
  }
