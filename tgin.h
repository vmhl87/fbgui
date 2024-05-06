#include <iostream>
#include <cstring>
#include <cstdarg>
#include <cstdio>

const int W_CHARS = 170,
		  H_CHARS = 48,
		  W_CHAR = 8,
		  H_CHAR = 16;

bool _curs_show = 1;

void curs_set(bool s){
	_curs_show = s;
	printf("\033[?25");
	if(s) printf("h");
	else printf("l");
	fflush(stdout);
}

void move(int x, int y){
	printf("\033[%d;%dH", y, x);
	fflush(stdout);
}

void center(const char *s, int x, int y){
	move(x - strlen(s)/2 + 1, y);
	printf("%s", s);
	fflush(stdout);
}

void center(const char *s){
	center(s, W_CHARS/2, H_CHARS/2);
}

void center(std::string s){
	center(s.c_str());
}

std::string gets(){
	std::string ret;
	getline(std::cin, ret);
	return ret;
}

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

constexpr int BG(const int c){
	return c + 10;
}

void attr(int s){
	printf("\033[%dm", s);
	fflush(stdout);
}

std::string text_box(int x, int y, int w, const char *prompt){
	move(x, y);
	printf(prompt);
	w -= strlen(prompt);
	while(w--) printf(" ");
	move(x+strlen(prompt), y);
	bool _curs = _curs_show;
	curs_set(1);
	std::string ret = gets();
	curs_set(_curs);
	return ret;
}

std::string text_box(int x, int y, int w){
	return text_box(x, y, w, "");
}
