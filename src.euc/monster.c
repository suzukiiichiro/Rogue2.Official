/*
 * monster.c
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
#include "monster.h"
#include "display.h"
#include "hit.h"
#include "message.h"
#include "move.h"
#include "object.h"
#include "pack.h"
#include "random.h"
#include "room.h"
#include "spechit.h"
#include "throw.h"

object level_monsters;
boolean mon_disappeared;

char *m_names[] = {
    mesg[307], mesg[308], mesg[309], mesg[310], mesg[311], mesg[312],
    mesg[313], mesg[314], mesg[315], mesg[316], mesg[317], mesg[318],
    mesg[319], mesg[320], mesg[321], mesg[322], mesg[323], mesg[324],
    mesg[325], mesg[326], mesg[327], mesg[328], mesg[329], mesg[330],
    mesg[331], mesg[332]
};

object mon_tab[MONSTERS] = {
    {(ASLEEP | WAKENS | WANDERS | RUSTS), "0d0", 25, 'A', 20, 9, 18, 100, 0, 0, 0, 0, 0},
    {(ASLEEP | WANDERS | FLITS), "1d3", 10, 'B', 2, 1, 8, 60, 0, 0, 0, 0, 0},
    {(ASLEEP | WANDERS), "3d3/2d5", 32, 'C', 15, 7, 16, 85, 0, 10, 0, 0, 0},
    {(ASLEEP | WAKENS | FLAMES), "4d6/4d9", 145, 'D', 5000, 21, 126, 100, 0, 90, 0, 0, 0},
    {(ASLEEP | WAKENS), "1d3", 11, 'E', 2, 1, 7, 65, 0, 0, 0, 0, 0},
    {(HOLDS | STATIONARY), "5d5", 73, 'F', 91, 12, 126, 80, 0, 0, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS | FLIES), "5d5/5d5", 115, 'G', 2000, 20, 126, 85, 0, 10, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS), "1d3/1d2", 15, 'H', 3, 1, 10, 67, 0, 0, 0, 0, 0},
    {(ASLEEP | FREEZES), "0d0", 15, 'I', 5, 2, 11, 68, 0, 0, 0, 0, 0},
    {(ASLEEP | WANDERS), "3d10/4d5", 132, 'J', 3000, 21, 126, 100, 0, 0, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS | FLIES), "1d4", 10, 'K', 2, 1, 6, 60, 0, 0, 0, 0, 0},
    {(ASLEEP | STEALS_GOLD), "0d0", 25, 'L', 21, 6, 16, 75, 0, 0, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS | CONFUSES), "4d4/3d7", 97, 'M', 250, 18, 126, 85, 0, 25, 0, 0, 0},
    {(ASLEEP | STEALS_ITEM), "0d0", 25, 'N', 39, 10, 19, 75, 0, 100, 0, 0, 0},
    {(ASLEEP | WANDERS | WAKENS | SEEKS_GOLD), "1d6", 25, 'O', 5, 4, 13, 70, 0, 10, 0, 0, 0},
    {(ASLEEP | INVISIBLE | WANDERS | FLITS), "5d4", 76, 'P', 120, 15, 24, 80, 0, 50, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS), "3d5", 30, 'Q', 20, 8, 17, 78, 0, 20, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS | STINGS), "2d5", 19, 'R', 10, 3, 12, 70, 0, 0, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS), "1d3", 8, 'S', 2, 1, 9, 50, 0, 0, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS), "4d6/1d4", 75, 'T', 125, 13, 22, 75, 0, 33, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS), "4d10", 90, 'U', 200, 17, 26, 85, 0, 33, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS | DRAINS_LIFE), "1d14/1d4", 55, 'V', 350, 19, 126, 85, 0, 18, 0, 0, 0},
    {(ASLEEP | WANDERS | DROPS_LEVEL), "2d8", 45, 'W', 55, 14, 23, 75, 0, 0, 0, 0, 0},
    {(ASLEEP | IMITATES), "4d6", 42, 'X', 110, 16, 25, 75, 0, 0, 0, 0, 0},
    {(ASLEEP | WANDERS), "3d6", 35, 'Y', 50, 11, 20, 80, 0, 20, 0, 0, 0},
    {(ASLEEP | WAKENS | WANDERS), "1d7", 21, 'Z', 8, 5, 14, 69, 0, 0, 0, 0, 0}
};

extern short cur_level;
extern short cur_room, party_room;
extern short blind, halluc, haste_self;
extern boolean detect_monster, see_invisible, r_see_invisible;
extern short stealthy;

void
put_mons(void)
{
    short i;
    short n;
    object *monster;
    short row, col;

    n = get_rand(4, 6);

    for (i = 0; i < n; i++) {
	monster = gr_monster((object *) 0, 0);
	if ((monster->m_flags & WANDERS) && coin_toss()) {
	    wake_up(monster);
	}
	gr_row_col(&row, &col, (FLOOR | TUNNEL | STAIRS | OBJECT));
	put_m_at(row, col, monster);
    }
}

object *
gr_monster(object *monster, int mn)
{
    if (!monster) {
	monster = alloc_object();

	for (;;) {
	    mn = get_rand(0, MONSTERS - 1);
	    if ((cur_level >= mon_tab[mn].first_level) &&
		(cur_level <= mon_tab[mn].last_level)) {
		break;
	    }
	}
    }
    *monster = mon_tab[mn];
    if (monster->m_flags & IMITATES) {
	monster->disguise = gr_obj_char();
    }
    if (cur_level > (AMULET_LEVEL + 2)) {
	monster->m_flags |= HASTED;
    }
    monster->trow = NO_ROOM;
    return monster;
}

void
mv_mons(void)
{
    object *monster, *next_monster;
    boolean flew;

    if (haste_self % 2) {
	return;
    }

    monster = level_monsters.next_monster;

    while (monster) {
	next_monster = monster->next_monster;
	if (monster->m_flags & HASTED) {
	    mon_disappeared = 0;
	    mv_monster(monster, rogue.row, rogue.col);
	    if (mon_disappeared) {
		goto NM;
	    }
	} else if (monster->m_flags & SLOWED) {
	    monster->slowed_toggle = !monster->slowed_toggle;
	    if (monster->slowed_toggle) {
		goto NM;
	    }
	}
	if ((monster->m_flags & CONFUSED) && move_confused(monster)) {
	    goto NM;
	}
	flew = 0;
	if ((monster->m_flags & FLIES) && !(monster->m_flags & NAPPING)
	    && !mon_can_go(monster, rogue.row, rogue.col)) {
	    flew = 1;
	    mv_monster(monster, rogue.row, rogue.col);
	}
	if (!(flew && mon_can_go(monster, rogue.row, rogue.col))) {
	    mv_monster(monster, rogue.row, rogue.col);
	}
    NM:
	monster = next_monster;
    }
}

void
party_monsters(int rn, int n)
{
    short i, j;
    short row, col;
    object *monster;
    boolean found;

    n += n;

    for (i = 0; i < MONSTERS; i++) {
	mon_tab[i].first_level -= (cur_level % 3);
    }
    for (i = 0; i < n; i++) {
	if (no_room_for_monster(rn)) {
	    break;
	}
	for (j = found = 0; ((!found) && (j < 250)); j++) {
	    row = get_rand(rooms[rn].top_row + 1, rooms[rn].bottom_row - 1);
	    col = get_rand(rooms[rn].left_col + 1, rooms[rn].right_col - 1);
	    if ((!(dungeon[row][col] & MONSTER)) &&
		(dungeon[row][col] & (FLOOR | TUNNEL))) {
		found = 1;
	    }
	}
	if (found) {
	    monster = gr_monster((object *) 0, 0);
	    if (!(monster->m_flags & IMITATES)) {
		monster->m_flags |= WAKENS;
	    }
	    put_m_at(row, col, monster);
	}
    }
    for (i = 0; i < MONSTERS; i++) {
	mon_tab[i].first_level += (cur_level % 3);
    }
}

int
gmc_row_col(int row, int col)
{
    object *monster;

    if ((monster = object_at(&level_monsters, row, col)))
	return gmc(monster);
    return '&';			/* BUG if this ever happens */
}

