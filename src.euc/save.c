/*
 * save.c
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
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>

#include "rogue.h"
#include "save.h"
#include "display.h"
#include "init.h"
#include "machdep.h"
#include "message.h"
#include "object.h"
#include "pack.h"
#include "ring.h"
#include "score.h"

short write_failed = 0;
char *save_file = "";

extern boolean detect_monster;
extern short cur_level, max_level;
extern char hunger_str[];
extern char login_name[];
extern short party_room;
extern short party_counter;
extern short foods;
extern boolean is_wood[];
extern short cur_room;
extern boolean being_held;
extern short bear_trap;
extern short halluc;
extern short blind;
extern short confused;
extern short levitate;
extern short haste_self;
extern boolean see_invisible;
extern boolean detect_monster;
extern boolean wizard;
extern boolean score_only;
extern short m_moves;

extern boolean msg_cleared;

void
save_game(void)
{
    char fname[64];

#if defined( JAPAN )
    if (!get_input_line("セーブするファイル名は？", save_file, fname,
			"ゲームのセーブを中止しました。", 0, 1)) {
#else /* not JAPAN */
    if (!get_input_line("File name?", save_file, fname,
			"Game not saved", 0, 1)) {
#endif /* not JAPAN */
	return;
    }
    check_message();
    message(fname, 0);
    save_into_file(fname);
}

void
save_into_file(char *sfile)
{
    FILE *fp;
    int file_id;
    char name_buffer[80];
    char *hptr;
    struct rogue_time rt_buf;

#if !defined( ORIGINAL )
    if (org_dir && *org_dir) {
	chdir(org_dir);
    }
#endif /* not ORIGINAL */
    if (sfile[0] == '~') {
	if ((hptr = md_ghome())) {
	    (void) strcpy(name_buffer, hptr);
	    (void) strcat(name_buffer, sfile + 1);
	    sfile = name_buffer;
	}
    }
    if (((fp = fopen(sfile, "wb")) == NULL) ||
	((file_id = md_get_file_id(sfile)) == -1)) {
#if defined( JAPAN )
	message("セーブファイルにアクセスできません。", 0);
#else /* not JAPAN */
	message("Problem accessing the save file", 0);
#endif /* not JAPAN */
	goto err_return;
    }
    md_ignore_signals();
    write_failed = 0;
    (void) xxx(1);
    r_write(fp, (char *) &detect_monster, sizeof(detect_monster));
    r_write(fp, (char *) &cur_level, sizeof(cur_level));
    r_write(fp, (char *) &max_level, sizeof(max_level));
    write_string(hunger_str, fp);
    write_string(login_name, fp);
    r_write(fp, (char *) &party_room, sizeof(party_room));
    r_write(fp, (char *) &party_counter, sizeof(party_counter));
    write_pack(&level_monsters, fp);
    write_pack(&level_objects, fp);
    r_write(fp, (char *) &file_id, sizeof(file_id));
    rw_dungeon(fp, 1);
    r_write(fp, (char *) &foods, sizeof(foods));
    r_write(fp, (char *) &rogue, sizeof(fighter));
    write_pack(&rogue.pack, fp);
    rw_id(id_potions, fp, POTIONS, 1);
    rw_id(id_scrolls, fp, SCROLS, 1);
    rw_id(id_wands, fp, WANDS, 1);
    rw_id(id_rings, fp, RINGS, 1);
    r_write(fp, (char *) traps, (MAX_TRAPS * sizeof(trap)));
    r_write(fp, (char *) is_wood, (WANDS * sizeof(boolean)));
    r_write(fp, (char *) &cur_room, sizeof(cur_room));
    rw_rooms(fp, 1);
    r_write(fp, (char *) &being_held, sizeof(being_held));
    r_write(fp, (char *) &bear_trap, sizeof(bear_trap));
    r_write(fp, (char *) &halluc, sizeof(halluc));
    r_write(fp, (char *) &blind, sizeof(blind));
    r_write(fp, (char *) &confused, sizeof(confused));
    r_write(fp, (char *) &levitate, sizeof(levitate));
    r_write(fp, (char *) &haste_self, sizeof(haste_self));
    r_write(fp, (char *) &see_invisible, sizeof(see_invisible));
    r_write(fp, (char *) &detect_monster, sizeof(detect_monster));
    r_write(fp, (char *) &wizard, sizeof(wizard));
    r_write(fp, (char *) &score_only, sizeof(score_only));
    r_write(fp, (char *) &m_moves, sizeof(m_moves));
    md_gct(&rt_buf);
    rt_buf.second += 10;	/* allow for some processing time */
    r_write(fp, (char *) &rt_buf, sizeof(rt_buf));
    fclose(fp);

    if (write_failed) {
	(void) md_df(sfile);	/* delete file */
    } else {
	clean_up("");
    }

err_return:
    ;
#if !defined( ORIGINAL )
    if (game_dir && *game_dir) {
	chdir(game_dir);
    }
#endif /* not ORIGINAL */
}

