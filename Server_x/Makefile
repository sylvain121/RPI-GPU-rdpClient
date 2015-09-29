LDFLAGS	+= -lX11 -lXext -lXtst
INC += -I/home/lui/include/ffmpeg-2.8
all:xcapture

xcapture: xcapture.o
	gcc -o xcapture xcapture.o -lX11 -lXext -lXtst -g -std=c99 $(INC)

xcapture.o: XCapture.c
	gcc -o xcapture.o -c XCapture.c -lX11 -lXext -lXtst -g -std=c99 $(INC)

