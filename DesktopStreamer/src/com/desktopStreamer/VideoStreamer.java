package com.desktopStreamer;


import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;

public class VideoStreamer {

    private final OutputStream out;
    ;

    public VideoStreamer(OutputStream out) {
        this.out = out;
    }

    public native void stream();



    public void streamByte(byte[] videoStream) {
        try {
            this.out.write(videoStream);
            this.out.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }


    }
}
