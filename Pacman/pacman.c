#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define NROWS 50 // lines
#define NCOLS 50 // columns

#define FRAME_DELAY_US 150000 // 150ms

// Global variables
static int x = 2;
static int y = 2;
static int *ptr_x = &x;
static int *ptr_y = &y;
static char state_move = ' ';
static int life = 3;
static int score = 0;
static int sig_game_over = 0;
static char grid_map[NROWS][NCOLS];
static char grid_wall[NROWS][NCOLS];
static char grid_pellet[NROWS][NCOLS];

// Prototypes
void init_pellets();
void show_map(int pos_x_fruits, int pos_y_fruits, int pos_x_enemy_1, int pos_y_enemy_1);
void clean_screen();
int is_blocked(int i, int j);
void move_pacman();
void move_enemy(int *x_enemy, int *y_enemy);
void eat(int *x_fruits, int *y_fruits);
void game_over_check();

int main() {
	/* ncurses console */
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	/* ncurses start color */
	start_color();

	/* randomize once */
	srand(time(NULL));
	int pos_x_fruits = (rand() % (NROWS - 2)) + 1;
	int pos_y_fruits = (rand() % (NCOLS - 2)) + 1;
	int pos_x_enemy_1 = (rand() % (NROWS - 2)) + 1;
	int pos_y_enemy_1 = (rand() % (NCOLS - 2)) + 1;
	for (int i = 0; i < NROWS; i++)
		for (int j = 0; j < NCOLS; j++)
			(i > 0 && i < NROWS - 1 && j > 0 && j < NCOLS - 1) ? (grid_wall[i][j] = rand() % 2) : (grid_wall[i][j] = ' ');

	init_pellets();

	while(sig_game_over != 1) {
		clean_screen();
		show_map(pos_x_fruits, pos_y_fruits, pos_x_enemy_1, pos_y_enemy_1);
		move_pacman();
		move_enemy(&pos_x_enemy_1, &pos_y_enemy_1);
		eat(&pos_x_fruits, &pos_y_fruits);
		game_over_check();
		usleep(FRAME_DELAY_US);
	}
	endwin();
	return 0;
}

void init_pellets() {
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			int is_border = (i == 0 || j == 0 || i == NROWS - 1 || j == NCOLS - 1);
			int is_wall = (grid_wall[i][j] = 1 && i % 2 == 0 && j % 2 == 0);
			grid_pellet[i][j] = (!is_border && !is_wall) ? '.' : ' ';
		}
	}
}

void show_map(int pos_x_fruits, int pos_y_fruits, int pos_x_enemy_1, int pos_y_enemy_1) {
	/* init map */
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			if (i == 0 || j == 0 || i == NROWS - 1 || j == NCOLS - 1) {grid_map[i][j] = '*';}
			else if (grid_wall[i][j] == 1 && i % 2 == 0 && j % 2 == 0) {grid_map[i][j] = '|';}
			else {grid_map[i][j] = grid_pellet[i][j];}
		}
	}

	/* fruits init */
	grid_map[pos_x_fruits][pos_y_fruits] = 'O';

	/* pacman sprite init */
	grid_map[x][y] = 'C';

	/* ghost enemy sprite init */
	grid_map[pos_x_enemy_1][pos_y_enemy_1] = 'W';

	/* print the map */
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			 mvprintw(i, j * 3, "%c", grid_map[i][j]);
		}
	}
	printw("\n\n\t\t\tLIFE : %d\tSCORE: %d", life, score);
	refresh();
}

void clean_screen() {
	clear();
}

void game_over_check() {
	if (life == 0) {
		clear();
		mvprintw(NROWS / 2, NCOLS / 2, "GAME OVER!");
		refresh();
		napms(1500);
		sig_game_over = 1;
	}
}

int is_blocked(int i, int j) {
	if (i < 0 || i >= NROWS || j < 0 || j >= NCOLS) return 1;
	return (grid_map[i][j] == '*' || grid_map[i][j] == '|');
}

void move_pacman() {
	int c = getch();

	switch(c) {
		case KEY_UP: state_move = 'u'; break;
		case KEY_DOWN: state_move = 'd'; break;
		case KEY_LEFT: state_move = 'l'; break;
		case KEY_RIGHT: state_move = 'r'; break;
		default: break;
	}

	switch(state_move) {
		case 'u': if(!is_blocked(x - 1, y)) *ptr_x -= 1; else state_move = ' '; break;
		case 'd': if(!is_blocked(x + 1, y)) *ptr_x += 1; else state_move = ' '; break;
		case 'l': if(!is_blocked(x, y - 1)) *ptr_y -= 1; else state_move = ' '; break;
		case 'r': if(!is_blocked(x, y + 1)) *ptr_y += 1; else state_move = ' '; break;
		default: break;
	}
}

void move_enemy(int *x_enemy, int *y_enemy) {
	if (*x_enemy > *ptr_x && !is_blocked(*x_enemy - 1, *y_enemy)) (*x_enemy -= 1);
	else if (*x_enemy < *ptr_x && !is_blocked(*x_enemy + 1, *y_enemy)) (*x_enemy += 1);

	if (*y_enemy > *ptr_y && !is_blocked(*x_enemy, *y_enemy - 1)) (*y_enemy -= 1);
	else if (*y_enemy < *ptr_y && !is_blocked(*x_enemy, *y_enemy + 1)) (*y_enemy += 1);

	if (*x_enemy == *ptr_x && *y_enemy == *ptr_y) {
		life -= 1;
		*ptr_x = 2;
		*ptr_y = 2;
		*x_enemy = (rand() % (NROWS - 2)) + 1;
		*y_enemy = (rand() % (NCOLS - 2)) + 1;
	}
}

void eat(int *x_fruits, int *y_fruits) {
	if(*ptr_x == *x_fruits && *ptr_y == *y_fruits) {
		score += 2500;
		*x_fruits = (rand() % (NROWS - 2)) + 1;
		*y_fruits = (rand() % (NCOLS - 2)) + 1;
	}
	if (grid_pellet[x][y] == '.') {
		score += 250;
		grid_pellet[x][y] = ' ';
	}
}
