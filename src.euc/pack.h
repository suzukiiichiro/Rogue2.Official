#if !defined( __PACK_H__ )
#define __PACK_H__

extern object *add_to_pack(object *obj, object *pack, int condense);
extern void take_from_pack(object *obj, object *pack);
extern object *pick_up(int row, int col, short *status);
extern void drop(void);
extern object *check_duplicate(object *obj, object *pack);
extern int next_avail_ichar(void);
extern void wait_for_ack(void);
extern int pack_letter(char *prompt, unsigned short mask);
extern void take_off(void);
extern void wear(void);
extern void unwear(object *obj);
extern void do_wear(object *obj);
extern void wield(void);
extern void do_wield(object *obj);
extern void unwield(object *obj);
extern void call_it(void);
extern int pack_count(object *new_obj);
extern boolean mask_pack(object *pack, unsigned short mask);
extern int is_pack_letter(short *c, unsigned short *mask);
extern int has_amulet(void);
extern void kick_into_pack(void);

#endif /* not __PACK_H__ */
