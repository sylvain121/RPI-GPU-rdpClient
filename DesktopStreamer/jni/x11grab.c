
#include <stdio.h>
#include "utils.h"
//X11GRAB IMPORT

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <X11/X.h>
#include <X11/Xlibint.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <sys/shm.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xfixes.h>
#include "x11grab.h"



//X11GRAB vars
static int screenNumber;
static char *displayname = ":0";
static Display *display = NULL;
static Window rootWindow;
static Screen *screen = NULL;
static XImage *image = NULL;
XColor color;

static XShmSegmentInfo __xshminfo;


int x11_init (int *width, int *height, int *depth)
{
  // init
  int ignore = 0;
  bzero(&__xshminfo, sizeof(__xshminfo));

  // open display
  if((display = XOpenDisplay(NULL)) == NULL) {
    printf("cannot open display \"%s\"\n", displayname ? displayname : "DEFAULT");
    return -1;
  }

  // check MIT extension
  if(XQueryExtension(display, "MIT-SHM", &ignore, &ignore, &ignore) ) {
    int major, minor;
    Bool pixmaps;
    if(XShmQueryVersion(display, &major, &minor, &pixmaps) == True) {
      printf("XShm extention version %d.%d %s shared pixmaps\n",
          major, minor, (pixmaps==True) ? "with" : "without");
    } else {
      printf("XShm extension not supported.\n");
    }
  }
  // get default screen
  screenNumber = XDefaultScreen(display);
  if((screen = XScreenOfDisplay(display, screenNumber)) == NULL) {
    printf("cannot obtain screen #%d\n", screenNumber);
  }
  // get screen hight, width, depth
  *width = XDisplayWidth(display, screenNumber);
  *height = XDisplayHeight(display, screenNumber);
  *depth = XDisplayPlanes(display, screenNumber);
  printf("X-Window-init: dimension: %dx%dx%d @ %d/%d\n",
      *width, *height, *depth,
      screenNumber, XScreenCount(display));
  //create image context
  if((image = XShmCreateImage(display,
      XDefaultVisual(display, screenNumber),
      *depth, ZPixmap, NULL, &__xshminfo,
      *width, *height)) == NULL) {
    printf("XShmCreateImage failed.\n");
  }


  //get shm info
  if((__xshminfo.shmid = shmget(IPC_PRIVATE,
        image->bytes_per_line*image->height,
        IPC_CREAT | 0777)) < 0) {
    printf("shmget error");
  }

    //
  __xshminfo.shmaddr = image->data = (char*) shmat(__xshminfo.shmid, 0, 0);
  __xshminfo.readOnly = False;
  if(XShmAttach(display, &__xshminfo) == 0) {
    printf("XShmAttach failed.\n");
  }
  //
  rootWindow = XRootWindow(display, screenNumber);
}


void x11_getNextFrame(RGBQUAD *rgb, int *width, int *height) {

        // get image from X11

        if(XShmGetImage(display, rootWindow, image, 0, 0, XAllPlanes()) == 0) {
          // image->data RGBA
          printf("FATAL: XShmGetImage failed.\n");
          exit(-1);
        }


        //DO RGBQUADS conversion

        for(int y = 0; y < *height; ++y) {
          for(int x=0; x < *width; ++x) {

            //extract RGB data
            //printf(" width : %d, height : %d \n", x, y);
            //printf("x: %d, y: %d \n", x, y);
            unsigned long p = XGetPixel(image, x, y);
            //printf("pixel : %u \n", p);
            RGBQUAD px = rgb[(y-1)*(*width)+(x-1)];
            px.rgbBlue = p & image->blue_mask;
            px.rgbGreen = (p & image->green_mask) >> 8;
            px.rgbRed = (p & image->red_mask) >> 16;
            rgb[x*y] = px; 

            //printf("R : %u, G : %u, B : %u \n",px.rgbRed, px.rgbGreen, px.rgbBlue);
          }
        }
}
