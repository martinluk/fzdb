## Finds and/or configures boost

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  

find_package(Boost 1.59.0 COMPONENTS thread date_time filesystem system) 

if(Boost_FOUND)   
  message("Found a Boost system install!") 
else()
  message("Attempting to download Boost")
  set(MY_URL "http://www.dcs.warwick.ac.uk/~csulak/boost_1_59_0.zip")
  set(MY_DOWNLOAD_PATH "./external/boost_1_59_0.zip")
  set(BOOST_ROOT "${CMAKE_BINARY_DIR}/boost_1_59_0")

  if (NOT EXISTS "${MY_DOWNLOAD_PATH}")
      file(DOWNLOAD "${MY_URL}" "${MY_DOWNLOAD_PATH}" SHOW_PROGRESS)      
  else()
    message("Boost appears to already be downloaded")
  endif()  

  if (NOT EXISTS "${BOOST_ROOT}")
    message("Extracting Boost")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz "./external/boost_1_59_0.zip")
  endif()

  if (NOT EXISTS "${BOOST_ROOT}/b2.exe")
    message("Boost.Build is not built")

    if(UNIX) 
      message("Building Boost.Build with BASH")
      execute_process(COMMAND ./bootstrap.sh WORKING_DIRECTORY "${BOOST_ROOT}")
    endif()

    if(WIN32)
      message("Building Boost.Build with BAT")
      execute_process(COMMAND bootstrap.bat WORKING_DIRECTORY "${BOOST_ROOT}") 
      message("Boost.Build built")  
    endif()
  else()
    message("Boost.Build is already built")
  endif()

  if(UNIX)
    set(Boost_BUILD_TOOLSET gcc)
  elseif(MSVC14)
    set(Boost_BUILD_TOOLSET msvc-14.0)
  elseif(MSVC12)
    set(Boost_BUILD_TOOLSET msvc-12.0)
  else()
    message(FATAL_ERROR "Configuration for your target does not exist - on windows please use msvc12 or msvc14 compilers!")
  endif()
  execute_process(COMMAND ./b2 -j4 toolset=${Boost_BUILD_TOOLSET} --with-system --with-thread --with-date_time --with-filesystem --with-regex WORKING_DIRECTORY ${BOOST_ROOT})

  set(Boost_INCLUDE_DIRS ${BOOST_ROOT})
  set(Boost_LIBRARY_DIRS "${BOOST_ROOT}/stage/lib")
  if(UNIX)
     #the ordering is important in gcc4.8 (used by travis)
     set(Boost_LIBRARIES "${Boost_LIBRARY_DIRS}/libboost_thread.a"
                         "${Boost_LIBRARY_DIRS}/libboost_system.a"
                         "${Boost_LIBRARY_DIRS}/libboost_date_time.a"
                         "${Boost_LIBRARY_DIRS}/libboost_filesystem.a"
                         "${Boost_LIBRARY_DIRS}/libboost_regex.a")
  elseif(WIN32)
     #msvc is clever enough to work out the ordering itself, and has more annoying filenames
     if(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        file(GLOB Boost_LIBRARIES "${Boost_LIBRARY_DIRS}/*-mt-gd-1_59.lib")
     elseif(${CMAKE_BUILD_TYPE} STREQUAL "RELEASE")
        file(GLOB Boost_LIBRARIES "${Boost_LIBRARY_DIRS}/*-mt-1_59.lib")
     endif()
  endif()
endif()
