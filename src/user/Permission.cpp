#include "./Permission.h"
#include <cassert>

void Permission::assertViewDBPermission(UserGroup group) {
	assertPermission(group, ViewDB);
}

void Permission::assertModifyDBPermission(UserGroup group) {
	assertPermission(group, ModifyDB);
}

void Permission::assertUserOpPermission(UserGroup group) {
	assertPermission(group, UserOp);
}

bool Permission::checkPermission(UserGroup group, PermissionType permType) {
	switch(group) {
		case GUEST:
			return guestPermission(permType);
		case EDITOR:
			return editorPermission(permType);
		case ADMIN:
			return adminPermission(permType);
		default:
			//A new user group was added, but permission is not updated here.
			assert(false);
	}
}

void Permission::assertPermission(UserGroup group, PermissionType permType){
	if (!checkPermission(group, permType)) {
		throw (UserPermissionException(group, permType));
	}
}

bool Permission::guestPermission(PermissionType permType) {
	switch(permType) {
		case ViewDB:
			return true;
		case ModifyDB:
			return false;
		case UserOp:
			return false;
		default:
			//A new user group was added, but permission is not updated here.
			assert(false);
	}
}

bool Permission::editorPermission(PermissionType permType) {
	switch(permType) {
		case ViewDB:
			return true;
		case ModifyDB:
			return true;
		case UserOp:
			return false;
		default:
			//A new user group was added, but permission is not updated here.
			assert(false);
	}
}

bool Permission::adminPermission(PermissionType permType) {
	switch(permType) {
		case ViewDB:
			return true;
		case ModifyDB:
			return false;
		case UserOp:
			return true;
		default:
			//A new user group was added, but permission is not updated here.
			assert(false);
	}
}

