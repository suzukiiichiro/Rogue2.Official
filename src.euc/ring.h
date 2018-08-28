#if !defined( __RING_H__ )
#define __RING_H__

extern void put_on_ring(void);
extern void do_put_on(object *ring, boolean on_left);
extern void remove_ring(void);
extern void un_put_on(object *ring);
extern void gr_ring(object *ring, boolean assign_wk);
extern void inv_rings(void);
extern void ring_stats(boolean pr);

#endif /* not __RING_H__ */
