/*
 * move.c
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
#include <string.h>
#include <curses.h>

#include "rogue.h"
#include "move.h"
#include "display.h"
#include "hit.h"
#include "invent.h"
#include "message.h"
#include "monster.h"
#include "object.h"
#include "pack.h"
#include "random.h"
#include "room.h"
#include "score.h"
#include "trap.h"
#include "use.h"

short m_moves = 0;
#if !defined( ORIGINAL )
boolean jump = 0;
boolean bent_passage;
#else /* ORIGINAL */
boolean jump = 1;
#endif /* ORIGINAL */
char *you_can_move_again = mesg[66];

extern short cur_room, halluc, blind, levitate;
extern short cur_level, max_level;
extern short bear_trap, haste_self, confused;
extern short e_rings, regeneration, auto_search;
extern char hunger_str[];
extern boolean being_held, interrupted, r_teleport;
#if !defined( ORIGINAL )
extern boolean pass_go;
#endif /* not ORIGINAL */

int
one_move_rogue(short dirch, short pickup)
{
    short row, col;
    short r, c;
    char *p;
    object *obj;
    char desc[ROGUE_COLUMNS];
    short status;

    r = rogue.row;
    c = rogue.col;
#if !defined( ORIGINAL )
    bent_passage = 0;
#endif /* not ORIGINAL */

    if (confused) {
	dirch = gr_dir();
    }
    get_dir_rc(dirch, &r, &c, 1);
    row = r;
    col = c;

    if (!can_move(rogue.row, rogue.col, row, col)) {
#if !defined( ORIGINAL )
	if (cur_room == PASSAGE && !blind && !confused
	    && !strchr("yubn", dirch))
	    bent_passage = 1;
#endif /* not ORIGINAL */
	return (MOVE_FAILED);
    }
    if (being_held || bear_trap) {
	if (!(dungeon[row][col] & MONSTER)) {
	    if (being_held) {
		message(mesg[67], 1);
	    } else {
		message(mesg[68], 0);
		(void) reg_move();
	    }
	    return (MOVE_FAILED);
	}
    }
    if (r_teleport) {
	if (rand_percent(R_TELE_PERCENT)) {
	    tele();
	    return (STOPPED_ON_SOMETHING);
	}
    }
    if (dungeon[row][col] & MONSTER) {
	rogue_hit(object_at(&level_monsters, row, col), 0);
	(void) reg_move();
	return (MOVE_FAILED);
    }
    if (dungeon[row][col] & DOOR) {
	if (cur_room == PASSAGE) {
	    cur_room = get_room_number(row, col);
	    light_up_room(cur_room);
	    wake_room(cur_room, 1, row, col);
	} else {
	    light_passage(row, col);
	}
    } else if ((dungeon[rogue.row][rogue.col] & DOOR) &&
	       (dungeon[row][col] & TUNNEL)) {
	light_passage(row, col);
	wake_room(cur_room, 0, rogue.row, rogue.col);
	darken_room(cur_room);
	cur_room = PASSAGE;
    } else if (dungeon[row][col] & TUNNEL) {
	light_passage(row, col);
    }
    mvaddch_rogue(rogue.row, rogue.col,
		  get_dungeon_char(rogue.row, rogue.col));
    mvaddch_rogue(row, col, rogue.fchar);

    if (!jump) {
	refresh();
    }
    rogue.row = row;
    rogue.col = col;
    if (dungeon[row][col] & OBJECT) {
	if (levitate && pickup) {
	    return (STOPPED_ON_SOMETHING);
	}
	if (pickup && !levitate) {
	    if ((obj = pick_up(row, col, &status))) {
		get_desc(obj, desc, 1);
		if (obj->what_is == GOLD) {
		    free_object(obj);
#if defined( JAPAN )
		    strcat(desc, mesg[69]);
#endif /* JAPAN */
		    goto NOT_IN_PACK;
		}
	    } else if (!status) {
		goto MVED;
	    } else {
		goto MOVE_ON;
	    }
	} else {
	MOVE_ON:
	    obj = object_at(&level_objects, row, col);
#if defined( JAPAN )
	    get_desc(obj, desc, 0);
	    (void) strcat(desc, mesg[70]);
#else /* not JAPAN */
	    (void) strcpy(desc, mesg[70]);
	    get_desc(obj, desc + strlen(mesg[70]), 0);
#endif /* not JAPAN */
	    goto NOT_IN_PACK;
	}
#if defined( JAPAN )
	strcat(desc, mesg[69]);
#endif /* JAPAN */
	p = desc + strlen(desc);
	*p++ = '(';
	*p++ = obj->ichar;
	*p++ = ')';
	*p = 0;
    NOT_IN_PACK:
	message(desc, 1);
	(void) reg_move();
	return (STOPPED_ON_SOMETHING);
    }
    if (dungeon[row][col] & (DOOR | STAIRS | TRAP)) {
	if ((!levitate) && (dungeon[row][col] & TRAP)) {
	    trap_player(row, col);
	}
	(void) reg_move();
	return (STOPPED_ON_SOMETHING);
    }
MVED:
    if (reg_move()) {		/* fainted from hunger */
	return (STOPPED_ON_SOMETHING);
    }
    return ((confused ? STOPPED_ON_SOMETHING : MOVED));
}

