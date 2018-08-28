/*
 * level.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#include <stdio.h>
#include <curses.h>

#include "rogue.h"
#include "level.h"
#include "display.h"
#include "invent.h"
#include "message.h"
#include "monster.h"
#include "object.h"
#include "pack.h"
#include "random.h"
#include "room.h"
#include "score.h"

#define swap(x,y) {t = x; x = y; y = t;}

short cur_level = 0, max_level = 1, cur_room;
char *new_level_message = 0;
short party_room = NO_ROOM;
short r_de;

long level_points[MAX_EXP_LEVEL] = {
    10L,
    20L,
    40L,
    80L,
    160L,
    320L,
    640L,
    1300L,
    2600L,
    5200L,
    10000L,
    20000L,
    40000L,
    80000L,
    160000L,
    320000L,
    1000000L,
    3333333L,
    6666666L,
    MAX_EXP,
    99900000L
};

short random_rooms[MAXROOMS] = { 3, 7, 5, 2, 0, 6, 1, 4, 8 };

extern boolean being_held, wizard, detect_monster;
extern boolean see_invisible;
extern short bear_trap, levitate, extra_hp, less_hp, cur_room;
extern short party_counter;

void
make_level(void)
{
    short i, j;
    short must_exist1, must_exist2, must_exist3;
    boolean big_room;
    boolean vertical;

    if (cur_level < LAST_DUNGEON) {
	cur_level++;
    }
    if (cur_level > max_level) {
	max_level = cur_level;
    }
    must_exist1 = get_rand(0, 2);
    if ((vertical = coin_toss())) {
	must_exist3 = (must_exist2 = must_exist1 + 3) + 3;
    } else {
	must_exist3 = (must_exist2 = (must_exist1 *= 3) + 1) + 1;
    }
    big_room = ((cur_level == party_counter) && rand_percent(1));
    if (big_room) {
	make_room(BIG_ROOM, 0, 0, 0);
    } else {
	for (i = 0; i < MAXROOMS; i++) {
	    make_room(i, must_exist1, must_exist2, must_exist3);
	}
    }
    if (!big_room) {
	add_mazes();

	mix_random_rooms();

	for (j = 0; j < MAXROOMS; j++) {

	    i = random_rooms[j];

	    if (i < (MAXROOMS - 1)) {
		(void) connect_rooms(i, i + 1);
	    }
	    if (i < (MAXROOMS - 3)) {
		(void) connect_rooms(i, i + 3);
	    }
	    if (i < (MAXROOMS - 2)) {
		if ((rooms[i + 1].is_room & R_NOTHING) &&
		    (i + 1 != 4 || vertical)) {
		    if (connect_rooms(i, i + 2)) {
			rooms[i + 1].is_room = R_CROSS;
		    }
		}
	    }
	    if (i < (MAXROOMS - 6)) {
		if ((rooms[i + 3].is_room & R_NOTHING) &&
		    (i + 3 != 4 || !vertical)) {
		    if (connect_rooms(i, i + 6)) {
			rooms[i + 3].is_room = R_CROSS;
		    }
		}
	    }
	    if (is_all_connected()) {
		break;
	    }
	}
	fill_out_level();
    }
    if (!has_amulet() && (cur_level >= AMULET_LEVEL)) {
	put_amulet();
    }
}

void
make_room(short rn, short r1, short r2, short r3)
{
    short left_col, right_col, top_row, bottom_row;
    short width, height;
    short row_offset, col_offset;
    short i, j;
    short ch;

    if (rn == BIG_ROOM) {
	top_row = get_rand(MIN_ROW, MIN_ROW + 5);
	bottom_row = get_rand(ROGUE_LINES - 7, ROGUE_LINES - 2);
	left_col = get_rand(0, 10);
#if !defined( ORIGINAL )
	right_col = get_rand(ROGUE_COLUMNS - 11, ROGUE_COLUMNS - 2);
#else /* ORIGINAL */
	right_col = get_rand(ROGUE_COLUMNS - 11, ROGUE_COLUMNS - 1);
