/*
 * room.c
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
#include "room.h"
#include "display.h"
#include "monster.h"
#include "move.h"
#include "object.h"
#include "random.h"
#include "spechit.h"

room rooms[MAXROOMS];
boolean rooms_visited[MAXROOMS];

extern short blind;
extern boolean detect_monster;

void
light_up_room(int rn)
{
    short i, j;

    if (!blind) {
	for (i = rooms[rn].top_row; i <= rooms[rn].bottom_row; i++) {
	    for (j = rooms[rn].left_col; j <= rooms[rn].right_col; j++) {
		if (dungeon[i][j] & MONSTER) {
		    object *monster;

		    if ((monster = object_at(&level_monsters, i, j))) {
			dungeon[monster->row][monster->col] &= (~MONSTER);
			monster->trail_char =
			    get_dungeon_char(monster->row, monster->col);
			dungeon[monster->row][monster->col] |= MONSTER;
		    }
		}
		mvaddch_rogue(i, j, get_dungeon_char(i, j));
	    }
	}
	mvaddch_rogue(rogue.row, rogue.col, rogue.fchar);
    }
}

void
light_passage(int row, int col)
{
    short i, j, i_end, j_end;

    if (blind) {
	return;
    }
    i_end = (row < (ROGUE_LINES - 2)) ? 1 : 0;
    j_end = (col < (ROGUE_COLUMNS - 1)) ? 1 : 0;

    for (i = ((row > MIN_ROW) ? -1 : 0); i <= i_end; i++) {
	for (j = ((col > 0) ? -1 : 0); j <= j_end; j++) {
	    if (can_move(row, col, row + i, col + j)) {
		mvaddch_rogue(row + i, col + j,
			      get_dungeon_char(row + i, col + j));
	    }
	}
    }
}

void
darken_room(short rn)
{
    short i, j;

    for (i = rooms[rn].top_row + 1; i < rooms[rn].bottom_row; i++) {
	for (j = rooms[rn].left_col + 1; j < rooms[rn].right_col; j++) {
	    if (blind) {
		mvaddch_rogue(i, j, ' ');
	    } else {
		if (!(dungeon[i][j] & (OBJECT | STAIRS)) &&
		    !(detect_monster && (dungeon[i][j] & MONSTER))) {
		    if (!imitating(i, j)) {
			mvaddch_rogue(i, j, ' ');
		    }
		    if ((dungeon[i][j] & TRAP) && (!(dungeon[i][j] & HIDDEN))) {
			mvaddch_rogue(i, j, '^');
		    }
		}
	    }
	}
    }
}

int
get_dungeon_char(int row, int col)
{
    unsigned short mask = dungeon[row][col];

    if (mask & MONSTER) {
	return gmc_row_col(row, col);
    }
    if (mask & OBJECT) {
	object *obj;

	obj = object_at(&level_objects, row, col);
	return get_mask_char(obj->what_is);
    }
    if (mask & (TUNNEL | STAIRS | HORWALL | VERTWALL | FLOOR | DOOR)) {
	if ((mask & (TUNNEL | STAIRS)) && (!(mask & HIDDEN))) {
	    return ((mask & STAIRS) ? '%' : '#');
	}
	if (mask & HORWALL) {
	    return '-';
	}
	if (mask & VERTWALL) {
	    return '|';
	}
	if (mask & FLOOR) {
	    if (mask & TRAP) {
		if (!(dungeon[row][col] & HIDDEN)) {
		    return '^';
		}
	    }
	    return '.';
	}
	if (mask & DOOR) {
	    if (mask & HIDDEN) {
		if (((col > 0) && (dungeon[row][col - 1] & HORWALL)) ||
		    ((col < (ROGUE_COLUMNS - 1))
		     && (dungeon[row][col + 1] & HORWALL))) {
		    return '-';
		} else {
		    return '|';
		}
	    } else {
		return '+';
	    }
	}
    }
    return ' ';
}

int
get_mask_char(unsigned short mask)
{
    switch (mask) {
    case SCROL:
	return '?';
    case POTION:
	return '!';
    case GOLD:
	return '*';
    case FOOD:
	return ':';
    case WAND:
	return '/';
    case ARMOR:
	return ']';
    case WEAPON:
	return ')';
    case RING:
	return '=';
    case AMULET:
	return ',';
    default:
	return '~';		/* unknown, something is wrong */
    }
}

