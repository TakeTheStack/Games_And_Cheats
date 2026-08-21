#include <stdio.h>
#include <stdlib.h>
#include <string.h> // only for strcnpy (i didn't had time for this implementation)
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include <locale.h>

// ROW AND COL
#define ROW 50
#define COL 75

// The Time Frame
#define TIME_FRAME 150000 // 150ms

// General Clock condition
#define CLOCK_COND (clock_count % 100)

// Filename
#define FILE_ENEMY "enemy.txt"
#define FILE_SKILLS "skills.txt"

// New key for ncurses
#define KEY_SPACE ' '
#define KEY_ATTACK 'a'

// Index for skill id
#define IDX_SKILLS 21

// Max size for a name (skill / enemy / loot)
#define NAME_LEN 32

// Skill number with class (for lisibility, put this into a header file)
#define UNIVERSAL_SKILL_ID 1
#define UNIVERSAL_SKILL_NAME "punch"

#define WARRIOR_SKILL_ID_1
#define WARRIOR_SKILL_NAME_1

#define WARRIOR_SKILL_ID_2
#define WARRIOR_SKILL_NAME_2

#define WARRIOR_SKILL_ID_3
#define WARRIOR_SKILL_NAME_3

#define WARRIOR_SKILL_ID_4
#define WARRIOR_SKILL_NAME_4

#define WARRIOR_SKILL_ID_5
#define WARRIOR_SKILL_NAME_5

#define MAGE_SKILL_ID_1
#define MAGE_SKILL_NAME_1

#define MAGE_SKILL_ID_2
#define MAGE_SKILL_NAME_2

#define MAGE_SKILL_ID_3
#define MAGE_SKILL_NAME_3

#define MAGE_SKILL_ID_4
#define MAGE_SKILL_NAME_4

#define MAGE_SKILL_ID_5
#define MAGE_SKILL_NAME_5

#define ROGUE_SKILL_ID_1
#define ROGUE_SKILL_NAME_1

#define ROGUE_SKILL_ID_2
#define ROGUE_SKILL_NAME_2

#define ROGUE_SKILL_ID_3
#define ROGUE_SKILL_NAME_3

#define ROGUE_SKILL_ID_4
#define ROGUE_SKILL_NAME_4

#define ROGUE_SKILL_ID_5
#define ROGUE_SKILL_NAME_5

#define MONK_SKILL_ID_1
#define MONK_SKILL_NAME_1

#define MONK_SKILL_ID_2
#define MONK_SKILL_NAME_2

#define MONK_SKILL_ID_3
#define MONK_SKILL_NAME_3

#define MONK_SKILL_ID_4
#define MONK_SKILL_NAME_4

#define MONK_SKILL_ID_5
#define MONK_SKILL_NAME_5

#define ARCHER_SKILL_1
#define ARCHER_SKILL_NAME_1

#define ARCHER_SKILL_2
#define ARCHER_SKILL_NAME_2

#define ARCHER_SKILL_3
#define ARCHER_SKILL_NAME_3

#define ARCHER_SKILL_4
#define ARCHER_SKILL_NAME_4

#define ARCHER_SKILL_5
#define ARCHER_SKILL_NAME_5

// structure
typedef struct {
	int x;
	int y;
} Move;

typedef struct {
	char *job;
	int xp;
	int lvl;
	int health;
	int mana;
	int strength;
	int range;
	int defense;
} Stat;

typedef struct {
	// store skills atributes
	int id;
	char name[NAME_LEN];
	int lvl;
	int health;
	int mana;
	int strength;
	int range;
	int defense;
} SkillEntry;

typedef struct {
	int id;
	char name[NAME_LEN]; // description of the object in the game
} Assets;

typedef struct {
	Assets head;
	Assets neck;
	Assets body;
	Assets left_arm;
	Assets right_arm;
	Assets left_leg;
	Assets right_leg;
} Armor;

typedef struct {
	char head;
	char body;
	char leg;
} Sprite;

typedef struct {
	Move move;
	Stat stat;
	Armor armor;
	Sprite sprite;
} Player;

typedef struct {
	int id;
	char name[NAME_LEN];
	Move move;
	Stat stat;
	Assets drop_loots;
	Sprite sprite;
} Enemy;

// global variable
static SkillEntry skills[IDX_SKILLS];