void
restore(char *fname)
{
    FILE *fp = { NULL };	/* 未初期化変数の使用の警告除去のため NULL で初期化 */
    struct rogue_time saved_time, mod_time;
    char buf[4];
    char tbuf[40];
    int new_file_id, saved_file_id;

#if !defined( ORIGINAL )
    if (org_dir && *org_dir) {
	chdir(org_dir);
    }
#endif /* ORIGINAL */

    if (((new_file_id = md_get_file_id(fname)) == -1) ||
	((fp = fopen(fname, "rb")) == NULL)) {
#if defined( JAPAN )
	clean_up("ファイルがオープンできませんでした。");
#else /* not JAPAN */
	clean_up("Cannot open file");
#endif /* not JAPAN */
    }
    if (md_link_count(fname) > 1) {
#if defined( JAPAN )
	clean_up("ファイルはリンクされています。");
#else /* not JAPAN */
	clean_up("File has link");
#endif /* not JAPAN */
    }
    (void) xxx(1);
    r_read(fp, (char *) &detect_monster, sizeof(detect_monster));
    r_read(fp, (char *) &cur_level, sizeof(cur_level));
    r_read(fp, (char *) &max_level, sizeof(max_level));
    read_string(hunger_str, fp);

    (void) strcpy(tbuf, login_name);
    read_string(login_name, fp);
    if (strcmp(tbuf, login_name)) {
#if defined( JAPAN )
	clean_up("セーブファイルの持ち主が違います。");
#else /* not JAPAN */
	clean_up("You're not the original player");
#endif /* not JAPAN */
    }

    r_read(fp, (char *) &party_room, sizeof(party_room));
    r_read(fp, (char *) &party_counter, sizeof(party_counter));
    read_pack(&level_monsters, fp, 0);
    read_pack(&level_objects, fp, 0);
    r_read(fp, (char *) &saved_file_id, sizeof(saved_file_id));
    if (new_file_id != saved_file_id) {
#if defined( JAPAN )
	clean_up("これは元のセーブファイルではありません。");
#else /* not JAPAN */
	clean_up("Sorry, saved game is not in the same file");
#endif /* not JAPAN */
    }
    rw_dungeon(fp, 0);
    r_read(fp, (char *) &foods, sizeof(foods));
    r_read(fp, (char *) &rogue, sizeof(fighter));
    read_pack(&rogue.pack, fp, 1);
    rw_id(id_potions, fp, POTIONS, 0);
    rw_id(id_scrolls, fp, SCROLS, 0);
    rw_id(id_wands, fp, WANDS, 0);
    rw_id(id_rings, fp, RINGS, 0);
    r_read(fp, (char *) traps, (MAX_TRAPS * sizeof(trap)));
    r_read(fp, (char *) is_wood, (WANDS * sizeof(boolean)));
    r_read(fp, (char *) &cur_room, sizeof(cur_room));
    rw_rooms(fp, 0);
    r_read(fp, (char *) &being_held, sizeof(being_held));
    r_read(fp, (char *) &bear_trap, sizeof(bear_trap));
    r_read(fp, (char *) &halluc, sizeof(halluc));
    r_read(fp, (char *) &blind, sizeof(blind));
    r_read(fp, (char *) &confused, sizeof(confused));
    r_read(fp, (char *) &levitate, sizeof(levitate));
    r_read(fp, (char *) &haste_self, sizeof(haste_self));
    r_read(fp, (char *) &see_invisible, sizeof(see_invisible));
    r_read(fp, (char *) &detect_monster, sizeof(detect_monster));
    r_read(fp, (char *) &wizard, sizeof(wizard));
    r_read(fp, (char *) &score_only, sizeof(score_only));
    r_read(fp, (char *) &m_moves, sizeof(m_moves));
    r_read(fp, (char *) &saved_time, sizeof(saved_time));

    if (fread(buf, sizeof(char), 1, fp) > 0) {
	clear();
#if defined( JAPAN )
	clean_up("ファイル中によけいな文字があります。");
#else /* JAPAN */
	clean_up("Extra characters in file");
#endif /* JAPAN */
    }

    md_gfmt(fname, &mod_time);	/* get file modification time */

    if (has_been_touched(&saved_time, &mod_time)) {
	clear();
#if defined( JAPAN )
	clean_up("ファイルが変更されています。");
#else /* not JAPAN */
	clean_up("Sorry, file has been touched");
#endif /* not JAPAN */
    }
    fclose(fp);
    if ((!wizard) && !md_df(fname)) {
#if defined( JAPAN )
	clean_up("ファイルを消すことができません。");
#else /* not JAPAN */
	clean_up("Cannot delete file");
#endif /* not JAPAN */
    }
    msg_cleared = 0;
    ring_stats(0);

#if !defined( ORIGINAL )
    if (game_dir && *game_dir) {
	chdir(game_dir);
    }
#endif /* not ORIGINAL */
}

