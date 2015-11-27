#include <string>

//User: Object that is superclass of all user permission
//Three actions for each user group:view database, modify database, and user operaetion
//Three groups of users: Guest, editor user (assumed to be academics), and admin

struct PermissionMatrix {
		bool viewDbPerm;
		bool modifyDbPerm;
		bool userOpPerm;
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
		LoggedOnUser(bool viewDBPermission, bool modifyDbPermission, bool userOpPermission, std::string userName, std::string password) :
			User(viewDBPermission, modifyDbPermission, userOpPermission)	{
				//TODO If verification fails then throw an exception
				//TODO Hash and salted password
	};
	protected:
		int userID;
		std::string hashed_salted_password;

	private: 
		void verifyLogin(std::string userName, std::string password);
};

class Editor: public LoggedOnUser{
	//Editor can edit and view DB
	public: 
		Editor(std::string userName, std::string password) : LoggedOnUser(true, true, false, userName, password){};
};

class UserAdmin: public LoggedOnUser{
	//User admin can only do user operations, without ability to view and edit DB
	public: 
		UserAdmin(std::string userName, std::string password) : LoggedOnUser(false, false, true,userName, password){};
};