int
gmc(object *monster)
{
    if ((!(detect_monster || see_invisible || r_see_invisible) &&
	 (monster->m_flags & INVISIBLE)) || blind) {
	return monster->trail_char;
    }
    if (monster->m_flags & IMITATES) {
	return ((int) (monster->disguise));
    }
    return monster->m_char;
}

void
mv_monster(object *monster, short row, short col)
{
    short i, n;
    boolean tried[6];

    if (monster->m_flags & ASLEEP) {
	if (monster->m_flags & NAPPING) {
	    if (--monster->nap_length <= 0) {
		monster->m_flags &= (~(NAPPING | ASLEEP));
	    }
	    return;
	}
	if ((monster->m_flags & WAKENS) &&
	    rogue_is_around(monster->row, monster->col) &&
	    rand_percent(((stealthy > 0) ?
			  (WAKE_PERCENT / (STEALTH_FACTOR + stealthy)) :
			  WAKE_PERCENT))) {
	    wake_up(monster);
	}
	return;
    } else if (monster->m_flags & ALREADY_MOVED) {
	monster->m_flags &= (~ALREADY_MOVED);
	return;
    }
    if ((monster->m_flags & FLITS) && flit(monster)) {
	return;
    }
    if ((monster->m_flags & STATIONARY) &&
	(!mon_can_go(monster, rogue.row, rogue.col))) {
	return;
    }
    if (monster->m_flags & FREEZING_ROGUE) {
	return;
    }
    if ((monster->m_flags & CONFUSES) && m_confuse(monster)) {
	return;
    }
    if (mon_can_go(monster, rogue.row, rogue.col)) {
	mon_hit(monster, (char *) 0, 0);
	return;
    }
    if ((monster->m_flags & FLAMES) && flame_broil(monster)) {
	return;
    }
    if ((monster->m_flags & SEEKS_GOLD) && seek_gold(monster)) {
	return;
    }
    if ((monster->trow == monster->row) && (monster->tcol == monster->col)) {
	monster->trow = NO_ROOM;
    } else if (monster->trow != NO_ROOM) {
	row = monster->trow;
	col = monster->tcol;
    }
    if (monster->row > row) {
	row = monster->row - 1;
    } else if (monster->row < row) {
	row = monster->row + 1;
    }
    if ((dungeon[row][monster->col] & DOOR) &&
	mtry(monster, row, monster->col)) {
	return;
    }
    if (monster->col > col) {
	col = monster->col - 1;
    } else if (monster->col < col) {
	col = monster->col + 1;
    }
    if ((dungeon[monster->row][col] & DOOR) &&
	mtry(monster, monster->row, col)) {
	return;
    }
    if (mtry(monster, row, col)) {
	return;
    }

    for (i = 0; i <= 5; i++) {
	tried[i] = 0;
    }

    for (i = 0; i < 6; i++) {
	do {
	    n = get_rand(0, 5);
	} while (tried[n]);
	switch (n) {
	case 0:
	    if (mtry(monster, row, monster->col - 1)) {
		goto O;
	    }
	    break;
	case 1:
	    if (mtry(monster, row, monster->col)) {
		goto O;
	    }
	    break;
	case 2:
	    if (mtry(monster, row, monster->col + 1)) {
		goto O;
	    }
	    break;
	case 3:
	    if (mtry(monster, monster->row - 1, col)) {
		goto O;
	    }
	    break;
	case 4:
	    if (mtry(monster, monster->row, col)) {
		goto O;
	    }
	    break;
	case 5:
	    if (mtry(monster, monster->row + 1, col)) {
		goto O;
	    }
	    break;
	}
	tried[n] = 1;
    }
O:
    if ((monster->row == monster->o_row) && (monster->col == monster->o_col)) {
	if (++(monster->o) > 4) {
	    if ((monster->trow == NO_ROOM) &&
		(!mon_sees(monster, rogue.row, rogue.col))) {
		monster->trow = get_rand(1, (ROGUE_LINES - 2));
		monster->tcol = get_rand(0, (ROGUE_COLUMNS - 1));
	    } else {
		monster->trow = NO_ROOM;
		monster->o = 0;
	    }
	}
    } else {
	monster->o_row = monster->row;
	monster->o_col = monster->col;
	monster->o = 0;
    }
}

