/*
 * use.c
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
#include <unistd.h>
#include <curses.h>

#include "rogue.h"
#include "use.h"
#include "display.h"
#include "invent.h"
#include "level.h"
#include "machdep.h"
#include "message.h"
#include "monster.h"
#include "move.h"
#include "object.h"
#include "pack.h"
#include "random.h"
#include "ring.h"
#include "room.h"
#include "save.h"

short halluc = 0;
short blind = 0;
short confused = 0;
short levitate = 0;
short haste_self = 0;
boolean see_invisible = 0;
short extra_hp = 0;
boolean detect_monster = 0;
char *strange_feeling = mesg[230];

extern short bear_trap;
extern char hunger_str[];
extern short cur_room;
extern long level_points[];
extern boolean being_held;
extern char *fruit, *you_can_move_again;
extern boolean sustain_strength;

#if defined( JAPAN )
void
quaff(void)
{
    short ch;
    char buf[80];
    object *obj;

    ch = pack_letter(mesg[231], POTION);
    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[232], 0);
	return;
    }
    if (obj->what_is != POTION) {
	message(mesg[233], 0);
	return;
    }
    switch (obj->which_kind) {
    case INCREASE_STRENGTH:
	message(mesg[234], 0);
	rogue.str_current++;
	if (rogue.str_current > rogue.str_max) {
	    rogue.str_max = rogue.str_current;
	}
	break;
    case RESTORE_STRENGTH:
	rogue.str_current = rogue.str_max;
	message(mesg[235], 0);
	break;
    case HEALING:
	message(mesg[236], 0);
	potion_heal(0);
	break;
    case EXTRA_HEALING:
	message(mesg[237], 0);
	potion_heal(1);
	break;
    case POISON:
	if (!sustain_strength) {
	    rogue.str_current -= get_rand(1, 3);
	    if (rogue.str_current < 1) {
		rogue.str_current = 1;
	    }
	}
	message(mesg[238], 0);
	if (halluc) {
	    unhallucinate();
	}
	break;
    case RAISE_LEVEL:
	rogue.exp_points = level_points[rogue.exp - 1];
	add_exp(1, 1);
	break;
    case BLINDNESS:
	go_blind();
	break;
    case HALLUCINATION:
	message(mesg[239], 0);
	halluc += get_rand(500, 800);
	break;
    case DETECT_MONSTER:
	show_monsters();
	if (!(level_monsters.next_monster)) {
	    message(strange_feeling, 0);
	}
	break;
    case DETECT_OBJECTS:
	if (level_objects.next_object) {
	    if (!blind) {
		show_objects();
	    }
	} else {
	    message(strange_feeling, 0);
	}
	break;
    case CONFUSION:
	message((halluc ? mesg[240]
		 : mesg[241]), 0);
	confuse();
	break;
    case LEVITATION:
	message(mesg[242], 0);
	levitate += get_rand(15, 30);
	being_held = bear_trap = 0;
	break;
    case HASTE_SELF:
	message(mesg[243], 0);
	haste_self += get_rand(11, 21);
	if (!(haste_self % 2)) {
	    haste_self++;
	}
	break;
    case SEE_INVISIBLE:
	sprintf(buf, mesg[244], fruit);
	message(buf, 0);
	if (blind) {
	    unblind();
	}
	see_invisible = 1;
	relight();
	break;
    }
    print_stats((STAT_STRENGTH | STAT_HP));
#if !defined( ORIGINAL )
    id_potions[obj->which_kind].id_status = IDENTIFIED;
#else /* ORIGINAL */
    if (id_potions[obj->which_kind].id_status != CALLED) {
	id_potions[obj->which_kind].id_status = IDENTIFIED;
    }
#endif /* ORIGINAL */
    vanish(obj, 1, &rogue.pack);
}

