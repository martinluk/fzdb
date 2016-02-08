#pragma once
#include <stdexcept>
/**
 * Permission
 * This class provides methods that given UserGroup enum, asserts whether usergroup has given permission
 */
enum class UserGroup { GUEST, EDITOR, ADMIN };
enum class PermissionType {ViewDB, ModifyDB, UserOp};
class Permission {
	public:
		static void assertViewDBPermission(UserGroup group);
		static void assertModifyDBPermission(UserGroup group);
		static void assertUserOpPermission(UserGroup group);

	private: 
		static bool checkPermission(UserGroup group, PermissionType permType);
		static void assertPermission(UserGroup group, PermissionType permType);
		//TODO Can be refactored using a struct
		static bool guestPermission(PermissionType permType);
		static bool editorPermission(PermissionType permType);
		static bool adminPermission(PermissionType permType);
};

using std::runtime_error;

class UserPermissionException : public runtime_error {
	public:
		   UserPermissionException(UserGroup group, PermissionType permType): runtime_error( "Permission exception" ) {}
};