int
mtry(object *monster, short row, short col)
{
    if (mon_can_go(monster, row, col)) {
	move_mon_to(monster, row, col);
	return 1;
    }
    return 0;
}

void
move_mon_to(object *monster, short row, short col)
{
    short c;
    int mrow, mcol;

    mrow = monster->row;
    mcol = monster->col;

    dungeon[mrow][mcol] &= ~MONSTER;
    dungeon[row][col] |= MONSTER;

    c = mvinch_rogue(mrow, mcol);

    if ((c >= 'A') && (c <= 'Z')) {
	if (!detect_monster) {
	    mvaddch_rogue(mrow, mcol, monster->trail_char);
	} else {
	    if (rogue_can_see(mrow, mcol)) {
		mvaddch_rogue(mrow, mcol, monster->trail_char);
	    } else {
		if (monster->trail_char == '.') {
		    monster->trail_char = ' ';
		}
		mvaddch_rogue(mrow, mcol, monster->trail_char);
	    }
	}
    }
    monster->trail_char = mvinch_rogue(row, col);
    if (!blind && (detect_monster || rogue_can_see(row, col))) {
	if ((!(monster->m_flags & INVISIBLE) ||
	     (detect_monster || see_invisible || r_see_invisible))) {
	    mvaddch_rogue(row, col, gmc(monster));
	}
    }
    if ((dungeon[row][col] & DOOR) &&
	(get_room_number(row, col) != cur_room) &&
	(dungeon[mrow][mcol] == FLOOR) && !blind) {
	mvaddch_rogue(mrow, mcol, ' ');
    }
    if (dungeon[row][col] & DOOR) {
	dr_course(monster, ((dungeon[mrow][mcol] & TUNNEL) ? 1 : 0), row, col);
    } else {
	monster->row = row;
	monster->col = col;
    }
}

