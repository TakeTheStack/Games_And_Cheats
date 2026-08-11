#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

// ROW | COLUMN
#define NROWS 25
#define NCOLS 50

// Time Frame
#define TIME_FRAME 250000 // 250ms

// Global variables (not extern for the majority)
static int x_base_player = NCOLS / 2;
static int y_base_player = NROWS - 2;
static int x_gun_player = NCOLS / 2;
static int y_gun_player = NROWS - 3;
static int *ptr_x_base_player = &x_base_player;
static int *ptr_y_base_player = &y_base_player;
static int *ptr_x_gun_player = &x_gun_player;
static int *ptr_y_gun_player = &y_gun_player;
static int life = 3;
static int score = 0;
static int sig_game_over = 0;
static char grid_map[NROWS][NCOLS];
static char grid_border[NROWS][NCOLS];
static char grid_protection[NROWS][NCOLS];
static int grid_enemy[NROWS][NCOLS];

// Prototypes
void init_map();
void init_player();
void init_enemy();
void show_map();
void collision_border();
void move_player(int c);
void shoot_player(int c);
void move_enemy();
void shoot_enemy();
void win_condition_check();
void game_over_check();
void clear_screen();

int main() {
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	srand(time(NULL));

	/* Init Game */
	init_map();
	init_player();
	init_enemy();

	while(sig_game_over != 1) {
		show_map();
		collision_border();
		int c = getch();
		move_player(c);
		shoot_player(c);
		move_enemy();
		shoot_enemy();
		win_condition_check();
		game_over_check();
		clear_screen();
		usleep(TIME_FRAME);
	}
	/* GAME OVER! CODEZONE */
	clear();
	mvprintw(NROWS / 2, NCOLS / 2, "GAME OVER!");
	printw("\n\n\t\tSCORE: %d\n", score);
	refresh();
	napms(2500);
	endwin();

	return 0;
}

void init_map() {
	/* init border of the map */
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			if ((i == 0 && j == 0) || (i == (NROWS - 1) && j == 0) || (i == 0 && j == (NCOLS - 1)) || (i == (NROWS - 1) && j == (NCOLS - 1))) {grid_border[i][j] = '#';}
			else if (i == 0 || i == (NROWS - 1)) {grid_border[i][j] = '_';}
			else if (j == 0 || j == (NCOLS - 1)) {grid_border[i][j] = '|';}
			else {grid_border[i][j] = ' ';}
			grid_map[i][j] = grid_border[i][j];
		}
	}

	/* init protection for the player */
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			if (i == (int)(NROWS / 1.5) && (j % 7) == 3 && j != (NCOLS - 1)) {grid_protection[i][j] = '^';}
			else {grid_protection[i][j] = 0;}

			/* add to map whithout overwriting on the bad place*/
			switch(grid_protection[i][j]) {
				case '^': grid_map[i][j] = grid_protection[i][j]; break;
				case 0: grid_map[i][j] += grid_protection[i][j]; break;
				default: break;
			}
		}
	}
}

void init_player() {
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			/* base player init */
			(i == y_base_player && j == x_base_player) ? (grid_map[i][j] = 'W') : (0);

			/*gun player init */
			(i == y_gun_player && j == x_gun_player) ? (grid_map[i][j] = 'T') : (0);
		}
	}
}

// Global variable for init enemy function
static int count_id_enemy = 1;
static int id_enemy = 1;
static int shift[27] = {0, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7, 8, -8, 9, -9, 10, -10, 11, -11, 12, -12, 13, -13};

void init_enemy() {
	if (count_id_enemy > 5) {return;}

	for (int j = 0; j < NCOLS; j++) {
		grid_enemy[count_id_enemy][j] = 0;
	}

	int *ptr_shift = shift;
	for (int k = 0; k < id_enemy && (ptr_shift - shift) < 27; k++) {
		grid_enemy[count_id_enemy][NCOLS/2 + (*ptr_shift)] = id_enemy;
		ptr_shift++;
	}

	id_enemy += id_enemy + 1;
	count_id_enemy++;
	init_enemy();
}

void show_map() {
	/* We don't need to update the border */
	/* But we need to update : player, enemy, protection, map, etc. */

	/* update-init map */
	for (int i = 0; i < NROWS; i++)
		for (int j = 0; j < NCOLS; j++)
			if (i > 0 && j > 0 && i < (NROWS - 1) && j < (NCOLS - 1))
				grid_map[i][j] = ' ';

	/* update player */
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			(i == y_base_player && j == x_base_player) ? (grid_map[i][j] = 'W') : (0);
			(i == y_gun_player && j == x_gun_player) ? (grid_map[i][j] = 'T') : (0);
		}
	}

	/* update enemy */
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			switch(grid_enemy[i][j]) {
				case 1: grid_map[i][j] = 'Y'; break;
				case 3: grid_map[i][j] = 'X'; break;
				case 7: grid_map[i][j] = 'A'; break;
				case 15: grid_map[i][j] = 'B'; break;
				case 31: grid_map[i][j] = 'V'; break;
				default: break;
			}
		}
	}

	/* update protection */
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			switch(grid_protection[i][j]) {
				case '^': grid_map[i][j] = grid_protection[i][j]; break;
				case 0: grid_map[i][j] += grid_protection[i][j]; break;
				default: break;
			}
		}
	}

	/* The real show map functionality */
	for (int i = 0; i < NROWS; i++) {
		for (int j = 0; j < NCOLS; j++) {
			mvprintw(i * 2, j, "%c", grid_map[i][j]);
		}
	}

	(life == 3) ? (printw("\n\n\t\tLIFE: w w w\tSCORE: %d", score)) : (0);
	(life == 2) ? (printw("\n\n\t\tLIFE: w w 0\tSCORE: %d", score)) : (0);
	(life == 1) ? (printw("\n\n\t\tLIFE: w 0 0\tSCORE: %d", score)) : (0);
	refresh();
}

