/*
 * machdep.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#if defined( HAVE_WINDOWS_H )
# include <windows.h>
# include <stdlib.h>
#endif /* HAVE_WINDOWS_H */

#include "rogue.h"
#include "machdep.h"
#include "init.h"

/*
 * getlogin 関数が存在しない時のでっちあげ関数
 */
#if !defined( HAVE_GETLOGIN )
char *
getlogin(void)
{
    return NULL;
}
#endif /* HAVE_GETLOGIN */

/* md_heed_signals():
 *
 * This routine tells the program to call particular routines when
 * certain interrupts/events occur:
 *
 *      SIGINT: call onintr() to interrupt fight with monster or long rest.
 *      SIGQUIT: call byebye() to check for game termination.
 *      SIGHUP: call error_save() to save game when terminal hangs up.
 *
 *		On VMS, SIGINT and SIGQUIT correspond to ^C and ^Y.
 *
 * This routine is not strictly necessary and can be stubbed.  This will
 * mean that the game cannot be interrupted properly with keyboard
 * input, this is not usually critical.
 */

void
md_heed_signals(void)
{
    void onintr(int);
    signal(SIGINT, onintr);
    signal(SIGQUIT, byebye);
    signal(SIGHUP, error_save);
}

/* md_ignore_signals():
 *
 * This routine tells the program to completely ignore the events mentioned
 * in md_heed_signals() above.  The event handlers will later be turned on
 * by a future call to md_heed_signals(), so md_heed_signals() and
 * md_ignore_signals() need to work together.
 *
 * This function should be implemented or the user risks interrupting
 * critical sections of code, which could cause score file, or saved-game
 * file, corruption.
 */

void
md_ignore_signals(void)
{
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
}

/* md_get_file_id():
 *
 * This function returns an integer that uniquely identifies the specified
 * file.  It need not check for the file's existence.  In UNIX, the inode
 * number is used.
 *
 * This function need not be implemented.  To stub the routine, just make
 * it return 0.  This will make the game less able to prevent users from
 * modifying saved-game files.  This is probably no big deal.
 */

int
md_get_file_id(char *fname)
{
    struct stat sbuf;

    if (stat(fname, &sbuf)) {
	return -1;
    }
    return ((int) sbuf.st_ino);
}

/* md_link_count():
 *
 * This routine returns the number of hard links to the specified file.
 *
 * This function is not strictly necessary.  On systems without hard links
 * this routine can be stubbed by just returning 1.
 */

int
md_link_count(char *fname)
{
    struct stat sbuf;

    stat(fname, &sbuf);
    return ((int) sbuf.st_nlink);
}

/* md_gct(): (Get Current Time)
 *
 * This function returns the current year, month(1-12), day(1-31), hour(0-23),
 * minute(0-59), and second(0-59).  This is used for identifying the time
 * at which a game is saved.
 *
 * This function is not strictly necessary.  It can be stubbed by returing
 * zeros instead of the correct year, month, etc.  If your operating
 * system doesn't provide all of the time units requested here, then you
 * can provide only those that it does, and return zeros for the others.
 * If you cannot provide good time values, then users may be able to copy
 * saved-game files and play them.  
 */

void
md_gct(struct rogue_time *rt_buf)
{
    struct tm *t;
    time_t seconds;

    time(&seconds);
    t = localtime(&seconds);

    rt_buf->year = t->tm_year;
    rt_buf->month = t->tm_mon + 1;
    rt_buf->day = t->tm_mday;
    rt_buf->hour = t->tm_hour;
    rt_buf->minute = t->tm_min;
    rt_buf->second = t->tm_sec;
}

/* md_gfmt: (Get File Modification Time)
 *
 * This routine returns a file's date of last modification in the same format
 * as md_gct() above.
 *
 * This function is not strictly necessary.  It is used to see if saved-game
 * files have been modified since they were saved.  If you have stubbed the
 * routine md_gct() above by returning constant values, then you may do
 * exactly the same here.
 * Or if md_gct() is implemented correctly, but your system does not provide
 * file modification dates, you may return some date far in the past so
 * that the program will never know that a saved-game file being modified.  
 * You may also do this if you wish to be able to restore games from
 * saved-games that have been modified.
 */

