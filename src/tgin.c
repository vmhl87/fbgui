#ifndef TGIN_EXISTS
#define TGIN_EXISTS

// necessary headers for input/output and string ops
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include "kbhit.c"
#include "tgui.c"

// constants for terminal dimensions
int W_CHARS = 170,
	H_CHARS = 48,
	W_CHAR = 8,
	H_CHAR = 16;

// set cursor state
void curs_set(int s){
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

// reset sequence
void reset_tty(){
	printf("\033c");
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

// alignment - 0 = left, 1 = center, 2 = right
int x_align = 0, y_align = 0;
void text_align(int xm, int ym){
	x_align = xm, y_align = ym;
}

// text color, size
int text_col[3] = {255, 255, 255}, text_siz = 1;
void text_color(int r, int g, int b){
	text_col[0] = r, text_col[1] = g, text_col[2] = b;
}
void text_size(int s){
	text_siz = s;
}

// pseudo push/pop
int sx_a = 0, sy_a = 0, st_co[3] = {255, 255, 255}, st_s = 1;
void _save_state(){
	sx_a = x_align, sy_a = y_align;
	for(int i=0; i<3; ++i) st_co[i] = text_col[i];
	st_s = text_siz;
}
void _reset_state(){
	x_align = sx_a, y_align = sy_a;
	for(int i=0; i<3; ++i) text_col[i] = st_co[i];
	text_siz = st_s;
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
int getch(char **str){
	int count = _kbhit();
	if(!count) return 0;
	*str = malloc(sizeof(char) * count);
	fread(*str, count, 1, stdin);
	return count;
}

// char stack
typedef struct chll{
	char c;
	int len;
	struct chll *prev;
}chll;

// draw character c at location (x,y) with scale s
void schar(char c, int x, int y){
	int s = text_siz;
	move(0, 0);
	printf("\033[0;1;37m%c", c);
	fflush(stdout);
	x += (s*W_CHAR*x_align)/2;
	y += (s*H_CHAR*y_align)/2;
	char *ptr = fbdata;
	for(int i=0; i<H_CHAR; ++i){
		for(int j=0; j<W_CHAR; ++j){
			if(*ptr == -1) rect(x + j*s, y + i*s, s, s,
					text_col[0], text_col[1], text_col[2]);
			ptr += 4;
		}
		ptr += linel - W_CHAR*bytes;
	}
	rect(0, 0, W_CHAR, H_CHAR, 0, 0, 0);
	printf("\033[0m"); fflush(stdout);
}

// draw string str centered at (x,y) with scale s
void sstr(const char *str, int x, int y){
	int p = -strlen(str)*x_align, p2 = (text_siz*H_CHAR*y_align)/2;
	do schar(*str, x+(p*text_siz*W_CHAR)/2, y-p2), p += 2;
	while(*(++str) != 0);
}

// graphical wrapping text box
int text_box(char **str, int x, int y, int w, int r, int g, int b){
#define s text_siz
	chll *ret = malloc(sizeof(chll)); ret->len = 0;
	int newline = 0;
	while(!newline){
		char *tmp;
		int count = getch(&tmp);
		if(count){
			for(int i=0; i<count; ++i){
				if(tmp[i] == '\n'){
					newline = 1;
					break;
				}
				if(tmp[i] == 127 || tmp[i] == 8){
					if(ret->len){
						chll *old = ret;
						ret = ret->prev;
						free(old);
					}
					continue;
				}
				chll *new = malloc(sizeof(chll));
				new->c = tmp[i], new->len = ret->len + 1;
				new->prev = ret; ret = new;
			}
			free(tmp);
			int offset = ret->len - w + 1;
			if(offset < 1) offset = 1;
			chll *p = ret;
			rect(x, y, W_CHAR*s*w, H_CHAR*s, r, g, b);
			while(p->len >= offset){
				schar(p->c, x+(p->len-offset)*W_CHAR, y);
				p = p->prev;
			}
		}
		usleep(100000);
	}
	*str = malloc(sizeof(char) * (1 + ret->len));
	(*str)[ret->len] = '\0';
	int len = ret->len;
	while(ret->len){
		(*str)[ret->len - 1] = ret->c;
		chll *old = ret;
		ret = ret->prev;
		free(old);
	}
	free(ret);
	return len;
}

#endif
