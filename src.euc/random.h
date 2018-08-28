#if !defined( __RANDOM_H__ )
#define __RANDOM_H__

extern void srrandom(int x);
extern long rrandom(void);
extern int get_rand(int x, int y);
extern int rand_percent(int percentage);
extern int coin_toss(void);

#endif /* not __RANDOM_H__ */
