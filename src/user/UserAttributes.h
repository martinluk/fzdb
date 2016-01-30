#pragma once
struct UserAttributes {
	std::string userName;
	std::string passwordHash;
	std::string salt;
	UserGroup userGroup;
};
