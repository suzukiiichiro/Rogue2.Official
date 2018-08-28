#if !defined( __MACHDEP_H__ )
#define __MACHDEP_H__

/* POSIX システムのシグナル名 SIGQUIT, SIGHUP について
 * そのマクロ名が処理系に存在しない場合は C99 で認めているシグナル名
 * に強制変換するという後ろ向きな解決方法
 */
#if !defined( SIGQUIT )
# define SIGQUIT SIGTERM
#endif

#if !defined( SIGHUP )
# define SIGHUP SIGABRT
#endif

#if !defined( HAVE_ASSUME_DEFAULT_COLORS )
# define assume_default_colors(to, from) init_pair(0, (to), (from))
#endif

/* 使用の関数を使わないことで回避 */
//あとで考えなおす
#if !defined( HAVE_ATTR_GET )
# define attr_get
#endif

#if !defined( HAVE_GETLOGIN )
extern char *getlogin(void);
#endif

#if !defined( HAVE_SLEEP ) && defined( HAVE_WINDOWS_H )
# define sleep(nsecs) _sleep((nsecs)*1000)
#endif

extern void putstr(char *s);
extern void md_heed_signals(void);
extern void md_ignore_signals(void);
extern int md_get_file_id(char *fname);
extern int md_link_count(char *fname);
extern void md_gct(struct rogue_time *rt_buf);
extern void md_gfmt(char *fname, struct rogue_time *rt_buf);
extern boolean md_df(char *fname);
extern char *md_gln(void);
extern char *md_malloc(int n);
extern int md_gseed(void);
extern void md_exit(int status);
extern char *md_ghome(void);

#endif /* not __MACHDEP_H__ */
