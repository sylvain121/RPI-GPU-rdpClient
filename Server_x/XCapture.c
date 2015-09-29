  

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


// FFMPEG IMPORT

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>


//X11GRAB vars
static int screenNumber;
static int width, height, depth;

static char *displayname = ":0";
static Display *display = NULL;
static Window rootWindow;
static Screen *screen = NULL;
static XImage *image = NULL;

static XShmSegmentInfo __xshminfo;


//FFMEG vars
AVCodecID codec_id;
AVCodec *codec;
AVCodecContext *c;
AVFrame *frame;
AVPacket pkt;
int i; //frame counter



typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned int      uint32_t;
typedef unsigned long int uint64_t;

uint8_t *yuv[3];
  

int main(int argc, char *argv[]) {

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
  width = XDisplayWidth(display, screenNumber);
  height = XDisplayHeight(display, screenNumber);
  depth = XDisplayPlanes(display, screenNumber);
  printf("X-Window-init: dimension: %dx%dx%d @ %d/%d\n",
      width, height, depth,
      screenNumber, XScreenCount(display));
  //create image context
  if((image = XShmCreateImage(display,
      XDefaultVisual(display, screenNumber),
      depth, ZPixmap, NULL, &__xshminfo,
      width, height)) == NULL) {
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


  //FFMPEG CODEC INIT
    c = NULL;
    codec_id = AV_CODEC_ID_H264;
    i=0;
        avcodec_register_all();

    /* find the mpeg1 video encoder */
    codec = avcodec_find_encoder(codec_id);
    if (!codec) {
      fprintf(stderr, "Codec not found\n");
      exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
      fprintf(stderr, "Could not allocate video codec context\n");
      exit(1);
    }

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = width;
    c->height = height;
    /* frames per second */
    AVRational r;
    r.den=1;
    r.num=25;
    c->time_base = r;
    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 0;
    c->refs = 0;
    c->pix_fmt = AV_PIX_FMT_YUV420P;//AV_PIX_FMT_YUV444P;

    // ultrafast,superfast, veryfast, faster, fast, medium, slow, slower, veryslow
    if (codec_id == AV_CODEC_ID_H264) {
      av_opt_set(c->priv_data, "preset", "veryfast", 0);
      av_opt_set(c->priv_data, "tune", "zerolatency", 0);
      av_opt_set(c->priv_data, "movflags", "faststart", 0);
    }

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
      fprintf(stderr, "Could not open codec\n");
      exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
      fprintf(stderr, "Could not allocate video frame\n");
      exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    int ret = av_image_alloc(frame->data, frame->linesize, c->width, c->height,
               c->pix_fmt, 32);
    if (ret < 0) {
      fprintf(stderr, "Could not allocate raw picture buffer\n");
      exit(1);
    }


  for(;;) {


    // get image from X11

    if(XShmGetImage(display, rootWindow, image, 0, 0, XAllPlanes()) == 0) {
      // image->data RGBA
      printf("FATAL: XShmGetImage failed.\n");
      exit(-1);
    }

    //DO YUV conversion

    for(int x = 0; x < width; ++x) {
      for(int y=0; y< height; ++y) {

        //extract RGB data

        unsigned long pixel = XGetPixel(image, x, y);
        unsigned char blue = pixel && image->blue_mask;
        unsigned char green = (pixel & image->green_mask) >> 8;
        unsigned char red = (pixel & image->red_mask) >>16;

        //printf("R : %u, G : %u, B : %u \n",red, green, blue);

        // convert to YUV

        int Y = ( (  66 * red + 129 * green +  25 * blue + 128) >> 8) +  16;
        int U = ( ( -38 * red -  74 * green + 112 * blue + 128) >> 8) + 128;
        int V = ( ( 112 * red -  94 * green -  18 * blue + 128) >> 8) + 128;

        frame->data[0][y * frame->linesize[0] + x] = Y;
        //frame->data[1][y * frame->linesize[0] + x] = U;
        //frame->data[2][y * frame->linesize[0] + x] = V;
        
        frame->data[1][(y >> 1) * frame->linesize[1] + (x >> 1)] = U;
        frame->data[2][(y >> 1) * frame->linesize[2] + (x >> 1)] = V;



      }
    }

    // ENCODE YUV FRAME TO H264

    frame->pts = i;
    i++;
    /* encode the image */
    int got_output;
    int ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
    if (ret < 0) {
      fprintf(stderr, "Error encoding frame\n");
      exit(1);
    }

    
    // SEND IT TO SOCKET
    if (got_output) {
      /*printf("Write frame (size=%5d)\n", pkt.size);
      //fwrite(pkt.data, 1, pkt.size, f);
      boost::asio::write(*sock, buffer((char*)pkt.data, pkt.size));
      av_free_packet(&pkt);*/
    }

  }

}

 