#endif /* ORIGINAL */
	rn = 0;
	goto B;
    }
    switch (rn % 3) {
    case 0:
	left_col = 0;
	right_col = COL1 - 1;
	break;
    case 1:
	left_col = COL1 + 1;
	right_col = COL2 - 1;
	break;
    default:
    case 2:
	left_col = COL2 + 1;
#if !defined( ORIGINAL )
	right_col = ROGUE_COLUMNS - 2;
#else /* ORIGINAL */
	right_col = ROGUE_COLUMNS - 1;
#endif /* ORIGINAL */
	break;
    }
    switch (rn / 3) {
    case 0:
	top_row = MIN_ROW;
	bottom_row = ROW1 - 1;
	break;
    case 1:
	top_row = ROW1 + 1;
	bottom_row = ROW2 - 1;
	break;
    default:
    case 2:
	top_row = ROW2 + 1;
	bottom_row = ROGUE_LINES - 2;
	break;
    }
    height = get_rand(4, (bottom_row - top_row + 1));
    width = get_rand(7, (right_col - left_col - 2));

    row_offset = get_rand(0, ((bottom_row - top_row) - height + 1));
    col_offset = get_rand(0, ((right_col - left_col) - width + 1));

    top_row += row_offset;
    bottom_row = top_row + height - 1;

    left_col += col_offset;
    right_col = left_col + width - 1;

    if ((rn != r1) && (rn != r2) && (rn != r3) && rand_percent(40)) {
	goto END;
    }
B:
    rooms[rn].is_room = R_ROOM;

    for (i = top_row; i <= bottom_row; i++) {
	for (j = left_col; j <= right_col; j++) {
	    if (i == top_row || i == bottom_row) {
		ch = HORWALL;
	    } else if (j == left_col || j == right_col) {
		ch = VERTWALL;
	    } else {
		ch = FLOOR;
	    }
	    dungeon[i][j] = ch;
	}
    }
END:
    rooms[rn].top_row = top_row;
    rooms[rn].bottom_row = bottom_row;
    rooms[rn].left_col = left_col;
    rooms[rn].right_col = right_col;
}

int
connect_rooms(short room1, short room2)
{
    short row1, col1, row2, col2, dir, rev;
    door *dp;

    if ((!(rooms[room1].is_room & (R_ROOM | R_MAZE))) ||
	(!(rooms[room2].is_room & (R_ROOM | R_MAZE)))) {
	return 0;
    }
    if (same_row(room1, room2)) {
	if (rooms[room1].left_col > rooms[room2].right_col) {
	    dir = LEFT;
	    rev = RIGHT;
	} else {		/*if (rooms[room2].left_col > rooms[room1].right_col) */

	    dir = RIGHT;
	    rev = LEFT;
	}
    } else if (same_col(room1, room2)) {
	if (rooms[room1].top_row > rooms[room2].bottom_row) {
	    dir = UPWARD;
	    rev = DOWN;
	} else {		/*if (rooms[room2].top_row > rooms[room1].bottom_row) */

	    dir = DOWN;
	    rev = UPWARD;
	}
    } else {
	return 0;
    }
    put_door(&rooms[room1], dir, &row1, &col1);
    put_door(&rooms[room2], rev, &row2, &col2);

    do {
	draw_simple_passage(row1, col1, row2, col2, dir);
    } while (rand_percent(4));

    dp = &(rooms[room1].doors[dir / 2]);
    dp->oth_room = room2;
    dp->oth_row = row2;
    dp->oth_col = col2;

    dp = &(rooms[room2].doors[(((dir + 4) % DIRS) / 2)]);
    dp->oth_room = room1;
    dp->oth_row = row1;
    dp->oth_col = col1;
    return 1;
}

