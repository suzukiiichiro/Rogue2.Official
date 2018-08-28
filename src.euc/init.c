/*
 * init.c
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
#include <getopt.h>

#include "rogue.h"
#include "init.h"
#include "display.h"
#include "invent.h"
#include "machdep.h"
#include "main.h"
#include "message.h"
#include "object.h"
#include "pack.h"
#include "random.h"
#include "ring.h"
#include "save.h"
#include "score.h"

char login_name[30];
char *nick_name = "";
char *rest_file = 0;
boolean cant_int = 0, did_int = 0, score_only = 0, init_curses = 0;
boolean save_is_interactive = 1;
boolean show_skull = 1;
boolean ask_quit = 1;
#if !defined( ORIGINAL )
boolean pass_go = 1, do_restore = 0;
char org_dir[64], *game_dir = "";
#endif /* Not ORIGINAL */
boolean use_color = 1;
char *error_file = "rogue.esave";

extern char *fruit;
extern char *save_file;
extern short party_room, party_counter;
extern boolean jump;

int
init(int argc, char *argv[])
{
    char *pn;
    int seed;
    WINDOW *main_window;

#if !defined( ORIGINAL )
    getcwd(org_dir, 64);
#endif /* Not ORIGINAL */
    do_args(argc, argv);
    do_opts();

    pn = md_gln();
    if ((!pn) || (strlen(pn) >= 30)) {
	clean_up(mesg[13]);
    }
    (void) strcpy(login_name, pn);
    if (!nick_name[0]) {
	nick_name = login_name;
    }

    /* init curses */
    main_window = initscr();
    if (main_window == NULL) {
	return 1;
    }
    init_color_attr();

    if ((LINES < ROGUE_LINES) || (COLS < ROGUE_COLUMNS)) {
	clean_up(mesg[14]);
    }

    start_window();
    init_curses = 1;
    md_heed_signals();

    if (score_only) {
	message("", 0);		/* by Yasha */
	put_scores((object *) 0, 0);
    }
    seed = md_gseed();
    (void) srrandom(seed);
#if !defined( ORIGINAL )
    if (do_restore && save_file && *save_file) {
	rest_file = save_file;
    }
#endif /* Not ORIGINAL */
    if (rest_file) {
	restore(rest_file);
	return (1);
    }
    mix_colors();
    get_wand_and_ring_materials();
    make_scroll_titles();

    level_objects.next_object = 0;
    level_monsters.next_monster = 0;
    player_init();
    party_counter = get_rand(1, PARTY_TIME);
    ring_stats(0);
    return 0;
}

void
player_init(void)
{
    object *obj;

    rogue.pack.next_object = 0;

    obj = alloc_object();
    get_food(obj, 1);
    (void) add_to_pack(obj, &rogue.pack, 1);

    obj = alloc_object();	/* initial armor */
    obj->what_is = ARMOR;
    obj->which_kind = RINGMAIL;
    obj->class = RINGMAIL + 2;
    obj->is_protected = 0;
    obj->d_enchant = 1;
    (void) add_to_pack(obj, &rogue.pack, 1);
    do_wear(obj);

    obj = alloc_object();	/* initial weapons */
    obj->what_is = WEAPON;
    obj->which_kind = MACE;
    obj->damage = "2d3";
    obj->hit_enchant = obj->d_enchant = 1;
    obj->identified = 1;
    (void) add_to_pack(obj, &rogue.pack, 1);
    do_wield(obj);

    obj = alloc_object();
    obj->what_is = WEAPON;
    obj->which_kind = BOW;
    obj->damage = "1d2";
    obj->hit_enchant = 1;
    obj->d_enchant = 0;
    obj->identified = 1;
    (void) add_to_pack(obj, &rogue.pack, 1);

    obj = alloc_object();
    obj->what_is = WEAPON;
    obj->which_kind = ARROW;
    obj->quantity = get_rand(25, 35);
    obj->damage = "1d2";
    obj->hit_enchant = 0;
    obj->d_enchant = 0;
    obj->identified = 1;
    (void) add_to_pack(obj, &rogue.pack, 1);
}

void
clean_up(char *estr)
{
    if (save_is_interactive) {
	if (init_curses) {
	    move(ROGUE_LINES - 1, 0);
#if !defined( ORIGINAL )
	    clrtoeol();
#endif /* Not ORIGINAL */
	    refresh();
	    stop_window();
	}
	endwin();
	printf("\r\n");
	printf(estr);
	printf("\r\n");
    }
    md_exit(0);
}