void
write_pack(object *pack, FILE *fp)
{
    object t;

    while ((pack = pack->next_object)) {
	r_write(fp, (char *) pack, sizeof(object));
    }
    t.ichar = t.what_is = 0;
    r_write(fp, (char *) &t, sizeof(object));
}

void
read_pack(object *pack, FILE *fp, boolean is_rogue)
{
    object read_obj, *new_obj;

    for (;;) {
	r_read(fp, (char *) &read_obj, sizeof(object));
	if (read_obj.ichar == 0) {
	    pack->next_object = (object *) 0;
	    break;
	}
	new_obj = alloc_object();
	*new_obj = read_obj;
	if (is_rogue) {
	    if (new_obj->in_use_flags & BEING_WORN) {
		do_wear(new_obj);
	    } else if (new_obj->in_use_flags & BEING_WIELDED) {
		do_wield(new_obj);
	    } else if (new_obj->in_use_flags & (ON_EITHER_HAND)) {
		do_put_on(new_obj, ((new_obj->in_use_flags & ON_LEFT_HAND)
				    ? 1 : 0));
	    }
	}
	pack->next_object = new_obj;
	pack = new_obj;
    }
}

/*
 * rw_dungeon
 * セーブデータからのダンジョン情報の保存と復元
 */
void
rw_dungeon(FILE *fp, boolean rw)
{
    short i, j;
    char buf[ROGUE_COLUMNS];

    if (rw) {    // 書き込み時実行
	for (i = 0; i < ROGUE_LINES; i++) {
	    r_write(fp, (char *) dungeon[i], (ROGUE_COLUMNS * sizeof(dungeon[0][0])));
	    for (j = 0; j < ROGUE_COLUMNS; j++) {
		buf[j] = mvinch_rogue(i, j);
	    }
	    r_write(fp, buf, ROGUE_COLUMNS);
	}
	return;
    } else {    // 読み込み時実行
	for (i = 0; i < ROGUE_LINES; i++) {
	    r_read(fp, (char *) dungeon[i], (ROGUE_COLUMNS * sizeof(dungeon[0][0])));
	    r_read(fp, buf, ROGUE_COLUMNS);

	    if ( i < ROGUE_LINES - 1 ) {
		for (j = 0; j < ROGUE_COLUMNS; j++) {
		    mvaddch_rogue(i, j, (unsigned char) buf[j]);
		}
	    } else {
		print_stats(STAT_ALL);
	    }
	}
	return;
    }
}