void
clear_level(void)
{
    short i, j;

    for (i = 0; i < MAXROOMS; i++) {
	rooms[i].is_room = R_NOTHING;
	for (j = 0; j < 4; j++) {
	    rooms[i].doors[j].oth_room = NO_ROOM;
	}
    }

    for (i = 0; i < MAX_TRAPS; i++) {
	traps[i].trap_type = NO_TRAP;
    }
    for (i = 0; i < ROGUE_LINES; i++) {
	for (j = 0; j < ROGUE_COLUMNS; j++) {
	    dungeon[i][j] = NOTHING;
	}
    }
    detect_monster = see_invisible = 0;
    being_held = bear_trap = 0;
    party_room = NO_ROOM;
    rogue.row = rogue.col = -1;
    clear();
}

void
put_door(room *rm, short dir, short *row, short *col)
{
    short wall_width;

    wall_width = (rm->is_room & R_MAZE) ? 0 : 1;

    switch (dir) {
    case UPWARD:
    case DOWN:
	*row = ((dir == UPWARD) ? rm->top_row : rm->bottom_row);
	do {
	    *col = get_rand(rm->left_col + wall_width,
			    rm->right_col - wall_width);
	} while (!(dungeon[*row][*col] & (HORWALL | TUNNEL)));
	break;
    case RIGHT:
    case LEFT:
	*col = (dir == LEFT) ? rm->left_col : rm->right_col;
	do {
	    *row = get_rand(rm->top_row + wall_width,
			    rm->bottom_row - wall_width);
	} while (!(dungeon[*row][*col] & (VERTWALL | TUNNEL)));
	break;
    }
    if (rm->is_room & R_ROOM) {
	dungeon[*row][*col] = DOOR;
    }
    if ((cur_level > 2) && rand_percent(HIDE_PERCENT)) {
	dungeon[*row][*col] |= HIDDEN;
    }
    rm->doors[dir / 2].door_row = *row;
    rm->doors[dir / 2].door_col = *col;
}

void
draw_simple_passage(short row1, short col1, short row2, short col2, short dir)
{
    short i;
    short middle, t;

    if ((dir == LEFT) || (dir == RIGHT)) {
	if (col1 > col2) {
	    swap(row1, row2);
	    swap(col1, col2);
	}
	middle = get_rand(col1 + 1, col2 - 1);
	for (i = col1 + 1; i != middle; i++) {
	    dungeon[row1][i] = TUNNEL;
	}
	for (i = row1; i != row2; i += (row1 > row2) ? -1 : 1) {
	    dungeon[i][middle] = TUNNEL;
	}
	for (i = middle; i != col2; i++) {
	    dungeon[row2][i] = TUNNEL;
	}
    } else {
	if (row1 > row2) {
	    swap(row1, row2);
	    swap(col1, col2);
	}
	middle = get_rand(row1 + 1, row2 - 1);
	for (i = row1 + 1; i != middle; i++) {
	    dungeon[i][col1] = TUNNEL;
	}
	for (i = col1; i != col2; i += (col1 > col2) ? -1 : 1) {
	    dungeon[middle][i] = TUNNEL;
	}
	for (i = middle; i != row2; i++) {
	    dungeon[i][col2] = TUNNEL;
	}
    }
    if (rand_percent(HIDE_PERCENT)) {
	hide_boxed_passage(row1, col1, row2, col2, 1);
    }
}

int
same_row(int room1, int room2)
{
    return ((room1 / 3) == (room2 / 3));
}

int
same_col(int room1, int room2)
{
    return ((room1 % 3) == (room2 % 3));
}

void
add_mazes(void)
{
    short i, j;
    short start;
    short maze_percent;

    if (cur_level > 1) {
	start = get_rand(0, (MAXROOMS - 1));
	maze_percent = (cur_level * 5) / 4;

	if (cur_level > 15) {
	    maze_percent += cur_level;
	}
	for (i = 0; i < MAXROOMS; i++) {
	    j = ((start + i) % MAXROOMS);
	    if (rooms[j].is_room & R_NOTHING) {
		if (rand_percent(maze_percent)) {
		    rooms[j].is_room = R_MAZE;
		    make_maze(get_rand
			      (rooms[j].top_row + 1, rooms[j].bottom_row - 1),
			      get_rand(rooms[j].left_col + 1,
				       rooms[j].right_col - 1),
			      rooms[j].top_row, rooms[j].bottom_row,
			      rooms[j].left_col, rooms[j].right_col);
		    hide_boxed_passage(rooms[j].top_row, rooms[j].left_col,
				       rooms[j].bottom_row, rooms[j].right_col,
				       get_rand(0, 2));
		}
	    }
	}
    }
}

