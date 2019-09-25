#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define WIDTH 82
#define HEIGHT 19

void game(void);
void init_ncurses(void);
void end_ncurses(void);
int get_key(int direction);
int check_block(int direction, int x, int y, int screen_buffer[HEIGHT][WIDTH]);

int main(void) {

  init_ncurses();

  game();
  
  end_ncurses();
  return 0;
}

void game(void) {

  int screen_buffer[HEIGHT][WIDTH];

  int x = 0;
  
  for(int i = 0; x <= HEIGHT; i++) {
    if(i == (WIDTH)) {
      x++;
      i = 0;
    }
    if(x == (HEIGHT)) {
      break;
    }
    screen_buffer[x][i] = 0;
  }
  
  int player_x = 3;
  int player_y = 3;
  int alive = 1;
  int counter = 0;
  int death_counter = 0;
  int direction = 0;
  int y;
  
  while(alive) {
    screen_buffer[player_y][player_x] = 1;

    direction = get_key(direction);

    if(direction == 0) {
      alive = check_block(direction, player_x, player_y, screen_buffer);
      
      usleep(100000);
      player_x++;
    }
    else if(direction == 1) {
      alive = check_block(direction, player_x, player_y, screen_buffer);
      
      usleep(100000);
      player_x--;
    }
    else if(direction == 2) {
      alive = check_block(direction, player_x, player_y, screen_buffer);
      
      usleep(130000);
      player_y--;
    }
    else if(direction == 3) {
      alive = check_block(direction, player_x, player_y, screen_buffer);
      
      usleep(130000);
      player_y++;
    }

    y = 0;

    for(int i = 0;  y < HEIGHT; i++) {
      if(i == WIDTH) {
	y++;
	i = 0;
      }
      if(y == HEIGHT) {
	break;
      }
      if(screen_buffer[y][i] == 1) {
	mvprintw(y+1,i+1, "#");
      }
    }

    if(alive == 1) {
      attron(COLOR_PAIR(2));
      mvprintw(player_y+1,player_x+1, "@");
      attroff(COLOR_PAIR(2));
      mvprintw(0,0, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    for(int i = 1; i < HEIGHT+2; i++) {
      mvprintw(i,0, "+");
      mvprintw(i,83, "+");
    }
    mvprintw(20,0, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
      refresh();
    }
  } 
}

void init_ncurses(void) {
  initscr();
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_GREEN);
  clear();
  keypad(stdscr, TRUE);
  noecho();
  nodelay(stdscr, TRUE);
  curs_set(0);
}

void end_ncurses(void) {
  curs_set(1);
  echo();
  endwin();
}

int get_key(int direction) {
  int key = getch();

  if(key == KEY_RIGHT && direction != 1) {
    direction = 0;
  }
  else if(key == KEY_LEFT && direction != 0) {
    direction = 1;
  }
  else if(key == KEY_UP && direction != 3) {
    direction = 2;
  }
  else if(key == KEY_DOWN && direction != 2) {
    direction = 3;
  }
  
  return direction;
}

int check_block(int direction, int x, int y, int screen_buffer[HEIGHT][WIDTH]) {
  if(x >= WIDTH || x < 0) {
    return 0;
  }
  if(y >= HEIGHT || y < 0) {
    return 0;
  }
  if(direction == 0 && screen_buffer[y][x+1] == 1) {
    return 0;
  }
  else if(direction == 1 && screen_buffer[y][x-1] == 1) {
    return 0;
  }
  else if(direction == 2 && screen_buffer[y-1][x] == 1) {
    return 0;
  }
  else if(direction == 3 && screen_buffer[y+1][x] == 1) {
    return 0;
  }
  else {
    return 1;
  }
}
