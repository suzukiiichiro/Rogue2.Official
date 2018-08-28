#if !defined( __INIT_H__ )
#define __INIT_H__

extern int init(int argc, char *argv[]);
extern void player_init(void);
extern void clean_up(char *estr);
extern void start_window(void);
extern void stop_window(void);
extern void byebye(int sig);
extern void onintr(int sig);
extern void error_save(int sig);
extern void do_args(int argc, char *argv[]);
extern void do_opts(void);
extern void set_opts(char *env);
extern void env_get_value(char **s, char *e, boolean add_blank,
			  boolean no_colon);

#endif /* not __INIT_H__ */
