/*
 * message.c
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
#include "message.h"
#include "display.h"
#include "init.h"
#include "machdep.h"
#include "move.h"
#include "object.h"
#include "pack.h"

#define	CTRL(c)	((c) & 037)

char msg_line[ROGUE_COLUMNS] = "";
short msg_col = 0;
boolean msg_cleared = 1;
char hunger_str[8] = "";

extern boolean cant_int, did_int, interrupted, save_is_interactive;
extern short add_strength;
extern short cur_level;

void
message(char *msg, boolean intrpt)
{
    if (!save_is_interactive) {
	return;
    }
    if (intrpt) {
	interrupted = 1;
    }
    cant_int = 1;

    if (!msg_cleared) {
	mvaddstr_rogue(MIN_ROW - 1, msg_col, mesg[11]);
	refresh();
	wait_for_ack();
	check_message();
    }
    (void) strcpy(msg_line, msg);
    mvaddstr_rogue(MIN_ROW - 1, 0, msg);
    addch_rogue(' ');
    refresh();
    msg_cleared = 0;
    msg_col = strlen(msg);

    cant_int = 0;
    if (did_int) {
	did_int = 0;
	onintr(0);		/* 「0」に意味はないが警告除去のために値を入れる。onintr関数を見直す必要がある。 */
    }
}

void
remessage(void)
{
    if (msg_line[0]) {
	message(msg_line, 0);
    }
}

void
check_message(void)
{
    if (msg_cleared) {
	return;
    }
    move(MIN_ROW - 1, 0);
    clrtoeol();
    refresh();
    msg_cleared = 1;
}

int
get_direction(void)
{
    int dir;

    message(mesg[55], 0);
    while (!is_direction(dir = rgetchar())) {
	sound_bell();
    }
    check_message();
    return dir;
}

int
get_input_line(char *prompt, char *insert, char *buf, char *if_cancelled,
	       boolean add_blank, boolean do_echo)
{
    int n;

    n = do_input_line(1, 0, 0, prompt, insert,
		      buf, if_cancelled, add_blank, do_echo, 0);
    return ((n < 0) ? 0 : n);
}

int
input_line(int row, int col, char *insert, char *buf, int ch)
{
    return do_input_line(0, row, col, "", insert, buf, "", 0, 1, ch);
}

int
do_input_line(boolean is_msg, int row, int col, char *prompt, char *insert,
	      char *buf, char *if_cancelled, boolean add_blank,
	      boolean do_echo, int first_ch)
{
    short ch;
    short i = 0, n = 0;
#if defined( JAPAN )
    short k;
    char kanji[MAX_TITLE_LENGTH];
#endif /* JAPAN */

    if (is_msg) {
	message(prompt, 0);
	n = strlen(prompt) + 1;
    } else {
	mvaddstr_rogue(row, col, prompt);
    }

    if (insert[0]) {
	mvaddstr_rogue(row, col + n, insert);
	(void) strcpy(buf, insert);
	i = strlen(insert);
#if defined( JAPAN )
	k = 0;
	while (k < i) {
	    ch = insert[k];
#if defined( EUC )
	    if (ch & 0x80) {	/* for EUC code by Yasha */
		kanji[k] = kanji[k + 1] = 1;
		k += 2;
	    } else {
		kanji[k] = 0;
		k++;
	    }
#else /* not EUC */
	    if (ch >= 0x81 && ch <= 0x9f || ch >= 0xe0 && ch <= 0xfc) {
		kanji[k] = kanji[k + 1] = 1;
		k += 2;
	    } else {
		kanji[k] = 0;
		k++;
	    }
#endif /* not EUC */
	}
#endif /* JAPAN */
	move(row, col + n + i);
	refresh();
    }
#if defined( JAPAN )
    for (;;) {
	if (first_ch) {
	    ch = first_ch;
	    first_ch = 0;
	} else {
	    ch = rgetchar();
	}
	if (ch == '\r' || ch == '\n' || ch == CANCEL) {
	    break;
	}
	if ((ch == '\b') && (i > 0)) {
	    i -= kanji[i - 1] ? 2 : 1;
	    if (do_echo) {
		mvaddstr_rogue(row, col + n + i, "  ");
		move(row, col + n + i);
	    }
	} else if (
#if defined( EUC )
	    (ch >= ' ' && !(ch & 0x80)) && (i < MAX_TITLE_LENGTH - 2)
#else /* Shift JIS */
	    (ch >= ' ' && ch <= '~' || ch >= 0xa0 && ch <= 0xde) && (i < MAX_TITLE_LENGTH - 2)
#endif /* not EUC */
	    ) {
	    if ((ch != ' ') || (i > 0)) {
		buf[i] = ch;
		kanji[i] = 0;
		if (do_echo) {
		    addch(ch);
		}
		i++;
	    }
	} else if (
#if defined( EUC )
	    (ch & 0x80) && (i < MAX_TITLE_LENGTH - 3)
#else /* Shift JIS */
	    (ch >= 0x81 && ch <= 0x9f || ch >= 0xe0 && ch <= 0xfc) && (i < MAX_TITLE_LENGTH - 3)
#endif /* not EUC */
	    ) {
	    buf[i] = ch;
	    buf[i + 1] = rgetchar();
	    kanji[i] = kanji[i + 1] = 1;
	    if (do_echo) {
		addch_rogue(buf[i]);
		addch_rogue(buf[i + 1]);
	    }
	    i += 2;
	}
	refresh();
    }
    if (is_msg) {
	check_message();
    }
    while ((i > 0) && (buf[i - 1] == ' ') && (kanji[i - 1] == 0)) {
	i--;
    }
    if (add_blank) {
	buf[i++] = ' ';
    }
#else /* not JAPAN */
	while (((ch = rgetchar()) != '\r') && (ch != '\n') && (ch != CANCEL)) {
	if ((ch >= ' ') && (ch <= '~') && (i < MAX_TITLE_LENGTH - 2)) {
	    if ((ch != ' ') || (i > 0)) {
		buf[i++] = ch;
		if (do_echo) {
		    addch_rogue(ch);
		}
	    }
	}
	if ((ch == '\b') && (i > 0)) {
	    i--;
	    if (do_echo) {
		mvaddch_rogue(row, col + n + i, ' ');
		move(row, col + n + i);
	    }
	}
	refresh();
    }
    if (is_msg) {
	check_message();
    }
    if (add_blank) {
	buf[i++] = ' ';
    } else {
	while ((i > 0) && (buf[i - 1] == ' ')) {
	    i--;
	}
    }
#endif /* not JAPAN */
	buf[i] = 0;

    if ((ch == CANCEL) || (i == 0) || ((i == 1) && add_blank)) {
	if (is_msg && if_cancelled) {
	    message(if_cancelled, 0);
	}
	return ((ch == CANCEL) ? -1 : 0);
    }
    return i;
}

