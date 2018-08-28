/*
 * pack.c
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

#include "rogue.h"
#include "pack.h"
#include "invent.h"
#include "message.h"
#include "monster.h"
#include "move.h"
#include "object.h"
#include "ring.h"
#include "use.h"

char *curse_message = mesg[85];

object *
add_to_pack(object *obj, object *pack, int condense)
{
    object *op, *p;

    if (condense) {
	if ((op = check_duplicate(obj, pack))) {
	    free_object(obj);
	    return op;
	} else {
	    obj->ichar = next_avail_ichar();
	}
    }
#if !defined( ORIGINAL )
    for (op = pack; op->next_object; op = op->next_object) {
	if (op->next_object->what_is > obj->what_is) {
	    p = op->next_object;
	    op->next_object = obj;
	    obj->next_object = p;
	    return obj;
	}
    }
    op->next_object = obj;
    obj->next_object = 0;
    return obj;
#else /* ORIGINAL */
    if (pack->next_object == 0) {
	pack->next_object = obj;
    } else {
	op = pack->next_object;

	while (op->next_object) {
	    op = op->next_object;
	}
	op->next_object = obj;
    }
    obj->next_object = 0;
    return obj;
#endif /* ORIGINAL */
}

void
take_from_pack(object *obj, object *pack)
{
    while (pack->next_object != obj) {
	pack = pack->next_object;
    }
    pack->next_object = pack->next_object->next_object;
}

object *
pick_up(int row, int col, short *status)
{
    object *obj;

    obj = object_at(&level_objects, row, col);
    *status = 1;

    if ((obj->what_is == SCROL) && (obj->which_kind == SCARE_MONSTER) &&
	obj->picked_up) {
	message(mesg[86], 0);
	dungeon[row][col] &= (~OBJECT);
	vanish(obj, 0, &level_objects);
	*status = 0;
	if (id_scrolls[SCARE_MONSTER].id_status == UNIDENTIFIED) {
	    id_scrolls[SCARE_MONSTER].id_status = IDENTIFIED;
	}
	return 0;
    }
    if (obj->what_is == GOLD) {
	rogue.gold += obj->quantity;
	dungeon[row][col] &= ~(OBJECT);
	take_from_pack(obj, &level_objects);
	print_stats(STAT_GOLD);
	return obj;		/* obj will be free_object()ed in one_move_rogue() */
    }
    if (pack_count(obj) >= MAX_PACK_COUNT) {
	message(mesg[87], 1);
	return 0;
    }
    dungeon[row][col] &= ~(OBJECT);
    take_from_pack(obj, &level_objects);
    obj = add_to_pack(obj, &rogue.pack, 1);
    obj->picked_up = 1;
    return obj;
}

void
drop(void)
{
    object *obj, *new;
    short ch;
    char desc[ROGUE_COLUMNS];

    if (dungeon[rogue.row][rogue.col] & (OBJECT | STAIRS | TRAP)) {
	message(mesg[88], 0);
	return;
    }
    if (!rogue.pack.next_object) {
	message(mesg[89], 0);
	return;
    }
    if ((ch = pack_letter(mesg[90], ALL_OBJECTS)) == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[91], 0);
	return;
    }
    if (obj->in_use_flags & BEING_WIELDED) {
	if (obj->is_cursed) {
	    message(curse_message, 0);
	    return;
	}
	unwield(rogue.weapon);
    } else if (obj->in_use_flags & BEING_WORN) {
	if (obj->is_cursed) {
	    message(curse_message, 0);
	    return;
	}
	mv_aquatars();
	unwear(rogue.armor);
	print_stats(STAT_ARMOR);
    } else if (obj->in_use_flags & ON_EITHER_HAND) {
	if (obj->is_cursed) {
	    message(curse_message, 0);
	    return;
	}
	un_put_on(obj);
    }
    obj->row = rogue.row;
    obj->col = rogue.col;

    if ((obj->quantity > 1) && (obj->what_is != WEAPON)) {
	obj->quantity--;
	new = alloc_object();
	*new = *obj;
	new->quantity = 1;
	obj = new;
    } else {
	obj->ichar = 'L';
	take_from_pack(obj, &rogue.pack);
    }
    place_at(obj, rogue.row, rogue.col);
#if defined( JAPAN )
    get_desc(obj, desc, 0);
    (void) strcat(desc, mesg[92]);
#else /* not JAPAN */
    (void) strcpy(desc, mesg[92]);
    get_desc(obj, desc + strlen(mesg[92]), 0);
#endif /* not JAPAN */
    message(desc, 0);
    (void) reg_move();
}

