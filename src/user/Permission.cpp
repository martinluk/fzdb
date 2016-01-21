#include <user/Permission.h>
#include <cassert>


void Permission::assertViewDBPermission(UserGroup group) {
	assertPermission(group, PermissionType::ViewDB);
}

void Permission::assertModifyDBPermission(UserGroup group) {
	assertPermission(group, PermissionType::ModifyDB);
}

void Permission::assertUserOpPermission(UserGroup group) {
	assertPermission(group, PermissionType::UserOp);
}

bool Permission::checkPermission(UserGroup group, PermissionType permType) {
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
	}
}

void Permission::assertPermission(UserGroup group, PermissionType permType){
	if (!checkPermission(group, permType)) {
		throw (UserPermissionException(group, permType));
	}
}

bool Permission::guestPermission(PermissionType permType) {
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
	}
}

bool Permission::editorPermission(PermissionType permType) {
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
	}
}

bool Permission::adminPermission(PermissionType permType) {
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
	}
}

