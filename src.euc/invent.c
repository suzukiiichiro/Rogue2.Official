/*
 * invent.c
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
#include "invent.h"
#include "display.h"
#include "message.h"
#include "object.h"
#include "pack.h"
#include "random.h"

#define swap_string(x,y) {t = x; x = y; y = t;}

boolean is_wood[WANDS];

char *wand_materials[WAND_MATERIALS] = {
    mesg[410], mesg[411], mesg[412], mesg[413], mesg[414], mesg[415],
    mesg[416], mesg[417], mesg[418], mesg[419], mesg[420], mesg[421],
    mesg[422], mesg[423], mesg[424], mesg[425], mesg[426], mesg[427],
    mesg[428], mesg[429], mesg[430], mesg[431], mesg[432], mesg[433],
    mesg[434], mesg[435], mesg[436], mesg[437], mesg[438], mesg[439]
};

char *gems[GEMS] = {
    mesg[440], mesg[441], mesg[442], mesg[443], mesg[444], mesg[445],
    mesg[446], mesg[447], mesg[448], mesg[449], mesg[450], mesg[451],
    mesg[452], mesg[453]
};

char *syllables[MAXSYLLABLES] = {
    mesg[454], mesg[455], mesg[456], mesg[457], mesg[458], mesg[459],
    mesg[460], mesg[461], mesg[462], mesg[463], mesg[464], mesg[465],
    mesg[466], mesg[467], mesg[468], mesg[469], mesg[470], mesg[471],
    mesg[472], mesg[473], mesg[474], mesg[475], mesg[476], mesg[477],
    mesg[478], mesg[479], mesg[480], mesg[481], mesg[482], mesg[483],
    mesg[484], mesg[485], mesg[486], mesg[487], mesg[488], mesg[489],
    mesg[490], mesg[491], mesg[492], mesg[493]
};

extern boolean wizard;

char descs[ROGUE_LINES][ROGUE_COLUMNS];	/* multi-purpose screen saver */

void
inventory(object *pack, unsigned short mask)
{
    object *obj;
    short i, j, maxlen, n;
    short row, col;
    char *p;
#if defined( JAPAN )
    char *msg = "  ＝スペースを押してください＝";
    short len = 30;
#else /* not JAPAN */
    char *msg = " --Press space to continue--";
    short len = 28;
#endif /* not JAPAN */

    if (!(obj = pack->next_object)) {
	message(mesg[26], 0);
	return;
    }
#define	Protected(obj)	((obj->what_is & ARMOR) && obj->is_protected)
nextpage:
    i = 0;
    maxlen = len;
    while (obj && i < ROGUE_LINES - 2) {
	if (obj->what_is & mask) {
	    p = descs[i];
	    *p++ = ' ';
	    *p++ = obj->ichar;
	    *p++ = Protected(obj) ? '}' : ')';
	    *p++ = ' ';
	    get_desc(obj, p, 0);
	    if ((n = strlen(descs[i])) > maxlen) {
		maxlen = n;
	    }
	    i++;
	}
	obj = obj->next_object;
    }
    (void) strcpy(descs[i++], msg);

    if (i == 0) {
	return;
    }

    col = ROGUE_COLUMNS - (maxlen + 2);
    for (row = 0; row < i; row++) {
	if (row > 0) {
	    for (j = col; j < ROGUE_COLUMNS; j++) {
		descs[row - 1][j - col] = mvinch_rogue(row, j);
	    }
	    descs[row - 1][j - col] = 0;
	}
	mvaddstr_rogue(row, col, descs[row]);
	clrtoeol();
    }
    refresh();
    wait_for_ack();

    move(0, 0);
    clrtoeol();
#if defined( COLOR )
    for (j = 1; j < i; j++) {
	move(j, col);
	for (p = descs[j - 1]; *p; p++) {
	    addch_rogue(*p);
	}
    }
#else /* not COLOR */
#if !defined( JAPAN )			/* if.. by Yasha */
    for (j = 1; j < i; j++) {
	mvaddstr_rogue(j, col, descs[j - 1]);
    }
#else /* JAPAN */
    for (j = 1; j < i; j++) {	/* by Yasha */
	move(j, col);		/* by Yasha */
	clrtoeol();		/* by Yasha */
	addstr_rogue(descs[j - 1]);	/* by Yasha */
    }				/* by Yasha */
    move(ROGUE_LINES - 1, 0);		/* by Yasha */
    clrtoeol();			/* by Yasha */
    print_stats(STAT_ALL);	/* by Yasha */
#endif /* JAPAN */
#endif /* not COLOR */

    if (obj) {
	goto nextpage;
    }
}