void
fill_out_level(void)
{
    short i, rn;

    mix_random_rooms();

    r_de = NO_ROOM;

    for (i = 0; i < MAXROOMS; i++) {
	rn = random_rooms[i];
	if ((rooms[rn].is_room & R_NOTHING) ||
	    ((rooms[rn].is_room & R_CROSS) && coin_toss())) {
	    fill_it(rn, 1);
	}
    }
    if (r_de != NO_ROOM) {
	fill_it(r_de, 0);
    }
}

void
fill_it(int rn, boolean do_rec_de)
{
    short i;
    short tunnel_dir, door_dir, drow, dcol;
    short target_room, rooms_found = 0;
    short srow, scol, t;
    static short offsets[4] = { -1, 1, 3, -3 };
    boolean did_this = 0;

    for (i = 0; i < 10; i++) {
	srow = get_rand(0, 3);
	scol = get_rand(0, 3);
	t = offsets[srow];
	offsets[srow] = offsets[scol];
	offsets[scol] = t;
    }
    for (i = 0; i < 4; i++) {

	target_room = rn + offsets[i];

	if (((target_room < 0) || (target_room >= MAXROOMS)) ||
	    (!(same_row(rn, target_room) || same_col(rn, target_room))) ||
	    (!(rooms[target_room].is_room & (R_ROOM | R_MAZE)))) {
	    continue;
	}
	if (same_row(rn, target_room)) {
	    tunnel_dir = (rooms[rn].left_col < rooms[target_room].left_col) ?
		RIGHT : LEFT;
	} else {
	    tunnel_dir = (rooms[rn].top_row < rooms[target_room].top_row) ?
		DOWN : UPWARD;
	}
	door_dir = ((tunnel_dir + 4) % DIRS);
	if (rooms[target_room].doors[door_dir / 2].oth_room != NO_ROOM) {
	    continue;
	}
	if (((!do_rec_de) || did_this) ||
	    (!mask_room(rn, &srow, &scol, TUNNEL))) {
	    srow = (rooms[rn].top_row + rooms[rn].bottom_row) / 2;
	    scol = (rooms[rn].left_col + rooms[rn].right_col) / 2;
	}
	put_door(&rooms[target_room], door_dir, &drow, &dcol);
	rooms_found++;
	draw_simple_passage(srow, scol, drow, dcol, tunnel_dir);
	rooms[rn].is_room = R_DEADEND;
	dungeon[srow][scol] = TUNNEL;

	if ((i < 3) && (!did_this)) {
	    did_this = 1;
	    if (coin_toss()) {
		continue;
	    }
	}
	if ((rooms_found < 2) && do_rec_de) {
	    recursive_deadend(rn, offsets, srow, scol);
	}
	break;
    }
}

void
recursive_deadend(short rn, short *offsets, short srow, short scol)
{
    short i, de;
    short drow, dcol, tunnel_dir;

    rooms[rn].is_room = R_DEADEND;
    dungeon[srow][scol] = TUNNEL;

    for (i = 0; i < 4; i++) {
	de = rn + offsets[i];
	if (((de < 0) || (de >= MAXROOMS)) ||
	    (!(same_row(rn, de) || same_col(rn, de)))) {
	    continue;
	}
	if (!(rooms[de].is_room & R_NOTHING)) {
	    continue;
	}
	drow = (rooms[de].top_row + rooms[de].bottom_row) / 2;
	dcol = (rooms[de].left_col + rooms[de].right_col) / 2;
	if (same_row(rn, de)) {
	    tunnel_dir = (rooms[rn].left_col < rooms[de].left_col) ?
		RIGHT : LEFT;
	} else {
	    tunnel_dir = (rooms[rn].top_row < rooms[de].top_row) ?
		DOWN : UPWARD;
	}
	draw_simple_passage(srow, scol, drow, dcol, tunnel_dir);
	r_de = de;
	recursive_deadend(de, offsets, drow, dcol);
    }
}