#else /* not JAPAN */
quaff()
{
    short ch;
    char buf[80];
    object *obj;

    ch = pack_letter(mesg[231], POTION);
    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[232], 0);
	return;
    }
    if (obj->what_is != POTION) {
	message(mesg[233], 0);
	return;
    }
    switch (obj->which_kind) {
    case INCREASE_STRENGTH:
	message(mesg[234], 0);
	rogue.str_current++;
	if (rogue.str_current > rogue.str_max) {
	    rogue.str_max = rogue.str_current;
	}
	break;
    case RESTORE_STRENGTH:
	rogue.str_current = rogue.str_max;
	message(mesg[235], 0);
	break;
    case HEALING:
	message(mesg[236], 0);
	potion_heal(0);
	break;
    case EXTRA_HEALING:
	message(mesg[237], 0);
	potion_heal(1);
	break;
    case POISON:
	if (!sustain_strength) {
	    rogue.str_current -= get_rand(1, 3);
	    if (rogue.str_current < 1) {
		rogue.str_current = 1;
	    }
	}
	message(mesg[238], 0);
	if (halluc) {
	    unhallucinate();
	}
	break;
    case RAISE_LEVEL:
	rogue.exp_points = level_points[rogue.exp - 1];
	add_exp(1, 1);
	break;
    case BLINDNESS:
	go_blind();
	break;
    case HALLUCINATION:
	message(mesg[239], 0);
	halluc += get_rand(500, 800);
	break;
    case DETECT_MONSTER:
	show_monsters();
	if (!(level_monsters.next_monster)) {
	    message(strange_feeling, 0);
	}
	break;
    case DETECT_OBJECTS:
	if (level_objects.next_object) {
	    if (!blind) {
		show_objects();
	    }
	} else {
	    message(strange_feeling, 0);
	}
	break;
    case CONFUSION:
	message((halluc ? mesg[240] :
#if !defined( ORIGINAL )
		 mesg[241]), 0);
#else /* ORIGINAL */
		 "You feel confused"), 0);
#endif /* ORIGINAL */
	confuse();
	break;
    case LEVITATION:
	message(mesg[242], 0);
	levitate += get_rand(15, 30);
	being_held = bear_trap = 0;
	break;
    case HASTE_SELF:
	message(mesg[243], 0);
	haste_self += get_rand(11, 21);
	if (!(haste_self % 2)) {
	    haste_self++;
	}
	break;
    case SEE_INVISIBLE:
	sprintf(buf, mesg[244], fruit);
	message(buf, 0);
	if (blind) {
	    unblind();
	}
	see_invisible = 1;
	relight();
	break;
    }
    print_stats((STAT_STRENGTH | STAT_HP));
#if !defined( ORIGINAL )
    id_potions[obj->which_kind].id_status = IDENTIFIED;
#else /* ORIGINAL */
    if (id_potions[obj->which_kind].id_status != CALLED) {
	id_potions[obj->which_kind].id_status = IDENTIFIED;
    }
#endif /* ORIGINAL */
    vanish(obj, 1, &rogue.pack);
}
#endif /* not JAPAN */
#if defined( JAPAN )
    void
read_scroll(void)
{
    short ch;
    object *obj;
    char msg[ROGUE_COLUMNS];

    ch = pack_letter(mesg[245], SCROL);

    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[246], 0);
	return;
    }
    if (obj->what_is != SCROL) {
	message(mesg[247], 0);
	return;
    }
    switch (obj->which_kind) {
    case SCARE_MONSTER:
	message(mesg[248], 0);
	break;
    case HOLD_MONSTER:
	hold_monster();
	break;
    case ENCH_WEAPON:
	if (rogue.weapon) {
	    if (rogue.weapon->what_is == WEAPON) {
		sprintf(msg, mesg[249],
			name_of(rogue.weapon), get_ench_color());
		message(msg, 0);
		if (coin_toss()) {
		    rogue.weapon->hit_enchant++;
		} else {
		    rogue.weapon->d_enchant++;
		}
	    }
	    rogue.weapon->is_cursed = 0;
	} else {
	    message(mesg[250], 0);
	}
	break;
    case ENCH_ARMOR:
	if (rogue.armor) {
	    sprintf(msg, mesg[251], get_ench_color());
	    message(msg, 0);
	    rogue.armor->d_enchant++;
	    rogue.armor->is_cursed = 0;
	    print_stats(STAT_ARMOR);
	} else {
	    message(mesg[252], 0);
	}
	break;
    case IDENTIFY:
	message(mesg[253], 0);
	obj->identified = 1;
	id_scrolls[obj->which_kind].id_status = IDENTIFIED;
	idntfy();
	break;
    case TELEPORT:
	tele();
	break;
    case SLEEP:
	message(mesg[254], 0);
	take_a_nap();
	break;
    case PROTECT_ARMOR:
	if (rogue.armor) {
	    message(mesg[255], 0);
	    rogue.armor->is_protected = 1;
	    rogue.armor->is_cursed = 0;
	} else {
	    message(mesg[256], 0);
	}
	break;
    case REMOVE_CURSE:
	message((!halluc) ? mesg[257] : mesg[258], 0);
	uncurse_all();
	break;
    case CREATE_MONSTER:
	create_monster();
	break;
    case AGGRAVATE_MONSTER:
	aggravate();
	break;
    case MAGIC_MAPPING:
	message(mesg[259], 0);
	draw_magic_map();
	break;
    }
