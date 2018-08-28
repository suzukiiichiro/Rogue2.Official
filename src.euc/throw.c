/*
 * throw.c
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
#include "throw.h"
#include "display.h"
#include "hit.h"
#include "message.h"
#include "monster.h"
#include "object.h"
#include "pack.h"
#include "random.h"
#include "ring.h"
#include "room.h"
#include "spechit.h"
#include "use.h"
#include "zap.h"

extern short cur_room;
extern char *curse_message;
extern char hit_message[];

void
throw(void)
{
    short wch;
    object *weapon;
    short dir, row, col;
    object *monster;

    dir = get_direction();
    if (dir == CANCEL) {
	return;
    }
    if ((wch = pack_letter(mesg[210], WEAPON)) == CANCEL) {
	return;
    }
    check_message();

    if (!(weapon = get_letter_object(wch))) {
	message(mesg[211], 0);
	return;
    }
    if ((weapon->in_use_flags & BEING_USED) && weapon->is_cursed) {
	message(curse_message, 0);
	return;
    }
    row = rogue.row;
    col = rogue.col;

    if ((weapon->in_use_flags & BEING_WIELDED) && (weapon->quantity <= 1)) {
	unwield(rogue.weapon);
    } else if (weapon->in_use_flags & BEING_WORN) {
	mv_aquatars();
	unwear(rogue.armor);
	print_stats(STAT_ARMOR);
    } else if (weapon->in_use_flags & ON_EITHER_HAND) {
	un_put_on(weapon);
    }
    monster = get_thrown_at_monster(weapon, dir, &row, &col);
    mvaddch_rogue(rogue.row, rogue.col, rogue.fchar);
    refresh();

    if (rogue_can_see(row, col) && ((row != rogue.row) || (col != rogue.col))) {
	mvaddch_rogue(row, col, get_dungeon_char(row, col));
    }
    if (monster) {
	wake_up(monster);
	check_gold_seeker(monster);

	if (!throw_at_monster(monster, weapon)) {
	    flop_weapon(weapon, row, col);
	}
    } else {
	flop_weapon(weapon, row, col);
    }
    vanish(weapon, 1, &rogue.pack);
}

int
throw_at_monster(object *monster, object *weapon)
{
    short damage, hit_chance;
    short t;

    hit_chance = get_hit_chance(weapon);
    damage = get_weapon_damage(weapon);
    if ((weapon->which_kind == ARROW) &&
	(rogue.weapon && (rogue.weapon->which_kind == BOW))) {
	damage += get_weapon_damage(rogue.weapon);
	damage = ((damage * 2) / 3);
	hit_chance += (hit_chance / 3);
    } else if ((weapon->in_use_flags & BEING_WIELDED) &&
	       ((weapon->which_kind == DAGGER) ||
		(weapon->which_kind == SHURIKEN) ||
		(weapon->which_kind == DART))) {
	damage = ((damage * 3) / 2);
	hit_chance += (hit_chance / 3);
    }
    t = weapon->quantity;
    weapon->quantity = 1;
    sprintf(hit_message, mesg[212], name_of(weapon));
    weapon->quantity = t;

    if (!rand_percent(hit_chance)) {
	(void) strcat(hit_message, mesg[213]);
	return 0;
    }
    (void) strcat(hit_message, mesg[214]);
    if ((weapon->what_is == WAND) && rand_percent(75)) {
	zap_monster(monster, weapon->which_kind);
#if !defined( ORIGINAL )
    } else if (weapon->what_is == POTION) {
	potion_monster(monster, weapon->which_kind);
#endif /* ORIGINAL */
    } else {
	(void) mon_damage(monster, damage);
    }
    return 1;
}

object *
get_thrown_at_monster(object *obj, short dir, short *row, short *col)
{
    short orow, ocol;
    short i, ch;

    orow = *row;
    ocol = *col;

    ch = get_mask_char(obj->what_is);

    for (i = 0; i < 24; i++) {
	get_dir_rc(dir, row, col, 0);
	if ((dungeon[*row][*col] == NOTHING) ||
	    ((dungeon[*row][*col] & (HORWALL | VERTWALL | HIDDEN)) &&
	     (!(dungeon[*row][*col] & TRAP)))) {
	    *row = orow;
	    *col = ocol;
	    return 0;
	}
	if ((i != 0) && rogue_can_see(orow, ocol)) {
	    mvaddch_rogue(orow, ocol, get_dungeon_char(orow, ocol));
	}
	if (rogue_can_see(*row, *col)) {
	    if (!(dungeon[*row][*col] & MONSTER)) {
		mvaddch_rogue(*row, *col, ch);
	    }
	    refresh();
	}
	orow = *row;
	ocol = *col;
	if (dungeon[*row][*col] & MONSTER) {
	    if (!imitating(*row, *col)) {
		return (object_at(&level_monsters, *row, *col));
	    }
	}
	if (dungeon[*row][*col] & TUNNEL) {
	    i += 2;
	}
    }
    return 0;
}

