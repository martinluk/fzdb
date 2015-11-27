#include "./UserGroup.h"

UserGroup::UserGroup(bool viewDBPermission, bool modifyDbPermission, bool userOpPermission) {
	viewDbPerm = viewDBPermission;
	modifyDbPerm = modifyDbPermission;
	userOpPerm = userOpPermission;
}

bool UserGroup::hasViewDBPerm() {
	return viewDbPerm;
}

bool UserGroup::hasModifyDBPerm() {
	return modifyDbPerm;
}

bool UserGroup::hasUserOpPerm() {
	return userOpPerm;
}
