#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

// global vars - framebuffer state, dimensions, mmapped loc
int fb;
int width, height, bytes, fb_size, linel;
char *fbdata;

// does what it looks like it does
void sleepms(int t){
	struct timespec ts;
	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;
	nanosleep(&ts, &ts);
}

// open framebuffer device, memory map, get dimensions
void openfb(){
	// this might be different on other machines. Not sure.
	fb = open("/dev/fb0", O_RDWR);
	if(fb < 0) return;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	ioctl(fb, FBIOGET_VSCREENINFO, &vinfo);
	ioctl(fb, FBIOGET_FSCREENINFO, &finfo);
	width = vinfo.xres_virtual, height = vinfo.yres_virtual;
	bytes = vinfo.bits_per_pixel >> 3;
	linel = finfo.line_length;
	fb_size = width * height * bytes;
	fb_size = vinfo.yres_virtual * finfo.line_length;
	fbdata = (char*)mmap(0, fb_size, PROT_READ|PROT_WRITE, MAP_SHARED, fb, (off_t)0);
}

// set (x,y) to (r,g,b)
void pt(int x, int y, int r, int g, int b){
	if(x < 0 || y < 0 || x >= width || y >= height) return;
	char *cell = fbdata + x*bytes + y*linel;
	*(cell++) = b;
	*(cell++) = g;
	*(cell++) = r;
}

// internal - draw horizontal line from (x1,y) to (x2,y) with (r,g,b)
void _hline(int x1, int x2, int y, int r, int g, int b){
	if(y < 0 || y >= height) return;
	if(x1 > x2) x1 ^= x2, x2 ^= x1, x1 ^= x2;
	if(x1 < 0) x1 = 0;
	if(x2 < 0) x2 = 0;
	if(x1 >= width) x1 = width-1;
	if(x2 >= width) x2 = width-1;
	char *cell = fbdata + x1*bytes + y*linel;
	for(; x1 <= x2; ++x1){
		*(cell++) = b;
		*(cell++) = g;
		*(cell++) = r;
		++cell;
	}
}

// vertical line
void _vline(int x, int y1, int y2, int r, int g, int b){
	if(x < 0 || x >= width) return;
	if(y1 > y2) y1 ^= y2, y2 ^= y1, y1 ^= y2;
	if(y1 < 0) y1 = 0;
	if(y2 < 0) y2 = 0;
	if(y1 >= height) y1 = height-1;
	if(y2 >= height) y2 = height-1;
	char *cell = fbdata + x*bytes + y1*linel;
	for(; y1 <= y2; ++y1){
		*(cell++) = b;
		*(cell++) = g;
		*(cell++) = r;
		cell += linel - 3;
	}
}

// arbitrary line - more overhead than hline or vline
void line(int x1, int y1, int x2, int y2, int r, int g, int b){
	// uses bresenham
	int dx = abs(x2 - x1),
		sx = x1 < x2 ? 1 : -1,
		dy = -abs(y2 - y1),
		sy = y1 < y2 ? 1 : -1,
		err = dx + dy;
	char *cell = NULL;
	for(;;){
		if(x1 < 0 || y1 < 0 || x1 >= width || y1 >= height) cell = NULL;
		if(cell == NULL && x1 >= 0 && y1 >= 0 && x1 < width && y1 < height)
			cell = fbdata + x1*bytes + y1*linel;
		if(cell != NULL) *cell = b, *(cell+1) = g, *(cell+2) = r;
		if(x1 == x2 && y1 == y2) break;
		int e2 = 2 * err;
		if(e2 >= dy){
			if(x1 == x2) break;
			err += dy, x1 += sx;
			if(cell != NULL && x1 >= 0 && x1 < width) cell += sx*bytes;
		}
		if(e2 <= dx){
			if(y1 == y2) break;
			err += dx, y1 += sy;
			if(cell != NULL && y1 >= 0 && y1 < height) cell += sy*linel;
		}
	}
}

// store 8-bit color in 32-bit int
union color{
	uint32_t a;
	char b[4];
};

// convert int into color union
union color _to_color(int i){
	union color r; r.a = i;
	return r;
}

// convert r,g,b triplet into int
int to_color(int r, int g, int b){
	return (r&255) | (g&255)<<8 | (b&255)<<16;
}

// shade rectangular region with col() function
void shade(int x, int y, int w, int h, int (*col)(int, int)){
	if(x+w > width) w = width-x, x = width-w;
	if(y+h > height) h = height-y, y = height-h;
	if(x < 0) w += x, x = 0;
	if(y < 0) h += y, y = 0;
	char *cell = fbdata + x*bytes + y*linel;
	for(int i=0; i<h; ++i){
		for(int j=0; j<w; ++j){
			union color clr = _to_color(col(x+j, y+i));
			*(cell++) = clr.b[2];
			*(cell++) = clr.b[1];
			*(cell++) = clr.b[0];
			++cell;
		}
		cell += linel - w*bytes;
	}
}

// fill rectangle with (r,g,b)
void rect(int x, int y, int w, int h, int r, int g, int b){
	if(x+w > width) w = width-x, x = width-w;
	if(y+h > height) h = height-y, y = height-h;
	if(x < 0) w += x, x = 0;
	if(y < 0) h += y, y = 0;
	char *cell = fbdata + x*bytes + y*linel;
	for(int i=0; i<h; ++i){
		for(int j=0; j<w; ++j){
			*(cell++) = b;
			*(cell++) = g;
			*(cell++) = r;
			++cell;
		}
		cell += linel - w*bytes;
	}
}

// draw outline of rectangle with (r,g,b)
void rectl(int x, int y, int w, int h, int r, int g, int b){
	_hline(x, x+w, y, r, g, b);
	_hline(x, x+w, y+h, r, g, b);
	_vline(x, y, y+h, r, g, b);
	_vline(x+w, y, y+h, r, g, b);
}

// reset framebuffer to fully black
void blank(){
	memset(fbdata, 0, fb_size);
}

// close framebuffer and unmap memory
void closefb(){
	munmap(fbdata, fb_size);
	close(fb);
}
