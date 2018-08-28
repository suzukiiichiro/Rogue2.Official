/*
 * play.c
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
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>

#include "rogue.h"
#include "play.h"
#include "display.h"
#include "hit.h"
#include "init.h"
#include "invent.h"
#include "level.h"
#include "machdep.h"
#include "message.h"
#include "monster.h"
#include "move.h"
#include "object.h"
#include "pack.h"
#include "ring.h"
#include "room.h"
#include "save.h"
#include "score.h"
#include "throw.h"
#include "trap.h"
#include "use.h"
#include "zap.h"

#define	CTRL(c)	((c) & 037)

boolean interrupted = 0;
char *unknown_command = mesg[115];

extern short party_room, bear_trap;
extern char hit_message[];
extern boolean wizard, trap_door;

void
play_level(void)
{
    short ch, cmd, oldcmd;
    int count;

    cmd = '.';
    for (;;) {
	interrupted = 0;
	if (hit_message[0]) {
	    message(hit_message, 1);
	    hit_message[0] = 0;
	}
	if (trap_door) {
	    trap_door = 0;
	    return;
	}
	move(rogue.row, rogue.col);
	refresh();

	oldcmd = cmd;
	ch = cmd = rgetchar();
	check_message();
	count = 0;
    CH:
	switch (ch) {
	case '.':
	    rest((count > 0) ? count : 1);
	    break;
	case 's':
	    search(((count > 0) ? count : 1), 0);
	    break;
	case 'i':
	    inventory(&rogue.pack, ALL_OBJECTS);
	    break;
	case 'f':
	    fight(0);
	    break;
	case 'F':
	    fight(1);
	    break;
	case 'h':
	case 'j':
	case 'k':
	case 'l':
	case 'y':
	case 'u':
	case 'n':
	case 'b':
	    (void) one_move_rogue(ch, 1);
	    break;
	case 'H':
	case 'J':
	case 'K':
	case 'L':
	case 'B':
	case 'Y':
	case 'U':
	case 'N':
	case CTRL('H'):
	case CTRL('J'):
	case CTRL('K'):
	case CTRL('L'):
	case CTRL('Y'):
	case CTRL('U'):
	case CTRL('N'):
	case CTRL('B'):
	    multiple_move_rogue(ch);
	    break;
	case 'e':
	    eat();
	    break;
	case 'q':
	    quaff();
	    break;
	case 'r':
	    read_scroll();
	    break;
	case 'm':
	    move_onto();
	    break;
	case 'd':
	    drop();
	    break;
	case 'P':
	    put_on_ring();
	    break;
	case 'R':
	    remove_ring();
	    break;
	case CTRL('P'):
	    remessage();
	    break;
	case CTRL('W'):
	    wizardize();
	    break;
	case '>':
	    if (drop_check()) {
		return;
	    }
	    break;
	case '<':
	    if (check_up()) {
		return;
	    }
	    break;
	case ')':
	case ']':
	    inv_armor_weapon(ch == ')');
	    break;
	case '=':
	    inv_rings();
	    break;
	case '^':
	    id_trap();
	    break;
	case 'I':
	    single_inv(0);
	    break;
	case 'T':
	    take_off();
	    break;
	case 'W':
	    wear();
	    break;
	case 'w':
	    wield();
	    break;
	case 'c':
	    call_it();
	    break;
	case 'z':
	    zapp();
	    break;
	case 't':
	    throw();
	    break;
	case 'v':
	    message(PACKAGE_STRING " (by FUNABARA Masao) ", 0);
	    message("base by ...", 0);
	    message("Rogue-clone: Version II. (Tim Stoehr was here), tektronix!zeus!tims ", 0);
	    message("Japanese edition: Ver.1.3a (enhanced by ohta@src.ricoh.co.jp)", 0);
	    message("Ver.1.3aS program bug fix/separate (by brx@kmc.kyoto-u.ac.jp)", 0);
	    message(mesg[1], 0);     /* for message version */
	    break;
	case 'Q':
	    byebye(0);
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    move(rogue.row, rogue.col);
	    refresh();
	    do {
		if (count < 100) {
		    count = (10 * count) + (ch - '0');
		}
		ch = rgetchar();
	    } while (is_digit(ch));
	    if (ch != CANCEL) {
		goto CH;
	    }
	    break;
	case ' ':
	    break;
	case CTRL('I'):
	    if (wizard) {
		inventory(&level_objects, ALL_OBJECTS);
	    } else {
		message(unknown_command, 0);
	    }
	    break;
	case CTRL('S'):
	    if (wizard) {
		draw_magic_map();
	    } else {
		message(unknown_command, 0);
	    }
	    break;
	case CTRL('T'):
	    if (wizard) {
		show_traps();
	    } else {
		message(unknown_command, 0);
	    }
	    break;
	case CTRL('O'):
	    if (wizard) {
		show_objects();
	    } else {
		message(unknown_command, 0);
	    }
	    break;
	case CTRL('A'):
	    show_average_hp();
	    break;
