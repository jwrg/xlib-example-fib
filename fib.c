/*
 * fib.c -- a simple X program that illustrates the golden ratio and spiral
 *
 * gcc fib.c -o fib -lX11 -lm
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h> 
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define M_PHI 1.618033988749895
#define WINDOW_HEIGHT 1000

/* 
 * this expects a 24-bit colour value (i.e., 0xRRGGBB)
 * the high 8 bits are ignored 
 */
void set_colour(Display *display, int s, Colormap cmap, unsigned int rgb_value)
{
  XColor colour;
  colour.red = (rgb_value & 0xFF0000) >> 8;
  colour.green = (rgb_value & 0xFF00);
  colour.blue = (rgb_value & 0xFF) << 8;
  colour.flags = DoRed | DoGreen | DoBlue;
  XAllocColor(display, cmap, &colour);

  XSetForeground(display, DefaultGC(display, s), colour.pixel);
}

void draw_section(Display *display, Window window, int s, Colormap cmap, unsigned int xpos, unsigned int ypos, unsigned int height, unsigned short index)
{
  if (index % 2)
    XFillRectangle(display, window, DefaultGC(display, s), xpos, ypos, height, height * M_PHI);
  else
    XFillRectangle(display, window, DefaultGC(display, s), xpos, ypos, height * M_PHI, height);

  set_colour(display, s, cmap, 0xFFFFFF);

  switch(index % 4)
  {
    case 0:
      XDrawArc(display, window, DefaultGC(display, s), xpos + 1, ypos + 1, 2 * height - 2, 2 * height - 2, 90 * 64, 90 * 64);
      break;
    case 1:
      XDrawArc(display, window, DefaultGC(display, s), xpos - height + 1, ypos + 1, 2 * height - 2, 2 * height - 2, 0 * 64, 90 * 64);
      break;
    case 2:
      XDrawArc(display, window, DefaultGC(display, s), xpos + height * (M_PHI - 2) + 1, ypos - height + 1, 2 * height - 1, 2 * height - 2, 270 * 64, 90 * 64);
      break;
    case 3:
      XDrawArc(display, window, DefaultGC(display, s), xpos + 1, ypos + height * (M_PHI - 2) + 1, 2 * height - 2, 2 * height - 2, 180 * 64, 90 * 64);
      break;
  }
}

void draw_spiral(Display *display, Window window, int s, Colormap cmap, unsigned int xpos, unsigned int ypos, unsigned int height, unsigned int seed)
{
unsigned short index = 0;
  while (height > 2)
  {
    set_colour(display, s, cmap, rand_r(&seed));
    draw_section(display, window, s, cmap, xpos, ypos, height, index);

    switch(index % 4)
    {
      case 0:
        xpos += height;
        break;
      case 1:
        ypos += height;
        break;
    }

    height = round((double)height / M_PHI);
    index++;
  }
}

int main(void)
{
  Display *display;
  Window window;
  XSetWindowAttributes attributes;
  XSizeHints sizehints;
  XWMHints hints;
  XTextProperty windowName, iconName;
  XEvent event;
  Colormap cmap;
  XColor colour;

  char *window_name = "Fib -- an xlib golden spiral";
  char *icon_name = "Fib";

  int s;
  unsigned long value_mask;

  /* open connection to X server */
  display = XOpenDisplay(NULL);
  if (display == NULL)
  {
    fprintf(stderr, "ERROR -- cannot open display\n");
    exit(7);
  }

  s = DefaultScreen(display);
  cmap = DefaultColormap(display, s);

  /* set up attributes */
  attributes.background_pixel = BlackPixel(display, s);
  attributes.border_pixel = WhitePixel(display, s);
  attributes.event_mask = ButtonPressMask;
  value_mask = CWBackPixel | CWBorderPixel | CWEventMask;

  /* create window */
  window = XCreateWindow(display, 
      RootWindow(display, s), 
      10, 10, WINDOW_HEIGHT * M_PHI + 80, WINDOW_HEIGHT + 60, 2,
      DefaultDepth(display, s),
      InputOutput,
      DefaultVisual(display, s),
      value_mask, &attributes);

  /* select events we care about */
  XSelectInput(display, window, ExposureMask | KeyPressMask);

  /* give the window hints */
  sizehints.flags = USPosition | USSize;
  XSetWMNormalHints(display, window, &sizehints);
  hints.initial_state = NormalState;
  hints.flags = StateHint;
  XSetWMHints(display, window, &hints);
  XStringListToTextProperty(&window_name, 1, &windowName);
  XSetWMName(display, window, &windowName);
  XStringListToTextProperty(&icon_name, 1, &iconName);
  XSetWMIconName(display, window, &iconName);

  /* map the window */
  XMapWindow(display, window);

  /* event loop */
  for (;;)
  {
    XNextEvent(display, &event);

    /* draw/redraw window */
    if (event.type == Expose)
    {
      draw_spiral(display, window, s, cmap, 40, 30, WINDOW_HEIGHT, 1428571428);
      XFlush(display);
    }

    /* exit on keystroke */
    if (event.type == KeyPress)
      break;
  }

  /* close connection to X server */
  XCloseDisplay(display);

  return 0;
}
