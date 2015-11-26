message("Attempting to download GMock")
set(MY_URL "https://github.com/google/googlemock/archive/release-1.7.0.zip")
set(MY_DOWNLOAD_PATH "${CMAKE_BINARY_DIR}/external/gmock.zip")
set(GMOCK_ROOT "${CMAKE_BINARY_DIR}/googlemock-release-1.7.0")

if (NOT EXISTS "${MY_DOWNLOAD_PATH}")
  file(DOWNLOAD "${MY_URL}" "${MY_DOWNLOAD_PATH}" SHOW_PROGRESS)      
else()
message("Google mock appears to already be downloaded")
endif()  

if (NOT EXISTS "${GMOCK_ROOT}")
message("Extracting GMock")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz "${MY_DOWNLOAD_PATH}")
endif()

 add_library(gmock OBJECT "${GMOCK_ROOT}/src/gmock-all.cc")

set_property(TARGET gmock PROPERTY INCLUDE_DIRECTORIES
    "${GMOCK_ROOT}"
    "${GMOCK_ROOT}/include"
    "${GTEST_ROOT}"
    "${GTEST_ROOT}/include")

set(GMOCK_INCLUDE_DIRS "${GTEST_ROOT}/include")
