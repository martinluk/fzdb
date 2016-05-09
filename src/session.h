#ifndef FUZZYDB_SESSION
#define FUZZYDB_SESSION

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include <vector>

#include "./isession.h"

#define CHUNK_LENGTH 2097152
#define OUT_CHUNK_LENGTH 1024

using boost::asio::ip::tcp;

class TCPServer;

// The session class is tied to a user for a particular command.
// It handles sending and receiving actual data from the user.
class TCPSession : public ISession, public std::enable_shared_from_this<TCPSession> {
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
  void readChunk();
  void copyDataToVector(std::size_t count);

  tcp::socket _socket;
  char _data[CHUNK_LENGTH];
  TCPServer*    _parent;
  boost::uuids::uuid _uuid;

  std::vector<char> _dataVec;
};

#endif