void
rw_id(struct id id_table[], FILE *fp, int n, boolean wr)
{
    short i;

    for (i = 0; i < n; i++) {
	if (wr) {
	    r_write(fp, (char *) &(id_table[i].value), sizeof(short));
	    r_write(fp, (char *) &(id_table[i].id_status),
		    sizeof(unsigned short));
	    write_string(id_table[i].title, fp);
	} else {
	    r_read(fp, (char *) &(id_table[i].value), sizeof(short));
	    r_read(fp, (char *) &(id_table[i].id_status),
		   sizeof(unsigned short));
	    read_string(id_table[i].title, fp);
	}
    }
}

void
write_string(char *s, FILE *fp)
{
    short n;

    n = strlen(s) + 1;
    xxxx(s, n);
    r_write(fp, (char *) &n, sizeof(short));
    r_write(fp, s, n);
}

void
read_string(char *s, FILE *fp)
{
    short n;

    r_read(fp, (char *) &n, sizeof(short));
    r_read(fp, s, n);
    xxxx(s, n);
}

void
rw_rooms(FILE *fp, boolean rw)
{
    short i;

    for (i = 0; i < MAXROOMS; i++) {
	rw ? r_write(fp, (char *) (rooms + i), sizeof(room)) :
	    r_read(fp, (char *) (rooms + i), sizeof(room));
    }
}

void
r_read(FILE *fp, char *buf, int n)
{
    if (fread(buf, sizeof(char), n, fp) != n) {
#if defined( JAPAN )
	clean_up("ファイルが読めません。");
#else /* not JAPAN */
	clean_up("Read() failed, don't know why");
#endif /* not JAPAN */
    }
}

void
r_write(FILE *fp, char *buf, int n)
{
    if (!write_failed) {
	if (fwrite(buf, sizeof(char), n, fp) != n) {
#if defined( JAPAN )
	    message("ファイルに書けません。", 0);
#else /* not JAPAN */
	    message("Write() failed, don't know why", 0);
#endif /* not JAPAN */
	    sound_bell();
	    write_failed = 1;
	}
    }
}

boolean
has_been_touched(struct rogue_time *saved_time, struct rogue_time *mod_time)
{
#if !defined( ORIGINAL )
    short *sav, *mod;
    int i;

    sav = (short *) saved_time;
    mod = (short *) mod_time;
    for (i = 0; i < 6; i++) {
	if (*sav < *mod)
	    return 1;
	else if (*sav++ > *mod++)
	    return 0;
    }
    return 0;
#else /* ORIGINAL */
    if (saved_time->year < mod_time->year) {
	return 1;
    } else if (saved_time->year > mod_time->year) {
	return 0;
    }
    if (saved_time->month < mod_time->month) {
	return 1;
    } else if (saved_time->month > mod_time->month) {
	return 0;
    }
    if (saved_time->day < mod_time->day) {
	return 1;
    } else if (saved_time->day > mod_time->day) {
	return 0;
    }
    if (saved_time->hour < mod_time->hour) {
	return 1;
    } else if (saved_time->hour > mod_time->hour) {
	return 0;
    }
    if (saved_time->minute < mod_time->minute) {
	return 1;
    } else if (saved_time->minute > mod_time->minute) {
	return 0;
    }
    if (saved_time->second < mod_time->second) {
	return 1;
    }
    return 0;
#endif /* ORIGINAL */
}
