#include "utils.h"
// FFMPEG IMPORT
#include <stdint.h>
// compatibility with newer API


#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame

extern "C" {
    #include "libavutil/opt.h"
    #include "libavcodec/avcodec.h"
    #include "libavutil/channel_layout.h"
    #include "libavutil/common.h"
    #include "libavutil/imgutils.h"
    #include "libavutil/mathematics.h"
    #include "libavutil/samplefmt.h"
  #include <libswscale/swscale.h>




#include "com_desktopStreamer_VideoStreamer.h"
#include "encoder_ffmpeg.h"

//FFMEG vars
AVCodec *codec;
AVCodecContext *c = NULL;
AVFrame *frame;
AVPacket pkt;
int i = 0; //frame counter

uint8_t *yuv[3];

void desktopStreamer_encoder_init(int *width, int *height)
{
      //FFMPEG CODEC INIT

        avcodec_register_all();

        /* find the mpeg1 video encoder */
        codec = avcodec_find_encoder(AV_CODEC_ID_H264);
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
        c->width = *width;
        c->height = *height;
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
        c->max_b_frames = 1;
        c->refs = 0;
        c->pix_fmt = AV_PIX_FMT_YUV420P; //AV_PIX_FMT_YUV444P;

        // ultrafast,superfast, veryfast, faster, fast, medium, slow, slower, veryslow
        av_opt_set(c->priv_data, "preset", "ultrafast", 0);
        av_opt_set(c->priv_data, "tune", "zerolatency", 0);
        av_opt_set(c->priv_data, "movflags", "faststart", 0);

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
}

void desktopStreamer_encoder_encodeFrame(RGBQUAD *rgb, int *w, int *h)
{
    
    int width = *w;
    int height = *h;
    av_init_packet(&pkt);
    pkt.data = NULL;    // packet data will be allocated by the encoder
    pkt.size = 0;

    fflush(stdout);
    for (int y = 0; y < width; y++) {
      for (int x = 0; x < height; x++) {
        
        RGBQUAD px = rgb[y*width+x];
        //printf(" pos : %d \n", y*width+x);
        //printf("R : %u, G : %u, B : %u \n ", px.rgbRed, px.rgbGreen, px.rgbBlue);
        int Y = ( (  66 * px.rgbRed + 129 * px.rgbGreen +  25 * px.rgbBlue + 128) >> 8) +  16;
        int U = ( ( -38 * px.rgbRed -  74 * px.rgbGreen + 112 * px.rgbBlue + 128) >> 8) + 128;
        int V = ( ( 112 * px.rgbRed -  94 * px.rgbGreen -  18 * px.rgbBlue + 128) >> 8) + 128;
        //printf(" Y %d, U : %d, V : %d \n", Y, U, V);

        frame->data[0][y * frame->linesize[0] + x] = Y;
        //frame->data[1][y * frame->linesize[0] + x] = U;
        //frame->data[2][y * frame->linesize[0] + x] = V;
        
        frame->data[1][(y >> 1) * frame->linesize[1] + (x >> 1)] = U;
        frame->data[2][(y >> 1) * frame->linesize[2] + (x >> 1)] = V;
      }
    }

    frame->pts = i;
    i++;
    /* encode the image */
    int got_output;
    int ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
    if (ret < 0) {
      fprintf(stderr, "Error encoding frame\n");
      exit(1);
    }

    if (got_output) {
      printf("Write frame (size=%5d)\n", pkt.size);
      streamBytes(pkt.data, pkt.size);
      //fwrite(pkt.data, 1, pkt.size, f);
      //boost::asio::write(*sock, buffer((char*)pkt.data, pkt.size));
      av_free_packet(&pkt);
    }

}
}
