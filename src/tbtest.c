#include <stdlib.h>
#include "tgui.c"
#include "tgin.c"

int main(){
	reset_tty();
	curs_set(0);
	set_echo(0);
	openfb();

	rect(16, 16, width-32, height-32, 10, 10, 10);

	sstr("Hello World", 32, 64);

	char *str;
	int len = text_box(&str, 32, 32, 30, 30, 10, 20);

	blank();
	closefb();
	set_echo(1);
	curs_set(1);
	reset_tty();
	printf("l: %d\ns: %s\n", len, str);
}
