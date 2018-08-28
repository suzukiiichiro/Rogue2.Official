/*
 * zap.c
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
#include "zap.h"
#include "display.h"
#include "hit.h"
#include "message.h"
#include "monster.h"
#include "move.h"
#include "object.h"
#include "pack.h"
#include "random.h"
#include "room.h"
#include "save.h"
#include "score.h"
#include "spechit.h"
#include "use.h"

boolean wizard = 0;
#if !defined( ORIGINAL )
char *wiz_passwd = "\253\104\114\266\134\245\000\333\355\064\000";
#endif /* ORIGINAL */

extern boolean being_held, score_only, detect_monster;

void
zapp(void)
{
    short wch;
    object *wand;
    short dir, row, col;
    object *monster;

    dir = get_direction();
    if (dir == CANCEL) {
	return;
    }
    if ((wch = pack_letter(mesg[278], WAND)) == CANCEL) {
	return;
    }
    check_message();

    if (!(wand = get_letter_object(wch))) {
	message(mesg[279], 0);
	return;
    }
    if (wand->what_is != WAND) {
	message(mesg[280], 0);
	return;
    }
    if (wand->class <= 0) {
	message(mesg[281], 0);
    } else {
	wand->class--;
	row = rogue.row;
	col = rogue.col;
#if !defined( ORIGINAL )
	if (wand->which_kind == MAGIC_MISSILE) {
	    monster = get_missiled_monster(dir, &row, &col);
	    mvaddch_rogue(rogue.row, rogue.col, rogue.fchar);
	    refresh();
	    if ((row != rogue.row || col != rogue.col)
		&& rogue_can_see(row, col)) {
		mvaddch_rogue(row, col, get_dungeon_char(row, col));
	    }
	} else {
	    monster = get_zapped_monster(dir, &row, &col);
	}
#else /* ORIGINAL */
	monster = get_zapped_monster(dir, &row, &col);
#endif /* ORIGINAL */
	if (monster) {
	    wake_up(monster);
	    zap_monster(monster, wand->which_kind);
	    relight();
	}
    }
    (void) reg_move();
}

object *
get_zapped_monster(short dir, short *row, short *col)
{
    short orow, ocol;

    for (;;) {
	orow = *row;
	ocol = *col;
	get_dir_rc(dir, row, col, 0);
	if (((*row == orow) && (*col == ocol)) ||
	    (dungeon[*row][*col] & (HORWALL | VERTWALL)) ||
	    (dungeon[*row][*col] == NOTHING)) {
	    return 0;
	}
	if (dungeon[*row][*col] & MONSTER) {
	    if (!imitating(*row, *col)) {
		return (object_at(&level_monsters, *row, *col));
	    }
	}
    }
}

#if !defined( ORIGINAL )
object *
get_missiled_monster(short dir, short *row, short *col)
{
    short orow, ocol, first = 1;

    orow = *row;
    ocol = *col;
    for (;;) {
	get_dir_rc(dir, row, col, 0);
	if (((*row == orow) && (*col == ocol)) ||
	    (dungeon[*row][*col] & (HORWALL | VERTWALL)) ||
	    (dungeon[*row][*col] == NOTHING)) {
	    *row = orow;
	    *col = ocol;
	    return 0;
	}
	if (!first && rogue_can_see(orow, ocol)) {
	    mvaddch_rogue(orow, ocol, get_dungeon_char(orow, ocol));
	}
	if (rogue_can_see(*row, *col)) {
	    if (!(dungeon[*row][*col] & MONSTER)) {
		mvaddch_rogue(*row, *col, '*');
	    }
	    refresh();
	}
	if (dungeon[*row][*col] & MONSTER) {
	    if (!imitating(*row, *col)) {
		return (object_at(&level_monsters, *row, *col));
	    }
	}
	first = 0;
	orow = *row;
	ocol = *col;
    }
}
#endif /* not ORIGINAL */