#if !defined( ORIGINAL )
    id_scrolls[obj->which_kind].id_status = IDENTIFIED;
#else /* ORIGINAL */
    if (id_scrolls[obj->which_kind].id_status != CALLED) {
	id_scrolls[obj->which_kind].id_status = IDENTIFIED;
    }
#endif /* ORIGINAL */
    vanish(obj, (obj->which_kind != SLEEP), &rogue.pack);
}

#else /* not JAPAN */
read_scroll()
{
    short ch;
    object *obj;
    char msg[ROGUE_COLUMNS];

    ch = pack_letter(mesg[245], SCROL);

    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[246], 0);
	return;
    }
    if (obj->what_is != SCROL) {
	message(mesg[247], 0);
	return;
    }
    switch (obj->which_kind) {
    case SCARE_MONSTER:
	message(mesg[248], 0);
	break;
    case HOLD_MONSTER:
	hold_monster();
	break;
    case ENCH_WEAPON:
	if (rogue.weapon) {
	    if (rogue.weapon->what_is == WEAPON) {
		sprintf(msg, mesg[249],
			name_of(rogue.weapon),
			((rogue.weapon->quantity <= 1) ? "s" : ""),
			get_ench_color());
		message(msg, 0);
		if (coin_toss()) {
		    rogue.weapon->hit_enchant++;
		} else {
		    rogue.weapon->d_enchant++;
		}
	    }
	    rogue.weapon->is_cursed = 0;
	} else {
	    message(mesg[250], 0);
	}
	break;
    case ENCH_ARMOR:
	if (rogue.armor) {
	    sprintf(msg, mesg[251], get_ench_color());
	    message(msg, 0);
	    rogue.armor->d_enchant++;
	    rogue.armor->is_cursed = 0;
	    print_stats(STAT_ARMOR);
	} else {
	    message(mesg[252], 0);
	}
	break;
    case IDENTIFY:
	message(mesg[253], 0);
	obj->identified = 1;
	id_scrolls[obj->which_kind].id_status = IDENTIFIED;
	idntfy();
	break;
    case TELEPORT:
	tele();
	break;
    case SLEEP:
	message(mesg[254], 0);
	take_a_nap();
	break;
    case PROTECT_ARMOR:
	if (rogue.armor) {
	    message(mesg[255], 0);
	    rogue.armor->is_protected = 1;
	    rogue.armor->is_cursed = 0;
	} else {
	    message(mesg[256], 0);
	}
	break;
    case REMOVE_CURSE:
	message((!halluc) ? mesg[257] : mesg[258], 0);
	uncurse_all();
	break;
    case CREATE_MONSTER:
	create_monster();
	break;
    case AGGRAVATE_MONSTER:
	aggravate();
	break;
    case MAGIC_MAPPING:
	message(mesg[259], 0);
	draw_magic_map();
	break;
    }
#if !defined( ORIGINAL )
    id_scrolls[obj->which_kind].id_status = IDENTIFIED;
#else /* ORIGINAL */
    if (id_scrolls[obj->which_kind].id_status != CALLED) {
	id_scrolls[obj->which_kind].id_status = IDENTIFIED;
    }
