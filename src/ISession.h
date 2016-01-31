#ifndef FUZZYDB_ISESSION
#define FUZZYDB_ISESSION

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include <user/Permission.h>
class ISession
{
public:
	virtual boost::asio::ip::tcp::socket& socket() = 0;
	virtual void start() = 0;
	virtual void respond(const std::string response) = 0;
    virtual boost::uuids::uuid uuid() = 0;
	virtual ~ISession() {};
	//XXX I don't think this is the prettiest way to do it, as improper coding can circumvent permission checks.
	UserGroup getCurrentUserGroup() { return currentUserGroup;}
	void setCurrentUserGroup(UserGroup grp) {currentUserGroup=grp;}
protected:

	virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred) = 0;
	virtual void handle_write(const boost::system::error_code& error) = 0;

private:
	UserGroup currentUserGroup = UserGroup::GUEST;
};

#endif
