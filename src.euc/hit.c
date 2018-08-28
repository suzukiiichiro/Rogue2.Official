/*
 * hit.c
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
#include "hit.h"
#include "display.h"
#include "level.h"
#include "message.h"
#include "monster.h"
#include "move.h"
#include "object.h"
#include "pack.h"
#include "random.h"
#include "room.h"
#include "score.h"
#include "spechit.h"

extern char *nick_name;
object *fight_monster = 0;
char hit_message[80] = "";

extern short halluc, blind, cur_level;
extern short add_strength, ring_exp, r_rings;
extern boolean being_held, interrupted, wizard;

void
mon_hit(object *monster, char *other, boolean flame)
{
    short damage, hit_chance;
    char *mn;
    long minus;

    if (fight_monster && (monster != fight_monster)) {
	fight_monster = 0;
    }
    monster->trow = NO_ROOM;
    if (cur_level >= (AMULET_LEVEL * 2)) {
	hit_chance = 100;
    } else {
	hit_chance = monster->m_hit_chance;
	hit_chance -= (((2 * rogue.exp) + (2 * ring_exp)) - r_rings);
    }
    if (wizard) {
	hit_chance /= 2;
    }
    if (!fight_monster) {
	interrupted = 1;
    }
    mn = mon_name(monster);

    if (other) {
	hit_chance -= ((rogue.exp + ring_exp) - r_rings);
    }

    if (!rand_percent(hit_chance)) {
	if (!fight_monster) {
	    sprintf(hit_message + strlen(hit_message),
		    mesg[18], (other ? other : mn));
	    message(hit_message, 1);
	    hit_message[0] = 0;
	}
	return;
    }
    if (!fight_monster) {
	sprintf(hit_message + strlen(hit_message),
		mesg[19], (other ? other : mn), (other ? mesg[20] : mesg[21]));
	message(hit_message, 1);
	hit_message[0] = 0;
    }
    if (!(monster->m_flags & STATIONARY)) {
	damage = get_damage(monster->m_damage, 1);
	if (other) {
	    if (flame) {
		if ((damage -= get_armor_class(rogue.armor)) < 0) {
		    damage = 1;
		}
	    }
	}
	if (cur_level >= (AMULET_LEVEL * 2)) {
	    minus = (long) ((AMULET_LEVEL * 2) - cur_level);
	} else {
	    minus = (long) (get_armor_class(rogue.armor) * 3);
	    minus = minus * (long) damage / 100L;
	}
	damage -= (short) minus;
    } else {
	damage = monster->stationary_damage++;
    }
    if (wizard) {
	damage /= 3;
    }
    if (damage > 0) {
	rogue_damage(damage, monster);
    }
    if (monster->m_flags & SPECIAL_HIT) {
	special_hit(monster);
    }
}

void
rogue_hit(object *monster, boolean force_hit)
{
    short damage, hit_chance;

    if (monster) {
	if (check_imitator(monster)) {
	    return;
	}
	hit_chance = force_hit ? 100 : get_hit_chance(rogue.weapon);

	if (wizard) {
	    hit_chance *= 2;
	}
	if (!rand_percent(hit_chance)) {
	    if (!fight_monster) {
		sprintf(hit_message, mesg[22], nick_name);
	    }
	    goto RET;
	}
	damage = get_weapon_damage(rogue.weapon);
	if (wizard) {
	    damage *= 3;
	}
	if (mon_damage(monster, damage)) {	/* still alive? */
	    if (!fight_monster) {
		sprintf(hit_message, mesg[23], nick_name);
	    }
	}
    RET:
	check_gold_seeker(monster);
	wake_up(monster);
    }
}

void
rogue_damage(short d, object *monster)
{
    if (d >= rogue.hp_current) {
	rogue.hp_current = 0;
	print_stats(STAT_HP);
	killed_by(monster, 0);
    }
    rogue.hp_current -= d;
    print_stats(STAT_HP);
}

int
get_damage(char *ds, boolean r)
{
    int i = 0, j, n, d, total = 0;

    while (ds[i]) {
	n = get_number(ds + i);
	while (ds[i++] != 'd')
	    continue;
	d = get_number(ds + i);
	while ((ds[i] != '/') && ds[i]) {
	    i++;
	}

	for (j = 0; j < n; j++) {
	    if (r) {
		total += get_rand(1, d);
	    } else {
		total += d;
	    }
	}
	if (ds[i] == '/') {
	    i++;
	}
    }
    return total;
}

int
get_w_damage(object *obj)
{
    char new_damage[12];
    int to_hit, damage;
    int i = 0;

    if ((!obj) || (obj->what_is != WEAPON)) {
	return -1;
    }
    to_hit = get_number(obj->damage) + obj->hit_enchant;
    while (obj->damage[i++] != 'd')
	continue;
    damage = get_number(obj->damage + i) + obj->d_enchant;

    sprintf(new_damage, "%dd%d", to_hit, damage);

    return (get_damage(new_damage, 1));
}

