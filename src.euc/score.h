#if !defined( __SCORE_H__ )
#define __SCORE_H__

extern void killed_by(object *monster, short other);
extern void win(void);
extern void mvaddbanner(int row, int col, int *ban);
extern void quit(boolean from_intrpt);
extern void put_scores(object *monster, short other);
extern void insert_score(char scores[][82], char n_names[][30], char *n_name,
			 short rank, short n, object *monster, int other);
extern int is_vowel(short ch);
extern void sell_pack(void);
extern int get_value(object *obj);
extern void id_all(void);
extern void xxxx(char *buf, short n);
extern long xxx(boolean st);
extern void nickize(char *buf, char *score, char *n_name);
extern void center(short row, char *buf);
extern void sf_error(void);

#endif /* not __SCORE_H__ */
