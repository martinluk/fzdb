#ifndef FUZZYDB_SESSION
#define FUZZYDB_SESSION

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include "./ISession.h"


using boost::asio::ip::tcp;

class TCPServer;

// TODO: How to we handle when the connection to this session is ended?
// We'll want to do this in order to inform the TCPServer that we are no
// longer live and can be cleaned up.
class TCPSession : public ISession, public std::enable_shared_from_this<TCPSession>
{
public:

	TCPSession(boost::asio::io_service& io_service, TCPServer* parent, boost::uuids::uuid identifier);
	~TCPSession();

	tcp::socket& socket();
	void start();
	void respond(const std::string response);

	boost::uuids::uuid uuid();

private:

	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error);	

	tcp::socket _socket;
	enum { max_length = 1024 };
	char _data[max_length];
	TCPServer*	_parent;
	boost::uuids::uuid _uuid;
};

#endif
