#include "./UserGroups.h"
#include "./Permission.h"
#include <cassert>

void Permission::assertViewDBPermission(UserGroup group) {
	assert(checkPermission(group, ViewDB));
}

void Permission::assertModifyDBPermission(UserGroup group) {
	assert(checkPermission(group, ModifyDB));
}

void Permission::assertUserOpPermission(UserGroup group) {
	assert(checkPermission(group, UserOp));
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
	assert(checkPermission(group, permType));
}

bool Permission::guestPermission(PermissionType permType) {
	switch(permType) {
		case ViewDB:
			return true;
		case ModifyDB:
			return false;
		case UserOp:
			return false;
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
	}
}