static Player player = {
	// Move init
	.move.x = COL - (COL / 3),
	.move.y = ROW - (ROW / 3),

	// Statisticals init
	.stat.job = "none",
	.stat.xp = 0,
	.stat.lvl = 0,
	.stat.health = 0,
	.stat.mana = 0,
	.stat.strength = 0,
	.stat.range = 0,
	.stat.defense = 0,

	// head armor description
	.armor.head.id = 0,
	.armor.head.name = "nothing", // optional, just for the description

	// neck armor description
	.armor.neck.id = 0,
	.armor.neck.name = "nothing",

	// body armor description
	.armor.body.id = 0,
	.armor.body.name = "nothing",

	// left arm armor description
	.armor.left_arm.id = 0,
	.armor.left_arm.name = "nothing",

	// right arm armor description
	.armor.right_arm.id = 0,
	.armor.right_arm.name = "nothing",

	// left leg armor description
	.armor.left_leg.id = 0,
	.armor.left_leg.name = "nothing",

	// right leg armor description
	.armor.right_leg.id = 0,
	.armor.right_leg.name = "nothing",

	// Sprite
	.sprite.head = 0,
	.sprite.body = 0,
	.sprite.leg = 0,
};
int *ptr_x_player = &player.move.x;
int *ptr_y_player = &player.move.y;
static int xp_limit_player = 100;
static char state_move = ' ';

static Enemy enemy = {
	// we want to recognise an enemy by an id
	.id = 0,
	.name = "no enemy",

	// because we want the enemy on the right randomly
	.move.x = 0, // compute the width randomly between 50 and 75 because ROW / 4 = 100 / 4 = 25 and because 100 - 25 = 75
	.move.y = 0, // do the same thing for the height

	// Statisticals init
	.stat.lvl = 0,
	.stat.health = 0,
	.stat.mana = 0,
	.stat.strength = 0,
	.stat.range = 0,
	.stat.defense = 0,

	// if dead, then drop a loots by luck
	.drop_loots.id = 0,
	.drop_loots.name = "nothing",

	// Sprite
	.sprite.head = 0,
	.sprite.body = 0,
	.sprite.leg = 0,
};
int *ptr_x_enemy = &enemy.move.x;
int *ptr_y_enemy = &enemy.move.y;

// prototypes
void class_assignation_player(); // the player choose a class between : warrior, mage, rogue, monk, archer

void load_all_skills(FILE *fp_skills);
void get_random_enemy_position(int *x, int *y);
void read_enemy_record(FILE *fp_enemy);

void init_enemy(FILE *fp_enemy);
void init_map(char grid_map[][COL]);
void show_map(char grid_map[][COL], int left_meter, int right_meter);
void move_player(char grid_map[][COL], int c, int *l, int *r);
void collision_border(char grid_map[][COL]);
void gravity(char grid_map[][COL]);

void is_player_alive(int *sig_game_over);
void is_enemy_alive(FILE *fp_enemy, int clock_count, int left_meter, int right_meter);

void jump_player(char grid_map[][COL]);
void attack_player(char grid_map[][COL], int c);
void player_xp_up();
void xp_limit();

void universal_skill_effect(char grid_map[][COL]);

void warrior_skills(char grid_map[][COL], int c);
void mage_skills(char grid_map[][COL], int c);
void rogue_skills(char grid_map[][COL], int c);
void monk_skills(char grid_map[][COL], int c);
void archer_skills(char grid_map[][COL], int c);

void warrior();
void mage();
void rogue();
void monk();
void archer();

// because normally we import string.h, but here i decided to implement my_strcmp and str_len for the practice and the exercice
int my_strcmp(char *s1, char *s2);
int my_strcmp(char *s1, char *s2) {
	int i;
	for (i = 0; s1[i] == s2[i]; i++) {
		if (s1[i] == '\0' || s2[i] == '\0') { break; }
	}
	if (s1[i] == s2[i]) {return 0;}
	else {return -1;}
}

int str_len(char *text);
int str_len(char *text)	{
	int len_text = 0;;

	while (*text != '\0') {
		len_text++;
		text++;
	}
	return len_text;
}

