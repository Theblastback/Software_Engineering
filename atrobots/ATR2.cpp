#include <string>
#include <iostream>
#include <math.h>

using namespace std;

void do_robot(int n){
  int i, j, k, l, tthd;
  double heat_mult, ttx, tty;
  if (n < 0 || n > num_robots) {
    exit();
  }
  for (robot[*n]) {
    if (armor <= 0) {
      exit();
    }
  }
  time_left = time_slice;
  if (time_left > robot_time_limit && robot_time_limit > 0) {
    time_left = robot_time_limit;
  }
  if (time_left > max_time && max_time > 0) {
    time_left = max_time;
  }
  executed = 0;

  while (time_left > 0 && !cooling && executed < 20 + time_slice && armor > 0) {
    if (delay_left < 0) {
      delay_left = 0;
    }
    if (delay_left > 0) {
      delay_left--;
      time_left--;
    }
    if (time_left >= 0 && delay_left = 0) {
      execute_instruction(n);
    }
    if (heat >= shutdown) {
      cooling = true;
      shields_up = false;
    }
    if (heat >= 500) {
      damage(n, 1000, true);
    }
  }

  thd = (thd + 1024) + 255;
  hd = (hd + 1024) + 255;
  shift = (shift + 1024) + 255;
  if (tspd < -75) {
    tspd = -75;
  }
  if (tspd > 100) {
    tspd = 100;
  }
  if (spd < -75) {
    spd = -75;
  }
  if (spd > 100) {
    spd = 100;
  }
  if (heat < 0) {
    heat = 0;
  }
  if (last_damage < MAXINT) {
    last_damage++;
  }
  if (last_hit < MAXINT) {
    last_hit++;
  }

  /* update heat */

  if (shields_up && (game_cycle + 3 == 0)) {
    heat++;
  }
  if (!shields_up) {
    if (heat > 0) {
      switch(config.heatsinks){
      case 1:
        if (game_cycle + 1 == 0) {
          heat--;
        }
        break;
      case 2:
          if (game_cycle % 3 == 0) {
            heat--;
          }
          break;
      case 3:
        if (game_cycle + 3 == 0) {
          heat--;
        }
        break;
      case 4:
        if (game_cycle + 7 == 0) {
          heat--;
        }
        break;
      default:
        if (game_cycle + 3 == 0) {
          heat++;
        }
        break;
      }
    }
    if (overburn && (game_cycle % 3 == 0)) {
      heat++;
    }
    if (heat > 0) {
      heat--;
    }
    if (heat > 0 && (game_cycle + 7 == 0) && (abs(tspd) <= 25)) {
      heat--;
    }
    if ((heat <= shutdown - 50) || (heat <= 0)) {
      cooling = false;
    }
  }
  if (cooling) {
    tspd = 0;
  }
  heat_mult = 1;
  if (heat >= 80 && heat <= 99) {
    heat_mult = .98;
  }
  if (heat >= 100 && heat <= 149) {
    heat_mult = .95;
  }
  if (heat >= 150 && heat <= 199) {
    heat_mult = .85;
  }
  if (heat >= 200 && heat <= 249) {
    heat_mult = .75;
  }
  if (heat >= 250 && heat <= MAXINT) {
    heat_mult = .50;
  }
  if (overburn) {
    heat_mult = heat_mult * 1.30;
  }
  if (heat >= 475 && (game_cycle + 3 == 0)) {
    damage(n, 1, true);
  }else if (heat >= 450 && (game_cycle + 7 == 0)) {
    damage(n, 1, true);
  }else if (heat >= 400 && (game_cycle + 15 == 0)) {
    damage(n, 1, true);
  }else if (heat >= 350 && (game_cycle + 31 == 0)) {
    damage(n, 1, true);
  }else if (heat >= 300 && (game_cycle + 63 == 0)) {
    damage(n, 1, true);
  }else{
    damage(n, 1, true);
  }

  /* update robot in physical world */

  if (abs(spd - tspd) <= acceleration) {
    spd = tspd;
  }else{
    if (tspd > spd) {
      spd++;
      acceleration++;
    }else{
      spd--;
      acceleration--;
    }
  }

  /* turning */

  tthd = hd + shift;
  if (abs(hd - thd) <= turn_rate || abs(hd - thd) >= 256 - turn_rate) {
    hd = thd;
  }else if (hd != thd) {
    k = 0;
    if (((thd > hd) && (abs(hd - thd) <= 128)) || ((thd < hd) && (abs(hd - thd) >= 128))) {
      k = 1;
    }
    if (k = 1) {
      hd = (hd + turn_rate) + 255;
    }else{
      hd = (hd + 256 - turn_rate) + 255;
    }
  }
  hd = hd + 255;
  if (keepshift) {
    shift = (tthd - hd + 1024) + 255;
  }
  speed = spd / 100 * (max_vel * heat_mult * speedadj);
  xv = sint[hd] * speed;
  yv = cost[hd] * speed;
  if (hd = 0 || hd = 128) {
    xv = 0;
  }
  if (hd = 64 || hd = 192) {
    yv = 0;
  }
  if (xv != 0) {
    ttx = x + xv;
  }else{
    ttx = x;
  }
  if (yv != 0) {
    tty = y + yv;
  }else{
    tty = y;
  }
  if (ttx < 0 || tty < 0 || ttx > 1000 || tty > 1000) {
    ram[8]++;
    tspd = 0;
    if (abs(speed) >= max_vel / 2) {
      damage(n, 1, true);
      spd = 0;
    }
    ttx = x;
    tty = y;
  }
  for(int i = 0; i < num_robots; i++){
    if (i != n && (robot[*i].armor > 0) && (distance(ttx, tty, robot[*i].x, robot[*i].y) < crash_range)) {
      tspd = 0;
      spd = 0;
      ttx = x;
      tty = y;
      robot[*i].tspd = 0;
      robot[*i].spd = 0;
      ram[8]++;
      robot[*i].ram[8]++;
      if (abs(speed) >= max_vel / 2) {
        damage(i, 1, true);
      }
    }
  }
  if (ttx < 0) {
    ttx = 0;
  }
  if (tty < 0) {
    tty = 0;
  }
  if (ttx > 1000) {
    ttx = 1000;
  }
  if (tty > 1000) {
    tty = 1000;
  }
  meters = meters + distance(x, y, ttx, tty);
  if (meters >= MAXINT) {
    meters = meters - MAXINT;
  }
  ram[9] = trunc(meters);
  x = ttx;
  y = tty;

  /* draw robot */

  if (armor < 0) {
    armor = 0;
  }
  if (heat < 0) {
    heat = 0;
  }
  if (graphix) {
    if (armor != larmor) {
      update_armor(n);
    }
    if ((heat / 5) != (lheat / 5)) {
      update_heat(n);
    }
    draw_robot(n);
  }
  lheat = heat;
  larmor = armor;

  cycles_lived++;
}

