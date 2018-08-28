#if !defined( __ZAP_H__ )
#define __ZAP_H__

extern void zapp(void);
extern object *get_zapped_monster(short dir, short *row, short *col);
extern object *get_missiled_monster(short dir, short *row, short *col);
extern void zap_monster(object *monster, unsigned short kind);
extern void tele_away(object *monster);
extern void wizardize(void);

#endif /* not __ZAP_H__ */
