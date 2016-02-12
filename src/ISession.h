#ifndef FUZZYDB_ISESSION
#define FUZZYDB_ISESSION

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include "user/Permission.h"
#include "user/UserOperation.h"
#include "singletons.h"

class ISession
{
public:
	virtual boost::asio::ip::tcp::socket& socket() = 0;
	virtual void start() = 0;
	virtual void respond(const std::string response) = 0;
    virtual boost::uuids::uuid uuid() = 0;
	virtual ~ISession() {}

	//methods for maintaining user state
	void setCurrentUserName(const std::string &username) { 
		_username=username;
	}
	void clearCurrentUserName() { _username.clear(); }

	Permission::UserGroup getCurrentUserUserGroup() const
	{
		return Singletons::cDatabase()->users().getUserGroup(_username);
	}

protected:
	virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred) = 0;
	virtual void handle_write(const boost::system::error_code& error) = 0;

private:
	std::string _username = ""; //Empty string as guest
};

#endif
