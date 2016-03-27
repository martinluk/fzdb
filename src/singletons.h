#ifndef SINGLETONS_H
#define SINGLETONS_H

#include <string>
#include "model/Database.h"

#include <boost/thread/shared_mutex.hpp>

namespace Singletons
{
    void initialise();
    void shutdown();
    void setDataFilePath(const std::string &path);

    // Only call these if you know what you're doing!
    // Normal operations should be performed through Job subclasses.
    // There's probably a better way to do this.
    Database* database();
    const Database* cDatabase();
    boost::shared_mutex& databaseMutex();
}

#endif    // SINGLETONS_H
