#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

//#define WIDTH 82
//#define HEIGHT 19

void game(int* score1, int* score2);
void init_ncurses(void);
void end_ncurses(void);
void get_key(int* direction, int* direction2, int prev_dir1, int prev_dir2);
int check_block(int direction, int x, int y, int** screen_buffer);
int change_x(int direction, int x);
int change_y(int direction, int y);
//ssize_t write_log(const char *path);
int HEIGHT = 0;
int WIDTH = 0;

//int log_fd;

int main(void) {
  //log_fd = open("tron_log.txt", O_CREAT | O_WRONLY);
  // write_log("---Start of Tron---\n");
  int score1 = 0;
  int score2 = 0;
  int playing = 1;
  
  init_ncurses();
  //write_log("init completed\n");
  while(playing) {
    game(&score1, &score2);
    erase();
    mvprintw(3,3, "Play again(y/n) ");
    while(1) {
      int key = getch();
      if(key == 'y') {
	erase();
	break;
      }
      if(key == 'n') {
	playing = 0;
	break;
      }
    }
  }
  end_ncurses();
  
  return 0;
}

void game(int* score1, int* score2) {
  getmaxyx(stdscr, HEIGHT, WIDTH);
  
  int* data = calloc(WIDTH*HEIGHT, sizeof(int));
  int** screen_buffer = malloc(HEIGHT*sizeof(int*));
  
  for(int i=0; i<HEIGHT; i++)
  {
    screen_buffer[i] = data + i * WIDTH;
  }

  //write_log("created screen buffer\n");
  
  int winner;
  int player1_x = 3;
  int player1_y = 3;
  int player2_x = WIDTH/2+WIDTH/4-WIDTH/6+WIDTH/8-WIDTH/10+WIDTH/12-WIDTH/14+WIDTH/16-WIDTH/18+WIDTH/20-WIDTH/22+WIDTH/24-WIDTH/26+WIDTH/28;
  int player2_y = 3;
  int alive = 1;
  int counter = 0;
  int direction1 = 0;
  int direction2 = 1;
  int prior_time = 0;
  int prev_dir[2];

  prev_dir[0] = 1;
  prev_dir[1] = 0;
  
  while(alive) {
    get_key(&direction1, &direction2, prev_dir[0], prev_dir[1]);
    if((clock() - prior_time) > 100000) {
      //write_log("inside main game logic (clock - prior_time > 1000000)\n");
      prior_time = clock();
	
      screen_buffer[player1_y][player1_x] = 2;
      screen_buffer[player2_y][player2_x] = 3;

      //write_log("Both players have been given their tail\n");
      
      //write_log("gotten key input");
      if(alive == 1) {
	alive = check_block(direction1, player1_x, player1_y, screen_buffer);
	//write_log("checked whether player 1 is alive\n");
	
	if(alive != 1) {
	  winner = 2;
	  *score2 += 1;
	  break;
	}
	
	alive = check_block(direction2, player2_x, player2_y, screen_buffer);
	//	write_log("checked whether player 2 is alive\n");

	if(alive != 1) {
	  *score1 += 1;
	  winner = 1;
	  break;
	}

	player1_x = change_x(direction1, player1_x);
	player1_y = change_y(direction1, player1_y);
	player2_x = change_x(direction2, player2_x);
	player2_y = change_y(direction2, player2_y);
	//write_log("changed player 1 and 2 x and y positions\n");

      }

      //printing stuff
      for(int i = 0, y = 0;  y < HEIGHT; i++) {
	if(screen_buffer[y][i] != 0) {
	  attron(COLOR_PAIR(screen_buffer[y][i]));
	  mvprintw(y,i, "#");
	}
	if(i == WIDTH) {
	  y++;
	  i = 0;
	}
      }

      //  write_log("printed the screen\n");
      
      if(alive == 1) {
	attron(COLOR_PAIR(2));
	mvprintw(player1_y,player1_x, "@");
	attroff(COLOR_PAIR(2));
	attron(COLOR_PAIR(3));
	mvprintw(player2_y,player2_x, "@");
	attroff(COLOR_PAIR(3));
	attron(COLOR_PAIR(4));
	for(int i = 0; i < WIDTH-1; i++) {
	  mvprintw(0,i, "+");
	}
	for(int i = 0; i < HEIGHT-1; i++) {
	  mvprintw(i,0, "+");
	  mvprintw(i,WIDTH-1, "+");
	}
	for(int i = 0; i <= WIDTH-1; i++) {
	  mvprintw(HEIGHT-1,i, "+");
	}
	attroff(COLOR_PAIR(4));
	refresh();
      }
      //  mvprintw(0,0, "1x:%d 1y:%d 2x:%d 2y:%d", player1_x, player1_y, player2_x,
      //       player2_x);
      prev_dir[0] = direction1;
      prev_dir[1] = direction2;
    }
    //        get_key(&direction1, &direction2);
  }

  //  usleep(3000000);
  erase();
  if(winner == 1) {
    mvprintw(4,3, "####     ##     #     #   #   #   #   ####");
    mvprintw(5,3, "#  #    # #     #     #   #   ##  #  #");
    mvprintw(6,3, "####      #     #  #  #   #   # # #   ###");
    mvprintw(7,3, "#         #     #  #  #   #   #  ##      #");
    mvprintw(8,3, "#       #####    ## ##    #   #   #  #### ");
  }

  if(winner == 2) {
    mvprintw(4,3, "####     ###    #     #   #   #   #   ####");
    mvprintw(5,3, "#  #    #   #   #     #   #   ##  #  #");
    mvprintw(6,3, "####      #     #  #  #   #   # # #   ###");
    mvprintw(7,3, "#        #      #  #  #   #   #  ##      #");
    mvprintw(8,3, "#       ####     ## ##    #   #   #  #### ");
  }
  
  mvprintw(0,3, "P1 score: %d", *score1);
  mvprintw(2,3, "P2 score: %d", *score2);

  mvprintw(10,3, "(Press any key to escape)");
  
  refresh();

  usleep(1000000);

  while(1) {
    int key = getch();
    if(key != -1) {
      break;
    }
  }
  free(screen_buffer);
  free(data);
}

