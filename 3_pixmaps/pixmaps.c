#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

XImage *CreatePixmapImage(Display *display, Visual *visual,
    void ** data, int width, int height) {
  static XShmSegmentInfo shminfo;
  XImage *image = XShmCreateImage(display, visual, 24,
    ZPixmap, 0, &shminfo, width, height); 
  
  shminfo.shmid = shmget(IPC_PRIVATE,
    image->bytes_per_line * image->height, IPC_CREAT|0777);
  shminfo.shmaddr = image->data = *data = shmat(shminfo.shmid, 0, 0);
  shminfo.readOnly = False;

  XShmAttach(display, &shminfo);
  return image;
}

void redrawImage(unsigned char *image, int width, int height) {
  int i, j, q;
  static double time = 0;
  //unsigned char *image32 = (unsigned char *)malloc(width*height*4);
  unsigned char *p = image;
  double ttime = sin(time) * 256;
  for (i = 0; i<width; i++) {
    for (j = 0; j<height; j++) {
      q = (int)(i * j) % 256;
      *p++ = q;     // blue
      *p++ = q;     // green
      *p++ = ttime; // red
     
      p++;
    }
  }
  time += 0.06;
  if (time >= M_PI) time = 0;
}

void processEvent(Display *display, Window window,
    Visual* visual, int width, int height) {
  XEvent ev;
  XImage *ximage;
  
  if (XPending(display)) {
    XNextEvent(display, &ev);
    switch (ev.type) {
      case Expose: break;
      case ButtonPress:
      case KeyPress: exit(0); break;
    }
  }
}

int main(int argc, char **argv) {
  int width = 512, height = 512;
  Display *display = XOpenDisplay(NULL);
  Visual *visual = DefaultVisual(display, 0);
  Window window = XCreateSimpleWindow(display, RootWindow(display, 0),
    0, 0, width, height, 1, 0, 0);
  XImage *ximage;
  uint8_t *image;
  if (visual->class != TrueColor) {
    fprintf(stderr, "Could not handle non-true-color visual...\n");
    exit(1);
  }
  
  XSelectInput(display, window,
    ButtonPressMask | ExposureMask | KeyPressMask);
  XMapWindow(display, window);
  
  ximage = CreatePixmapImage(display, visual, (void**)&image,
    width, height);
  
  while (1) {
    processEvent(display, window, visual, width, height);
    redrawImage(image, width, height);
    XPutImage(display, window, DefaultGC(display, 0), ximage,
      0, 0, 0, 0, width, height);
    usleep(10000);
  }
}
