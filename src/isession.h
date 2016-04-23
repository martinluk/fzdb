#ifndef FUZZYDB_ISESSION
#define FUZZYDB_ISESSION

#include "./user/permission.h"
#include "./user/user_operation.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

// Implements base session functionality.
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
    Permission::UserGroup const getUserGroup() {
        return _usergroup;
    }

protected:
    virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred) = 0;
    virtual void handle_write(const boost::system::error_code& error) = 0;

private:
    Permission::UserGroup _usergroup = Permission::UserGroup::GUEST;
    std::string _username = ""; //Empty string as guest
    unsigned int _userId = 0;

    void userId(unsigned int id) {
        _userId = id;
    }

    //methods for maintaining user state
    void setCurrentUserName(const std::string &username) {
        _username = username;
    }
    void setUserGroup(Permission::UserGroup ug) {_usergroup=ug;}
    void clearCurrentUserName() { _username.clear(); }
};

#endif
