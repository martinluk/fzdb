#ifndef FUZZYDB_ISESSION
#define FUZZYDB_ISESSION

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

class ISession
{
    friend class UserOperation;

public:
    virtual boost::asio::ip::tcp::socket& socket() = 0;
    virtual void start() = 0;
    virtual void respond(const std::string response) = 0;
    virtual boost::uuids::uuid uuid() = 0;
    virtual ~ISession() {}    
    std::string username() const { return _username; }
    unsigned int userId() const { return _userId; }

    // JONATHAN: Removing this - database access should now only be through the Job class' member pointer.
//    Permission::UserGroup getCurrentUserUserGroup() const
//    {
//        return Singletons::cDatabase()->users().getUserGroup(_username);
//    }

protected:
    virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred) = 0;
    virtual void handle_write(const boost::system::error_code& error) = 0;

private:
    std::string _username = ""; //Empty string as guest
    unsigned int _userId = 0;

    void userId(unsigned int id) {
        _userId = id;
    }

    //methods for maintaining user state
    void setCurrentUserName(const std::string &username) {
        _username = username;
    }
    void clearCurrentUserName() { _username.clear(); }
};

#endif
