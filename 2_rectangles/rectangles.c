#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(int argc, char *argv[]) {
  Display *dpy;
  int screen;
  Window win;
  XEvent e;
  int done;

  dpy = XOpenDisplay(NULL);
  if (!dpy) {
    puts("Could not open display.");
  } else {
    screen = DefaultScreen(dpy);
    win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
      100, 100, 200, 150, 1,
      BlackPixel(dpy, screen), WhitePixel(dpy, screen));

    XStoreName(dpy, win, "X11 Test");

    Atom WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &WM_DELETE_WINDOW, 1);

    XSelectInput(dpy, win, ExposureMask | KeyPressMask);
    XMapWindow(dpy, win);

    done = 0;
    while (!done) {
      XNextEvent(dpy, &e);
      if (e.type == Expose) {
        GC gc = DefaultGC(dpy, screen);
        XFillRectangle(dpy, win, gc, 50, 50, 20, 50);
        XFillRectangle(dpy, win, gc, 75, 50, 20, 50);
      } else if (e.type == KeyPress) {
        char buf[128] = {0};
        KeySym keysym;
        int len = XLookupString(&e.xkey, buf, sizeof(buf),
                                &keysym, NULL);
        if (keysym == XK_Escape) {
          done = 1;
        }
      } else if ((e.type == ClientMessage) &&
                 (e.xclient.data.l[0] == WM_DELETE_WINDOW)) {
        done = 1;
      }
    }
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
  }

  return 0;
}