boolean
mask_room(short rn, short *row, short *col, unsigned short mask)
{
    short i, j;

    for (i = rooms[rn].top_row; i <= rooms[rn].bottom_row; i++) {
	for (j = rooms[rn].left_col; j <= rooms[rn].right_col; j++) {
	    if (dungeon[i][j] & mask) {
		*row = i;
		*col = j;
		return 1;
	    }
	}
    }
    return 0;
}

void
make_maze(short r, short c, short tr, short br, short lc, short rc)
{
    char dirs[4];
    short i, t;

    dirs[0] = UPWARD;
    dirs[1] = DOWN;
    dirs[2] = LEFT;
    dirs[3] = RIGHT;

    dungeon[r][c] = TUNNEL;

    if (rand_percent(33)) {
	for (i = 0; i < 10; i++) {
	    short t1, t2;

	    t1 = get_rand(0, 3);
	    t2 = get_rand(0, 3);

	    swap(dirs[t1], dirs[t2]);
	}
    }
    for (i = 0; i < 4; i++) {
	switch (dirs[i]) {
	case UPWARD:
	    if (((r - 1) >= tr) &&
		(dungeon[r - 1][c] != TUNNEL) &&
		(dungeon[r - 1][c - 1] != TUNNEL) &&
		(dungeon[r - 1][c + 1] != TUNNEL) &&
		(dungeon[r - 2][c] != TUNNEL)) {
		make_maze((r - 1), c, tr, br, lc, rc);
	    }
	    break;
	case DOWN:
	    if (((r + 1) <= br) &&
		(dungeon[r + 1][c] != TUNNEL) &&
		(dungeon[r + 1][c - 1] != TUNNEL) &&
		(dungeon[r + 1][c + 1] != TUNNEL) &&
		(dungeon[r + 2][c] != TUNNEL)) {
		make_maze((r + 1), c, tr, br, lc, rc);
	    }
	    break;
	case LEFT:
	    if (((c - 1) >= lc) &&
		(dungeon[r][c - 1] != TUNNEL) &&
		(dungeon[r - 1][c - 1] != TUNNEL) &&
		(dungeon[r + 1][c - 1] != TUNNEL) &&
		(dungeon[r][c - 2] != TUNNEL)) {
		make_maze(r, (c - 1), tr, br, lc, rc);
	    }
	    break;
	case RIGHT:
	    if (((c + 1) <= rc) &&
		(dungeon[r][c + 1] != TUNNEL) &&
		(dungeon[r - 1][c + 1] != TUNNEL) &&
		(dungeon[r + 1][c + 1] != TUNNEL) &&
		(dungeon[r][c + 2] != TUNNEL)) {
		make_maze(r, (c + 1), tr, br, lc, rc);
	    }
	    break;
	}
    }
}

void
hide_boxed_passage(short row1, short col1, short row2, short col2, short n)
{
    short i, j, t;
    short row, col, row_cut, col_cut;
    short h, w;

    if (cur_level > 2) {
	if (row1 > row2) {
	    swap(row1, row2);
	}
	if (col1 > col2) {
	    swap(col1, col2);
	}
	h = row2 - row1;
	w = col2 - col1;

	if ((w >= 5) || (h >= 5)) {
	    row_cut = ((h >= 2) ? 1 : 0);
	    col_cut = ((w >= 2) ? 1 : 0);

	    for (i = 0; i < n; i++) {
		for (j = 0; j < 10; j++) {
		    row = get_rand(row1 + row_cut, row2 - row_cut);
		    col = get_rand(col1 + col_cut, col2 - col_cut);
		    if (dungeon[row][col] == TUNNEL) {
			dungeon[row][col] |= HIDDEN;
			break;
		    }
		}
	    }
	}
    }
}