int
rgetchar(void)
{
    int ch;

    for (;;) {
	ch = getch();

	switch (ch) {
	case '\022':
	    wrefresh(curscr);
	    break;
#if !defined( ORIGINAL )
	    /*
	     * can't use X for save_screen purpose
	     * because 2nd byte of kanji might be an 'X'!
	     */
	case CTRL('D'):
#else /* ORIGINAL */
	case 'X':
#endif /* ORIGINAL */
	    save_screen();
	    break;
	default:
	    return ch;
	}
    }
}

/*
Level: 99 Gold: 999999 Hp: 999(999) Str: 99(99) Arm: 99 Exp: 21/10000000 Hungry
階: 99 金塊: 999999 体力: 999(999) 強さ: 99(99) 守備: 99 経験: 21/10000000 空腹
0    5    1    5    2    5    3    5    4    5    5    5    6    5    7    5
*/

void
print_stats(int stat_mask)
{
    char buf[16];
    boolean label;
    int row = ROGUE_LINES - 1;

    label = (stat_mask & STAT_LABEL) ? 1 : 0;

    if (stat_mask & STAT_LEVEL) {
	if (label) {
	    mvaddstr_rogue(row, 0, mesg[56]);
	}
	/* max level taken care of in make_level() */
	sprintf(buf, "%d", cur_level);
#if defined( JAPAN )
	mvaddstr_rogue(row, 4, buf);
#else /* not JAPAN */
	mvaddstr_rogue(row, 7, buf);
#endif /* not JAPAN */
	pad(buf, 2);
    }
    if (stat_mask & STAT_GOLD) {
	if (label) {
	    if (rogue.gold > MAX_GOLD) {
		rogue.gold = MAX_GOLD;
	    }
#if defined( JAPAN )
	    mvaddstr_rogue(row, 7, mesg[57]);
#else /* not JAPAN */
	    mvaddstr_rogue(row, 10, mesg[57]);
#endif /* not JAPAN */
	}
	sprintf(buf, "%ld", rogue.gold);
#if defined( JAPAN )
	mvaddstr_rogue(row, 13, buf);
#else /* not JAPAN */
	mvaddstr_rogue(row, 16, buf);
#endif /* not JAPAN */
	pad(buf, 6);
    }
    if (stat_mask & STAT_HP) {
	if (label) {
#if defined( JAPAN )
	    mvaddstr_rogue(row, 20, mesg[58]);
#else /* not JAPAN */
	    mvaddstr_rogue(row, 23, mesg[58]);
#endif /* not JAPAN */
	    if (rogue.hp_max > MAX_HP) {
		rogue.hp_current -= (rogue.hp_max - MAX_HP);
		rogue.hp_max = MAX_HP;
	    }
	}
	sprintf(buf, "%d(%d)", rogue.hp_current, rogue.hp_max);
#if defined( JAPAN )
	mvaddstr_rogue(row, 26, buf);
#else /* not JAPAN */
	mvaddstr_rogue(row, 27, buf);
#endif /* not JAPAN */
	pad(buf, 8);
    }
    if (stat_mask & STAT_STRENGTH) {
	if (label) {
#if defined( JAPAN )
	    mvaddstr_rogue(row, 35, mesg[59]);
#else /* not JAPAN */
	    mvaddstr_rogue(row, 36, mesg[59]);
#endif /* not JAPAN */
	}
	if (rogue.str_max > MAX_STRENGTH) {
	    rogue.str_current -= (rogue.str_max - MAX_STRENGTH);
	    rogue.str_max = MAX_STRENGTH;
	}
	sprintf(buf, "%d(%d)", (rogue.str_current + add_strength),
		rogue.str_max);
	mvaddstr_rogue(row, 41, buf);
	pad(buf, 6);
    }
    if (stat_mask & STAT_ARMOR) {
	if (label) {
	    mvaddstr_rogue(row, 48, mesg[60]);
	}
	if (rogue.armor && (rogue.armor->d_enchant > MAX_ARMOR)) {
	    rogue.armor->d_enchant = MAX_ARMOR;
	}
	sprintf(buf, "%d", get_armor_class(rogue.armor));
#if defined( JAPAN )
	mvaddstr_rogue(row, 54, buf);
#else /* not JAPAN */
	mvaddstr_rogue(row, 53, buf);
#endif /* not JAPAN */
	pad(buf, 2);
    }
    if (stat_mask & STAT_EXP) {
	if (label) {
#if defined( JAPAN )
	    mvaddstr_rogue(row, 57, mesg[61]);
#else /* not JAPAN */
	    mvaddstr_rogue(row, 56, mesg[61]);
#endif /* not JAPAN */
	}
	/*  Max exp taken care of in add_exp() */
	sprintf(buf, "%d/%ld", rogue.exp, rogue.exp_points);
#if defined( JAPAN )
	mvaddstr_rogue(row, 63, buf);
#else /* not JAPAN */
	mvaddstr_rogue(row, 61, buf);
#endif /* not JAPAN */
	pad(buf, 11);
    }
    if (stat_mask & STAT_HUNGER) {
#if defined( JAPAN )
	mvaddstr_rogue(row, 75, hunger_str);
	clrtoeol();
#else /* not JAPAN */
	mvaddstr_rogue(row, 73, hunger_str);
	clrtoeol();
#endif /* not JAPAN */
    }
    refresh();
}