void
zap_monster(object *monster, unsigned short kind)
{
    short row, col;
    object *nm;
    short tc;

    row = monster->row;
    col = monster->col;

    switch (kind) {
    case SLOW_MONSTER:
	if (monster->m_flags & HASTED) {
	    monster->m_flags &= (~HASTED);
	} else {
	    monster->slowed_toggle = 0;
	    monster->m_flags |= SLOWED;
	}
	break;
    case HASTE_MONSTER:
	if (monster->m_flags & SLOWED) {
	    monster->m_flags &= (~SLOWED);
	} else {
	    monster->m_flags |= HASTED;
	}
	break;
    case TELE_AWAY:
	tele_away(monster);
	break;
    case CONFUSE_MONSTER:
	monster->m_flags |= CONFUSED;
	monster->moves_confused += get_rand(12, 22);
	break;
    case INVISIBILITY:
	monster->m_flags |= INVISIBLE;
	break;
    case POLYMORPH:
	if (monster->m_flags & HOLDS) {
	    being_held = 0;
	}
	nm = monster->next_monster;
	tc = monster->trail_char;
	(void) gr_monster(monster, get_rand(0, MONSTERS - 1));
	monster->row = row;
	monster->col = col;
	monster->next_monster = nm;
	monster->trail_char = tc;
	if (!(monster->m_flags & IMITATES)) {
	    wake_up(monster);
	}
	break;
    case PUT_TO_SLEEP:
	monster->m_flags |= (ASLEEP | NAPPING);
	monster->nap_length = get_rand(3, 6);
	break;
    case MAGIC_MISSILE:
	rogue_hit(monster, 1);
	break;
    case CANCELLATION:
	if (monster->m_flags & HOLDS) {
	    being_held = 0;
	}
	if (monster->m_flags & STEALS_ITEM) {
	    monster->drop_percent = 0;
	}
	monster->m_flags &= (~(FLIES | FLITS | SPECIAL_HIT | INVISIBLE |
			       FLAMES | IMITATES | CONFUSES | SEEKS_GOLD |
			       HOLDS));
	break;
    case DO_NOTHING:
	message(mesg[282], 0);
	break;
    }
}

void
tele_away(object *monster)
{
    short row, col;

    if (monster->m_flags & HOLDS) {
	being_held = 0;
    }
    gr_row_col(&row, &col, (FLOOR | TUNNEL | STAIRS | OBJECT));
    mvaddch_rogue(monster->row, monster->col,
		  (unsigned char) monster->trail_char);
    dungeon[monster->row][monster->col] &= ~MONSTER;
    monster->row = row;
    monster->col = col;
    dungeon[row][col] |= MONSTER;
    monster->trail_char = mvinch_rogue(row, col);
    if (detect_monster || rogue_can_see(row, col)) {
	mvaddch_rogue(row, col, gmc(monster));
    }
}

void
wizardize(void)
{
#if defined( WIZARD )
    char buf[100];

    if (wizard) {
	wizard = 0;
#if defined( JAPAN )
	message("もはや、魔法使いではない。", 0);
#else /* not JAPAN */
	message("Not wizard anymore", 0);
#endif
    } else {
#if defined( JAPAN )
	if (get_input_line("魔法使いの合言葉は？",
#else /* not JAPAN */
	if (get_input_line("Wizard's password:",
#endif /* not JAPAN */
			   "", buf, "", 0, 0)) {
	    (void) xxx(1);
	    xxxx(buf, strlen(buf));
#if !defined( ORIGINAL )
	    if (!memcmp(buf, wiz_passwd, 11)) {
#else /* ORIGINAL */
	    if (!strncmp(buf, "\247\104\126\272\115\243\027", 7)) {
#endif /* ORIGINAL */
		wizard = 1;
		score_only = 1;
#if defined( JAPAN )
		message("ようこそ、魔法使いよ！", 0);
#else /* not JAPAN */
		message("Welcome, mighty wizard!", 0);
#endif /* not JAPAN */
	    } else {
#if defined( JAPAN )
		message("そんな合言葉、知らないね。", 0);
#else /* not JAPAN */
		message("Sorry", 0);
#endif /* not JAPAN */
	    }
	}
    }
#else /* not WIZARD */
#if defined( JAPAN )
   message("魔法使いは封印されている。", 0);
#else
   message("Wizard has been blocked.", 0);
#endif
#endif /* not WIZARD */
}