void
md_gfmt(char *fname, struct rogue_time *rt_buf)
{
    struct stat sbuf;
    time_t seconds;
    struct tm *t;

    stat(fname, &sbuf);
    seconds = (long) sbuf.st_mtime;
    t = localtime(&seconds);

    rt_buf->year = t->tm_year;
    rt_buf->month = t->tm_mon + 1;
    rt_buf->day = t->tm_mday;
    rt_buf->hour = t->tm_hour;
    rt_buf->minute = t->tm_min;
    rt_buf->second = t->tm_sec;
}

/* md_df: (Delete File)
 *
 * This function deletes the specified file, and returns true (1) if the
 * operation was successful.  This is used to delete saved-game files
 * after restoring games from them.
 *
 * Again, this function is not strictly necessary, and can be stubbed
 * by simply returning 1.  In this case, saved-game files will not be
 * deleted and can be replayed.
 */

boolean
md_df(char *fname)
{
    if (unlink(fname)) {
	return 0;
    }
    return 1;
}

/* md_gln: (Get login name)
 *
 * This routine returns the login name of the user.  This string is
 * used mainly for identifying users in score files.
 *
 * A dummy string may be returned if you are unable to implement this
 * function, but then the score file would only have one name in it.
 */

char *
md_gln(void)
{
    char *name;

    /* 環境変数 FIGHTER が設定されているなら最優先で取得する */
    name = getenv("FIGHTER");
    if (name != NULL) {
        return name;
    }

    /* 上記が存在しないなら getlogin 関数ログイン名を取得する */
    name = getlogin();
    if (name != NULL) {
        return name;
    }

    /* 上記が存在しないなら環境変数 USER を取得する */
    name = getenv("USER");
    if (name != NULL) {
        return name;
    }

    /* 上記全てが取得できないならば A FIGHTER とする */
    return "A FIGHTER";
}

/*
 * md_ghome
 * getenv("HOME") のラッパー関数
 */
char *
md_ghome(void)
{
    char *home;

    /* 環境変数 HOME が設定されているなら最優先で取得する */
    home = getenv("HOME");
    if (home != NULL) {
	return home;
    }

    /* 上記環境変数が取得できないならば、カレントディレクトリを取得する */
    getcwd(home, ROGUE_PATH_MAX);
    return home;
}

/* md_malloc()
 *
 * This routine allocates, and returns a pointer to, the specified number
 * of bytes.  This routines absolutely MUST be implemented for your
 * particular system or the program will not run at all.  Return zero
 * when no more memory can be allocated.
 */

char *
md_malloc(int n)
{
    void *malloc();

    return malloc(n);
}

/* md_gseed() (Get Seed)
 *
 * This function returns a seed for the random number generator (RNG).  This
 * seed causes the RNG to begin generating numbers at some point in it's
 * sequence.  Without a random seed, the RNG will generate the same set
 * of numbers, and every game will start out exactly the same way.  A good
 * number to use is the process id, given by getpid() on most UNIX systems.
 *
 * You need to find some single random integer, such as:
 *   process id.
 *   current time (minutes + seconds) returned from md_gct(), if implemented.
 *   
 * It will not help to return "get_rand()" or "rand()" or the return value of
 * any pseudo-RNG.  If you don't have a random number, you can just return 1,
 * but this means your games will ALWAYS start the same way, and will play
 * exactly the same way given the same input.
 */

int
md_gseed(void)
{
    return (int)time(NULL);
}

/* md_exit():
 *
 * This function causes the program to discontinue execution and exit.
 * This function must be implemented or the program will continue to
 * hang when it should quit.
 */

void
md_exit(int status)
{
#if !defined( ORIGINAL )
    if (org_dir && *org_dir)
	chdir(org_dir);
#endif /* not ORIGINAL */
    exit(status);
}