void init_ncurses(void) {
  initscr();
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_GREEN);
  init_pair(3, COLOR_BLACK, COLOR_CYAN);
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);
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

void get_key(int* direction, int* direction2, int prev_dir1, int prev_dir2) {
  int key = getch();

  if(key == KEY_RIGHT && *direction != 1 && prev_dir1 != 1) {
    *direction = 0;
  }
  if(key == KEY_LEFT && *direction != 0 && prev_dir1 != 0) {
    *direction = 1;
  }
  if(key == KEY_UP && *direction != 3 && prev_dir1 != 3) {
    *direction = 2;
  }
  if(key == KEY_DOWN && *direction != 2 && prev_dir1 != 2) {
    *direction = 3;
  }
  if((key == 'd' || key == 'D') && *direction2 != 1 && prev_dir2 != 1) {
    *direction2 = 0;
  }
  if((key == 'a' || key == 'A') && *direction2 != 0 && prev_dir2 != 0) {
    *direction2 = 1;
  }
  if((key == 'w' || key == 'W') && *direction2 != 3 && prev_dir2 != 3) {
    *direction2 = 2;
  }
  if((key == 's' || key == 'S') && *direction2 != 2 && prev_dir2 != 2) {
    *direction2 = 3;
  }
}


int check_block(int direction, int x, int y, int** screen_buffer) {
  if(x >= WIDTH - 1 || x <= 0) {
    return 0;
  }
  if(y >= HEIGHT - 1 || y <= 0) {
    return 0;
  }
  if(direction == 0 && screen_buffer[y][x+1] != 0) {
    return 0;
  }
  else if(direction == 1 && screen_buffer[y][x-1] != 0) {
    return 0;
  }
  else if(direction == 2 && screen_buffer[y-1][x] != 0) {
    return 0;
  }
  else if(direction == 3 && screen_buffer[y+1][x] != 0) {
    return 0;
  }
  else {
    return 1;
  }
}


int change_x(int direction, int x) {
  if(direction == 0) {
    x++;
  }
  if(direction == 1) {
    x--;
  }
  return x;
}

int change_y(int direction, int y) {
  if(direction == 2) {
    y--;
  }
  if(direction == 3) {
    y++;
  }
  return y;
}

// exploitable if improperly path is not c string (does not have null
// terminator)
//ssize_t write_log(const char *path) {
//  return write(log_fd, path, strlen(path));/
//}
