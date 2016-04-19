project(fuzzy-database)
cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)

enable_testing()

INCLUDE(ExternalProject)

if(UNIX)
  set (CMAKE_CXX_FLAGS "-std=c++11")
  #set (CMAKE_VERBOSE_MAKEFILE true )
endif()

#Default is DEBUG build
IF(NOT CMAKE_BUILD_TYPE MATCHES "RELEASE")
  message("Assuming debug build, use -DCMAKE_BUILD_TYPE=RELEASE for release build")
  SET(CMAKE_BUILD_TYPE "DEBUG")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  message("Starting DEBUG configuration..")
elseif(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  message("Starting RELEASE configuration..")
else()
  message( FATAL_ERROR "CMAKE_BUILD_TYPE must be DEBUG or RELEASE - it's currently ${CMAKE_BUILD_TYPE}" )
endif()


message("Using generator ${CMAKE_GENERATOR}")

include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/pthreads.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/boost-nowide.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/gtest.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/gmock.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/spdlog.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/rapidjson.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/build_modules/vedis.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${Boost_Nowide_INCLUDES}) 
include_directories(${Boost_INCLUDE_DIRS}) 
include_directories(${GTEST_INCLUDE_DIRS}) 
include_directories(${SPDLOG_INCLUDE_DIRS}) 
include_directories(${RAPIDJSON_INCLUDE_DIRS}) 
include_directories(${VEDIS_INCLUDE_DIRS}) 

message("DEBUG: ${RAPIDJSON_INCLUDE_DIRS}")

FILE(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
FILE(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

#Admin account set up
#TODO(Martin Luk) Refactor Admin setting into some kind of config file?
if(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
	message("Release Build - Database user json file should be added to correct dirrectory before building project.")
else()
	message("Debug build - Assuming no user json file ")
	add_definitions("-DINIT_ADD_ADMIN")

	SET(ADMIN_USERNAME "fzydb_admin")
	add_definitions("-DADMIN_USERNAME=\"${ADMIN_USERNAME}\"")
	message("Admin username set")

	SET(ADMIN_PASSWORD "password")
	add_definitions("-DADMIN_PASSWORD=\"${ADMIN_PASSWORD}\"")
	message("Admin password set as default")

endif()

# disable warnings about C++ standard library
if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -D_WIN32_WINNT=0x060001")
endif()

macro(GroupSources curdir)
  file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
  foreach(child ${children})
    if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
      GroupSources(${curdir}/${child})
    else()
      string(REPLACE "/" "\\" groupname ${curdir})

      if(${child} MATCHES .*\\.h$)
        string(REPLACE "src" "Headers" groupname ${groupname})
        source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
      else()
        if(${child} MATCHES .*\\.cc$)
          string(REPLACE "src" "Source Files" groupname ${groupname})
          source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        endif()
      endif()        
    endif()
  endforeach()
endmacro()

GroupSources(src)

SET(SOURCES_NO_MAIN ${SOURCES})
LIST(REMOVE_ITEM SOURCES_NO_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc")

add_library(fuzzy-database-lib OBJECT ${SOURCES_NO_MAIN} ${HEADERS})

add_executable(fuzzy-database ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc $<TARGET_OBJECTS:fuzzy-database-lib> $<TARGET_OBJECTS:vedis-lib>)

add_dependencies(fuzzy-database nowide)
add_dependencies(fuzzy-database spdlog)
add_dependencies(fuzzy-database rapidjson)
add_dependencies(fuzzy-database vedis-lib)

target_link_libraries(fuzzy-database ${Boost_LIBRARIES})
target_link_libraries(fuzzy-database ${Boost_Nowide_LIBRARIES})
target_link_libraries(fuzzy-database ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(fuzzy-database ${CMAKE_THREAD_LIBS_INIT})

if(UNIX AND NOT APPLE)
   target_link_libraries(fuzzy-database -lrt) 
endif()

if(WIN32)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/service)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