object *
check_duplicate(object *obj, object *pack)
{
    object *op;

    if (!(obj->what_is & (WEAPON | FOOD | SCROL | POTION))) {
	return 0;
    }
    if ((obj->what_is == FOOD) && (obj->which_kind == FRUIT)) {
	return 0;
    }
    op = pack->next_object;

    while (op) {
	if ((op->what_is == obj->what_is) &&
	    (op->which_kind == obj->which_kind)) {

	    if ((obj->what_is != WEAPON) ||
		((obj->what_is == WEAPON) &&
		 ((obj->which_kind == ARROW) ||
		  (obj->which_kind == DAGGER) ||
		  (obj->which_kind == DART) ||
		  (obj->which_kind == SHURIKEN)) &&
		 (obj->quiver == op->quiver))) {
		op->quantity += obj->quantity;
		return op;
	    }
	}
	op = op->next_object;
    }
    return 0;
}

int
next_avail_ichar(void)
{
    object *obj;
    int i;
    boolean ichars[26];

    for (i = 0; i < 26; i++) {
	ichars[i] = 0;
    }
    obj = rogue.pack.next_object;
    while (obj) {
	ichars[(obj->ichar - 'a')] = 1;
	obj = obj->next_object;
    }
    for (i = 0; i < 26; i++) {
	if (!ichars[i]) {
	    return (i + 'a');
	}
    }
    return '?';
}

void
wait_for_ack(void)
{
    while (rgetchar() != ' ');
}

int
pack_letter(char *prompt, unsigned short mask)
{
    short ch;
    unsigned short tmask = mask;

    if (!mask_pack(&rogue.pack, mask)) {
	message(mesg[93], 0);
	return (CANCEL);
    }
    for (;;) {

	message(prompt, 0);

	for (;;) {
	    ch = rgetchar();
	    if (!is_pack_letter(&ch, &mask)) {
		sound_bell();
	    } else {
		break;
	    }
	}

	if (ch == LIST) {
	    check_message();
	    inventory(&rogue.pack, mask);
	} else {
	    break;
	}
	mask = tmask;
    }
    check_message();
    return ch;
}

void
take_off(void)
{
    char desc[ROGUE_COLUMNS];
    object *obj;

    if (rogue.armor) {
	if (rogue.armor->is_cursed) {
	    message(curse_message, 0);
	} else {
	    mv_aquatars();
	    obj = rogue.armor;
	    unwear(rogue.armor);
#if defined( JAPAN )
	    get_desc(obj, desc, 0);
	    (void) strcat(desc, mesg[94]);
#else /* not JAPAN */
	    (void) strcpy(desc, mesg[94]);
	    get_desc(obj, desc + strlen(mesg[94]), 0);
#endif /* not JAPAN */
	    message(desc, 0);
	    print_stats(STAT_ARMOR);
	    (void) reg_move();
	}
    } else {
	message(mesg[95], 0);
    }
}

void
wear(void)
{
    short ch;
    object *obj;
    char desc[ROGUE_COLUMNS];

    if (rogue.armor) {
	message(mesg[96], 0);
	return;
    }
    ch = pack_letter(mesg[97], ARMOR);

    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[98], 0);
	return;
    }
    if (obj->what_is != ARMOR) {
	message(mesg[99], 0);
	return;
    }
    obj->identified = 1;
#if defined( JAPAN )
    get_desc(obj, desc, 0);
    (void) strcat(desc, mesg[100]);
#else /* not JAPAN */
    (void) strcpy(desc, mesg[100]);
    get_desc(obj, desc + strlen(mesg[100]), 0);
#endif /* not JAPAN */
    message(desc, 0);
    do_wear(obj);
    print_stats(STAT_ARMOR);
    (void) reg_move();
}

void
unwear(object *obj)
{
    if (obj) {
	obj->in_use_flags &= (~BEING_WORN);
    }
    rogue.armor = (object *) 0;
}

void
do_wear(object *obj)
{
    rogue.armor = obj;
    obj->in_use_flags |= BEING_WORN;
    obj->identified = 1;
}

void
wield(void)
{
    short ch;
    object *obj;
    char desc[ROGUE_COLUMNS];

    if (rogue.weapon && rogue.weapon->is_cursed) {
	message(curse_message, 0);
	return;
    }
    ch = pack_letter(mesg[101], WEAPON);

    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[102], 0);
	return;
    }
    if (obj->what_is & (ARMOR | RING)) {
	sprintf(desc, mesg[103],
		((obj->what_is == ARMOR) ? mesg[104] : mesg[105]));
	message(desc, 0);
	return;
    }
    if (obj->in_use_flags & BEING_WIELDED) {
	message(mesg[106], 0);
    } else {
	unwield(rogue.weapon);
#if defined( JAPAN )
	get_desc(obj, desc, 0);
	(void) strcat(desc, mesg[107]);
#else /* not JAPAN */
	(void) strcpy(desc, mesg[107]);
	get_desc(obj, desc + strlen(mesg[107]), 0);
#endif /* not JAPAN */
	message(desc, 0);
	do_wield(obj);
	(void) reg_move();
    }
}

