package com.desktopStreamer;

import java.io.IOException;
import java.net.ServerSocket;

public class Main {

    static {
        System.loadLibrary("encode");
    }

    public static void main(String[] args) {
        new VideoSocket(9000);
    }
}
