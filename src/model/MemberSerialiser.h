#ifndef MODEL_MEMBERSERIALISER_H
#define MODEL_MEMBERSERIALISER_H

#include <vector>
#include "Serialiser.h"
#include <string>
#include <stdexcept>

// Convenience class for serialising and unserialising member variables
// whose values can simply be set by copying into their memory footprint.
class MemberSerialiser
{
public:
    class InvalidInputMemberException : public std::runtime_error
    {
    public:
        explicit InvalidInputMemberException(const std::string &msg) : std::runtime_error(msg)
        {
        }
    };

    // Interface implemented by dynamic members.
    class IDynamicMember
    {
    public:
        virtual ~IDynamicMember() {}
        
        // Location from which to copy data.
        // This location should be valid from the point of the call
        // to the next point that the member changes.
        virtual const void* memberLocation() const = 0;
        
        // The size of the data to copy from memberLocation(), in bytes.
        virtual std::size_t memberSize() const = 0;
        
        // Given some data and a length, set the member value according to the data.
        virtual void memberUnserialise(const char* serialisedData, std::size_t length) = 0;
    };
    
    // For example:
    class DynamicStringMember : public IDynamicMember
    {
    public:
        DynamicStringMember(std::string& str) : IDynamicMember(), _string(str)
        {
            
        }
        
        virtual const void* memberLocation() const
        {
            return _string.c_str();
        }
        
        virtual std::size_t memberSize() const
        {
            return _string.size() + 1;    // Including null terminator.
        }
        
        virtual void memberUnserialise(const char *serialisedData, std::size_t)
        {
            // The data should be null-terminated, so we can just read in.
            // TODO: (Jonathan) Security implications!!
            _string = serialisedData;
        }
        
    private:
        std::string&    _string;
    };
    
    MemberSerialiser();
    
    // Primitives are things like int, char, float, etc.
    // Their size is known at compile time.
    void addPrimitive(void* location, std::size_t size);
    void clearPrimitives();
    int primitiveMemberCount() const;
    
    // Dynamic members' sizes are not known until runtime.
    void addDynamicMember(IDynamicMember* idm);
    void clearDynamicMembers();
    int dynamicMemberCount() const;
    
    std::size_t serialiseAll(Serialiser &serialiser) const;

    // TODO: Handle the length better here? We don't know the individual lengths of the
    // primitive and dynamic member chunks.
    std::size_t unserialiseAll(const char* serialisedData, std::size_t length);

    void setInitialised();

	bool initialised() const {
		return _initialised;
	}
    
private:
    typedef std::pair<void*, std::size_t> PrimitivePair;

    std::size_t serialisePrimitives(Serialiser &serialiser) const;
    std::size_t serialiseDynamicMembers(Serialiser &serialiser) const;
    std::size_t unserialisePrimitives(const char* serialisedData, std::size_t length);
    std::size_t unserialiseDynamicMembers(const char* serialisedData, std::size_t length);
    std::size_t totalPrimitiveBytes() const;
    std::size_t totalDynamicBytes() const;
    
    std::vector<PrimitivePair> _primitives;
    std::vector<IDynamicMember*> _dynamicMembers;
    bool _initialised;
};

#endif // MODEL_MEMBERSERIALISER_H