#if !defined( ORIGINAL )
	case CTRL('G'):
#endif /* not ORIGINAL */
	case CTRL('C'):
	    if (wizard) {
		new_object_for_wizard();
	    } else {
		message(unknown_command, 0);
	    }
	    break;
	case CTRL('M'):
	    if (wizard) {
		show_monsters();
	    } else {
		message(unknown_command, 0);
	    }
	    break;
	case 'S':
	    save_game();
	    break;
	case ',':
	    kick_into_pack();
	    break;
#if !defined( ORIGINAL )
	case CTRL('X'):
	    if (!wizard) {
		message(unknown_command, 0);
		break;
	    }
	    draw_magic_map();
	    show_monsters();
	    show_objects();
	    show_traps();
	    break;
	case '?':
	    help();
	    break;
	case '@':
	    print_stats(STAT_ALL);
	    break;
	case 'D':
	    discovered();
	    break;
	case '/':
	    identify();
	    break;
	case 'o':
	    options();
	    break;
	case '!':
	    doshell();
	    break;
	case 'a':
	    ch = cmd = oldcmd;
	    goto CH;
#endif /* not ORIGINAL */
	default:
	    message(unknown_command, 0);
	    break;
	}
    }
}

#if !defined( ORIGINAL )
char *help_message[] = {
    mesg[116], mesg[117], mesg[118], mesg[119], mesg[120], mesg[121],
    mesg[122], mesg[123], mesg[124], mesg[125], mesg[126], mesg[127],
    mesg[128], mesg[129], mesg[130], mesg[131], mesg[132], mesg[133],
    mesg[134], mesg[135], mesg[136], mesg[137],
    "",
#if defined( JAPAN )
    "＝スペースを押してください＝",
#else /* not JAPAN */
    "--Press space to continue--",
#endif /* not JAPAN */
    (char *) 0
};

/*
 * help
 * ヘルプメッセージを表示する
 */
void
help(void)
{
    int lines, columns;
    int n;
    char disp_message[ROGUE_COLUMNS+1];

    /* 現在の画面を保存する */
    for (lines = 0; lines < ROGUE_LINES; lines++) {
	for (columns = 0; columns < ROGUE_COLUMNS; columns++) {
	    descs[lines][columns] = mvinch_rogue(lines, columns);
	}
    }

    /* ヘルプメッセージを表示する */
    clear();
    for (n = 0; help_message[n]; n++) {
	mvaddstr_rogue(n, 0, help_message[n]);
	clrtoeol();
    }
    refresh();
    wait_for_ack();

    /* 保持した画面を復帰させる */
    for (lines = 0; lines < ROGUE_LINES; lines++) {
	move(lines, 0);
	if (lines > 0 && lines < ROGUE_LINES - 1) {
	    for (columns = 0; columns < ROGUE_COLUMNS; columns++) {
		addch_rogue(descs[lines][columns]);
	    }
	} else {
	    /* メッセージデータの最後に末端記号を付加する */
	    strncpy(disp_message, descs[lines], ROGUE_COLUMNS);
	    disp_message[ROGUE_COLUMNS] = '\0';

	    addstr_rogue(disp_message);
	}
    }
    refresh();
}

extern char *m_names[];
static char *o_names[] = {
    mesg[138], mesg[139], mesg[140], mesg[141], mesg[142], mesg[143],
    mesg[144], mesg[145], mesg[146], mesg[147], mesg[148], mesg[149],
    mesg[150], mesg[151], mesg[152], mesg[153], mesg[154]
};

void
identify(void)
{
    short ch, n;
    char *p, buf[80];

    message(mesg[155], 0);

again:
    ch = rgetchar();
    if (ch == '\033') {
	check_message();
	return;
    } else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
	check_message();
	if (ch >= 'a' && ch <= 'z')
	    ch += 'A' - 'a';
	p = m_names[ch - 'A'];
    } else if ((n = r_index("@.|-+#%^*:])?!/=,", ch, 0)) != -1) {
	check_message();
	p = o_names[n];
    } else {
	sound_bell();
	goto again;
    }
    sprintf(buf, "'%c': %s", ch, p);
    message(buf, 0);
}

/*
 * options
 * オプション画面の表示と設定を行う
 */
