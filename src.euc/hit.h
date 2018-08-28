#if !defined( __HIT_H__ )
#define __HIT_H__

extern void mon_hit(object *monster, char *other, boolean flame);
extern void rogue_hit(object *monster, boolean force_hit);
extern void rogue_damage(short d, object *monster);
extern int get_damage(char *ds, boolean r);
extern int get_w_damage(object *obj);
extern int get_number(char *s);
extern long lget_number(char *s);
extern int to_hit(object *obj);
extern int damage_for_strength(void);
extern int mon_damage(object *monster, int damage);
extern void fight(boolean to_the_death);
extern void get_dir_rc(short dir, short *row, short *col,
		       short allow_off_screen);
extern int get_hit_chance(object *weapon);
extern int get_weapon_damage(object *weapon);

#endif /* not __HIT_H__ */
