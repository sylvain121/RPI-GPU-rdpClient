#pragma one

using namespace std;
using namespace boost::asio;
using ip::udp;

class Streamer {

private:

	void start_receive()
	{
		socket_.async_receive_from(
			boost::asio::buffer(recv_buffer_), remote_endpoint_,
			boost::bind(&Streamer::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive(const boost::system::error_code& error,
		std::size_t /*bytes_transferred*/)
	{
		// nothing to do, we will never receive data only sender
	}

	void handle_send(boost::shared_ptr<std::string> /*message*/,
		const boost::system::error_code& /*error*/,
		std::size_t /*bytes_transferred*/)
	{
	}



	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	char recv_buffer_[1];
	boost::asio::io_service io_service;

protected:

public:
	Streamer()
		: socket_(io_service, udp::endpoint(udp::v4(), 13))
	{

	}

	void start() {
		start_receive();
	}

	void send(){
			boost::shared_ptr<std::string> message(
				new std::string("DATA!!!!!!!!!!!!!!!!!!"));

			socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
				boost::bind(&Streamer::handle_send,
				this,
				message,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}


};