#endif /* ORIGINAL */
    vanish(obj, (obj->which_kind != SLEEP), &rogue.pack);
}
#endif /* not JAPAN */
/* vanish() does NOT handle a quiver of weapons with more than one
   arrow (or whatever) in the quiver.  It will only decrement the count.
*/
    void
vanish(object *obj, short rm, object *pack)
{
    if (obj->quantity > 1) {
	obj->quantity--;
    } else {
	if (obj->in_use_flags & BEING_WIELDED) {
	    unwield(obj);
	} else if (obj->in_use_flags & BEING_WORN) {
	    unwear(obj);
	} else if (obj->in_use_flags & ON_EITHER_HAND) {
	    un_put_on(obj);
	}
	take_from_pack(obj, pack);
	free_object(obj);
    }
    if (rm) {
	(void) reg_move();
    }
}

void
potion_heal(int extra)
{
    long ratio;
    short add;

    rogue.hp_current += rogue.exp;

    ratio = rogue.hp_current * 100L / rogue.hp_max;

    if (ratio >= 100L) {
	rogue.hp_max += (extra ? 2 : 1);
	extra_hp += (extra ? 2 : 1);
	rogue.hp_current = rogue.hp_max;
    } else if (ratio >= 90L) {
	rogue.hp_max += (extra ? 1 : 0);
	extra_hp += (extra ? 1 : 0);
	rogue.hp_current = rogue.hp_max;
    } else {
	if (ratio < 33L) {
	    ratio = 33L;
	}
	if (extra) {
	    ratio += ratio;
	}
	add = (short) (ratio * (rogue.hp_max - rogue.hp_current) / 100L);
	rogue.hp_current += add;
	if (rogue.hp_current > rogue.hp_max) {
	    rogue.hp_current = rogue.hp_max;
	}
    }
    if (blind) {
	unblind();
    }
    if (confused && extra) {
	unconfuse();
    } else if (confused) {
	confused = (confused / 2) + 1;
    }
    if (halluc && extra) {
	unhallucinate();
    } else if (halluc) {
	halluc = (halluc / 2) + 1;
    }
}

void
idntfy(void)
{
    short ch;
    object *obj;
    struct id *id_table;
    char desc[ROGUE_COLUMNS];
AGAIN:
    ch = pack_letter(mesg[260], ALL_OBJECTS);
    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[261], 0);
	message("", 0);
	check_message();
	goto AGAIN;
    }
    obj->identified = 1;
    if (obj->what_is & (SCROL | POTION | WEAPON | ARMOR | WAND | RING)) {
	id_table = get_id_table(obj);
	id_table[obj->which_kind].id_status = IDENTIFIED;
    }
    get_desc(obj, desc, 1);
    message(desc, 0);
}

void
eat(void)
{
    short ch;
    short moves;
    object *obj;
    char buf[70];

    ch = pack_letter(mesg[262], FOOD);
    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[263], 0);
	return;
    }
    if (obj->what_is != FOOD) {
	message(mesg[264], 0);
	return;
    }
    if ((obj->which_kind == FRUIT) || rand_percent(60)) {
	moves = get_rand(900, 1100);
	if (obj->which_kind == RATION) {
#if !defined( ORIGINAL )
	    if (get_rand(1, 10) == 1) {
		message(mesg[265], 0);
	    } else
#endif /* not ORIGINAL */
		message(mesg[266], 0);
	} else {
	    sprintf(buf, mesg[267], fruit);
	    message(buf, 0);
	}
    } else {
	moves = get_rand(700, 900);
	message(mesg[268], 0);
	add_exp(2, 1);
    }
    rogue.moves_left /= 3;
    rogue.moves_left += moves;
    hunger_str[0] = 0;
    print_stats(STAT_HUNGER);

    vanish(obj, 1, &rogue.pack);
}

