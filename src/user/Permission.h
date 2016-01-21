#include <stdexcept>
/**
 * Permission
 * This class provides methods that given UserGroup enum, asserts whether usergroup has given permission
 */
enum UserGroup { GUEST, EDITOR, ADMIN };
enum PermissionType {ViewDB, ModifyDB, UserOp};
class Permission {
	public:
		void assertViewDBPermission(UserGroup group);
		void assertModifyDBPermission(UserGroup group);
		void assertUserOpPermission(UserGroup group);

	private: 
		bool checkPermission(UserGroup group, PermissionType permType);
		void assertPermission(UserGroup group, PermissionType permType);
		//TODO Can be refactored using a struct
		bool guestPermission(PermissionType permType);
		bool editorPermission(PermissionType permType);
		bool adminPermission(PermissionType permType);
};

using std::runtime_error;

class UserPermissionException : public runtime_error {
	public:
		   UserPermissionException(UserGroup group, PermissionType permType): runtime_error( "Permission exception" ) {}
};
