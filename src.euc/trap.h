#if !defined( __TRAP_H__ )
#define __TRAP_H__

extern int trap_at(int row, int col);
extern void trap_player(short row, short col);
extern void add_traps(void);
extern void id_trap(void);
extern void show_traps(void);
extern void search(short n, boolean is_auto);

#endif /* not __TRAP_H__ */
