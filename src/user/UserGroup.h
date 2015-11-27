#include <string>

//UserGroup: Object that is superclass of all user permission
//Three actions for each user group:view database, modify database, and user operaetion
//Three groups of users: Guest, normal user (assumed to be academics), and admin
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

