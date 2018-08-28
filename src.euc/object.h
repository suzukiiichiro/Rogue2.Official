#if !defined( __OBJECT_H__ )
#define __OBJECT_H__

extern void put_objects(void);
extern void put_gold(void);
extern void plant_gold(short row, short col, boolean is_maze);
extern void place_at(object *obj, int row, int col);
extern object *object_at(object *pack, short row, short col);
extern object *get_letter_object(int ch);
extern void free_stuff(object *objlist);
extern char *name_of(object *obj);
extern object *gr_object(void);
extern unsigned short gr_what_is(void);
extern void gr_scroll(object *obj);
extern void gr_potion(object *obj);
extern void gr_weapon(object *obj, int assign_wk);
extern void gr_armor(object *obj, int assign_wk);
extern void gr_wand(object *obj);
extern void get_food(object *obj, boolean force_ration);
extern void put_stairs(void);
extern int get_armor_class(object *obj);
extern object *alloc_object(void);
extern void free_object(object *obj);
extern void make_party(void);
extern void show_objects(void);
extern void put_amulet(void);
extern void rand_place(object *obj);
extern void new_object_for_wizard(void);
extern void list_object(object *obj, short max);
extern int next_party(void);

#endif /* not __OBJECT_H__ */