void
gr_row_col(short *row, short *col, unsigned short mask)
{
    short rn;
    short r, c;

    do {
	r = get_rand(MIN_ROW, ROGUE_LINES - 2);
	c = get_rand(0, ROGUE_COLUMNS - 1);
	rn = get_room_number(r, c);
    } while ((rn == NO_ROOM) ||
	     (!(dungeon[r][c] & mask)) ||
	     (dungeon[r][c] & (~mask)) ||
	     (!(rooms[rn].is_room & (R_ROOM | R_MAZE))) ||
	     ((r == rogue.row) && (c == rogue.col)));

    *row = r;
    *col = c;
}

int
gr_room(void)
{
    short i;

    do {
	i = get_rand(0, MAXROOMS - 1);
    } while (!(rooms[i].is_room & (R_ROOM | R_MAZE)));

    return i;
}

int
party_objects(int rn)
{
    short i, j, nf = 0;
    object *obj;
    short n, N, row, col;
    boolean found;

    N = ((rooms[rn].bottom_row - rooms[rn].top_row) - 1) *
	((rooms[rn].right_col - rooms[rn].left_col) - 1);
    n = get_rand(5, 10);
    if (n > N) {
	n = N - 2;
    }
    for (i = 0; i < n; i++) {
	for (j = found = 0; ((!found) && (j < 250)); j++) {
	    row = get_rand(rooms[rn].top_row + 1, rooms[rn].bottom_row - 1);
	    col = get_rand(rooms[rn].left_col + 1, rooms[rn].right_col - 1);
	    if ((dungeon[row][col] == FLOOR) || (dungeon[row][col] == TUNNEL)) {
		found = 1;
	    }
	}
	if (found) {
	    obj = gr_object();
	    place_at(obj, row, col);
	    nf++;
	}
    }
    return nf;
}

int
get_room_number(int row, int col)
{
    short i;

    for (i = 0; i < MAXROOMS; i++) {
	if ((row >= rooms[i].top_row) && (row <= rooms[i].bottom_row) &&
	    (col >= rooms[i].left_col) && (col <= rooms[i].right_col)) {
	    return i;
	}
    }
    return (NO_ROOM);
}

int
is_all_connected(void)
{
    short i, starting_room = 0;	/* 未初期化変数の警告除去のため 0 で初期化 */

    for (i = 0; i < MAXROOMS; i++) {
	rooms_visited[i] = 0;
	if (rooms[i].is_room & (R_ROOM | R_MAZE)) {
	    starting_room = i;
	}
    }

    visit_rooms(starting_room);

    for (i = 0; i < MAXROOMS; i++) {
	if ((rooms[i].is_room & (R_ROOM | R_MAZE)) && (!rooms_visited[i])) {
	    return 0;
	}
    }
    return 1;
}

void
visit_rooms(int rn)
{
    short i;
    short oth_rn;

    rooms_visited[rn] = 1;

    for (i = 0; i < 4; i++) {
	oth_rn = rooms[rn].doors[i].oth_room;
	if ((oth_rn >= 0) && (!rooms_visited[oth_rn])) {
	    visit_rooms(oth_rn);
	}
    }
}

