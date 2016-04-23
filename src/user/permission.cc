#include "./permission.h"
#include <cassert>

/**
 * @brief Implements the access control matrix.
 */
namespace Permission
{
    /**
     * @brief Asserts whether the given group has view database permission.
     *
     * @param group User group.
     */
    void assertViewDBPermission(UserGroup group) {
        assertPermission(group, PermissionType::ViewDB);
    }

    /**
     * @brief Asserts whether the given group has modify database permission.
     *
     * @param group User group.
     */
    
    void assertModifyDBPermission(UserGroup group) {
        assertPermission(group, PermissionType::ModifyDB);
    }
    
    /**
     * @brief Asserts whether the given group has modify user file permission.
     *
     * @param group User group.
     */
    
    void assertUserOpPermission(UserGroup group) {
        assertPermission(group, PermissionType::UserOp);
    }

    /**
     * @brief Returns the std::string of the user group
     *
     * @param group User group.
     *
     * @return Name of the user group.
     */
    std::string userGroupName(UserGroup group) {
        switch(group) {
            case UserGroup::GUEST:
                return "GUEST";
            case UserGroup::EDITOR:
                return "EDITOR";
            case UserGroup::ADMIN:
                return "ADMIN";
            default:
                //A new user group was added, but permission is not updated here.
                assert(false);
                return "";
        }
    }
    

    /**
     * @brief Checks whether the given user group has the right permission for the operation type
     *
     * @param group User group.
     * @param permType Operation type.
     *
     * @return True if has operation, false otherwise.
     */
    bool checkPermission(UserGroup group, PermissionType permType) {
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
                return false;
        }
    }
    
    /**
     * @brief Throws UserPermissionException if given user group does not have enough permission for the operation.
     *
     * @param group User group.
     * @param permType The Operation type.
     */
    void assertPermission(UserGroup group, PermissionType permType){
        if (!checkPermission(group, permType)) {
            throw (UserPermissionException(group, permType));
        }
    }
    

    /**
     * @brief Evaluates whether guest has the permission to execute the given operation.
     *
     * @param permType Operation
     *
     * @return True if it has, false otherwise.
     */
    bool guestPermission(PermissionType permType) {
        switch(permType) {
            case PermissionType::ViewDB:
                return true;
            case PermissionType::ModifyDB:
                return false;
            case PermissionType::UserOp:
                return false;
            case PermissionType::LoggedInUser:
                return false;
            default:
                //A new user group was added, but permission is not updated here.
                assert(false);
                return false;
        }
    }
    /**
     * @brief Evaluates whether editor has the permission to execute the given operation.
     *
     * @param permType Operation
     *
     * @return True if it has, false otherwise.
     */
    
    bool editorPermission(PermissionType permType) {
        switch(permType) {
            case PermissionType::ViewDB:
                return true;
            case PermissionType::ModifyDB:
                return true;
            case PermissionType::UserOp:
                return false;
            case PermissionType::LoggedInUser:
                return true;
            default:
                //A new user group was added, but permission is not updated here.
                assert(false);
                return false;
        }
    }

    /**
     * @brief Evaluates whether admin has the permission to execute the given operation.
     *
     * @param permType Operation
     *
     * @return True if it has, false otherwise.
     */
    
    bool adminPermission(PermissionType permType) {
        switch(permType) {
            case PermissionType::ViewDB:
                return true;
            case PermissionType::ModifyDB:
                return true;
            case PermissionType::UserOp:
                return true;
            case PermissionType::LoggedInUser:
                return true;
            default:
                //A new user group was added, but permission is not updated here.
                assert(false);
                return false;
        }
    }
}
