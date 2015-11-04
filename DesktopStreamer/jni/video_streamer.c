
#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"
#include "x11grab.h"
#include "encoder_ffmpeg.h"
#include "video_streamer.h"

bool WITH_NVENC = false;
bool X11GRAB = true;

int screenWidth, screenHeight, screenDepths;


void start_streaming()
{

    if(X11GRAB) {
        x11_init(&screenWidth, &screenHeight, &screenDepths);

    }


    RGBQUAD *pRgb = malloc( screenWidth * screenHeight * 3 * sizeof(RGBQUAD));

    if(WITH_NVENC){

    } else {
     desktopStreamer_encoder_init(&screenWidth, &screenHeight);
    }

    for(;;) {
           if(X11GRAB) {
              x11_getNextFrame(pRgb, &screenWidth, &screenHeight);
           }


           if(WITH_NVENC){

           } else {
            desktopStreamer_encoder_encodeFrame(pRgb, &screenWidth, &screenHeight);
           }
    }

}


int main(int argc,char *argv[]) {
    start_streaming();
}





