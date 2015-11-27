#include <string>

//UserGroup: Object that is superclass of all user permission
//Three actions for each user group:view database, modify database, and user operaetion
//Three groups of users: Guest, editor user (assumed to be academics), and admin
//

class UserGroup {
	public: 
		//Instantiation 
		UserGroup(bool viewDBPermission, bool modifyDbPermission, bool userOpPermission);
		bool hasViewDBPerm();
		bool hasModifyDBPerm();
		bool hasUserOpPerm();
	protected:
		//Boolean that defines the three types of permission concern with the system
		bool viewDbPerm;
		bool modifyDbPerm;
		bool userOpPerm;
};

class GuestGroup: public UserGroup {
	//Guest can only view the DB
	public: 
		GuestGroup() : UserGroup(true, false, false){};
};

class EditorGroup: public UserGroup {
	//Editor can edit and view DB
	public: 
		EditorGroup() : UserGroup(true, true, false){};
};

class UserAdminGroup: public UserGroup {
	//User admin can only do user operations, without ability to view and edit DB
	public: 
		UserAdminGroup() : UserGroup(false, false, true){};
};