int
mon_can_go(object *monster, short row, short col)
{
    object *obj;
    short dr, dc;

    dr = monster->row - row;	/* check if move distance > 1 */
    dc = monster->col - col;
    if (dr >= 2 || dr <= -2 || dc >= 2 || dc <= -2) {
	return 0;
    }
    if (!dungeon[monster->row][col] || !dungeon[row][monster->col] ||
	!is_passable(row, col) || (dungeon[row][col] & MONSTER)) {
	return 0;
    }
    if ((monster->row != row) && (monster->col != col) &&
	((dungeon[row][col] & DOOR) ||
	 (dungeon[monster->row][monster->col] & DOOR))) {
	return 0;
    }
    if (!(monster->m_flags & (FLITS | CONFUSED | CAN_FLIT)) &&
	(monster->trow == NO_ROOM)) {
	if (((monster->row < rogue.row) && (row < monster->row)) ||
	    ((monster->row > rogue.row) && (row > monster->row)) ||
	    ((monster->col < rogue.col) && (col < monster->col)) ||
	    ((monster->col > rogue.col) && (col > monster->col)))
	    return 0;
    }
    if (dungeon[row][col] & OBJECT) {
	obj = object_at(&level_objects, row, col);
	if ((obj->what_is == SCROL) && (obj->which_kind == SCARE_MONSTER)) {
	    return 0;
	}
    }
    return 1;
}

void
wake_up(object *monster)
{
    if (!(monster->m_flags & NAPPING)) {
	monster->m_flags &= (~(ASLEEP | IMITATES | WAKENS));
    }
}