int main() {
	/* ncurses configure */
	setlocale(LC_ALL, ""); /* for utf-8 characteres */
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	/* enemy file */
	static FILE *fp_enemy;
	fp_enemy = fopen(FILE_ENEMY, "r");
	if (fp_enemy == NULL) {
		clear();
		printw("No such file\n");
		refresh();
		napms(2500);
		endwin();
		fclose(fp_enemy);
		exit(1);
	}

	/* skills file */
	static FILE *fp_skills;
	fp_skills = fopen(FILE_SKILLS, "r");
	if (fp_skills == NULL) {
		clear();
		printw("No such file\n");
		refresh();
		napms(2500);
		endwin();
		fclose(fp_skills);
		exit(1);
	}

	/* for the map */
	char (*grid_map)[COL] = malloc(ROW * sizeof(*grid_map));
	if (grid_map == NULL) {
		clear();
		printw("Memory Allocation Failed\n");
		refresh();
		napms(2500);
		endwin();
		fclose(fp_enemy);
		fclose(fp_skills);
		exit(1);
	}
	/* game over variable */
	int sig_game_over = 0;

	/* how we want to calculate the stat of the enemy, with distance in the grid */
	static int left_meter = 0;
	static int right_meter = 0;

	/* clock count */
	static int clock_count = 1;

	/* configure random */
	srand(time(NULL));

	/* class assignation for the player */
	class_assignation_player();
	nodelay(stdscr, TRUE);

	/* load the skills once */
	load_all_skills(fp_skills);
	fclose(fp_skills);

	/* init */
	init_map(grid_map);
	init_enemy(fp_enemy);

	/* loop for the game */
	while (sig_game_over != 1) {
		/* graphics */
		clear();
		show_map(grid_map, left_meter, right_meter);

		/* player */
		int c = getch(); /* the movement of the sprite player */
		move_player(grid_map, c, &left_meter, &right_meter);
		attack_player(grid_map, c);

		collision_border(grid_map);
		gravity(grid_map);

		/* alive ? */
		is_player_alive(&sig_game_over);
		is_enemy_alive(fp_enemy, clock_count, left_meter, right_meter);

		/* time */
		clock_count++;
		usleep(TIME_FRAME);
	}
	/* GAME OVER ZONE */
	clear();
	mvprintw(ROW / 2, COL / 2, "GAME OVER!");
	refresh();
	napms(2500);
	endwin();

	fclose(fp_enemy);
	free(grid_map);
	return 0;
}

void load_all_skills(FILE *fp_skills) {
	fscanf(fp_skills, "%*[^\n]%*c");

	for (int i = 0; i < IDX_SKILLS; i++) {
		int n = fscanf(fp_skills, "%d %31s %d %d %d %d %d %d", &skills[i].id, skills[i].name, &skills[i].lvl, &skills[i].health, &skills[i].mana, &skills[i].strength, &skills[i].range, &skills[i].defense);

		if (n != 8) {
			clear();
			printw("Error: Your file may be corrupted\n");
			refresh();
			napms(2500);
			endwin();
			fclose(fp_skills);
			exit(1);
		}
	}
}

void player_xp_up() {
	switch(enemy.stat.lvl) {
		case 1: case 2: case 3: player.stat.xp += enemy.stat.lvl * 25; break;
		case 4: case 5: player.stat.xp += enemy.stat.lvl * 50; break;
		default: break;
	}
	xp_limit();
}

void xp_limit() {
	if (xp_limit_player != 0 && (player.stat.xp % xp_limit_player) == 0) {
		player.stat.lvl += 1;
		player.stat.xp = 0;
		clear();
		printw("LEVEL UP!");
		refresh();
		napms(2500);
	}
}

void is_player_alive(int *sig_game_over) {
	(player.stat.health <= 0) ? (*sig_game_over = 1) : (0);
}

void get_random_enemy_position(int *x, int *y) {
	int min_x = COL / 2;
	int max_x = COL - 2;
	*x = min_x + (rand() % (max_x - min_x + 1));

	int min_y = 3;
	int max_y = ROW - 2;
	*y = min_y + (rand() % (max_y - min_y + 1));
}