void
flop_weapon(object *weapon, short row, short col)
{
    object *new_weapon, *monster;
    short i = 0;
    char msg[80];
    boolean found = 0;
    short mch, dch;
    unsigned short mon;

    while ((i < 9) && dungeon[row][col] & ~(FLOOR | TUNNEL | DOOR | MONSTER)) {
	rand_around(i++, &row, &col);
	if ((row > (ROGUE_LINES - 2)) || (row < MIN_ROW) ||
	    (col > (ROGUE_COLUMNS - 1)) || (col < 0) || (!dungeon[row][col]) ||
	    (dungeon[row][col] & ~(FLOOR | TUNNEL | DOOR | MONSTER))) {
	    continue;
	}
	found = 1;
	break;
    }

    if (found || (i == 0)) {
	new_weapon = alloc_object();
	*new_weapon = *weapon;
	new_weapon->in_use_flags = NOT_USED;
	new_weapon->quantity = 1;
	new_weapon->ichar = 'L';
	place_at(new_weapon, row, col);
	if (rogue_can_see(row, col) &&
	    ((row != rogue.row) || (col != rogue.col))) {
	    mon = dungeon[row][col] & MONSTER;
	    dungeon[row][col] &= (~MONSTER);
	    dch = get_dungeon_char(row, col);
	    if (mon) {
		mch = mvinch_rogue(row, col);
		if ((monster = object_at(&level_monsters, row, col))) {
		    monster->trail_char = dch;
		}
		if ((mch < 'A') || (mch > 'Z')) {
		    mvaddch_rogue(row, col, dch);
		}
	    } else {
		mvaddch_rogue(row, col, dch);
	    }
	    dungeon[row][col] |= mon;
	}
    } else {
	short t;

	t = weapon->quantity;
	weapon->quantity = 1;
	sprintf(msg, mesg[215], name_of(weapon));
	weapon->quantity = t;
	message(msg, 0);
    }
}

void
rand_around(short i, short *r, short *c)
{
/*	static char* pos = "\010\007\001\003\004\005\002\006\0";	*/
    static char pos[] = "\010\007\001\003\004\005\002\006\0";	/* by Yasha */
    static short row, col;
    short j;
    static short ra[] = { 1, 1, -1, -1, 0, 1, 0, -1, 0 };
    static short ca[] = { 1, -1, 1, -1, 1, 0, 0, 0, -1 };

    if (i == 0) {
	short x, y, o, t;

	row = *r;
	col = *c;

	o = get_rand(1, 8);

	for (j = 0; j < 5; j++) {
	    x = get_rand(0, 8) % 9;
	    y = (x + o) % 9;
	    t = pos[x];
	    pos[x] = pos[y];
	    pos[y] = t;
	}
    }
    j = (short) pos[i] % 9;
    *r = row + ra[j];
    *c = col + ca[j];
}

#if !defined( ORIGINAL )
void
potion_monster(object *monster, unsigned short kind)
{
    short maxhp;

    maxhp = mon_tab[monster->m_char - 'A'].hp_to_kill;

    switch (kind) {
    case RESTORE_STRENGTH:
    case LEVITATION:
    case HALLUCINATION:
    case DETECT_MONSTER:
    case DETECT_OBJECTS:
    case SEE_INVISIBLE:
	break;
    case EXTRA_HEALING:
	monster->hp_to_kill += (maxhp - monster->hp_to_kill) * 2 / 3;
	break;
    case INCREASE_STRENGTH:
    case HEALING:
    case RAISE_LEVEL:
	monster->hp_to_kill += (maxhp - monster->hp_to_kill) / 5;
	break;
    case POISON:
	mon_damage(monster, (monster->hp_to_kill / 4 + 1));
	break;
    case BLINDNESS:
	monster->m_flags |= (ASLEEP | WAKENS);
	break;
    case CONFUSION:
	monster->m_flags |= CONFUSED;
	monster->moves_confused += get_rand(12, 22);
	break;
    case HASTE_SELF:
	if (monster->m_flags & SLOWED)
	    monster->m_flags &= (~SLOWED);
	else
	    monster->m_flags |= HASTED;
	break;
    }
}
#endif /* not ORIGINAL */
