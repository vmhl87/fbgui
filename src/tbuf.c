#ifndef TBUF_EXISTS
#define TBUF_EXISTS

#include <termios.h>
<<<<<<< HEAD
#include <signal.h>
=======
>>>>>>> fb02cc5ce3bd8f4b35adc149cc020de28a46d6bb
#include <unistd.h>
#include <stdio.h>

void tbuf(int f){
	static struct termios old, new;
	static int init = 0;

	if(!init){
		init = 1;
		tcgetattr(STDIN_FILENO, &old);
		new = old, new.c_lflag &= (~ICANON & ~ECHO);
	}

<<<<<<< HEAD
	tcsetattr(STDIN_FILENO, TCSANOW, f ? &old : &new);
}

void curs_set(int s){
	printf("\033[?25%c", s ? 'h' : 'l');
	fflush(stdout);
}

void move(int x, int y){
	printf("\033[%d:%dH", y, x);
	fflush(stdout);
}

volatile sig_atomic_t signal_status = 0;
void sighandler(int s) { signal_status = s; }
void sig_start() { signal(SIGINT, sighandler), signal_status = 0; }
int sig_now() { return signal_status == SIGINT ? 1 : 0; }
void sig_stop() { signal(SIGINT, SIG_DFL); }

=======
	tcsetattr(STDIN_FILENO, TCSANOW, f ? &new : &old);
}

>>>>>>> fb02cc5ce3bd8f4b35adc149cc020de28a46d6bb
#endif
