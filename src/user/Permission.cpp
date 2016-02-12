#include "Permission.h"
#include <cassert>

namespace Permission
{
	void assertViewDBPermission(UserGroup group) {
		assertPermission(group, PermissionType::ViewDB);
	}
	
	void assertModifyDBPermission(UserGroup group) {
		assertPermission(group, PermissionType::ModifyDB);
	}
	
	void assertUserOpPermission(UserGroup group) {
		assertPermission(group, PermissionType::UserOp);
	}
	
	bool checkPermission(UserGroup group, PermissionType permType) {
		switch(group) {
			case UserGroup::GUEST:
				return guestPermission(permType);
			case UserGroup::EDITOR:
				return editorPermission(permType);
			case UserGroup::ADMIN:
				return adminPermission(permType);
			default:
				//A new user group was added, but permission is not updated here.
				assert(false);
				return false;
		}
	}
	
	void assertPermission(UserGroup group, PermissionType permType){
		if (!checkPermission(group, permType)) {
			throw (UserPermissionException(group, permType));
		}
	}
	
	bool guestPermission(PermissionType permType) {
		switch(permType) {
			case PermissionType::ViewDB:
				return true;
			case PermissionType::ModifyDB:
				return false;
			case PermissionType::UserOp:
				return false;
			default:
				//A new user group was added, but permission is not updated here.
				assert(false);
				return false;
		}
	}
	
	bool editorPermission(PermissionType permType) {
		switch(permType) {
			case PermissionType::ViewDB:
				return true;
			case PermissionType::ModifyDB:
				return true;
			case PermissionType::UserOp:
				return false;
			default:
				//A new user group was added, but permission is not updated here.
				assert(false);
				return false;
		}
	}
	
	bool adminPermission(PermissionType permType) {
		switch(permType) {
			case PermissionType::ViewDB:
				return true;
			case PermissionType::ModifyDB:
				return false;
			case PermissionType::UserOp:
				return true;
			default:
				//A new user group was added, but permission is not updated here.
				assert(false);
				return false;
		}
	}
}