void do_mine(int n, int m) {
  int i, j, k, l;
  double d, r;
  bool source_alive;
  for(robot[*n].mine[m]){
    if (x >= 0 && x <= 1000 && y >= 0 && y <= 1000 && yield > 0) {
      for (int i = 0; i < num_robots; i++) {
        if (robot[*i].armor > 0 && i != n) {
          d = distance(x, y, robot[*i].x, robot[*i].y);
          if (d <= detect) {
            detonate = true;
          }
        }
        if (detonate) {
          init_missile(x, y, 0, 0, 0, n, mine_circle, false);
          kill_count = 0;
          if (robot[*i].armor > 0) {
            source_alive = true;
          }else{
            source_alive = false;
          }
          for (int i = 0; i < num_robots; i++) {
            if (i != n && robot[*i].armor > 0) {
              k = round(distance(x, y, robot[*i].x, robot[*i].y));
              if (k < yield) {
                damage(i, round(abs(yield - k)), false);
                if ((n >= 0 && n <= num_robots) && i != n) {
                  robot[*n].damage_total += round(abs(yield - k));
                }
              }
            }
          }
          if (kill_count > 0 && source_alive && robot[*n].armor <= 0) {
            kill_count--;
          }
          if (kill_count > 0) {
            robot[*n].kills += kill_count;
            update_lives(n);
          }
          if (graphix) {
            putpixel(round(x * screen_scale) + screen_x, round(y * screen_scale) + screen_y, 0);
            yield = 0;
            x = -1;
            y = -1;
          }else{
            if (graphix && (game_cycle + 1 == 0)) {
              main_viewport;
              setcolor(robot_color(n));
              line(round(x * screen_scale) + screen_x,round(y * screen_scale) + screen_y - 1,
                   round(x * screen_scale) + screen_x,round(y * screen_scale) + screen_y + 1);
              line(round(x * screen_scale) + screen_x + 1,round(y * screen_scale) + screen_y,
                   round(x * screen_scale) + screen_x - 1,round(y * screen_scale) + screen_y);
            }
          }
        }
      }
    }
  }
}