void
wake_room(short rn, boolean entering, short row, short col)
{
    object *monster;
    short wake_percent;
    boolean in_room;

    wake_percent = (rn == party_room) ? PARTY_WAKE_PERCENT : WAKE_PERCENT;
    if (stealthy > 0) {
	wake_percent /= (STEALTH_FACTOR + stealthy);
    }

    monster = level_monsters.next_monster;

    while (monster) {
	in_room = (rn == get_room_number(monster->row, monster->col));
	if (in_room) {
	    if (entering) {
		monster->trow = NO_ROOM;
	    } else {
		monster->trow = row;
		monster->tcol = col;
	    }
	}
	if ((monster->m_flags & WAKENS) &&
	    (rn == get_room_number(monster->row, monster->col))) {
	    if (rand_percent(wake_percent)) {
		wake_up(monster);
	    }
	}
	monster = monster->next_monster;
    }
}

char *
mon_name(object *monster)
{
    short ch;

    if (blind || ((monster->m_flags & INVISIBLE) &&
		  !(detect_monster || see_invisible || r_see_invisible))) {
	return mesg[63];
    }
    if (halluc) {
	ch = get_rand('A', 'Z') - 'A';
	return m_names[ch];
    }
    ch = monster->m_char - 'A';
    return m_names[ch];
}

int
rogue_is_around(short row, short col)
{
    short rdif, cdif;

    rdif = row - rogue.row;
    cdif = col - rogue.col;

    return ((rdif >= -1) && (rdif <= 1) && (cdif >= -1) && (cdif <= 1));
}

void
wanderer(void)
{
    object *monster;
    short row, col, i;
    boolean found = 0;

    for (i = 0; ((i < 15) && (!found)); i++) {
	monster = gr_monster((object *) 0, 0);
	if (!(monster->m_flags & (WAKENS | WANDERS))) {
	    free_object(monster);
	} else {
	    found = 1;
	}
    }
    if (found) {
	found = 0;
	wake_up(monster);
	for (i = 0; ((i < 25) && (!found)); i++) {
	    gr_row_col(&row, &col, (FLOOR | TUNNEL | STAIRS | OBJECT));
	    if (!rogue_can_see(row, col)) {
		put_m_at(row, col, monster);
		found = 1;
	    }
	}
	if (!found) {
	    free_object(monster);
	}
    }
}

void
show_monsters(void)
{
    object *monster;

    detect_monster = 1;

    if (blind) {
	return;
    }
    monster = level_monsters.next_monster;

    while (monster) {
	mvaddch_rogue(monster->row, monster->col, monster->m_char);
	if (monster->m_flags & IMITATES) {
	    monster->m_flags &= (~IMITATES);
	    monster->m_flags |= WAKENS;
	}
	monster = monster->next_monster;
    }
}

void
create_monster(void)
{
    short row, col;
    short r, c;
    short i;
    boolean found = 0;
    object *monster;

    r = rogue.row;
    c = rogue.col;

    for (i = 0; i < 9; i++) {
	rand_around(i, &r, &c);
	row = r;
	col = c;
	if (((row == rogue.row) && (col == rogue.col)) ||
	    (row < MIN_ROW) || (row > (ROGUE_LINES - 2)) ||
	    (col < 0) || (col > (ROGUE_COLUMNS - 1))) {
	    continue;
	}
	if ((!(dungeon[row][col] & MONSTER)) &&
	    (dungeon[row][col] & (FLOOR | TUNNEL | STAIRS | DOOR))) {
	    found = 1;
	    break;
	}
    }
    if (found) {
	monster = gr_monster((object *) 0, 0);
	put_m_at(row, col, monster);
	mvaddch_rogue(row, col, gmc(monster));
	if (monster->m_flags & (WANDERS | WAKENS)) {
	    wake_up(monster);
	}
    } else {
	message(mesg[64], 0);
    }
}

