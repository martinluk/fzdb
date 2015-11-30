#include <string>

//User: Object that is superclass of all user permission
//Three actions for each user group:view database, modify database, and user operaetion
//Three groups of users: Guest, editor user (assumed to be academics), and admin

struct PermissionMatrix {
		bool viewDbPerm;
		bool modifyDbPerm;
		bool userOpPerm;
};

struct HashedCredentials {
	int uid;
	std::string hashed_salted_password;
};

class User{
	public: 
		//Instantiation with permission
		User(bool viewDBPermission, bool modifyDbPermission, bool userOpPermission);
		bool hasViewDBPerm();
		bool hasModifyDBPerm();
		bool hasUserOpPerm();
	protected:
		//Boolean that defines the three types of permission concern with the system
		PermissionMatrix permMatrix;
};

class Guest: public User{
	//Guest can only view the DB
	public: 
		Guest() : User(true, false, false){};
};

class LoggedOnUser: public User{
	//User with a username and password
	public:
		LoggedOnUser(bool viewDBPermission, bool modifyDbPermission, bool userOpPermission, HashedCredentials cred) :
			User(viewDBPermission, modifyDbPermission, userOpPermission)	{
	};
	protected:
		HashedCredentials credentials;
		void verifyLogin();
};

class Editor: public LoggedOnUser{
	//Editor can edit and view DB
	public: 
		Editor(HashedCredentials cred) : LoggedOnUser(true, true, false, cred){};
};

class UserAdmin: public LoggedOnUser{
	//User admin can only do user operations, without ability to view and edit DB
	public: 
		UserAdmin(HashedCredentials cred) : LoggedOnUser(false, false, true, cred){};
};