void
mix_colors(void)
{
    short i, j;
    char *t;
    extern char *po_color[];

    for (i = 0; i < POTIONS; i++) {
	strcpy(id_potions[i].title, po_color[i]);	/* by Yahsa */
/*		id_potions[i].title = po_color[i];*/
    }
    for (i = 0; i < POTIONS; i++) {
	j = get_rand(i, POTIONS - 1);
	swap_string(id_potions[i].title, id_potions[j].title);
    }
}

void
make_scroll_titles(void)
{
    short i, j, n, len;
    short sylls, s;

    for (i = 0; i < SCROLS; i++) {
	sylls = get_rand(2, 5);
#if defined( JAPAN )
	(void) strcpy(id_scrolls[i].title, "「");
	len = 2;
	for (j = 0; j < sylls; j++) {
	    s = get_rand(1, (MAXSYLLABLES - 1));
	    n = strlen(syllables[s]);
	    if (len + n - 1 >= MAX_TITLE_LENGTH - 2) {
		break;
	    }
	    (void) strcat(id_scrolls[i].title, syllables[s]);
	    len += n;
	}
	(void) strcpy(id_scrolls[i].title + (len - 1), "」");
#else /* not JAPAN */
	(void) strcpy(id_scrolls[i].title, "'");
	for (j = 0; j < sylls; j++) {
	    s = get_rand(1, (MAXSYLLABLES - 1));
	    (void) strcat(id_scrolls[i].title, syllables[s]);
	}
	n = strlen(id_scrolls[i].title);
	(void) strcpy(id_scrolls[i].title + (n - 1), "' ");
#endif /* not JAPAN */
    }
}