void
multiple_move_rogue(int dirch)
{
    short row, col;
    short m;
#if !defined( ORIGINAL )
    short n, i, ch = 0;		/* 未初期化変数の警告除去のため 0 で初期化 */
    char *dir;
#endif /* not ORIGINAL */

    switch (dirch) {
    case '\010':
    case '\012':
    case '\013':
    case '\014':
    case '\031':
    case '\025':
    case '\016':
    case '\002':
#if !defined( ORIGINAL )
	dirch += 96;
	do {
	retry:
	    row = rogue.row;
	    col = rogue.col;
	    m = one_move_rogue(dirch, 1);
	    if (m == STOPPED_ON_SOMETHING || interrupted) {
		break;
	    }
	    if (m != MOVE_FAILED) {
		continue;
	    }
	    if (!pass_go || !bent_passage) {
		break;
	    }
	    for (n = 0, dir = "hjkl", i = 0; i < 4; i++) {
		row = rogue.row;
		col = rogue.col;
		get_dir_rc(dir[i], &row, &col, 1);
		if (is_passable(row, col) && dirch != dir[3 - i]) {
		    n++, ch = dir[i];
		}
	    }
	    if (n == 1) {
		dirch = ch;
		goto retry;
	    }
	    break;
	} while (!next_to_something(row, col));
	break;
#else /* ORIGINAL */
	do {
	    row = rogue.row;
	    col = rogue.col;
	    if (((m = one_move_rogue((dirch + 96), 1)) == MOVE_FAILED) ||
		(m == STOPPED_ON_SOMETHING) || interrupted) {
		break;
	    }
	} while (!next_to_something(row, col));
	break;
#endif /* ORIGINAL */
    case 'H':
    case 'J':
    case 'K':
    case 'L':
    case 'B':
    case 'Y':
    case 'U':
    case 'N':
#if !defined( ORIGINAL )
	dirch += 32;
	for (;;) {
	retry2:
	    m = one_move_rogue(dirch, 1);
	    if (interrupted) {
		break;
	    }
	    if (m == MOVED) {
		continue;
	    }
	    if (m != MOVE_FAILED || !pass_go || !bent_passage) {
		break;
	    }
	    for (n = 0, dir = "hjkl", i = 0; i < 4; i++) {
		row = rogue.row;
		col = rogue.col;
		get_dir_rc(dir[i], &row, &col, 1);
		if (is_passable(row, col) && dirch != dir[3 - i]) {
		    n++, ch = dir[i];
		}
	    }
	    if (n == 1) {
		dirch = ch;
		goto retry2;
	    }
	    break;
	}
	break;
#else /* ORIGINAL */
	while ((!interrupted) && (one_move_rogue((dirch + 32), 1) == MOVED));
	break;
#endif /* ORIGINAL */
    }
}