void read_enemy_record(FILE *fp_enemy) {
	fscanf(fp_enemy, "%d", &enemy.id);
	fscanf(fp_enemy, "%31s", enemy.name);

	get_random_enemy_position(&enemy.move.x, &enemy.move.y);

	fscanf(fp_enemy, "%d", &enemy.stat.lvl);
	fscanf(fp_enemy, "%d", &enemy.stat.health);
	fscanf(fp_enemy, "%d", &enemy.stat.mana);
	fscanf(fp_enemy, "%d", &enemy.stat.strength);
	fscanf(fp_enemy, "%d", &enemy.stat.range);
	fscanf(fp_enemy, "%d", &enemy.stat.defense);

	fscanf(fp_enemy, "%d", &enemy.drop_loots.id);
	fscanf(fp_enemy, "%31s", enemy.drop_loots.name);

	fscanf(fp_enemy, " %c", &enemy.sprite.head);
	fscanf(fp_enemy, " %c", &enemy.sprite.body);
	fscanf(fp_enemy, " %c", &enemy.sprite.leg);
}

void is_enemy_alive(FILE *fp_enemy, int clock_count, int left_meter, int right_meter) {
	if (enemy.stat.health <= 0) {
		player_xp_up();

		fscanf(fp_enemy, "%*[^\n]%*c");

		if (left_meter < 150 || right_meter < 150) {
			for (int i = 0; i < clock_count % 3; i++) {fscanf(fp_enemy, "%*[^\n]%*c");}
		} else if ((left_meter > 150 && left_meter < 300) || (right_meter > 150 && right_meter < 300)) {
			for (int i = 4; i < clock_count % 7; i++) {fscanf(fp_enemy, "%*[^\n]%*c");}
		} else {
			for (int i = 7; i < clock_count % 10; i++) {fscanf(fp_enemy, "%*[^\n]%*c");}
		}

		if (feof(fp_enemy)) {
			rewind(fp_enemy);
			fscanf(fp_enemy, "%*[^\n]%*c");
		}
		read_enemy_record(fp_enemy);
	}
}

void init_enemy(FILE *fp_enemy) {
	/* ignore the first line with regexp */
	fscanf(fp_enemy, "%*[^\n]%*c");
	read_enemy_record(fp_enemy);
}

void init_map(char grid_map[][COL]) {
	for (int i = 0; i < ROW; i++)
		for (int j = 0; j < COL; j++)
			if (i == 0 || i == (ROW - 1)) {grid_map[i][j] = '_';}
			else if (j == 0 || j == (COL - 1)) {grid_map[i][j] = '|';}
			else {grid_map[i][j] = ' ';}
}

void show_map(char grid_map[][COL], int left_meter, int right_meter) {
	/* update the enemy */
	for (int i = 0; i < ROW; i++)
		for (int j = 0; j < COL; j++)
			if (i == *ptr_y_enemy && j == *ptr_x_enemy) {grid_map[i][j] = enemy.sprite.leg;}
			else if (i == (*ptr_y_enemy) - 1 && j == *ptr_x_enemy) {grid_map[i][j] = enemy.sprite.body;}
			else if (i == (*ptr_y_enemy) - 2 && j == *ptr_x_enemy) {grid_map[i][j] = enemy.sprite.head;}

	/* update player */
	for (int i = 0; i < ROW; i++)
		for (int j = 0; j < COL; j++)
			if (i == *ptr_y_player && j == *ptr_x_player) {grid_map[i][j] = player.sprite.leg;}
			else if (i == (*ptr_y_player) - 1 && j == *ptr_x_player) {grid_map[i][j] = player.sprite.body;}
			else if (i == (*ptr_y_player) - 2 && j == *ptr_x_player) {grid_map[i][j] = player.sprite.head;}

	/* the real show map code */
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			mvprintw(i, j, "%c", grid_map[i][j]);
		}
	}
	printw("\n\n\tHEALTH:%d\tMANA:%d\tJOB:%s\tLEFT METER:%d\tRIGHT METER:%d\n", player.stat.health, player.stat.mana, player.stat.job, left_meter, right_meter);
	refresh();
}

void move_player(char grid_map[][COL], int c, int *l, int *r) {
	switch (c) {
		case KEY_LEFT: {
			state_move = 'l';
			if (*r == 0) {
				*l += 1;
			}
			(*r > 0) ? (*r -= 1) : (0);
			*ptr_x_player -= 1;
			grid_map[*ptr_y_player][*ptr_x_player + 1] = ' ';
			grid_map[*ptr_y_player - 1][*ptr_x_player + 1] = ' ';
			grid_map[*ptr_y_player - 2][*ptr_x_player + 1] = ' ';
			break;
		}
		case KEY_RIGHT: {
			state_move = 'r';
			if (*l == 0) {
				*r += 1;
			}
			(*l > 0) ? (*l -= 1) : (0);
			*ptr_x_player += 1;
			grid_map[*ptr_y_player][*ptr_x_player - 1] = ' ';
			grid_map[*ptr_y_player - 1][*ptr_x_player - 1] = ' ';
			grid_map[*ptr_y_player - 2][*ptr_x_player - 1] = ' ';
			break;
		}
		case KEY_SPACE: state_move = 'j'; jump_player(grid_map); break;
		default: break;
	}
}

