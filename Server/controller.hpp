#pragma once
#include "../common/controller_message.hpp"
#include "streamer.hpp"

using namespace std;
using namespace boost::asio;
using ip::tcp;
using ip::udp;

class Controller {
public:
	void init(int monitorId, int port)
	{
		this->monitorID = monitorId;
		this->controllerPort = port;
	}
	void startControllerSocket()
	{
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), this->controllerPort));
		for (;;)
		{
			socket_ptr sock(new tcp::socket(io_service));
			acceptor.accept(*sock);
			boost::thread workerThread(boost::bind(&Controller::controllerThread, this, sock));
		}
	}
	/**
	udp::socket getStreamerSocket()
	{
		return streamer_socket;
		//streamer_socket.send_to(boost::asio::buffer("message"), );
	}*/

private:
	typedef boost::shared_ptr<tcp::socket> socket_ptr;

protected : 
	int monitorID;
	int controllerPort;
	io_service io_service;
	udp::endpoint remote_endpoint;
	Streamer streamer;

	void controllerThread(socket_ptr sock)
	{
		char data[sizeof(SendStruct)];
		boost::system::error_code error;

		SendStruct* s;
		
		while (true) {
			size_t length = sock->read_some(buffer(data), error);
			if (error == error::eof)
				return; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			s = (SendStruct*)data;

			switch (s->type){
			case MOUSE_MOTION: // MotionNotify
				mouse_motion(s);
				break;

			case MOUSE_BUTTON_DOWN:
				mouse_button_down(s);
				break;
			case MOUSE_BUTTON_UP:
				mouse_button_up(s);
				break;

			case KEY_DOWN:
				key_down(s);
				break;
			case KEY_UP:
				key_up(s);
				break;
			case STREAMER_START:
				cout << "STREAMER_START receive from controller" << endl;
				streamer_start();
				break;
			}

		}
	}

	void mouse_motion(SendStruct* s){}
	void mouse_button_down(SendStruct* s){}
	void mouse_button_up(SendStruct* s){}
	void key_down(SendStruct* s){}
	void key_up(SendStruct* s){}


	void streamer_start()
	{
		streamer.start();
	}






};