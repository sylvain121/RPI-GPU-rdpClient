package com.desktopStreamer;

import java.io.IOException;
import java.net.ServerSocket;

/**
 * Created by sylvain on 02/11/15.
 */
public class VideoSocket {
    private ServerSocket socket = null;

    public VideoSocket(int port) {
        try {
            this.socket = new ServerSocket(port);
            new Thread(new VideoSocketHandler(this.socket)).start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
