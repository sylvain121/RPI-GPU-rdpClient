//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <fstream>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <bounded_buffer.h>

#include "fps.h"
#include "monitor.h"
#include "params.h"
#include "config.h"
#include "controller.hpp"

#define CONTROL_PORT 9991


#ifdef DIRECTX_FOUND
	#include "windows/WDDMCapture.h"
#else
	#include "windows/GDICapture.h"
#endif

#ifdef FFMPEG_FOUND
	#include "encoder_ffmpeg/FFMPEG_encoding.hpp"
#endif

#ifdef NVENCODER_FOUND
	#include "encoder_nvenc/NV_encoding.hpp"
#endif

#ifdef _WIN32
	#include "windows_controller.hpp"
	WindowsController controller;
#endif


using namespace std;
using namespace boost::asio;
using ip::tcp;

const int max_length = 1024;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

bounded_buffer<RGBQUAD*> screenToSendQueue(2);

void threadScreenCapture(UINT monitorID, RECT screen){
	int height = screen.bottom - screen.top;
	int width = screen.right - screen.left;

#ifdef DIRECTX_FOUND
	WDDMCapture capture;
#else
	GDICapture capture;
#endif

	capture.init(monitorID, screen);

	RGBQUAD* pPixels;
	FPS fps;
	while(true){
		int rc = capture.getNextFrame(&pPixels);
		if (rc == 0) {
			RGBQUAD* pixCopy = new RGBQUAD[width * height];
			memcpy(pixCopy, pPixels, width * height * sizeof(RGBQUAD));
			screenToSendQueue.push_front(pixCopy);

			capture.doneNextFrame();
			fps.newFrame();
		}
	}
}

void sessionVideo(socket_ptr sock, UINT monitorID, RECT screen)
{
	
	// get the height and width of the screen
	int height = screen.bottom - screen.top;
	int width = screen.right - screen.left;

#ifdef NVENCODER_FOUND
	NV_encoding nv_encoding;
	nv_encoding.load(width, height, sock, monitorID);
#elif defined(FFMPEG_FOUND)
	FFMPEG_encoding ffmpeg;
	ffmpeg.load(width, height, sock);
#endif

	boost::thread t(boost::bind(threadScreenCapture, monitorID, screen));

	FPS fps;
	RGBQUAD* pPixels;
	while(true){
		screenToSendQueue.pop_back(&pPixels);

#ifdef NVENCODER_FOUND
		nv_encoding.write(width, height, pPixels);
#elif defined(FFMPEG_FOUND)
		ffmpeg.write(width, height, pPixels);
#endif
		//fps.newFrame();

		free(pPixels);
	}
#ifdef NVENCODER_FOUND
	nv_encoding.close();
#elif defined(FFMPEG_FOUND)
	ffmpeg.close();
#endif
}


int main(int argc, const char* argv[])
{
    cout << "Version 0.9" << endl;
	Params params(argc, argv);
    if (params.monitor == -1)
    {
		cerr << "Usage: ./server [options]" << endl;
		cerr << "monitor <n>\n";
		cerr << "Sample: ./server monitor 0" << endl;
		return 1;
    }

	controller.init(params.monitor, CONTROL_PORT);
	controller.setScreenCoordinates();
	controller.startControllerSocket();
	
	return 0;
}