void
put_player(short nr)
{				/* try not to put in this room */
    short rn = nr, misses;
    short row, col;

    for (misses = 0; ((misses < 2) && (rn == nr)); misses++) {
	gr_row_col(&row, &col, (FLOOR | TUNNEL | OBJECT | STAIRS));
	rn = get_room_number(row, col);
    }
    rogue.row = row;
    rogue.col = col;

    if (dungeon[rogue.row][rogue.col] & TUNNEL) {
	cur_room = PASSAGE;
    } else {
	cur_room = rn;
    }
    if (cur_room != PASSAGE) {
	light_up_room(cur_room);
    } else {
	light_passage(rogue.row, rogue.col);
    }
    wake_room(get_room_number(rogue.row, rogue.col), 1, rogue.row, rogue.col);
    if (new_level_message) {
	message(new_level_message, 0);
	new_level_message = 0;
    }
    mvaddch_rogue(rogue.row, rogue.col, rogue.fchar);
}

int
drop_check(void)
{
    if (wizard) {
	return 1;
    }
    if (dungeon[rogue.row][rogue.col] & STAIRS) {
	if (levitate) {
	    message(mesg[48], 0);
	    return 0;
	}
	return 1;
    }
    message(mesg[49], 0);
    return 0;
}

int
check_up(void)
{
    if (!wizard) {
	if (!(dungeon[rogue.row][rogue.col] & STAIRS)) {
	    message(mesg[50], 0);
	    return 0;
	}
	if (!has_amulet()) {
	    message(mesg[51], 0);
	    return 0;
	}
    }
    new_level_message = mesg[52];
    if (cur_level == 1) {
	win();
    } else {
	cur_level -= 2;
	return 1;
    }
    return 0;
}

void
add_exp(int e, boolean promotion)
{
    char mbuf[40];
    short new_exp;
    short i, hp;
#if defined( JAPAN )
    char bf[8];
#endif /* JAPAN */

    rogue.exp_points += e;

    if (rogue.exp_points >= level_points[rogue.exp - 1]) {
	new_exp = get_exp_level(rogue.exp_points);
	if (rogue.exp_points > MAX_EXP) {
	    rogue.exp_points = MAX_EXP + 1;
	}
	for (i = rogue.exp + 1; i <= new_exp; i++) {
#if defined( JAPAN )
	    bf[0] = '\0';
	    znum(bf, i, 0);
	    sprintf(mbuf, mesg[53], bf);
#else /* not JAPAN */
	    sprintf(mbuf, mesg[53], i);
#endif /* not JAPAN */
	    message(mbuf, 0);
	    if (promotion) {
		hp = hp_raise();
		rogue.hp_current += hp;
		rogue.hp_max += hp;
	    }
	    rogue.exp = i;
	    print_stats(STAT_HP | STAT_EXP);
	}
    } else {
	print_stats(STAT_EXP);
    }
}

int
get_exp_level(long e)
{
    short i;

    for (i = 0; i < (MAX_EXP_LEVEL - 1); i++) {
	if (level_points[i] > e) {
	    break;
	}
    }
    return (i + 1);
}

int
hp_raise(void)
{
    int hp;

    hp = (wizard ? 10 : get_rand(3, 10));
    return hp;
}

void
show_average_hp(void)
{
    char mbuf[80];
    long real_average;
    long effective_average;

    if (rogue.exp == 1) {
	real_average = effective_average = 0L;
    } else {
	real_average = ((rogue.hp_max - extra_hp - INIT_HP) + less_hp)
	    * 100L / (rogue.exp - 1);
	effective_average = (rogue.hp_max - INIT_HP)
	    * 100L / (rogue.exp - 1);
    }
    sprintf(mbuf, mesg[54],
	    (int) (real_average / 100L), (int) (real_average % 100L),
	    (int) (effective_average / 100L), (int) (effective_average % 100L),
	    extra_hp, less_hp);
    message(mbuf, 0);
}

void
mix_random_rooms(void)
{
    short i, t;
    short j;

    for (i = 0; i < MAXROOMS; i++) {
	j = get_rand(i, MAXROOMS - 1);
	swap(random_rooms[i], random_rooms[j]);
    }
}
