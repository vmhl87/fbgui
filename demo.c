// import libraries + stdlib for random
#include <stdlib.h>
#include "tgui.c"
#include "tgin.c"

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
	curs_set(0);
	system("clear");
	openfb();

	// shade background with gradient shader (leave margin because
	// of textual overflow from inputs)
	shade(10, 10, width-20, height-20, bg);

	// draw a random rectangle because why not
	rect(0, 224, 72, 16, 100, 100, 180);

	// draw some text because why not
	move(0, 15);
	// colored black with white background
	attr(BG(WHITE)); attr(BLACK);
	printf("12345678");
	attr(NONE);

	// make a text box!
	attr(BG(WHITE)); attr(BLACK);
	char *s;
	xtext_box(&s, 20, 10, 60, "enter anything here: ");
	attr(NONE);

	// make another text box, and read two integers
	attr(CYAN);
	char *c;
	xtext_box(&c, 60, 12, 20, "coords: ");
	int x = 60, y = 30; sscanf(c, "%d %d", &x, &y);
	attr(NONE);

	// say "hello" at those coords entered
	move(x, y);
	attr(BG(CYAN)); attr(BLACK);
	printf("hello!");
	attr(NONE);

	set_echo(0);

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
	system("clear");
	curs_set(1);

	// print out everything inputted for debug
	printf("1: '%s'\nx: '%d'\ny: '%d'\n", s, x, y);
}