void jump_player(char grid_map[][COL]) {
	*ptr_y_player -= 6;
	for (int i = 1; i <= 6; i++)
		grid_map[*ptr_y_player + i][*ptr_x_player] = ' ';
}

void attack_player(char grid_map[][COL], int c) {
	if (my_strcmp(player.stat.job, "warrior") == 0) {warrior_skills(grid_map, c);}
	else if (my_strcmp(player.stat.job, "mage") == 0) {mage_skills(grid_map, c);}
	else if (my_strcmp(player.stat.job, "rogue") == 0) {rogue_skills(grid_map, c);}
	else if (my_strcmp(player.stat.job, "monk") == 0) {monk_skills(grid_map, c);}
	else if (my_strcmp(player.stat.job, "archer") == 0) {archer_skills(grid_map, c);}
}

void universal_skill_effect(char grid_map[][COL]) {
	if (player.stat.lvl >= skills[UNIVERSAL_SKILL_ID].lvl) {
		if (state_move == 'l') {
			if (grid_map[*ptr_y_player][*ptr_x_player - (skills[UNIVERSAL_SKILL_ID].range)] == grid_map[*ptr_y_enemy][*ptr_x_enemy]) {
				mvprintw(*ptr_y_enemy, *ptr_x_enemy, "(");
				refresh();
				napms(2500);
				if (enemy.stat.defense == 0) {enemy.stat.health -= (int)skills[UNIVERSAL_SKILL_ID].strength;}
				else {enemy.stat.health -= (int)skills[UNIVERSAL_SKILL_ID].strength / (int)enemy.stat.defense;}
			}
		} else if (state_move == 'r') {
			if (grid_map[*ptr_y_player][*ptr_x_player + (skills[UNIVERSAL_SKILL_ID].range)] == grid_map[*ptr_y_enemy][*ptr_x_enemy]) {
				mvprintw(*ptr_y_enemy, *ptr_x_enemy, ")");
				refresh();
				napms(2500);
				if (enemy.stat.defense == 0) {enemy.stat.health -= (int)skills[UNIVERSAL_SKILL_ID].strength;}
				else {enemy.stat.health -= (int)skills[UNIVERSAL_SKILL_ID].strength / (int)enemy.stat.defense;}
			}
		}
	}
}

void warrior_skills(char grid_map[][COL], int c) {
	switch(c) {
		case KEY_ATTACK: universal_skill_effect(grid_map); break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		default: break;
	}
}


void mage_skills(char grid_map[][COL], int c) {
	switch(c) {
		case KEY_ATTACK: universal_skill_effect(grid_map); break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		default: break;
	}
}

void rogue_skills(char grid_map[][COL], int c) {
	switch(c) {
		case KEY_ATTACK: universal_skill_effect(grid_map); break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		default: break;
	}
}

void monk_skills(char grid_map[][COL], int c) {
	switch(c) {
		case KEY_ATTACK: universal_skill_effect(grid_map); break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		default: break;
	}
}

void archer_skills(char grid_map[][COL], int c) {
	switch(c) {
		case KEY_ATTACK: universal_skill_effect(grid_map); break;
		case 1: break;
		case 2:	break;
		case 3:	break;
		case 4:	break;
		case 5:	break;
		default: break;
	}
}

void collision_border(char grid_map[][COL]) {
	(grid_map[*ptr_y_player][*ptr_x_player + 1] == '|') ? (*ptr_x_player -= 1) : (0);
	(grid_map[*ptr_y_player][*ptr_x_player - 1] == '|') ? (*ptr_x_player += 1) : (0);
}

