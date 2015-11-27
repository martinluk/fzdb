#include "./UserGroup.h"

User::User(bool viewDBPermission, bool modifyDbPermission, bool userOpPermission) {
	permMatrix.viewDbPerm = viewDBPermission;
	permMatrix.modifyDbPerm = modifyDbPermission;
	permMatrix.userOpPerm = userOpPermission;
}

bool User::hasViewDBPerm() {
	return permMatrix.viewDbPerm;
}

bool User::hasModifyDBPerm() {
	return permMatrix.modifyDbPerm;
}

bool User::hasUserOpPerm() {
	return permMatrix.userOpPerm;
}