int
is_passable(int row, int col)
{
    if ((row < MIN_ROW) || (row > (ROGUE_LINES - 2)) || (col < 0) ||
	(col > (ROGUE_COLUMNS - 1))) {
	return 0;
    }
    if (dungeon[row][col] & HIDDEN) {
	return ((dungeon[row][col] & TRAP) ? 1 : 0);
    }
    return (int) (dungeon[row][col] & (FLOOR | TUNNEL | DOOR | STAIRS | TRAP));
}

int
next_to_something(int drow, int dcol)
{
    short i, j, i_end, j_end, row, col;
    short pass_count = 0;
    unsigned short s;

    if (confused) {
	return 1;
    }
    if (blind) {
	return 0;
    }
    i_end = (rogue.row < (ROGUE_LINES - 2)) ? 1 : 0;
    j_end = (rogue.col < (ROGUE_COLUMNS - 1)) ? 1 : 0;

    for (i = ((rogue.row > MIN_ROW) ? -1 : 0); i <= i_end; i++) {
	for (j = ((rogue.col > 0) ? -1 : 0); j <= j_end; j++) {
	    if ((i == 0 && j == 0) ||
		(rogue.row + i == drow && rogue.col + j == dcol)) {
		continue;
	    }
	    row = rogue.row + i;
	    col = rogue.col + j;
	    s = dungeon[row][col];
	    if (s & HIDDEN) {
		continue;
	    }
	    /* If the rogue used to be right, up, left, down,
	     * or right of row, col, and now isn't,
	     * then don't stop */
	    if (s & (MONSTER | OBJECT | STAIRS)) {
		if ((row == drow || col == dcol) &&
		    (!(row == rogue.row || col == rogue.col))) {
		    continue;
		}
		return 1;
	    }
	    if (s & TRAP) {
		if (!(s & HIDDEN)) {
		    if ((row == drow || col == dcol) &&
			(!(row == rogue.row || col == rogue.col))) {
			continue;
		    }
		    return 1;
		}
	    }
	    if (((i - j == 1) || (i - j == -1)) && (s & TUNNEL)) {
		if (++pass_count > 1) {
		    return 1;
		}
	    }
	    if ((s & DOOR) && ((i == 0) || (j == 0))) {
		return 1;
	    }
	}
    }
    return 0;
}

int
can_move(int row1, int col1, int row2, int col2)
{
    if (!is_passable(row2, col2)) {
	return 0;
    }
    if ((row1 != row2) && (col1 != col2)) {
	if ((dungeon[row1][col1] & DOOR) || (dungeon[row2][col2] & DOOR)
	    || (!dungeon[row1][col2]) || (!dungeon[row2][col1])) {
	    return 0;
	}
    }
    return 1;
}

void
move_onto(void)
{
    short ch;

    ch = get_direction();
    if (ch != CANCEL) {
	(void) one_move_rogue(ch, 0);
    }
}

boolean
is_direction(int c)
{
    return (boolean) ((strchr("hjklbyun\033", c) != (char *) 0) ? 1 : 0);
}

