#ifndef USER_PERMISSION_H
#define USER_PERMISSION_H

#include <stdexcept>
/**
 * Permission
 * This class provides methods that given UserGroup enum, asserts whether usergroup has given permission
 */
namespace Permission
{
    enum class UserGroup
    {
        GUEST,
        EDITOR,
        ADMIN
    };
    
    enum class PermissionType {
        ViewDB,
        LoggedInUser,
        ModifyDB,
        UserOp
    };
    void assertViewDBPermission(UserGroup group);
    void assertModifyDBPermission(UserGroup group);
    void assertUserOpPermission(UserGroup group);

    bool checkPermission(UserGroup group, PermissionType permType);
    void assertPermission(UserGroup group, PermissionType permType);
    
    //TODO Can be refactored using a struct
    bool guestPermission(PermissionType permType);
    bool editorPermission(PermissionType permType);
    bool adminPermission(PermissionType permType);
    bool loggedinUserPermission(PermissionType permType);
    
    class UserPermissionException : public std::runtime_error
    {
    public:
        UserPermissionException(UserGroup group, PermissionType permType) : std::runtime_error( "Permission exception" )
        {
        }
    };
}

#endif
