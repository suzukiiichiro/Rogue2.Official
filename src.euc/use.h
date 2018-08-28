#if !defined( __USE_H__ )
#define __USE_H__

extern void quaff(void);
extern void read_scroll(void);
extern void vanish(object *obj, short rm, object *pack);
extern void potion_heal(int extra);
extern void idntfy(void);
extern void eat(void);
extern void hold_monster(void);
extern void tele(void);
extern void hallucinate(void);
extern void unhallucinate(void);
extern void unblind(void);
extern void relight(void);
extern void take_a_nap(void);
extern void go_blind(void);
extern char *get_ench_color(void);
extern void confuse(void);
extern void unconfuse(void);
extern void uncurse_all(void);

#endif /* not __USE_H__ */
