#if !defined( __MESSAGE_H__ )
#define __MESSAGE_H__

extern void message(char *msg, boolean intrpt);
extern void remessage(void);
extern void check_message(void);
extern int get_direction(void);
extern int get_input_line(char *prompt, char *insert, char *buf,
			  char *if_cancelled, boolean add_blank,
			  boolean do_echo);
extern int input_line(int row, int col, char *insert, char *buf, int ch);
extern int do_input_line(boolean is_msg, int row, int col, char *prompt,
			 char *insert, char *buf, char *if_cancelled,
			 boolean add_blank, boolean do_echo, int first_ch);
extern int rgetchar(void);
extern void print_stats(int stat_mask);
extern void pad(char *s, short n);
extern void save_screen(void);
extern void sound_bell(void);
extern boolean is_digit(short ch);
extern int r_index(char *str, int ch, boolean last);

#endif /* not __MESSAGE_H__ */
