#ifndef SINGLETONS_H
#define SINGLETONS_H

#include <string>
#include "model/Database.h"

namespace Singletons
{
	void initialise();
	void shutdown();
    void setDataFilePath(const std::string &path);
	
	Database* database();
	const Database* cDatabase();
}

#endif	// SINGLETONS_H
