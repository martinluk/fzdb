#include "./UserGroup.h"

UserGroup::UserGroup(bool viewDBPermission, bool modifyDbPermission, bool userOpPermission) {
	viewDbPerm = viewDBPermission;
	modifyDbPerm = modifyDbPermission;
	userOpPerm = userOpPermission;
}