void
hold_monster(void)
{
    short i, j;
    short mcount = 0;
    object *monster;
    short row, col;

    for (i = -2; i <= 2; i++) {
	for (j = -2; j <= 2; j++) {
	    row = rogue.row + i;
	    col = rogue.col + j;
	    if ((row < MIN_ROW) || (row > (ROGUE_LINES - 2)) || (col < 0) ||
		(col > (ROGUE_COLUMNS - 1))) {
		continue;
	    }
	    if (dungeon[row][col] & MONSTER) {
		monster = object_at(&level_monsters, row, col);
		monster->m_flags |= ASLEEP;
		monster->m_flags &= (~WAKENS);
		mcount++;
	    }
	}
    }
    if (mcount == 0) {
	message(mesg[269], 0);
    } else if (mcount == 1) {
	message(mesg[270], 0);
    } else {
	message(mesg[271], 0);
    }
}

void
tele(void)
{
    mvaddch_rogue(rogue.row, rogue.col,
		  get_dungeon_char(rogue.row, rogue.col));

    if (cur_room >= 0) {
	darken_room(cur_room);
    }
    put_player(get_room_number(rogue.row, rogue.col));
    being_held = 0;
    bear_trap = 0;
}

void
hallucinate(void)
{
    object *obj, *monster;
    short ch;

    if (blind) {
	return;
    }

    obj = level_objects.next_object;

    while (obj) {
	ch = mvinch_rogue(obj->row, obj->col);
	if (((ch < 'A') || (ch > 'Z')) &&
	    ((obj->row != rogue.row) || (obj->col != rogue.col)))
	    if ((ch != ' ') && (ch != '.') && (ch != '#') && (ch != '+')) {
		addch_rogue(gr_obj_char());
	    }
	obj = obj->next_object;
    }
    monster = level_monsters.next_monster;

    while (monster) {
	ch = mvinch_rogue(monster->row, monster->col);
	if ((ch >= 'A') && (ch <= 'Z')) {
	    addch_rogue(get_rand('A', 'Z'));
	}
	monster = monster->next_monster;
    }
}

void
unhallucinate(void)
{
    halluc = 0;
    relight();
    message(mesg[272], 1);
}

void
unblind(void)
{
    blind = 0;
    message(mesg[273], 1);
    relight();
    if (halluc) {
	hallucinate();
    }
    if (detect_monster) {
	show_monsters();
    }
}

void
relight(void)
{
    if (cur_room == PASSAGE) {
	light_passage(rogue.row, rogue.col);
    } else {
	light_up_room(cur_room);
    }
    mvaddch_rogue(rogue.row, rogue.col, rogue.fchar);
}

void
take_a_nap(void)
{
    short i;

    i = get_rand(2, 5);
    sleep(1);

    while (i--) {
	mv_mons();
    }
    sleep(1);
    message(you_can_move_again, 0);
}

void
go_blind(void)
{
    short i, j;

    if (!blind) {
	message(mesg[274], 0);
    }
    blind += get_rand(500, 800);

    if (detect_monster) {
	object *monster;

	monster = level_monsters.next_monster;

	while (monster) {
	    mvaddch_rogue(monster->row, monster->col, monster->trail_char);
	    monster = monster->next_monster;
	}
    }
    if (cur_room >= 0) {
	for (i = rooms[cur_room].top_row + 1;
	     i < rooms[cur_room].bottom_row; i++) {
	    for (j = rooms[cur_room].left_col + 1;
		 j < rooms[cur_room].right_col; j++) {
		mvaddch_rogue(i, j, ' ');
	    }
	}
    }
    mvaddch_rogue(rogue.row, rogue.col, rogue.fchar);
}

char *
get_ench_color(void)
{
    if (halluc) {
	return (id_potions[get_rand(0, POTIONS - 1)].title);
    }
    return (mesg[275]);
}

void
confuse(void)
{
    confused += get_rand(12, 22);
}

void
unconfuse(void)
{
    char msg[80];

    confused = 0;
    if (halluc) {
	sprintf(msg, mesg[276]);
    } else {
	sprintf(msg, mesg[277]);
    }
    message(msg, 1);
}

void
uncurse_all(void)
{
    object *obj;

    obj = rogue.pack.next_object;

    while (obj) {
	obj->is_cursed = 0;
	obj = obj->next_object;
    }
}
