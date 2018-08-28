#if !defined( __SPECHIT_H__ )
#define __SPECHIT_H__

extern void special_hit(object *monster);
extern void rust(object *monster);
extern void freeze(object *monster);
extern void steal_gold(object *monster);
extern void steal_item(object *monster);
extern void disappear(object *monster);
extern void cough_up(object *monster);
extern int try_to_cough(short row, short col, object *obj);
extern int seek_gold(object *monster);
extern int gold_at(short row, short col);
extern void check_gold_seeker(object *monster);
extern int check_imitator(object *monster);
extern int imitating(short row, short col);
extern void sting(object *monster);
extern void drop_level(void);
extern void drain_life(void);
extern int m_confuse(object *monster);
extern int flame_broil(object *monster);
extern void get_closer(short *row, short *col, short trow, short tcol);

#endif /* not __SPECHIT_H__ */