void
draw_magic_map(void)
{
    short i, j, ch, och;
    unsigned short mask = (HORWALL | VERTWALL | DOOR | TUNNEL | TRAP | STAIRS |
			   MONSTER);
    unsigned short s;

    for (i = 0; i < ROGUE_LINES; i++) {
	for (j = 0; j < ROGUE_COLUMNS; j++) {
	    s = dungeon[i][j];
	    if (s & mask) {
		ch = mvinch_rogue(i, j);
		if ((ch == ' ') ||
		    ((ch >= 'A') && (ch <= 'Z')) || (s & (TRAP | HIDDEN))) {
		    och = ch;
		    dungeon[i][j] &= (~HIDDEN);
		    if (s & HORWALL) {
			ch = '-';
		    } else if (s & VERTWALL) {
			ch = '|';
		    } else if (s & DOOR) {
			ch = '+';
		    } else if (s & TRAP) {
			ch = '^';
		    } else if (s & STAIRS) {
			ch = '%';
		    } else if (s & TUNNEL) {
			ch = '#';
		    } else {
			continue;
		    }
		    if ((!(s & MONSTER)) || (och == ' ')) {
			addch_rogue(ch);
		    }
		    if (s & MONSTER) {
			object *monster;

			if ((monster = object_at(&level_monsters, i, j))) {
			    monster->trail_char = ch;
			}
		    }
		}
	    }
	}
    }
}

void
dr_course(object *monster, boolean entering, short row, short col)
{
    short i, j, k, rn;
    short r, rr;

    monster->row = row;
    monster->col = col;

    if (mon_sees(monster, rogue.row, rogue.col)) {
	monster->trow = NO_ROOM;
	return;
    }
    rn = get_room_number(row, col);

    if (entering) {		/* entering room */
	/* look for door to some other room */
	r = get_rand(0, MAXROOMS - 1);
	for (i = 0; i < MAXROOMS; i++) {
	    rr = (r + i) % MAXROOMS;
	    if ((!(rooms[rr].is_room & (R_ROOM | R_MAZE))) || (rr == rn)) {
		continue;
	    }
	    for (k = 0; k < 4; k++) {
		if (rooms[rr].doors[k].oth_room == rn) {
		    monster->trow = rooms[rr].doors[k].oth_row;
		    monster->tcol = rooms[rr].doors[k].oth_col;
		    if ((monster->trow == row) && (monster->tcol == col)) {
			continue;
		    }
		    return;
		}
	    }
	}
	/* look for door to dead end */
	for (i = rooms[rn].top_row; i <= rooms[rn].bottom_row; i++) {
	    for (j = rooms[rn].left_col; j <= rooms[rn].right_col; j++) {
		if ((i != monster->row) && (j != monster->col) &&
		    (dungeon[i][j] & DOOR)) {
		    monster->trow = i;
		    monster->tcol = j;
		    return;
		}
	    }
	}
	/* return monster to room that he came from */
	for (i = 0; i < MAXROOMS; i++) {
	    for (j = 0; j < 4; j++) {
		if (rooms[i].doors[j].oth_room == rn) {
		    for (k = 0; k < 4; k++) {
			if (rooms[rn].doors[k].oth_room == i) {
			    monster->trow = rooms[rn].doors[k].oth_row;
			    monster->tcol = rooms[rn].doors[k].oth_col;
			    return;
			}
		    }
		}
	    }
	}
	/* no place to send monster */
	monster->trow = -1;
    } else {			/* exiting room */
	if (!get_oth_room(rn, &row, &col)) {
	    monster->trow = NO_ROOM;
	} else {
	    monster->trow = row;
	    monster->tcol = col;
	}
    }
}

int
get_oth_room(short rn, short *row, short *col)
{
    short d = -1;

    if (*row == rooms[rn].top_row) {
	d = UPWARD / 2;
    } else if (*row == rooms[rn].bottom_row) {
	d = DOWN / 2;
    } else if (*col == rooms[rn].left_col) {
	d = LEFT / 2;
    } else if (*col == rooms[rn].right_col) {
	d = RIGHT / 2;
    }
    if ((d != -1) && (rooms[rn].doors[d].oth_room >= 0)) {
	*row = rooms[rn].doors[d].oth_row;
	*col = rooms[rn].doors[d].oth_col;
	return 1;
    }
    return 0;
}