int
get_number(char *s)
{
    int total = 0;

    while (*s >= '0' && *s <= '9') {
	total = (10 * total) + (*s++ - '0');
    }
    return total;
}

long
lget_number(char *s)
{
    long total = 0;

    while (*s >= '0' && *s <= '9') {
	total = (10 * total) + (*s++ - '0');
    }
    return total;
}

int
to_hit(object *obj)
{
    if (!obj) {
	return 1;
    }
    return (get_number(obj->damage) + obj->hit_enchant);
}

int
damage_for_strength(void)
{
    short strength;
    int i;
    static short sa[] = { 14, 17, 18, 20, 21, 30, 9999 };
    static short ra[] = { 1, 3, 4, 5, 6, 7, 8 };

    strength = rogue.str_current + add_strength;
    if (strength <= 6) {
	return (strength - 5);
    }
    i = 0;
    for (;;) {
	if (strength <= sa[i]) {
	    return (int) ra[i];
	}
	i++;
    }
}

int
mon_damage(object *monster, int damage)
{
    char *mn;
    short row, col;

    monster->hp_to_kill -= damage;

    if (monster->hp_to_kill <= 0) {
	row = monster->row;
	col = monster->col;
	dungeon[row][col] &= ~MONSTER;
	mvaddch_rogue(row, col, get_dungeon_char(row, col));

	fight_monster = 0;
	cough_up(monster);
	mn = mon_name(monster);
	sprintf(hit_message + strlen(hit_message), mesg[24], mn);
	message(hit_message, 1);
	hit_message[0] = 0;
	add_exp(monster->kill_exp, 1);
	take_from_pack(monster, &level_monsters);

	if (monster->m_flags & HOLDS) {
	    being_held = 0;
	}
	free_object(monster);
	return 0;
    }
    return 1;
}

void
fight(boolean to_the_death)
{
    short ch, c;
    short row, col;
    short possible_damage;
    object *monster;

    ch = get_direction();
    if (ch == CANCEL) {
	return;
    }
    row = rogue.row;
    col = rogue.col;
    get_dir_rc(ch, &row, &col, 0);

    c = mvinch_rogue(row, col);
    if (((c < 'A') || (c > 'Z')) ||
	(!can_move(rogue.row, rogue.col, row, col))) {
	message(mesg[25], 0);
	return;
    }
    if (!(fight_monster = object_at(&level_monsters, row, col))) {
	return;
    }
    if (!(fight_monster->m_flags & STATIONARY)) {
	possible_damage = ((get_damage(fight_monster->m_damage, 0) * 2) / 3);
    } else {
	possible_damage = fight_monster->stationary_damage - 1;
    }
    while (fight_monster) {
	(void) one_move_rogue(ch, 0);
	if (((!to_the_death) && (rogue.hp_current <= possible_damage)) ||
	    interrupted || (!(dungeon[row][col] & MONSTER))) {
	    fight_monster = 0;
	} else {
	    monster = object_at(&level_monsters, row, col);
	    if (monster != fight_monster) {
		fight_monster = 0;
	    }
	}
    }
}

void
get_dir_rc(short dir, short *row, short *col, short allow_off_screen)
{
    switch (dir) {
    case 'h':
	if (allow_off_screen || (*col > 0)) {
	    (*col)--;
	}
	break;
    case 'j':
	if (allow_off_screen || (*row < (ROGUE_LINES - 2))) {
	    (*row)++;
	}
	break;
    case 'k':
	if (allow_off_screen || (*row > MIN_ROW)) {
	    (*row)--;
	}
	break;
    case 'l':
	if (allow_off_screen || (*col < (ROGUE_COLUMNS - 1))) {
	    (*col)++;
	}
	break;
    case 'y':
	if (allow_off_screen || ((*row > MIN_ROW) && (*col > 0))) {
	    (*row)--;
	    (*col)--;
	}
	break;
    case 'u':
	if (allow_off_screen || ((*row > MIN_ROW) && (*col < (ROGUE_COLUMNS - 1)))) {
	    (*row)--;
	    (*col)++;
	}
	break;
    case 'n':
	if (allow_off_screen || ((*row < (ROGUE_LINES - 2)) && (*col < (ROGUE_COLUMNS - 1)))) {
	    (*row)++;
	    (*col)++;
	}
	break;
    case 'b':
	if (allow_off_screen || ((*row < (ROGUE_LINES - 2)) && (*col > 0))) {
	    (*row)++;
	    (*col)--;
	}
	break;
    }
}

int
get_hit_chance(object *weapon)
{
    short hit_chance;

    hit_chance = 40 + 3 * to_hit(weapon);
    hit_chance += (((2 * rogue.exp) + (2 * ring_exp)) - r_rings);
    return hit_chance;
}

int
get_weapon_damage(object *weapon)
{
    short damage;

    damage = get_w_damage(weapon) + damage_for_strength();
    damage += ((((rogue.exp + ring_exp) - r_rings) + 1) / 2);
    return damage;
}
