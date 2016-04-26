#ifndef MODEL_SERIALISER_H
#define MODEL_SERIALISER_H

#include <utility>
#include <deque>
#include <algorithm>


// Class to assist the process of serialisation.
// SerialProperties are received, which consist of pointers
// to locations in memory and lengths of the sections to be
// serialised.
class Serialiser {
 public:
  typedef std::deque<char> SerialiserData;    
    
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
  SerialiserData::iterator begin();
  SerialiserData::const_iterator cbegin() const;

  // Returns a pointer to the last valid byte in the vector.
  SerialiserData::iterator end();
  SerialiserData::const_iterator cend() const;
  
  // Assumes there is already enough space within the deque to
  // write this value (ie. it won't write off the end).
  // This was a lot simpler when everything was contiguous...
  template<typename T>
  inline void overwrite(std::size_t index, const T* item)
  {
      SerialiserData::iterator it = serialData_.begin() + index;
      for ( std::size_t i = 0; i < sizeof(T); i++ )
      {
          *it = *(reinterpret_cast<const char*>(item)+i);
          ++it;
      }
  }
  
  template<typename T>
  inline T castAsPrimitive(std::size_t index) const
  {
      T ret;
      SerialiserData::const_iterator it = serialData_.cbegin() + index;
      for ( std::size_t i = 0; i < sizeof(T); i++ )
      {
          *(reinterpret_cast<char*>(&ret) + i) = *it;
          ++it;
      }
      
      return ret;
  }

  // Returns the size of the data.
  std::size_t size() const;

  // Return the number of bytes from the last call to serialise().
  // This is set to 0 on clear.
  std::size_t lastSerialiseBytes() const;
  
  void toVector(std::vector<char> &vec) const;

 private:
  // Vector that holds the actual serialised data.
  SerialiserData serialData_;
  std::size_t lastSerialiseBytes_;
};

#endif    // MODEL_SERIALISER_H
