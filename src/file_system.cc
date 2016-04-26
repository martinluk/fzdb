#include "./file_system.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <algorithm>

#include <boost/filesystem.hpp>

#define FZDB_VERSION 1
#define CHUNK_BUFFER_SIZE (1024*1024)

namespace FileSystem {

// File header - this should not be returned to anyone calling these functions.
// They just want their own data.
struct FileHeader {
  char identifier[4];     // Identifier - should be "FZDB".
  unsigned short version;   // File version.
  std::size_t size;       // Size of all serialised data after the file headers.
};

void initFileHeader(FileHeader* h, std::size_t size)
{
    h->identifier[0] = 'F';
    h->identifier[1] = 'Z';
    h->identifier[2] = 'D';
    h->identifier[3] = 'B';
    h->version = FZDB_VERSION;
    h->size = size;
}

void writeFile(const std::string &filename, const char *begin, std::size_t size) {
  std::fstream file;

  FileHeader header;
  Serialiser::zeroBuffer(&header, sizeof(FileHeader));
  initFileHeader(&header, size);

  try {
    file.open(filename.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    file.write(reinterpret_cast<const char*>(&header), sizeof(FileHeader));
    file.write(begin, size);
  } catch (const std::exception &ex) {
    file.close();
    throw ex;
  }

  file.close();
}

void writeFile(const std::string &filename, const Serialiser &serialiser) {
    std::fstream file;
    std::size_t size = serialiser.size();
    
    FileHeader header;
    Serialiser::zeroBuffer(&header, sizeof(FileHeader));
    initFileHeader(&header, size);
  
    try {
      file.open(filename.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
      file.write(reinterpret_cast<const char*>(&header), sizeof(FileHeader));
      
      // Now that the serialiser is no longer in contiguous memory,
      // we need to chunk things.
      char tempBuffer[CHUNK_BUFFER_SIZE];
      Serialiser::SerialiserData::const_iterator start = serialiser.cbegin();
      for ( std::size_t i = 0; i < size; i += CHUNK_BUFFER_SIZE )
      {
          std::size_t bytesThisIteration = std::min((std::size_t)CHUNK_BUFFER_SIZE, size-i);
          
          Serialiser::SerialiserData::const_iterator it = start + i;
          std::copy(it, it+bytesThisIteration, tempBuffer);
          file.write(tempBuffer, bytesThisIteration);
      }
      
    } catch (const std::exception &ex) {
      file.close();
      throw ex;
    }
  
    file.close();
}

std::string workingDirectory() {
  return boost::filesystem::current_path().generic_string();
}

void readFile(const std::string &filename, char *buffer, std::size_t length) {
  std::fstream file;

  try {
    file.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    file.seekg(0, file.end);
    std::streamoff flen = file.tellg();
    file.seekg(0, file.beg);

    if ( flen <= sizeof(FileHeader) ) {
      file.close();
      throw std::length_error("File too small to contain valid data");
    }

    // Hopefully reading automatically advances the read point in the file?
    FileHeader header;
    Serialiser::zeroBuffer(&header, sizeof(FileHeader));
    file.read(reinterpret_cast<char*>(&header), sizeof(FileHeader));

    if ( header.identifier[0] != 'F' ||     // Check identifier.
         header.identifier[1] != 'Z' ||
         header.identifier[2] != 'D' ||
         header.identifier[3] != 'B' ||
         header.version != FZDB_VERSION) {  // Check it's the version we're expecting.
      throw FileFormatError();
    }

    if ( flen - sizeof(FileHeader) != header.size ) {
      throw std::length_error("Specified internal data size does not equal physical size of file.");
    }

    if ( length < header.size ) {
      throw std::length_error("Buffer provided was too small to store the file data.");
    }

    file.read(buffer, header.size);
  } catch (const std::exception &ex) {
    file.close();
    throw ex;
  }

  file.close();
}

void readFile(const std::string &filename, std::vector<char> &buffer) {
  std::size_t length = dataLength(filename);
  if ( length < 1 )
    throw std::length_error("File contained no data");

  buffer.resize(length);
  readFile(filename, buffer.data(), buffer.size());
}

std::streamoff fileLength(const std::string &filename) {
  std::fstream file;
  std::streamoff length = 0;

  try {
    file.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    file.seekg(0, file.end);
    length = file.tellg();
  } catch (const std::exception &) {

  }

  file.close();
  return length;
}

std::size_t dataLength(const std::string &filename) {
  std::fstream file;
  std::size_t length = 0;

  try {
    file.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);

    FileHeader header;
    Serialiser::zeroBuffer(&header, sizeof(FileHeader));
    file.read(reinterpret_cast<char*>(&header), sizeof(FileHeader));
    length = header.size;
  } catch (const std::exception &) {

  }

  file.close();
  return length;
}
}
