/**
 * Permission
 * This class provides methods that given UserGroup enum, asserts whether usergroup has given permission
 */
class Permission {
	public:
		enum UserGroup { GUEST, EDITOR, ADMIN };
		void assertViewDBPermission(UserGroup group);
		void assertModifyDBPermission(UserGroup group);
		void assertUserOpPermission(UserGroup group);

	private: 
		enum PermissionType {ViewDB, ModifyDB, UserOp};
		bool checkPermission(UserGroup group, PermissionType permType);
		void assertPermission(UserGroup group, PermissionType permType);
		bool guestPermission(PermissionType permType);
		bool editorPermission(PermissionType permType);
		bool adminPermission(PermissionType permType);

};
