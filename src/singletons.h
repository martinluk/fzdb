#ifndef SINGLETONS_H
#define SINGLETONS_H

#include <string>
#include "model/Database.h"

#include <boost/thread/shared_mutex.hpp>

// Namespace of singletons for use throughout the application.
// Jobs should not use these as they must adhere to the const/non-const
// pointers provided in the Job class.
namespace Singletons
{
    void initialise();
    void shutdown();

    // Sets the path to the file that should be loaded when the database starts.
    void setDataFilePath(const std::string &path);

    // Only call these if you know what you're doing!
    // Normal operations should be performed through Job subclasses.
    // There's probably a better way to do this.
    Database* database();
    const Database* cDatabase();
    boost::shared_mutex& databaseMutex();
}

#endif    // SINGLETONS_H