void
get_desc(object *obj, char *desc, boolean capitalized)
{

#if defined( JAPAN )			/* for whole function */
    char *item_name, *p;
    struct id *id_table;
    char more_info[32];
    short i;

    *desc = 0;
    if (obj->what_is == AMULET) {
	(void) strcpy(desc, mesg[27]);
	return;
    }
    item_name = name_of(obj);
    if (obj->what_is == GOLD) {
	znum(desc, obj->quantity, 0);
	strcat(desc, mesg[28]);
	return;
    }
    if (obj->what_is == WEAPON && obj->quantity > 1) {
	znum(desc, obj->quantity, 0);
	strcat(desc, mesg[29]);
    } else if (obj->what_is == FOOD) {
	znum(desc, obj->quantity, 0);
	strcat(desc, (obj->which_kind == RATION) ? mesg[30] : mesg[31]);
	(void) strcat(desc, item_name);
	goto ANA;
    } else if (obj->what_is != ARMOR && obj->quantity > 1) {
	znum(desc, obj->quantity, 0);
	strcat(desc, mesg[32]);
    }
    id_table = get_id_table(obj);

    if (wizard) {
	goto ID;
    }
    if (obj->what_is & (WEAPON | ARMOR | WAND | RING)) {
	goto CHECK;
    }

    switch (id_table[obj->which_kind].id_status) {
    case UNIDENTIFIED:
    CHECK:
	switch (obj->what_is) {
	case SCROL:
	    (void) strcat(desc, id_table[obj->which_kind].title);
	    (void) strcat(desc, mesg[33]);
	    (void) strcat(desc, item_name);
	    break;
	case POTION:
	    (void) strcat(desc, id_table[obj->which_kind].title);
	    (void) strcat(desc, item_name);
	    break;
	case WAND:
	case RING:
	    if (obj->identified ||
		(id_table[obj->which_kind].id_status == IDENTIFIED)) {
		goto ID;
	    }
	    if (id_table[obj->which_kind].id_status == CALLED) {
		goto CALL;
	    }
	    (void) strcat(desc, id_table[obj->which_kind].title);
	    (void) strcat(desc, item_name);
	    break;
	case ARMOR:
	    if (obj->identified) {
		goto ID;
	    }
	    (void) strcpy(desc, id_table[obj->which_kind].title);
	    break;
	case WEAPON:
	    if (obj->identified) {
		goto ID;
	    }
	    (void) strcat(desc, name_of(obj));
	    break;
	}
	break;
    case CALLED:
    CALL:
	switch (obj->what_is) {
	case SCROL:
	case POTION:
	case WAND:
	case RING:
	    p = id_table[obj->which_kind].title;
#if defined( EUC )
	    if (*desc && *p >= ' ' && !(0x80 & *p)) {
		(void) strcat(desc, " ");
	    }
#else /* not EUC(Shift JIS) */
	    if (*desc && (*p >= ' ' && *p <= '~' || *p >= '\240' && *p < '\340')) {
		(void) strcat(desc, " ");
	    }
#endif /* not EUC */
	    (void) strcat(desc, p);
	    (void) strcat(desc, mesg[34]);
	    (void) strcat(desc, item_name);
	    break;
	}
	break;
    case IDENTIFIED:
    ID:
	switch (obj->what_is) {
	case SCROL:
	case POTION:
	    (void) strcat(desc, id_table[obj->which_kind].real);
	    (void) strcat(desc, item_name);
	    break;
	case RING:
	    (void) strcat(desc, id_table[obj->which_kind].real);
	    if (wizard || obj->identified) {
		if ((obj->which_kind == DEXTERITY) ||
		    (obj->which_kind == ADD_STRENGTH)) {
		    strcpy(more_info, "（");
		    znum(more_info, obj->class, 1);
		    strcat(more_info, "）");
		    (void) strcat(desc, more_info);
		}
	    }
	    (void) strcat(desc, item_name);
	    break;
	case WAND:
	    (void) strcat(desc, id_table[obj->which_kind].real);
	    (void) strcat(desc, item_name);
	    if (wizard || obj->identified) {
		strcpy(more_info, "［");
		znum(more_info, obj->class, 0);
		strcat(more_info, "］");
		(void) strcat(desc, more_info);
	    }
	    break;
	case ARMOR:
	    strcpy(desc, "（");
	    znum(desc, obj->d_enchant, 1);
	    strcat(desc, "）");
	    (void) strcat(desc, id_table[obj->which_kind].title);
	    strcpy(more_info, "［");
	    znum(more_info, get_armor_class(obj), 0);
	    strcat(more_info, "］");
	    (void) strcat(desc, more_info);
	    break;
	case WEAPON:
	    strcat(desc, "（");
	    znum(desc, obj->hit_enchant, 1);
	    strcat(desc, "，");
	    znum(desc, obj->d_enchant, 1);
	    strcat(desc, "）");
	    (void) strcat(desc, name_of(obj));
	    break;
	}
	break;
    }
ANA:
    i = obj->in_use_flags;
    if (i & BEING_WIELDED) {
	p = mesg[35];
    } else if (i & BEING_WORN) {
	p = mesg[36];
    } else if (i & ON_LEFT_HAND) {
	p = mesg[37];
    } else if (i & ON_RIGHT_HAND) {
	p = mesg[38];
    } else {
	p = "";
    }
    (void) strcat(desc, p);

#else /* not JAPAN for whole function get_desc */
    char *p;
    char *item_name;
    struct id *id_table;
    char more_info[32];
    short i;

    if (obj->what_is == AMULET) {
	(void) strcpy(desc, mesg[27]);
	if (!capitalized) {
	    *desc = 't';
	}
	return;
    }
    item_name = name_of(obj);

    if (obj->what_is == GOLD) {
	sprintf(desc, mesg[28], obj->quantity);
	return;
    }

    if (obj->what_is != ARMOR) {
	if (obj->quantity == 1) {
	    strcpy(desc, capitalized ? "A " : "a ");
	} else {
	    sprintf(desc, "%d ", obj->quantity);
	}
    }
    if (obj->what_is == FOOD) {
	if (obj->which_kind == RATION) {
	    if (obj->quantity > 1) {
		sprintf(desc, mesg[30], obj->quantity);
	    } else {
		strcpy(desc, capitalized ? "Some " : "some ");
	    }
	} else {
	    strcpy(desc, capitalized ? "A " : "a ");
	}
	(void) strcat(desc, item_name);
	goto ANA;
    }
    id_table = get_id_table(obj);

    if (wizard) {
	goto ID;
    }
    if (obj->what_is & (WEAPON | ARMOR | WAND | RING)) {
	goto CHECK;
    }

    switch (id_table[obj->which_kind].id_status) {
    case UNIDENTIFIED:
    CHECK:
	switch (obj->what_is) {
	case SCROL:
	    (void) strcat(desc, item_name);
	    (void) strcat(desc, mesg[33]);
	    (void) strcat(desc, id_table[obj->which_kind].title);
	    break;
	case POTION:
	    (void) strcat(desc, id_table[obj->which_kind].title);
	    (void) strcat(desc, item_name);
	    break;
	case WAND:
	case RING:
	    if (obj->identified ||
		(id_table[obj->which_kind].id_status == IDENTIFIED)) {
		goto ID;
	    }
	    if (id_table[obj->which_kind].id_status == CALLED) {
		goto CALL;
	    }
	    (void) strcat(desc, id_table[obj->which_kind].title);
	    (void) strcat(desc, item_name);
	    break;
	case ARMOR:
	    if (obj->identified) {
		goto ID;
	    }
	    (void) strcpy(desc, id_table[obj->which_kind].title);
	    break;
	case WEAPON:
	    if (obj->identified) {
		goto ID;
	    }
	    (void) strcat(desc, name_of(obj));
	    break;
	}
	break;
    case CALLED:
    CALL:
	switch (obj->what_is) {
	case SCROL:
	case POTION:
	case WAND:
	case RING:
	    (void) strcat(desc, item_name);
	    (void) strcat(desc, mesg[34]);
	    (void) strcat(desc, id_table[obj->which_kind].title);
	    break;
	}
	break;
    case IDENTIFIED:
    ID:
	switch (obj->what_is) {
	case SCROL:
	case POTION:
	    (void) strcat(desc, item_name);
	    (void) strcat(desc, id_table[obj->which_kind].real);
	    break;
	case RING:
	    if (wizard || obj->identified) {
		if ((obj->which_kind == DEXTERITY) ||
		    (obj->which_kind == ADD_STRENGTH)) {
		    sprintf(more_info, "%s%d ",
			    ((obj->class > 0) ? "+" : ""), obj->class);
		    (void) strcat(desc, more_info);
		}
	    }
	    (void) strcat(desc, item_name);
	    (void) strcat(desc, id_table[obj->which_kind].real);
	    break;
	case WAND:
	    (void) strcat(desc, item_name);
	    (void) strcat(desc, id_table[obj->which_kind].real);
	    if (wizard || obj->identified) {
		sprintf(more_info, "[%d]", obj->class);
		(void) strcat(desc, more_info);
	    }
	    break;
	case ARMOR:
	    sprintf(desc, "%s%d ",
		    ((obj->d_enchant >= 0) ? "+" : ""), obj->d_enchant);
	    (void) strcat(desc, id_table[obj->which_kind].title);
	    sprintf(more_info, "[%d] ", get_armor_class(obj));
	    (void) strcat(desc, more_info);
	    break;
	case WEAPON:
	    sprintf(desc + strlen(desc), "%s%d, %s%d ",
		    ((obj->hit_enchant >= 0) ? "+" : ""),
		    obj->hit_enchant,
		    ((obj->d_enchant >= 0) ? "+" : ""), obj->d_enchant);
	    (void) strcat(desc, name_of(obj));
	    break;
	}
	break;
    }
ANA:
    if (!strncmp(desc, (capitalized ? "A " : "a "), 2)) {
	if (is_vowel(desc[2])) {
	    for (i = strlen(desc) + 1; i > 1; i--) {
		desc[i] = desc[i - 1];
	    }
	    desc[1] = 'n';
	}
    }
    i = obj->in_use_flags;
    if (i & BEING_WIELDED) {
	p = mesg[35];
    } else if (i & BEING_WORN) {
	p = mesg[36];
    } else if (i & ON_LEFT_HAND) {
	p = mesg[37];
    } else if (i & ON_RIGHT_HAND) {
	p = "(on right hand)";
    } else {
	p = "";
    }
    (void) strcat(desc, p);
#endif /* not JAPAN for whole function get_desc() */
}