void
pad(char *s, short n)
{
    short i;

    for (i = strlen(s); i < n; i++) {
	addch_rogue(' ');
    }
}

void
save_screen(void)
{
    FILE *fp;
    short i, j;
    char buf[ROGUE_COLUMNS + 2];
    boolean found_non_blank;

    if ((fp = fopen("rogue.screen", "w")) != NULL) {
	for (i = 0; i < ROGUE_LINES; i++) {
	    found_non_blank = 0;
	    for (j = (ROGUE_COLUMNS - 1); j >= 0; j--) {
		buf[j] = mvinch_rogue(i, j);
		if (!found_non_blank) {
		    if ((buf[j] != ' ') || (j == 0)) {
			buf[j + ((j == 0) ? 0 : 1)] = 0;
			found_non_blank = 1;
		    }
		}
	    }
	    fputs(buf, fp);
	    putc('\n', fp);
	}
	fclose(fp);
    } else {
	sound_bell();
    }
}

void
sound_bell(void)
{
    putchar(7);
    fflush(stdout);
}

boolean
is_digit(short ch)
{
    return (boolean) ((ch >= '0') && (ch <= '9'));
}

int
r_index(char *str, int ch, boolean last)
{
    int i;

    if (last) {
	for (i = strlen(str) - 1; i >= 0; i--) {
	    if (str[i] == ch) {
		return i;
	    }
	}
    } else {
	for (i = 0; str[i]; i++) {
	    if (str[i] == ch) {
		return i;
	    }
	}
    }
    return -1;
}