boolean
check_hunger(boolean messages_only)
{
    short i, n;
    boolean fainted = 0;
    static short move_left_cou = 0;	/* Yasha */

    if (rogue.moves_left == HUNGRY) {
	(void) strcpy(hunger_str, mesg[71]);
	message(mesg[72], 0);
	print_stats(STAT_HUNGER);
    }
    if (rogue.moves_left == WEAK) {
	(void) strcpy(hunger_str, mesg[73]);
	message(mesg[74], 1);
	print_stats(STAT_HUNGER);
    }
    if (rogue.moves_left <= FAINT) {
	if (rogue.moves_left == FAINT) {
	    (void) strcpy(hunger_str, mesg[75]);
	    message(mesg[76], 1);
	    print_stats(STAT_HUNGER);
	}
	n = get_rand(0, (FAINT - rogue.moves_left));
	if (n > 0) {
	    fainted = 1;
	    if (rand_percent(40)) {
		rogue.moves_left++;
	    }
	    message(mesg[77], 1);
	    for (i = 0; i < n; i++) {
		if (coin_toss()) {
		    mv_mons();
		}
	    }
	    message(you_can_move_again, 1);
	}
    }
    if (messages_only) {
	return fainted;
    }
    if (rogue.moves_left <= STARVE) {
	killed_by((object *) 0, STARVATION);
    }

    switch (e_rings) {
	/*case -2:
	 * Subtract 0, i.e. do nothing.
	 * break; */
    case -1:
/*		rogue.moves_left -= (rogue.moves_left % 2);*/
	rogue.moves_left -= move_left_cou;	/* by Yasha */
	break;
    case 0:
	rogue.moves_left--;
	break;
    case 1:
	rogue.moves_left--;
	(void) check_hunger(1);
/*		rogue.moves_left -= (rogue.moves_left % 2);*/
	rogue.moves_left -= move_left_cou;	/* by Yasha */
	break;
    case 2:
	rogue.moves_left--;
	(void) check_hunger(1);
	rogue.moves_left--;
	break;
    }
    move_left_cou ^= 1;		/* by Yasha */

    return fainted;
}

boolean
reg_move(void)
{
    boolean fainted;

    if ((rogue.moves_left <= HUNGRY) || (cur_level >= max_level)) {
	fainted = check_hunger(0);
    } else {
	fainted = 0;
    }

    mv_mons();

    if (++m_moves >= 120) {
	m_moves = 0;
	wanderer();
    }
    if (halluc) {
	if (!(--halluc)) {
	    unhallucinate();
	} else {
	    hallucinate();
	}
    }
    if (blind) {
	if (!(--blind)) {
	    unblind();
	}
    }
    if (confused) {
	if (!(--confused)) {
	    unconfuse();
	}
    }
    if (bear_trap) {
	bear_trap--;
    }
    if (levitate) {
	if (!(--levitate)) {
	    message(mesg[78], 1);
	    if (dungeon[rogue.row][rogue.col] & TRAP) {
		trap_player(rogue.row, rogue.col);
	    }
	}
    }
    if (haste_self) {
	if (!(--haste_self)) {
	    message(mesg[79], 0);
	}
    }
    heal();
    if (auto_search > 0) {
	search(auto_search, auto_search);
    }
    return fainted;
}

void
rest(int count)
{
    int i;

    interrupted = 0;

    for (i = 0; i < count; i++) {
	if (interrupted) {
	    break;
	}
	(void) reg_move();
    }
}

int
gr_dir(void)
{
    return (*("jklhyubn" + get_rand(1, 8) - 1));
}

void
heal(void)
{
    static short heal_exp = -1, n, c = 0;
    static boolean alt;
    static char na[] = { 0, 20, 18, 17, 14, 13, 10, 9, 8, 7, 4, 3 };

    if (rogue.hp_current == rogue.hp_max) {
	c = 0;
	return;
    }
    if (rogue.exp != heal_exp) {
	heal_exp = rogue.exp;
	n = (heal_exp < 1 || heal_exp > 11) ? 2 : na[heal_exp];
    }
    if (++c >= n) {
	c = 0;
	rogue.hp_current++;
	if ((alt = !alt)) {
	    rogue.hp_current++;
	}
	if ((rogue.hp_current += regeneration) > rogue.hp_max) {
	    rogue.hp_current = rogue.hp_max;
	}
	print_stats(STAT_HP);
    }
}