void
get_wand_and_ring_materials(void)
{
    short i, j;
    char *p;
    boolean used[WAND_MATERIALS];

    for (i = 0; i < WAND_MATERIALS; i++) {
	used[i] = 0;
    }
    for (i = 0; i < WANDS; i++) {
	do {
	    j = get_rand(0, WAND_MATERIALS - 1);
	} while (used[j]);
	used[j] = 1;
	p = id_wands[i].title;
	(void) strcpy(p, wand_materials[j]);
#if defined( JAPAN )
	(void) strcat(p, mesg[39]);
#endif /* JAPAN */
	is_wood[i] = (j > MAX_METAL);
    }
    for (i = 0; i < GEMS; i++) {
	used[i] = 0;
    }
    for (i = 0; i < RINGS; i++) {
	do {
	    j = get_rand(0, GEMS - 1);
	} while (used[j]);
	used[j] = 1;
	p = id_rings[i].title;
	(void) strcpy(p, gems[j]);
#if defined( JAPAN )
	(void) strcat(p, mesg[40]);
#endif /* JAPAN */
    }
}

void
single_inv(short ichar)
{
    short ch;
    char *p;
    object *obj;
    char desc[ROGUE_COLUMNS];

    ch = ichar ? ichar : pack_letter(mesg[41], ALL_OBJECTS);

    if (ch == CANCEL) {
	return;
    }
    if (!(obj = get_letter_object(ch))) {
	message(mesg[41], 0);
	return;
    }
    p = desc;
    *p++ = ch;
    *p++ = ((obj->what_is & ARMOR) && obj->is_protected) ? '}' : ')';
    *p++ = ' ';
    get_desc(obj, p, 1);
    message(desc, 0);
}

