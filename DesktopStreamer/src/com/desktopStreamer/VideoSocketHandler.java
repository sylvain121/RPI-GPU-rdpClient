package com.desktopStreamer;

import java.io.IOException;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Created by sylvain on 02/11/15.
 */
public class VideoSocketHandler implements Runnable {
    private final ServerSocket socket;
    private Socket clientSocket;
    private OutputStream out;

    public VideoSocketHandler(ServerSocket socket) {
        this.socket = socket;
    }

    @Override
    public void run() {
        try {
            this.clientSocket = this.socket.accept();
            this.out = this.clientSocket.getOutputStream();
            VideoStreamer vStream = new VideoStreamer(this.out);
            vStream.stream();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }


    public void sendVideoStreamToClient() {

    }
}
