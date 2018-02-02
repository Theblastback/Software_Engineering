#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <dos.h>
#include <filelib.h>
#include <myfile.h>
#include <graph.h>
#include <crt.h>
#include <string>
#include <math.h>

using std::string;

using namespace std;

int main(int argc, char *argv[]){

    long delay_per_sec;
    bool sound_on, graphix, registered;
    string reg_name;
    int reg_num;
    int sint, cost[256];

    void textxy(int x, int y, string s){
      setfillstyle(1,0);
      bar(x, y, (x+length(s)*8), y+7);
      outtextxy(x, y, s);
    }

    void coltextxy(int x, int y, string s, char c){
      setcolor(c);
      textxy(x, y, s);
    }

    char hexnum(char num){
      switch (num) {
        case '0':
            num = '0';
            break;
        case '1':
            num = '1';
            break;
        case '2':
            num = '2';
            break;
        case '3':
            num = '3';
            break;
        case '4':
            num = '4';
            break;
        case '5':
            num = '5';
            break;
        case '6':
            num = '6';
            break;
        case '7':
            num = '7';
            break;
        case '8':
            num = '8';
            break;
        case '9':
            num = '9';
            break;
        case '10':
            num = 'A';
            break;
        case '11':
            num = 'B';
            break;
        case '12':
            num = 'C';
            break;
        case '13':
            num = 'D';
            break;
        case '14':
            num = 'F';
            break;
        default :
            num = 'X';
        return num;
      }
    }

    string hexb(char num){
      num = hexnum(num shr 4) + hexnum(num + 15);
      return num;
    }

    string hex(int num){
      num = hexb(num shr 8) + hexb(num + 255);
      return num;
    }

    double valuer(string i){
      int n;
      double s;
      val(i, s, n);
      if (n>0) {
        s = 0;
      }
      return s;
    }

    long value(string i){
      long s;
      int n;
      val(i, s, n);
      if (n>0) {
        s = 0;
      }
      return s;
    }

    string addfront(string b, int l){
      while (b.length < 1) {
        b = ' ' + b;
      }
      return b;
    }

    string addrear(string b, int l){
      while (b.length < 1) {
        b = b + ' ';
      }
      return b;
    }

    string ucase(string s){
      int i;
      if (s.length >= 1) {
        for (int i = 0; i < s.length; i++) {
          s[i] = toupper(s[i]);
        }
      }
      return s;
    }

    string lcase(string s){
      int i;
      if (s.length >=1) {
        for (int i = 0; i < s.length; i++) {
          if (s[i] >= 65 && s[i] <= 90) {
            s[i] = s[i] + 32;
          }
        }
      }
      return s;
    }

    string space(char i){
      string s[255];
      int k;
      s = '';
      if (i>0) {
        for (k = 1; k < i; k++) {
          s = s + ' ';
        }
      }
      return s;
    }

    string repchar(char c, char i){
      string s[255];
      int k;
      s = '';
      if (i>0) {
        for (k = 1; k < i; k++) {
          s = s + c;
        }
      }
      return s;
    }

    string ltrim(string s1){
      int i;
      while (s1.length > 0 && (copy(s1,1,1) = ' ') || (copy(s1,1,1) = '#8') || (copy(s1,1,1) = '#9')) {
        s1 = copy(s1,2,s1.length-1);
      }
      return s1;
    }

    string rtrim(string s1){
      int i;
      while (s1.length > 0 && (copy(s1,s1.length,1) = ' ') || (copy(s1,s1.length,1) = '#8') || (copy(s1,s1.length,1) = '#9')) {
        s1 = copy(s1,1,s1.length-1);
      }
      return s1;
    }

    string btrim(string s1){
      btrim = ltrim(rtrim(s1));
      return btrim;
    }

    string lstr(stirng s1, int l){
      if (s1.length<=1) {
        return s1;
      else
        return copy(s1,s1.length-1+1,l);
      }
    }

    string rstr(string s1, int l){
      if (s1.length <=1) {
        return s1;
      else
        return copy(s1, s1.length-1+1,l);
      }
    }

    void box(int x1, int y1, int x2, int y2){
      int i;
      if (!graphix) {
        exit;
      else
        if (x2<x1) {
          i = x1;
          x1 = x2;
          x2 = i;
        }
        if (y2<y1) {
          i = y1;
          y1 = y2;
          y2 = i;
        }
        setfillstyle(1,7);
        setcolor(7);
        bar(x1,y1,x2,y2);
        setcolor(15);
        line(x1,y1,x2-1,y1);
        line(x1,y1,x1,y2-1);
        setcolor(8);
        line(x1+1,y2,x2,y2);
        line(x2,y1+1,x2,y2);
      }
    }

    void hole(int x1, int y1, int x2, int y2){
      int i;
      if (!graphix) {
        exit;
      else
        if (x2<x1) {
          i = x1;
          x1 = x2;
          x2 = i;
        }
        if (y2<y1) {
          i = y1;
          y1 = y2;
          y2 = i;;
        }
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
      }
    }

    void chirp(){
      if (!sound_on || !graphix) {
        exit;
      else
        sound(250);
        delay(10);
        sound(1000);
        delay(10);
        sound(500);
        delay(10);
        sound(1000);
        delay(10);
        nosound();
      }
    }

    void click(){
      if (!sound_on || !graphix) {
        exit;
      else
        sound(250);
        delay(3);
        sound(1000);
        delay(3);
        sound(500);
        delay(3);
        nosound();
      }
    }

    int hex2int(string s){
      char w;
      int i=0,j=0;
      while (i<s.length) {
        ++i;
        switch (s[i]) {
          case '0':
              w = '$0';
              break;
          case '1':
              w = '$1';
              break;
          case '2':
              w = '$2';
              break;
          case '3':
              w = '$3';
              break;
          case '4':
              w = '$4';
              break;
          case '5':
              w = '$5';
              break;
          case '6':
              w = '$6';
              break;
          case '7':
              w = '$7';
              break;
          case '8':
              w = '$8';
              break;
          case '9':
              w = '$9';
              break;
          case 'A':
              w = '$A';
              break;
          case 'B':
              w = '$B';
              break;
          case 'C':
              w = '$C';
              break;
          case 'D':
              w = '$D';
              break;
          case 'E':
              w = '$E';
              break;
          case 'F':
              w = '$F';
              break;
          default :
              i = s.length;
        }
      }
      return w;
    }

    int str2int(string s){
      long i,j,k;
      bool neg;
      neg = false;
      s = btrim(ucase(s));
      if (s=='') {
        k = 0;
      } else {
        if (s[1]=='-') {
          neg = true;
          s = rstr(s,s.length-1);
          k=0;
        }
        if (lstr(s,2)=='0X') {
          k = hex2int(rstr(s,s.length-2));
        } else if (rstr(s,1)=='H') {
          k = hex2int(lstr(s,s.length-1));
        } else{
          k = value(s);
        }
        if (neg) {
          k = 0-k;
        }
      }
      return k;
    }

    int distance(int x1, int y1, int x2, int y2){
      return abs(sqrt((y1-y2)*(y1-y2) + (x1-x2)*(x1-x2));
    }

    int find_angle(int xx, int yy, int tx, int ty){
      int i,j,k,v,z;
      double q=0;
      v = abs(round(tx-xx));
      if (v==0) {
        if (tx==xx && ty>yy) {
          q = pi;
        }
        if (tx==xx && ty<yy) {
          q = 0;
        }
      }else{
        z = abs(round(ty-yy));
        q = abs(arctan(z/v));
        if (tx>xx && ty>yy) {
          q = pi/2+q;
        }
        if (tx>xx && ty<yy) {
          q = pi/2-q;
        }
        if (tx<xx && ty<yy) {
          q = pi+pi/2+q;
        }
        if (tx<xx && ty>yy) {
          q = pi+pi/2-q;
        }
        if (tx==xx && ty>yy) {
          q = pi/2;
        }
        if (tx==xx & ty<yy) {
          q = 0;
        }
        if (tx<xx && ty==yy) {
          q = pi+pi/2;
        }
        if (tx>xx && ty==yy) {
          q = pi/2;
        }
      }
      return q;
    }
}