struct id *
get_id_table(object *obj)
{
    switch (obj->what_is) {
    case SCROL:
	return id_scrolls;
    case POTION:
	return id_potions;
    case WAND:
	return id_wands;
    case RING:
	return id_rings;
    case WEAPON:
	return id_weapons;
    case ARMOR:
	return id_armors;
    }
    return ((struct id *) 0);
}

void
inv_armor_weapon(boolean is_weapon)
{
    if (is_weapon) {
	if (rogue.weapon) {
	    single_inv(rogue.weapon->ichar);
	} else {
	    message(mesg[43], 0);
	}
    } else {
	if (rogue.armor) {
	    single_inv(rogue.armor->ichar);
	} else {
	    message(mesg[44], 0);
	}
    }
}

#if !defined( ORIGINAL )
struct dlist
{
    short type, no;
    char *name, *real;
#if defined( JAPAN )
    char *sub;
#endif /* JAPAN */
} dlist[SCROLS + POTIONS + WANDS + RINGS + 4];

struct dobj
{
    short type;
    char ch, max;
    char *name;
    struct id *id;
} dobj[] = {
    {
    SCROL, '?', SCROLS, mesg[3], id_scrolls}, {
    POTION, '!', POTIONS, mesg[4], id_potions}, {
    WAND, '/', WANDS, mesg[5], id_wands}, {
    RING, '=', RINGS, mesg[8], id_rings}, {
    0}
};

