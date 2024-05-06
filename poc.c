// import everything necessary for framebuffer interface
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

// pause by some amount of milliseconds
void sleepms(int t){
	struct timespec ts;
	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;
	nanosleep(&ts, &ts);
}

// globals
int fb;
int width, height, bytes, fb_size, linel;
char *fbdata;

// open framebuffer etc
void openfb(){
	// this path might be different on other machines? unsure
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
	fbdata = mmap(0, fb_size, PROT_READ|PROT_WRITE, MAP_SHARED, fb, (off_t)0);
}

// close framebuffer
void closefb(){
	memset(fbdata, 0, fb_size);
	munmap(fbdata, fb_size);
	close(fb);
}

// draw a point at x,y
void pt(int x, int y, int r, int g, int b){
	if(x < 0 || y < 0 || x >= width || y >= height) return;
	int off = x * bytes + y * linel;
	fbdata[off++] = b;
	fbdata[off++] = g;
	fbdata[off++] = r;
}

int main(){
	// open framebuffer
	openfb();
	// iterate over every pixel and draw point with gradient
	for(int i=0; i<width; ++i)
		for(int j=0; j<height; ++j)
			pt(i, j, (255*i)/width, (255*j)/height, 100);
	// wait for 6 seconds
	sleepms(6000);
	// close framebuffer
	closefb();
}