void gravity(char grid_map[][COL]) {
	if (grid_map[*ptr_y_player + 1][*ptr_x_player] != '_') {
		*ptr_y_player += 1;
		grid_map[*ptr_y_player - 1][*ptr_x_player] = ' ';
		grid_map[*ptr_y_player - 2][*ptr_x_player] = ' ';
		grid_map[*ptr_y_player - 3][*ptr_x_player] = ' ';
	}
	if (grid_map[*ptr_y_enemy + 1][*ptr_x_player] != '_') {
		*ptr_y_enemy += 1;
		grid_map[*ptr_y_enemy - 1][*ptr_x_enemy] = ' ';
		grid_map[*ptr_y_enemy - 2][*ptr_x_enemy] = ' ';
		grid_map[*ptr_y_enemy - 3][*ptr_x_enemy] = ' ';
	}
}

void class_assignation_player() {
	char class_name[25] = ""; /* init the buffer string */
	int cond_warrior = my_strcmp(class_name, "warrior");
	int cond_mage = my_strcmp(class_name, "mage");
	int cond_rogue = my_strcmp(class_name, "rogue");
	int cond_monk = my_strcmp(class_name, "monk");
	int cond_archer = my_strcmp(class_name, "archer");

	while (cond_warrior || cond_mage || cond_rogue || cond_monk || cond_archer) {
		clear();
		mvprintw((ROW - (ROW / 2)), (COL - (COL / 2)), "CHOOSE A CLASS");
		mvprintw((ROW - (ROW / 2)) + 1, (COL - (COL / 2)) + 1, "WARRIOR | MAGE | ROGUE | MONK | ARCHER");
		printw("\n\n"); /* space */
		refresh();

		echo();
		scanw("%24s", class_name);
		noecho();

		clear();
		mvprintw(ROW / 2, COL / 2, "%s", class_name);
		refresh();
		napms(2500);

		if (my_strcmp(class_name, "warrior") == 0) {warrior(); break;}
		if (my_strcmp(class_name, "mage") == 0) {mage(); break;}
		if (my_strcmp(class_name, "rogue") == 0) {rogue(); break;}
		if (my_strcmp(class_name, "monk") == 0) {monk(); break;}
		if (my_strcmp(class_name, "archer") == 0) {archer(); break;}
	}
	return;
}

void warrior() {
	/* warrior stat */
	player.stat.job = "warrior";
	player.stat.lvl = 1;
	player.stat.health = 375;
	player.stat.mana = 25;
	player.stat.strength = 3;
	player.stat.range = 1;
	player.stat.defense = 3;

	/* warrior sprite */
	player.sprite.head = '^';
	player.sprite.body = 'o';
	player.sprite.leg = 'H';
}

void mage() {
	/* mage stat */
	player.stat.job = "mage";
	player.stat.lvl = 1;
	player.stat.health = 50;
	player.stat.mana = 350;
	player.stat.strength = 1;
	player.stat.range = 3;
	player.stat.defense = 1;

	/* mage sprite */
	player.sprite.head = 'o';
	player.sprite.body = '^';
	player.sprite.leg = 'N';
}

void rogue() {
	/* rogue stat */
	player.stat.job = "rogue";
	player.stat.lvl = 1;
	player.stat.health = 100;
	player.stat.mana = 150;
	player.stat.strength = 2;
	player.stat.range = 2;
	player.stat.defense = 1;

	player.armor.right_arm.id = 41;
	strncpy(player.armor.right_arm.name, "wooden_dagger", NAME_LEN - 1);

	/* rogue sprite */
	player.sprite.head = '~';
	player.sprite.body = '^';
	player.sprite.leg = 'v';
}

void monk() {
	/* monk stat */
	player.stat.job = "monk";
	player.stat.lvl = 1;
	player.stat.health = 200;
	player.stat.mana = 200;
	player.stat.strength = 2;
	player.stat.range = 1;
	player.stat.defense = 2;

	/* monk sprite */
	player.sprite.head = 'O';
	player.sprite.body = 'Y';
	player.sprite.leg = 'M';
}

void archer() {
	/* archer stat */
	player.stat.job = "archer";
	player.stat.lvl = 1;
	player.stat.health = 50;
	player.stat.mana = 100;
	player.stat.strength = 1;
	player.stat.range = 7;
	player.stat.defense = 1;

	player.armor.right_arm.id = 31;
	strncpy(player.armor.right_arm.name, "wooden_bow", NAME_LEN - 1);

	/* archer sprite */
	player.sprite.head = '^';
	player.sprite.body = 'O';
	player.sprite.leg = 'U';
}
