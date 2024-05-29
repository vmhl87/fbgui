#ifndef TGIN_EXISTS
#define TGIN_EXISTS

// necessary headers for input/output and string ops
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "kbhit.c"

// constants for terminal dimensions
int W_CHARS = 170,
	H_CHARS = 48,
	W_CHAR = 8,
	H_CHAR = 16;

// color constants
const int NONE = 0,
		  BOLD = 1,
		  FAINT = 2,
		  ITALIC = 3,
		  UNDER = 4,
		  BLACK = 30,
		  RED = 31,
		  GREEN = 32,
		  YELLOW = 33,
		  BLUE = 34,
		  MAGENTA = 35,
		  CYAN = 36,
		  WHITE = 37;

// cursor state
int _curs_show = 1;

// set cursor state
void curs_set(int s){
	_curs_show = s;
	printf("\033[?25");
	if(s) printf("h");
	else printf("l");
	fflush(stdout);
}

// echo state
void set_echo(int s){
	struct termios term;
	tcgetattr(fileno(stdin), &term);

	if(s){
		term.c_lflag |= ECHO;
		tcsetattr(fileno(stdin), 0, &term);
	}else{
		term.c_lflag &= ~ECHO;
		tcsetattr(fileno(stdin), 0, &term);
	}
}

// move cursor
void move(int x, int y){
	printf("\033[%d;%dH", y, x);
	fflush(stdout);
}

// print centered text
void center(const char *s, int x, int y){
	move(x - strlen(s)/2 + 1, y);
	printf("%s", s);
	fflush(stdout);
}

// absolutely centered text
void xcenter(const char *s){
	center(s, W_CHARS/2, H_CHARS/2);
}

// get string (raw input)
ssize_t gets(char **str){
	size_t len;
	ssize_t chr = getline(str, &len, stdin);
	// clip ending newline if exists
	if(chr && (*str)[chr-1] == '\n') (*str)[--chr] = 0;
	return chr;
}

// return caught character buffer
char* getch(){
	int count = _kbhit();
	if(!count) return 0;
	char *str = malloc(sizeof(char) * count);
	fread(str, count, 1, stdin);
	return str;
}

// backgroundify
int BG(const int c){
	return c + 10;
}

// apply text attribute
void attr(int s){
	printf("\033[%dm", s);
	fflush(stdout);
}

// draw text box with prompt
ssize_t xtext_box(char **str, int x, int y, int w, const char *prompt){
	move(x, y);
	printf(prompt);
	w -= strlen(prompt);
	while(w--) printf(" ");
	move(x+strlen(prompt), y);
	int _curs = _curs_show;
	curs_set(1);
	ssize_t len = gets(str);
	curs_set(_curs);
	return len;
}

// draw text box without prompt
ssize_t text_box(char **str, int x, int y, int w){
	return xtext_box(str, x, y, w, "");
}

// draw character c at location (x,y) with size s and color (r,g,b)
void schar(char c, int x, int y, int s, int r, int g, int b){
	attr(NONE); attr(BOLD); attr(WHITE);
	move(0, 0);
	printf("%c", c);
	fflush(stdout);
	char *ptr = fbdata;
	for(int i=0; i<H_CHAR; ++i){
		for(int j=0; j<W_CHAR; ++j){
			if(*ptr == -1) rect(x + j*s, y + i*s, s, s, r, g, b);
			ptr += 4;
		}
		ptr += linel - W_CHAR*bytes;
	}
	rect(0, 0, W_CHAR, H_CHAR, 0, 0, 0);
	attr(NONE);
}

// draw string str centered at (x,y) with size s and color (r,g,b)
void sstr(const char *str, int x, int y, int s, int r, int g, int b){
	int p = -strlen(str);
	do schar(*str, x+(p*s*W_CHAR)/2, y, s, r, g, b), p += 2;
	while(*(++str) != 0);
}

#endif
