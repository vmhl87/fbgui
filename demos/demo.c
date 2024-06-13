// import libraries + stdlib for random
#include <stdlib.h>
#include "../src/tgui.c"
#include "../src/tgin.c"

// dithering shader function
int speckle(int x, int y){
	if((x/2^y/2)&1) return 0;
	return (1<<24) - 1;
}

// background shader function - draws gradient
int bg(int x, int y){
	return to_color((255*y)/height, (255*x)/width, 100);
}

int main(){
	// hide cursor, clear screen, open framebuffer
	reset_tty();
	curs_set(0);
	set_echo(0);
	openfb();

	// shade background with gradient shader (leave margin because
	// of textual overflow from inputs)
	shade(10, 10, width-20, height-20, bg);

	// draw a random rectangle because why not
	rect(0, 224, 72, 16, 100, 100, 180);

	// draw some text because why not
	text_size(2);
	sstr("12345678", 0, 15*H_CHAR);
	text_size(1);

	// make a text box!
	save_state();
	text_color(120, 120, 120);
	rect(20*W_CHAR, 10*H_CHAR, 20*W_CHAR, H_CHAR, 180, 180, 180);
	sstr("enter stuff here", 20*W_CHAR, 10*H_CHAR);
	text_color(0, 0, 0);
	char *s;
	text_box(&s, 20*W_CHAR, 10*H_CHAR, 20, 180, 180, 180);
	reset_state();

	// make another text box, and read two integers
	save_state();
	text_color(140, 250, 250);
	rect(60*W_CHAR, 12*H_CHAR, 10*W_CHAR, H_CHAR, 30, 30, 30);
	sstr("coords", 60*W_CHAR, 12*H_CHAR);
	text_color(0, 180, 180);
	char *c;
	text_box(&c, 60*W_CHAR, 12*H_CHAR, 10, 30, 30, 30);
	int x = 60, y = 30; sscanf(c, "%d %d", &x, &y);
	reset_state();

	// say "hello" at those coords entered
	save_state();
	text_color(0, 255, 255);
	sstr("hello!", x*W_CHAR, y*H_CHAR);
	reset_state();

	// draw a lot of random boxes because why not
	srand(time(NULL));
	int lx = 0, ly = 0;
	for(int i=0; i<10; ++i){
		int x = rand()%width, y = rand()%height,
			w = 20+rand()%100, h = 20+rand()%100;
		x -= w/2, y -= h/2;
		shade(x, y, w, h, speckle);
		rectl(x, y, w, h, 255, 255, 255);
		// line(x, y, x+w, y+h, 205, 205, 255);
		// line(x+w, y, x, y+h, 255, 205, 205);
		line(x, y, lx, ly, 205, 205, 255);
		lx = x, ly = y;
		sleepms(500);
	}

	set_echo(1);

	// clear framebuffer
	blank();

	// close framebuffer, clear screen, show cursor
	closefb();
	reset_tty();
	curs_set(1);

	// print out everything inputted for debug
	printf("1: '%s'\nx: '%d'\ny: '%d'\n", s, x, y);
}