void
do_wield(object *obj)
{
    rogue.weapon = obj;
    obj->in_use_flags |= BEING_WIELDED;
}

void
unwield(object *obj)
{
    if (obj) {
	obj->in_use_flags &= (~BEING_WIELDED);
    }
    rogue.weapon = (object *) 0;
}

void
call_it(void)
{
    short ch;
    object *obj;
    struct id *id_table;
    char buf[MAX_TITLE_LENGTH + 2];

    ch = pack_letter(mesg[108], (SCROL | POTION | WAND | RING));

    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[109], 0);
	return;
    }
    if (!(obj->what_is & (SCROL | POTION | WAND | RING))) {
	message(mesg[110], 0);
	return;
    }
    id_table = get_id_table(obj);

#if defined( JAPAN )
    if (get_input_line(mesg[111],
		       "", buf, id_table[obj->which_kind].title, 0, 1)) {
	ch = *buf;
#if defined( EUC )
	if (ch >= ' ' && !(ch & 0x80)) {	/* by Yasha */
	    /* alphabet or kana character; append 1 blank */
	    (void) strcat(buf, " ");
	}
#else /* not EUC */
	if (ch >= ' ' && ch <= '~' || ch >= 0xa0 && ch <= 0xde) {
	    /* alphabet or kana character; append 1 blank */
	    (void) strcat(buf, " ");
	}
#endif /* not EUC */
	id_table[obj->which_kind].id_status = CALLED;
	(void) strcpy(id_table[obj->which_kind].title, buf);
    }
#else /* not JAPAN */
    if (get_input_line(mesg[111],
		       "", buf, id_table[obj->which_kind].title, 1, 1)) {
	id_table[obj->which_kind].id_status = CALLED;
	(void) strcpy(id_table[obj->which_kind].title, buf);
    }
#endif /* not JAPAN */
}

int
pack_count(object *new_obj)
{
    object *obj;
    short count = 0;

    obj = rogue.pack.next_object;

    while (obj) {
	if (obj->what_is != WEAPON) {
	    count += obj->quantity;
	} else if (!new_obj) {
	    count++;
	} else if ((new_obj->what_is != WEAPON) ||
		   ((obj->which_kind != ARROW) &&
		    (obj->which_kind != DAGGER) &&
		    (obj->which_kind != DART) &&
		    (obj->which_kind != SHURIKEN)) ||
		   (new_obj->which_kind != obj->which_kind) ||
		   (obj->quiver != new_obj->quiver)) {
	    count++;
	}
	obj = obj->next_object;
    }
    return (count);
}

boolean
mask_pack(object *pack, unsigned short mask)
{
    while (pack->next_object) {
	pack = pack->next_object;
	if (pack->what_is & mask) {
	    return 1;
	}
    }
    return 0;
}

int
is_pack_letter(short *c, unsigned short *mask)
{
    switch (*c) {
    case '?':
	*mask = SCROL;
	goto found;
    case '!':
	*mask = POTION;
	goto found;
    case ':':
	*mask = FOOD;
	goto found;
    case ')':
	*mask = WEAPON;
	goto found;
    case ']':
	*mask = ARMOR;
	goto found;
    case '/':
	*mask = WAND;
	goto found;
    case '=':
	*mask = RING;
	goto found;
    case ',':
	*mask = AMULET;
	goto found;
    default:
	return ((*c >= 'a' && *c <= 'z') || *c == CANCEL || *c == LIST);
    }
found:
    *c = LIST;
    return 1;
}

int
has_amulet(void)
{
    return (mask_pack(&rogue.pack, AMULET));
}

void
kick_into_pack(void)
{
    object *obj;
    char *p;
    char desc[ROGUE_COLUMNS];
    short stat;
    extern short levitate;

    if (!(dungeon[rogue.row][rogue.col] & OBJECT)) {
	message(mesg[112], 0);
    } else {
#if !defined( ORIGINAL )
	if (levitate) {
	    message(mesg[113], 0);
	    return;
	}
#endif /* ORIGINAL */
	if ((obj = pick_up(rogue.row, rogue.col, &stat))) {
	    get_desc(obj, desc, 1);
#if defined( JAPAN )
	    (void) strcat(desc, mesg[114]);
#endif /* JAPAN */
	    if (obj->what_is == GOLD) {
		message(desc, 0);
		free_object(obj);
	    } else {
		p = desc + strlen(desc);
		*p++ = '(';
		*p++ = obj->ichar;
		*p++ = ')';
		*p = 0;
		message(desc, 0);
	    }
	}
	if (obj || (!stat)) {
	    (void) reg_move();
	}
    }
}
