#include <stdlib.h>
#include "tgui.h"
#include "tgin.h"

int speckle(int x, int y){
	if((x/2^y/2)&1) return 0;
	return (1<<24) - 1;
}

int bg(int x, int y){
	return to_color((255*y)/height, (255*x)/width, 100);
}

int main(){
	curs_set(0);
	system("clear");
	openfb();

	shade(10, 10, width-20, height-20, bg);

	rect(0, 224, 72, 16, 100, 100, 180);

	move(0, 15);
	attr(BG(WHITE)); attr(BLACK);
	printf("12345678");
	attr(NONE);

	attr(BG(WHITE)); attr(BLACK);
	std::string s = text_box(20, 10, 60, "enter anything here: ");
	attr(NONE);

	attr(CYAN);
	std::string c = text_box(60, 12, 20, "coords: ");
	int x = 60, y = 30; sscanf(c.c_str(), "%d %d", &x, &y);
	attr(NONE);

	move(x, y);
	attr(BG(CYAN)); attr(BLACK);
	printf("hello!");
	attr(NONE);

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

	blank();

	closefb();
	system("clear");
	curs_set(1);

	printf("1: '%s'\nx: '%d'\ny: '%d'\n", s.c_str(), x, y);
}
