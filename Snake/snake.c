#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define NROWS 30 // lines
#define NCOLS 30 // columns

#define FRAME_DELAY_US 200000 // 200ms

#define BOX_TOP (NROWS / 3)
#define BOX_BOTTOM (NROWS - NROWS / 3 - 1)
#define BOX_LEFT (NCOLS / 3)
#define BOX_RIGHT (NCOLS - NCOLS / 3 - 1)

// Global variable
static int sig_game_over = 0;
static int size_snake = 1;
static int score = 0;
static int x = NROWS/2; // line - head of the snake
static int y = NCOLS/2; // columns - head of the snake
static int *ptr_x = &x;
static int *ptr_y = &y;
static char state_move = ' ';

// Prototypes
void show_map(int pos_x_apple, int pos_y_apple);
void clean_screen();
void move_snake();
void eat_apple(int *pos_x_apple, int *pos_y_apple);

int main() {
	/* ncurses init */
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	/* random position for an apple - initialisation */
	srand(time(NULL));
	int pos_x_apple = BOX_TOP + 1 + rand() % (BOX_BOTTOM - BOX_TOP - 1);
	int pos_y_apple = BOX_LEFT + 1 + rand() % (BOX_RIGHT - BOX_LEFT - 1);

	/* while the game over is not trigger */
	while (sig_game_over != 1) {
		clean_screen();
		show_map(pos_x_apple, pos_y_apple);
		move_snake();
		eat_apple(&pos_x_apple, &pos_y_apple);
		usleep(FRAME_DELAY_US);
	}
	endwin();
	return 0;
}

void show_map(int pos_x_apple, int pos_y_apple) {
	static char grid[NROWS][NCOLS];
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			int is_horizontal_wall = (i == BOX_TOP || i == BOX_BOTTOM) && j >= BOX_LEFT && j <= BOX_RIGHT;
			int is_vertical_wall = (j == BOX_LEFT || j == BOX_RIGHT) && i >= BOX_TOP && i <= BOX_BOTTOM;
			grid[i][j] = (is_horizontal_wall || is_vertical_wall) ? 'x' : ' ';
		}
	}
	grid[pos_x_apple][pos_y_apple] = 'o';

	/* Following the head */

	for (int k = 0; k < size_snake; k++) {
		if (*ptr_y + k >= 0 && *ptr_y + k < NCOLS && state_move == 'u') {
			grid[*ptr_x - k][*ptr_y] = '#';
		}
	}
	for (int k = 0; k < size_snake; k++) {
		if (*ptr_y + k >= 0 && *ptr_y + k < NCOLS && state_move == 'd') {
			grid[*ptr_x + k][*ptr_y] = '#';
		}
	}
	for (int k = 0; k < size_snake; k++) {
		if (*ptr_y + k >= 0 && *ptr_y + k < NCOLS && state_move == 'l') {
			grid[*ptr_x][*ptr_y - k] = '#';
		}
	}
	for (int k = 0; k < size_snake; k++) {
		if (*ptr_y + k >= 0 && *ptr_y + k < NCOLS && state_move == 'r') {
			grid[*ptr_x][*ptr_y + k] = '#';
		}
	}

	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			mvprintw(i, j * 3, "%c", grid[i][j]);
		}
	}
	printw("SCORE: %d", score);
	refresh();
}

void clean_screen() {
	clear();
}

void move_snake() {
	int c = getch(); /* get keyboard arrow values */
	switch(c) {
		case KEY_UP: state_move = 'u'; break;
		case KEY_DOWN: state_move = 'd'; break;
		case KEY_LEFT: state_move = 'l'; break;
		case KEY_RIGHT: state_move = 'r'; break;
		default: break;
	}
	switch(state_move) {
		case 'u': *ptr_x -= 1; break;
		case 'd': *ptr_x += 1; break;
		case 'l': *ptr_y -= 1; break;
		case 'r': *ptr_y += 1; break;
		default: break;
	}
	if (*ptr_x <= BOX_TOP || *ptr_x >= BOX_BOTTOM || *ptr_y <= BOX_LEFT || *ptr_y >= BOX_RIGHT) {
		clear();
		mvprintw(NROWS / 2, (NCOLS / 2) * 3 - 5, "GAME OVER!");
		refresh();
		napms(1500);
		endwin();
		exit(1);
	}
}

void eat_apple(int *pos_x_apple, int *pos_y_apple) {
	if (*ptr_x == *pos_x_apple && *ptr_y == *pos_y_apple) {
		size_snake += 1;
		score += 500;
		*pos_x_apple = BOX_TOP + 1 + rand() % (BOX_BOTTOM - BOX_TOP - 1);
		*pos_y_apple = BOX_LEFT + 1 + rand() % (BOX_RIGHT - BOX_LEFT - 1);
	}
}
