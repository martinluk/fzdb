#ifndef USER_USEROPERATION_H
#define USER_USEROPERATION_H

#define ADD_ADMIN_ON_INIT true

#include "../util/id_generator.h"

#include "./user_file_operations.h"
#include "./user_exceptions.h"
#include "./user_attributes.h"
#include "./hashing.h"

class ISession;

/**
 * Handles the user operation, by interating with userFileOperation class
 */
class UserOperation {
 public:
  UserOperation();

  /**
   * @brief Checks whether the user has provided with correct user name and password.  If login success, it updates the session object.  If login fail, it throws LoginUnsucessfulException
   * @param session The current session object, assuming is a guest session.
   * @param userName Username that the user provided
   * @param password Password that the user provided.
   *
   * @return Usergroup of the logged in user.
   */
  Permission::UserGroup login(std::shared_ptr<ISession>&& session, const std::string &userName, const std::string &password);

  /**
   * @brief Logout the current session.
   *
   * @param session Session object for the current session.
   */
  void logout(const std::shared_ptr<ISession>&& session) const;
  /**
   * @brief Initialise the class.
   */
  void init();
  /**
   * @brief Add user to the file. Super will throw UserAlreadyExistedException if user already exist
   *
   * @param userName username wished to be added
   * @param password password wished to be added
   * @param userGroup usergroup of the new user
   */
  void addUser(const std::string &userName, const std::string &password, Permission::UserGroup userGroup);

  /**
   * @brief Remove the user from the system. Throws UserAlreadyExistedException if already existed.
   *
   * @param userName username of the user that wished to be removed.
   */
  void removeUser(const std::string &userName);

  /**
   * @brief Update user group of a user. Throws UserNotExistException if not exist.
   *
   * @param userName username of the user.
   * @param newUserGroup The group that the user wished to be changed to.
   */
  void changeUserGroup(const std::string &userName, Permission::UserGroup newUserGroup);

  /**
   * @brief Get user group of a user. Throws UserNotExistException if not exist.
   *
   * @param userName of the user
   *
   * @return UserGroup of the requested user.
   */
  Permission::UserGroup getUserGroup(const std::string &userName) const;

  /**
   * @brief Checks the old password is valid, and change password of the current user.
   * Throws LoginUnsuccessfulException if old password is wrong
   *
   * @param session Session object of the current session
   * @param oldpassword The current password of the user
   * @param newpassword The new password of the user
   */
  void changeUserPassword(const std::shared_ptr<ISession>&& session ,
                          const std::string &oldpassword,
                          const std::string &newpassword);

  /**
   * @brief Directly changes password of the user. Used with reset password job.
   *
   * @param username Username of the user that wished have password reset.
   * @param newpassword Password that used to be reset to.
   */
  void changeUserPassword(const std::string &username,
                          const std::string &newpassword);

  /**
   * @brief Get username of the password.
   *
   * @param id userId.
   *
   * @return  Username of the user.
   */
  std::string getUserName(const unsigned int id) const;

 private:
  /**
   * @brief IdGenerator.
   */
  IdGenerator _idGen;
  /**
   * @brief The class where it opereates on the user file.
   */
  UserFileOperations _fileOperations;
};

#endif    // USER_USEROPERATION_H