void collision_border() {
	if (*ptr_x_base_player < 1 || *ptr_x_gun_player < 1) {*ptr_x_base_player += 1; *ptr_x_gun_player += 1;}
	if (*ptr_x_base_player > (NCOLS - 3) || *ptr_x_gun_player > (NCOLS - 3)) {*ptr_x_base_player -= 1; *ptr_x_gun_player -= 1;}
}

void move_player(int c) {
	switch(c) {
		case KEY_LEFT: {
			*ptr_x_base_player -= 1;
			*ptr_x_gun_player -= 1;
			break;
		}
		case KEY_RIGHT: {
			*ptr_x_base_player += 1;
			*ptr_x_gun_player += 1;
			break;
		}
		default: break;
	}
}

void shoot_player(int c) {
	if (c != KEY_UP) {return;}

	int row = *ptr_y_gun_player - 1;
	int col = *ptr_x_gun_player;

	while (row > 0) {
		/* Destroy a protection if touched */
		if (grid_protection[row][col] == '^') {
			grid_protection[row][col] = 0;
			break;
		}

		/* Destroy an enemy if touched */
		if (grid_map[row][col] != '^' && grid_map[row][col] != ' ' && grid_map[row][col] != '_') {
			switch (grid_map[row][col]) {
				case 'Y': grid_map[row][col] = ' '; grid_enemy[row][col] = 0; score += 10000; break;
				case 'X': grid_map[row][col] = ' '; grid_enemy[row][col] = 0; score += 5000; break;
				case 'A': grid_map[row][col] = ' '; grid_enemy[row][col] = 0; score += 1500; break;
				case 'B': grid_map[row][col] = ' '; grid_enemy[row][col] = 0; score += 500; break;
				case 'V': grid_map[row][col] = ' '; grid_enemy[row][col] = 0; score += 250; break;
			}
			break;
		}

		/* Animation of the shoot in the void */
		if (grid_map[row][col] == ' ') {
			mvprintw(row * 2, col, "*");
			refresh();
			napms(50);
			mvprintw(row * 2, col, " ");
			refresh();
		}

		/* Can't pass and destroy the border */
		if (grid_border[row][col] == '_' || grid_map[row][col] == '_') {
			grid_map[row][col] = '_';
			break;
		}

		row--;
	}
}

// internal clock for enemy
static int clock_enemy = 0;

void move_enemy() {
	clock_enemy++;

	int delta;
	if (clock_enemy % 6 == 0) {delta = 1;}
	else if (clock_enemy % 3 == 0) {delta = -1;}
	else {return;}

	for (int i = 1; i <= 5; i++) {
		int shifted_row[NCOLS] = {0};

		for (int j = 0; j < NCOLS; j++) {
			int new_col = j + delta;
			if (grid_enemy[i][j] != 0 && new_col >= 1 && new_col < (NCOLS - 1)) {
				shifted_row[new_col] = grid_enemy[i][j];
			}
		}

		for (int j = 0; j < NCOLS; j++) {
			grid_enemy[i][j] = shifted_row[j];
		}
	}
	refresh();
}

void shoot_enemy() {
	if (clock_enemy % 9 != 0) {return;}

	int cand_row[NROWS * NCOLS];
	int cand_col[NROWS * NCOLS];
	int count = 0;

	for (int i = 1; i <= 5; i++) {
		for (int j = 1; j < NCOLS - 1; j++) {
			if (grid_enemy[i][j] != 0) {
				cand_row[count] = i;
				cand_col[count] = j;
				count++;
			}
		}
	}
	if (count == 0) {return;}

	int pick = rand() % count;
	int row = cand_row[pick] + 1;
	int col = cand_col[pick];

	while (row < NROWS - 1) {
		if (grid_map[row][col] == ' ') {
			mvprintw(row * 2, col, "*");
			refresh();
			napms(50);
			mvprintw(row * 2, col, " ");
			refresh();
		}

		if (grid_protection[row][col] == '^') {
			grid_protection[row][col] = ' ';
			break;
		}

		if (grid_border[row][col] == '_' || grid_map[row][col] == '_') {
			grid_map[row][col] = '_';
			break;
		}

		if ((row == *ptr_y_gun_player && col == *ptr_x_gun_player) || (row == *ptr_y_base_player && col == *ptr_x_base_player)) {
			score -= 5000;
			life -= 1;
			break;
		}
		row++;
	}
}

void win_condition_check() {
	for (int i = 1; i < 5; i++) {
		for (int j = 0; j < NCOLS; j++) {
			if (grid_enemy[i][j] != 0) {
				return;
			}
		}
	}

	clear();
	mvprintw(NROWS / 2, NCOLS / 2, "YOU WIN! CONGRAT!");
	printw("\n\n\t\tSCORE: %d\n", score);
	refresh();
	napms(2500);
	endwin();
	exit(0);
}

void game_over_check() {
	(life == 0) ? (sig_game_over += 1) : 0;
}

void clear_screen() {
	clear();
}