void
options(void)
{
    int lines, columns;
    int n, i, j, changed;
    short ch;
    short pos[ROGUE_LINES];
    boolean bbuf[ROGUE_LINES];
    char cbuf[ROGUE_LINES][MAX_TITLE_LENGTH];
    char optbuf[BUFSIZ];

    /* 現在の画面を保存する */
    for (lines = 0; lines < ROGUE_LINES; lines++) {
	for (columns = 0; columns < ROGUE_COLUMNS; columns++) {
	    descs[lines][columns] = mvinch_rogue(lines, columns);
	}
    }

    /* オプション画面を表示する */
    clear();
    for (n = 0; envopt[n].name; n++) {
	mvaddstr_rogue(n, 2, optdesc[n]);
	addstr_rogue(" (\"");
	addstr_rogue(envopt[n].name);
	addstr_rogue("\"): ");
	if (envopt[n].bp) {
	    bbuf[n] = *(envopt[n].bp);
	    addstr_rogue(bbuf[n] ? "Yes" : "No");
	} else {
	    strcpy(cbuf[n], *(envopt[n].cp));
	    if (envopt[n].ab) {
		i = strlen(cbuf[n]);
		cbuf[n][i - 1] = 0;
	    }
	    addstr_rogue(cbuf[n]);
	}
	pos[n] = strlen(optdesc[n]) + strlen(envopt[n].name) + 7;
    }

    /* オプションの設定 */
    i = 0;
    while (i >= 0 && i < n) {
	mvaddch(i, 1, '>');
	if (envopt[i].bp) {
	    move(i, pos[i] + 2);
	} else {
	    move(i, pos[i] + strlen(cbuf[i]) + 2);
	}
	refresh();
	ch = rgetchar();
	if (ch == CANCEL)
	    break;
	if (r_index("-\r\n", ch, 0) > -1) {
	    getyx(stdscr, lines, columns);
	    mvaddch(i, 1, ' ');
	    move(lines, columns);
	    if (envopt[i].bp) {
		addstr_rogue(bbuf[i] ? "Yes" : "No");
		clrtoeol();
	    }
	    if (ch == '-') {
		i--;
	    } else {
		i++;
	    }
	    continue;
	}
	if (envopt[i].bp) {
	    if (ch >= 'A' && ch <= 'Z') {
		ch += 'a' - 'Z';
	    }
	    if (ch != 'y' && ch != 'n') {
		mvaddstr_rogue(i, pos[i] + 2, "(Yes or No)");
		continue;
	    }
	    mvaddch(i, 1, ' ');
	    move(i, pos[i] + 2);
	    addstr_rogue((ch == 'y') ? "Yes" : "No");
	    clrtoeol();
	    bbuf[i] = (ch == 'y');
	    i++;
	} else {
	    j = input_line(i, pos[i] + 2, cbuf[i], optbuf, ch);
	    mvaddch(i, 1, ' ');
	    if (j < 0) {
		break;
	    }
	    strcpy(cbuf[i], optbuf);
	    i++;
	}
    }
    changed = (i < 0 || i >= n);
    if (changed) {
	move(n + 1, 0);
#if defined( JAPAN )
	addstr_rogue("＝スペースを押してください＝");
#else /* JAPAN */
	addstr_rogue("--Press space to continue--");
#endif /* JAPAN */
	refresh();
	wait_for_ack();
    }

    if (changed) {
	optbuf[0] = 0;
	for (i = 0; i < n; i++) {
	    strcat(optbuf, ",");
	    if (envopt[i].bp) {
		if (!bbuf[i]) {
		    strcat(optbuf, "no");
		}
		strcat(optbuf, envopt[i].name);
	    } else {
		strcat(optbuf, envopt[i].name);
		strcat(optbuf, ":");
		strcat(optbuf, cbuf[i]);
	    }
	}
	set_opts(optbuf);
    }
    print_stats(STAT_ALL);

    init_color_attr();

#if !defined( JAPAN )			/* #if.. by Yasha */
    for (lines = 0; lines < ROGUE_LINES; lines++) {
#else /* not JAPAN */
    for (lines = 0; lines < ROGUE_LINES - 1; lines++) {	/* by Yasha */
#endif /* JAPAN */
	move(lines, 0);
	for (columns = 0; columns < ROGUE_COLUMNS; columns++) {
	    if (lines == ROGUE_LINES - 1 && columns == ROGUE_COLUMNS - 1) {
		continue;
	    }
	    if (lines < MIN_ROW || lines >= ROGUE_LINES - 1) {
		addch_rogue((unsigned char) descs[lines][columns]);
	    } else {
		addch_rogue(descs[lines][columns]);
	    }
	}
    }

    refresh();
}

void
doshell(void)
{
    char *cmd;

    if ((cmd = getenv("SHELL")) == NULL) {
	cmd = "/bin/sh";
    }
    move(ROGUE_LINES - 1, 0);
    refresh();
    stop_window();
    if (*org_dir) {
	chdir(org_dir);
    }
    md_ignore_signals();
    printf(mesg[157]);
    printf("\r\n");
    system(cmd);
    md_heed_signals();
    if (game_dir && *game_dir) {
	chdir(game_dir);
    }
    start_window();
    wrefresh(curscr);
}

#endif /* not ORIGINAL */