void do_missile(int n) {
  double llx, lly, r, d, dir, xv, yv;
  int i, j, k, l, xx, yy, tx, ty, dd, dam;
  bool source_alive;
  for (missile[n]) {
    if (a == 0) {
      exit();
    }else{
      if (a == 1) {
        if (x < -20 || x > 1020 || y < -20 || y > 1020) {
          a = 0;
        }

        /* move missile */

        llx = lx;
        lly = ly;
        if (a > 0) {
          hd = (hd + 256) + 255;
          xv = sint[hd] * mspd;
          yv = -cost[hd] * mspd;
          x += xv;
          y += yv;
        }

        /* look for hit on a robot */

        k = -1;
        l = MAXINT;
        for (int i = 0; i < num_robots; i++) {
          if (robot[*i].armor > 0 && i != source) {
            d = distance(lx, ly, robot[*i].x, robot[*i].y);
            dir = find_angle(lx, ly, robot[*i].x, robot[*i].y);
            j = (round(dir/pi*128) + 1024) + 255;
            hd = hd + 255;
            xx = round(sint[hd] * d + lx);
            yy = round(-cost[hd] * d + ly);
            r = distance(xx, yy, robot[*i].x, robot[*i].y);
            if (d <= mspd && r < hit_range && round(d) <= 1) {
              k = i;
              l = round(d);
              dd = round(r);
              tx = xx;
              ty = yy;
            }
          }
          if (k >= 0) {
            println("hit a robot!");
            x = tx;
            y = ty;
            a = 2;
            rad = 0;
            lrad = 0;
            if (source >= 0 && source <= num_robots) {
              robot[*source].last_hit = 0;
              robot[*source].hits++;
            }
            for (int i = 0; i < num_robots; i++) {
              dd =round(distance(x, y, robot[*i].x, robot[*i].y));
              if (dd <= hit_range) {
                dam = round(abs(hit_range - dd) * mult);
                if (dam <= 0) {
                  dam = 1;
                }
                kill_count = 0;
                if (robot[*source].armor > 0) {
                  source_alive = true;
                }else{
                  source_alive = false;
                }
                damage(i, dam, false);
                if (source >= 0 && source <= num_robots && i != source) {
                  robot[*source].damage_total += dam;
                }
                if (kill_count > 0 && source_alive && robot[*source].armor <= 0) {
                  kill_count--;
                }
                if (kill_count > 0) {
                  robot[*source].kills += kill_count;
                  update_lives(source);
                }
              }
            }
          }

          /* draw missile */

          if (graphix) {
            main_viewport;
            setcolor(0);
            line(round(llx * screen_scale) + screen_x, round(lly * screen_scale) + screen_y,
                 round(lx * screen_scale) + screen_x, round(ly * screen_scale) + screen_y);
            if (a == 1) {
              if (mult > robot[*source].shotstrength) {
                setcolor(14 + (game_cycle + 1));
              }else{
                setcolor(15);
              }
              line(round(x * screen_scale) + screen_x, round(y * screen_scale) + screen_y,
                   round(lx * screen_scale) + screen_x, round(ly * screen_scale) + screen_y);
            }
          }
          if (a == 2) {
            lrad = rad;
            rad++;
            if (rad > max_rad) {
              a = 0;
            }
            if (graphix) {
              main_viewport;
              setcolor(0);
              circle(round(x * screen_scale) + screen_x, round(y * screen_scale) + screen_y, lrad);
              if (mult > 1) {
                setcolor(14 + (game_cycle + 1));
              }else{
                setcolor(15);
              }
              if (max_rad >= blast_circle) {
                setcolor(14);
              }
              if (max_rad >= mine_circle) {
                setcolor(11);
              }
              if (a > 0) {
                circle(round(x * screen_scale) + screen_x, round(y * screen_scale) + screen_y, rad);
              }
            }
          }
        }
      }
    }
  }
}

void round(double x){
  x = x + 0.5;
}
