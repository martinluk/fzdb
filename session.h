
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class TCPSession
{
public:
	TCPSession(boost::asio::io_service& io_service)
		: socket_(io_service) { }

	tcp::socket& socket();

	void start();

	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

	void handle_write(const boost::system::error_code& error);

	void respond(const std::string response);

private:
	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
	std::string _command;
};