void
put_m_at(short row, short col, object *monster)
{
    monster->row = row;
    monster->col = col;
    dungeon[row][col] |= MONSTER;
    monster->trail_char = mvinch_rogue(row, col);
    (void) add_to_pack(monster, &level_monsters, 0);
    aim_monster(monster);
}

void
aim_monster(object *monster)
{
    short i, rn, d, r;

    rn = get_room_number(monster->row, monster->col);
    r = get_rand(0, 12);

    for (i = 0; i < 4; i++) {
	d = (r + i) % 4;
	if (rooms[rn].doors[d].oth_room != NO_ROOM) {
	    monster->trow = rooms[rn].doors[d].door_row;
	    monster->tcol = rooms[rn].doors[d].door_col;
	    break;
	}
    }
}

int
rogue_can_see(int row, int col)
{
    return (!blind && (((get_room_number(row, col) == cur_room) &&
			!(rooms[cur_room].is_room & R_MAZE)) ||
		       rogue_is_around(row, col)));
}

int
move_confused(object *monster)
{
    short i, row, col;

    if (!(monster->m_flags & ASLEEP)) {
	if (--monster->moves_confused <= 0) {
	    monster->m_flags &= (~CONFUSED);
	}
	if (monster->m_flags & STATIONARY) {
	    return (coin_toss()? 1 : 0);
	} else if (rand_percent(15)) {
	    return 1;
	}
	row = monster->row;
	col = monster->col;

	for (i = 0; i < 9; i++) {
	    rand_around(i, &row, &col);
	    if ((row == rogue.row) && (col == rogue.col)) {
		return 0;
	    }
	    if (mtry(monster, row, col)) {
		return 1;
	    }
	}
    }
    return 0;
}

int
flit(object *monster)
{
    short i, row, col;

    if (!rand_percent(FLIT_PERCENT)) {
	return 0;
    }
    if (rand_percent(10)) {
	return 1;
    }
    row = monster->row;
    col = monster->col;

    for (i = 0; i < 9; i++) {
	rand_around(i, &row, &col);
	if ((row == rogue.row) && (col == rogue.col)) {
	    continue;
	}
	if (mtry(monster, row, col)) {
	    return 1;
	}
    }
    return 1;
}

int
gr_obj_char(void)
{
    short r;
    char *rs = "%!?]=/):*";

    r = get_rand(0, 8);

    return rs[r];
}

int
no_room_for_monster(int rn)
{
    short i, j;

    for (i = rooms[rn].top_row + 1; i < rooms[rn].bottom_row; i++) {
	for (j = rooms[rn].left_col + 1; j < rooms[rn].right_col; j++) {
	    if (!(dungeon[i][j] & MONSTER)) {
		return 0;
	    }
	}
    }
    return 1;
}

void
aggravate(void)
{
    object *monster;

    message(mesg[65], 0);
    monster = level_monsters.next_monster;

    while (monster) {
	wake_up(monster);
	monster->m_flags &= (~IMITATES);
	if (rogue_can_see(monster->row, monster->col)) {
	    mvaddch_rogue(monster->row, monster->col, monster->m_char);
	}
	monster = monster->next_monster;
    }
}

boolean
mon_sees(object *monster, int row, int col)
{
    short rn, rdif, cdif;

    rn = get_room_number(row, col);

    if ((rn != NO_ROOM) &&
	(rn == get_room_number(monster->row, monster->col)) &&
	!(rooms[rn].is_room & R_MAZE)) {
	return 1;
    }
    rdif = row - monster->row;
    cdif = col - monster->col;

    return (boolean) ((rdif >= -1) && (rdif <= 1) && (cdif >= -1)
		      && (cdif <= 1));
}

void
mv_aquatars(void)
{
    object *monster;

    monster = level_monsters.next_monster;

    while (monster) {
	if ((monster->m_char == 'A') &&
	    mon_can_go(monster, rogue.row, rogue.col)) {
	    mv_monster(monster, rogue.row, rogue.col);
	    monster->m_flags |= ALREADY_MOVED;
	}
	monster = monster->next_monster;
    }
}