void
start_window(void)
{
    raw();
    crmode();
    noecho();
#if !defined( BAD_NONL )
    nonl();
#endif /* Not BAD_NONL */
}

void
stop_window(void)
{
    endwin();
}

void
byebye(int sig)
{
    md_ignore_signals();
    if (ask_quit) {
	quit(1);
    } else {
	clean_up(mesg[12]);	/* byebye_string */
    }
    md_heed_signals();
}

void
onintr(int sig)
{
    md_ignore_signals();
    if (cant_int) {
	did_int = 1;
    } else {
	check_message();
	message(mesg[15], 1);
    }
    md_heed_signals();
}

void
error_save(int sig)
{
    save_is_interactive = 0;
    save_into_file(error_file);
    clean_up("");
}

void
do_args(int argc, char *argv[])
{
    int ch;
    char *option_strings;
    extern int optind;

#if !defined( ORIGINAL )
    option_strings = "sr";
#else /* Not ORIGINAL */
    option_strings = "s";
#endif /* ORIGINAL */

    while ((ch = getopt(argc, argv, option_strings)) != EOF) {
	switch (ch) {
	case 's':
	    score_only = 1;
	    break;
#if !defined( ORIGINAL )
	case 'r':
	    do_restore = 1;
	    break;
#endif /* Not ORIGINAL */
	case '?':
	default:
	    usage();
	    break;
	}
    }

    argc -= optind;
    argv += optind;
    if (argc >= 3 || argc == 0) {
	usage();
	return;
    }

    if (read_mesg(argv[0])) {
	exit(1);
    }
#if !defined( ORIGINAL )
    if (argc == 2) {
	rest_file = argv[1];
    }
#endif /* Not ORIGINAL */

}

opt envopt[] = {
    {"askquit", &ask_quit, NULL, 0, 0} ,
    {"jump", &jump, NULL, 0, 0} ,
#if !defined( ORIGINAL )
    {"passgo", &pass_go, NULL, 0, 0} ,
    {"tombstone", &show_skull, NULL, 0, 0} ,
#else /* Not ORIGINAL */
    {"skull", &show_skull, NULL, 0, 0} ,
#endif /* ORIGINAL */
#if defined( COLOR )
    {"color", &use_color, NULL, 0, 0} ,
#endif /* COLOR */
#if defined( JAPAN )
    {"fruit", NULL, &fruit, 0, 0} ,
#else /* not JAPAN */
    {"fruit", NULL, &fruit, 1, 0} ,
#endif /* not JAPAN */
    {"file", NULL, &save_file, 0, 0} ,
    {"name", NULL, &nick_name, 0, 1} ,
#if !defined( ORIGINAL )
    {"directory", NULL, &game_dir, 0, 0} ,
#endif /* Not ORIGINAL */
#if defined( COLOR )
    {"map", NULL, &color_str, 0, 0} ,
#endif /* COLOR */
    {NULL, NULL, NULL, 0, 0}
};

#if defined( JAPAN )
char *optdesc[] = {
    "終了するかどうか確認をとる",
    "移動中の表示を行わない",
#if !defined( ORIGINAL )
    "通路の角で止まらずに進む",
    "ゲーム終了時に墓標を表示する",
#else /* Not ORIGINAL */
    "ゲーム終了時に骸骨を表示する",
#endif /* ORIGINAL */
#if defined( COLOR )
    "キャラクターをカラーで表示する",
#endif /* COLOR */
    mesg[16],
    "セーブファイル名",
    "ニックネーム",
#if !defined( ORIGINAL )
    "ゲームディレクトリー名",
#endif /* Not ORIGINAL */
#if defined( COLOR )
    "キャラクターの表示色マッピング",
#endif /* COLOR */
    NULL
};
#else /* not JAPAN */
char *optdesc[] = {
    "Ask whether quit or not on quit signal",
    "Show position only at end of run",
#if !defined( ORIGINAL )
    "Follow turnings in passageways",
    "Print out tombstone and score when killed",
#else /* Not ORIGINAL */
    "Print out skull and score when killed",
#endif /* ORIGINAL */
#if defined( COLOR )
    "Show characters in map with color",
#endif /* COLOR */
    mesg[16],
    "Save filename",
    "Your nickname",
#if !defined( ORIGINAL )
    "Game directory name",
#endif /* Not ORIGINAL */
#if defined( COLOR )
    "Color mapping for characters",
#endif /* COLOR */
    NULL
};
#endif /*JAPAN*/

