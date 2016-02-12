#include "./singletons.h"
#include "./model/EntityManager.h"
#include <iostream>

namespace Singletons
{
	EntityManager* entityManager_ = NULL;
        std::string dataFilePath_;

	void initialise()
	{
		entityManager_ = new EntityManager();
                if ( dataFilePath_.size() > 0 )
                {
                    if ( !entityManager_->loadFromFile(dataFilePath_))
                    {
                        std::cerr << "Could not load file: " << dataFilePath_ << std::endl;
                    }
                    else
                    {
                        std::cout << "Loaded file " << dataFilePath_ << " successfully." << std::endl;
                    }
                }
	}

	void shutdown()
	{
		delete entityManager_;
	}

	EntityManager* entityManager()
	{
		return entityManager_;
	}

        void setDataFilePath(const std::string &path)
        {
            dataFilePath_ = path;
        }
}
