#ifndef TBUF_EXISTS
#define TBUF_EXISTS

#include <termios.h>
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

	tcsetattr(STDIN_FILENO, TCSANOW, f ? &new : &old);
}

#endif