/*
 * do_opts
 * 対象となる環境変数を取得し解析にまわす
 */
void
do_opts(void)
{
    char *ep, *p;
    char envname[10];
    char envbuf[BUFSIZ];

    strcpy(envname, "ROGUEOPT?");
    envbuf[0] = 0;
    for (p = "S123456789"; *p; p++) {
	envname[8] = *p;
	if ((ep = getenv(envname))) {
	    strcat(envbuf, ",");
	    strcat(envbuf, ep);
	}
    }
    set_opts(envbuf);
}

/*
 * set_opts
 * 対象となる環境変数の字句解析・値の格納を行う
 */
void
set_opts(char *env)
{
    short not;
    char *ep, *p;
    opt *op;
    char optname[20];

    if (*env == 0) {
	return;
    }
    ep = env;
    for (;;) {
	while (*ep == ' ' || *ep == ',') {
	    ep++;
	}

	if (*ep == 0) {
	    break;
	}

	not = 0;
	if (!strncmp("no", ep, 2) || !strncmp("NO", ep, 2)) {
	    not = 1;
	    ep += 2;
	}
	p = optname;
	while (*ep && *ep != ',' && *ep != '=' && *ep != ':') {
	    *p++ = (*ep >= 'A' && *ep <= 'Z') ? (*ep++) - 'A' + 'a' : *ep++;
	}
	*p = 0;
	for (op = envopt; op->name; op++) {
	    if (strncmp(op->name, optname, strlen(optname))) {
		continue;
	    }
	    if (op->bp) {
		*(op->bp) = !not;
	    }
	    if (op->cp && (*ep == '=' || *ep == ':')) {
		env_get_value(op->cp, ep + 1, op->ab, op->nc);
	    }
	}

	while (*ep && *ep != ',') {
	    ep++;
	}
    }

#if !defined( ORIGINAL )
    if (game_dir && *game_dir) {
	chdir(game_dir);
    }
#endif /* Not ORIGINAL */
}

/*
 * env_get_value
 * 取得した環境変数の値を変数に格納する
 */
void
env_get_value(char **s, char *e, boolean add_blank, boolean no_colon)
{
    int i = 0;
    char *t;

    t = e;

    while ((*e) && (*e != ',')) {
#if defined( EUC )
	/* EUC のマルチバイト文字は読み飛ばす */
	if (*e & 0x80) {
	    if ((*e >= '\xA1' && *e <= '\xFE')
		&& (*(e + 1) >= '\xA1' && *(e + 1) <= '\xFE')) {
		/* 漢字 */
		e += 2;
		i += 2;
	    } else if ((*e == '\x8E')
		       && ((*(e + 1) >= '\xA0') && (*(e + 1) <= '\xDF'))) {
		/* 半角カナ */
		e += 2;
		i += 2;
	    } else if ((*e == '\x8F')
		       && ((*(e + 1) >= '\xA1') && (*(e + 1) <= '\xFE'))
		       && ((*(e + 2) >= '\xA1') && (*(e + 2) <= '\xFE'))) {
		/* 補助漢字 */
		e += 3;
		i += 3;
	    } else {
		/* その他の領域 */
		e += 1;
		i += 1;
	    }
	    continue;
	}
#else /* not EUC */
	/* Shift JIS のマルチバイト文字は読み飛ばす */
	if (*e > '\200' && *e < '\240' || *e >= '\340' && *e < '\360') {
	    e += 2;
	    i += 2;
	    continue;
	}
#endif /* not EUC */
	if (*e == ':' && no_colon) {
	    *e = ';';		/* ':' reserved for score file purposes */
	}
	e++;
	if (++i >= 30) {
	    break;
	}
    }

    /* 値入力 */
    if (!(*s = md_malloc(i + (add_blank ? 2 : 1)))) {
	clean_up(mesg[17]);
    }
    (void) strncpy(*s, t, i);
    if (add_blank) {
	(*s)[i++] = ' ';
    }
    (*s)[i] = '\0';
}