void
discovered(void)
{
    short i, j, n;
    short ch, maxlen, found;
    short row, col;
    struct dlist *dp, *enddp;
    struct dobj *op;
    char *p;
#if defined( JAPAN )
    char *msg = "  ＝スペースを押してください＝";
    short len = 30;
#else /* not JAPAN */
    char *msg = " --Press space to continue--";
    short len = 28;
#endif /* not JAPAN */

    message(mesg[45], 0);
    while (r_index("?!/=*\033", (ch = rgetchar()), 0) == -1) {
	sound_bell();
    }
    check_message();
    if (ch == '\033') {
	return;
    }

    found = 0;
    dp = dlist;
    for (op = dobj; op->type; op++) {
	if (ch != op->ch && ch != '*') {
	    continue;
	}
	for (i = 0; i < op->max; i++) {
	    j = op->id[i].id_status;
	    if (j == IDENTIFIED || j == CALLED) {
		dp->type = op->type;
		dp->no = i;
		dp->name = op->name;
		if (wizard || j == IDENTIFIED) {
		    dp->real = op->id[i].real;
#if defined( JAPAN )
		    dp->sub = "";
#endif /* JAPAN */
		} else {
		    dp->real = op->id[i].title;
#if defined( JAPAN )
		    dp->sub = mesg[34];
#endif /* JAPAN */
		}
#if !defined( JAPAN )
		if (op->type == WAND && is_wood[i]) {
		    dp->name = "staff ";
		}
#endif /* not JAPAN */
		found |= op->type;
		dp++;
	    }
	}
	if ((found & op->type) == 0) {
	    dp->type = op->type;
	    dp->no = -1;
	    dp->name = op->name;
	    dp++;
	}
	dp->type = 0;
	dp++;
    }
    enddp = dp;

    if (found == 0) {
	message(mesg[46], 0);
	return;
    }

    dp = dlist;

nextpage:
    i = 0;
    maxlen = len;
    while (dp < enddp && i < ROGUE_LINES - 2) {
	p = descs[i];
	if (dp->type == 0) {
	    (void) strcpy(p, "");
	} else if (dp->no < 0) {
	    (void) sprintf(p, mesg[47], dp->name);
#if !defined( JAPAN )
	    descs[i][strlen(p) - 1] = 's';
#endif /* not JAPAN */
	} else {
#if defined( JAPAN )
	    (void) strcpy(p, "  ");
	    (void) strcat(p, dp->real);
	    (void) strcat(p, dp->sub);
	    (void) strcat(p, dp->name);
#else /* not JAPAN */
	    p[0] = ' ';
	    (void) strcpy(p + 1, dp->name);
	    (void) strcat(p, dp->real);
	    p[1] -= 'a' - 'A';
#endif /* not JAPAN */
	}
	if ((n = strlen(p)) > maxlen) {
	    maxlen = n;
	}
	i++;
	dp++;
    }

    if (i == 0 || (i == 1 && !descs[0][0])) {
	/*
	 * can be here only in 2nd pass (exactly one page)
	 */
	return;
    }

    strcpy(descs[i++], msg);
    col = ROGUE_COLUMNS - (maxlen + 2);
    for (row = 0; row < i; row++) {
	if (row > 0) {
	    for (j = col; j < ROGUE_COLUMNS; j++) {
		descs[row - 1][j - col] = mvinch_rogue(row, j);
	    }
	    descs[row - 1][j - col] = 0;
	}
	mvaddstr_rogue(row, col, descs[row]);
	clrtoeol();
    }
    refresh();
    wait_for_ack();

    move(0, 0);
    clrtoeol();
#if defined( COLOR )
    for (j = 1; j < i; j++) {
	move(j, col);
	for (p = descs[j - 1]; *p; p++) {
	    addch_rogue(*p);
	}
    }
#else /* not COLOR */
#if !defined( JAPAN )			/* if.. by Yasha */
    for (j = 1; j < i; j++) {
	mvaddstr_rogue(j, col, descs[j - 1]);
    }
#else /* JAPAN */
    for (j = 1; j < i; j++) {	/* by Yasha */
	move(j, col);		/* by Yasha */
	clrtoeol();		/* by Yasha */
	addstr_rogue(descs[j - 1]);	/* by Yasha */
    }				/* by Yasha */
    move(ROGUE_LINES - 1, 0);		/* by Yasha */
    clrtoeol();			/* by Yasha */
    print_stats(STAT_ALL);	/* by Yasha */
#endif /* JAPAN */
#endif /* not COLOR */

    if (dp < enddp) {
	goto nextpage;
    }
}
#endif /* not ORIGINAL */
#if defined( JAPAN )
static char *_num[10] =
    { "０", "１", "２", "３", "４", "５", "６", "７", "８", "９" };

void
znum(char *buf, int n, int plus)
{
    char s[10], *p;

    while (*buf) {
	buf++;
    }
    if (plus && n >= 0) {
	strcpy(buf, "＋");
	buf += 2;
    }
    sprintf(s, "%d", n);
    for (p = s; *p; p++) {
	strcpy(buf, (*p == '-') ? "−" : _num[*p - '0']);
	buf += 2;
    }
}

void
lznum(char *buf, long n, int plus)
{
    char s[13], *p;

    while (*buf) {
	buf++;
    }
    if (plus && n >= 0L) {
	strcpy(buf, "＋");
	buf += 2;
    }
    sprintf(s, "%ld", n);
    for (p = s; *p; p++) {
	strcpy(buf, (*p == '-') ? "−" : _num[*p - '0']);
	buf += 2;
    }
}
#endif /* JAPAN */
