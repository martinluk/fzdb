#ifndef MODEL_SERIALISER_H
#define MODEL_SERIALISER_H

#include <utility>
#include <vector>

// Class to assist the process of serialisation.
// SerialProperties are received, which consist of pointers
// to locations in memory and lengths of the sections to be
// serialised.
class Serialiser
{
public:
    // Specifies a property to serialise.
    // Pointer points to the data, size specifies how many bytes.
    typedef std::pair<const void*,std::size_t> SerialProperty;

    // Useful for debugging. Set to something like 0xFF to see
    // where structs have padding inserted.
    static const unsigned char StructPaddingChar;
    static void zeroBuffer(void* dest, std::size_t size);

    Serialiser();

    // Serialise the given list of properties.
        std::size_t serialise(const std::vector<SerialProperty> &properties);
        std::size_t serialise(const SerialProperty &property);

    // Clear all data.
    void clear();

    // Returns a pointer to the beginning of the data.
        char* begin();
        const char* cbegin() const;

        // Returns a pointer to the last valid byte in the vector.
        char* end();
        const char* cend() const;

        // Returns a reinterpret_cast of the given byte of data.
        template <typename T>
        T reinterpretCast(int index)
        {
            return reinterpret_cast<T>(begin() + index);
        }

    // Returns the size of the data.
    std::size_t size() const;

        // Return the number of bytes from the last call to serialise().
        // This is set to 0 on clear.
        std::size_t lastSerialiseBytes() const;

private:
    // Vector that holds the actual serialised data.
    std::vector<char> serialData_;
    std::size_t lastSerialiseBytes_;
};

#endif    // MODEL_SERIALISER_H
