#include "./singletons.h"
#include "model/Database.h"
#include <iostream>

namespace Singletons
{
	Database* _database = NULL;
	std::string _dataFilePath;

	void initialise()
	{
		_database = new Database();
                if ( _dataFilePath.size() > 0 )
                {
                    if ( !_database->entityManager().loadFromFile(_dataFilePath))
                    {
                        std::cerr << "Could not load file: " << _dataFilePath << std::endl;
                    }
                    else
                    {
                        std::cout << "Loaded file " << _dataFilePath << " successfully." << std::endl;
                    }
                }
	}

	void shutdown()
	{
		delete _database;
	}

	Database* database()
	{
		return _database;
	}

        void setDataFilePath(const std::string &path)
        {
            _dataFilePath = path;
        }
}
