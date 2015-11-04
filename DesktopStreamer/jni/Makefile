LDFLAGS	+= -fPIC -Wall
DEBUG += -g

lib: x11grab ffmpeg main jni
	g++ -shared -I"/usr/lib/jvm/java-7-oracle/include" -I"/usr/lib/jvm/java-7-oracle/include/linux/" -o ../out/production/DesktopStreamer/libencode.so -Wl,-soname,encode.so -g -fPIC com_desktopStreamer_VideoStreamer.o x11grab.o encoder_ffmpeg.o main.o -lavutil -lavcodec -lX11 -lXext -lm -lz -lpthread -lx264 

x11grab:
	g++ -I"/usr/lib/jvm/java-7-oracle/include" -I"/usr/lib/jvm/java-7-oracle/include/linux/" -o x11grab.o x11grab.c -c -g -fPIC

ffmpeg:
	g++ -I"/usr/lib/jvm/java-7-oracle/include" -I"/usr/lib/jvm/java-7-oracle/include/linux/" -o encoder_ffmpeg.o encoder_ffmpeg.c -c -g -fPIC -fpermissive

main:
	g++  -I"/usr/lib/jvm/java-7-oracle/include" -I"/usr/lib/jvm/java-7-oracle/include/linux/" -o main.o video_streamer.c -c -g -fPIC -fpermissive

jni:
	g++  -I"/usr/lib/jvm/java-7-oracle/include" -I"/usr/lib/jvm/java-7-oracle/include/linux/" -o com_desktopStreamer_VideoStreamer.o com_desktopStreamer_VideoStreamer.c -c -g -fPIC

clean:
	rm *.o
	rm ../out/production/DesktopStreamer/libencode.so



