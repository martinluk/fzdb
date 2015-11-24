message("Attempting to download GTest")
set(MY_URL "https://github.com/google/googletest/archive/release-1.7.0.zip")
set(MY_DOWNLOAD_PATH "${CMAKE_BINARY_DIR}/external/gtest.zip")
set(GTEST_ROOT "${CMAKE_BINARY_DIR}/googletest-release-1.7.0")

if (NOT EXISTS "${MY_DOWNLOAD_PATH}")
  file(DOWNLOAD "${MY_URL}" "${MY_DOWNLOAD_PATH}" SHOW_PROGRESS)      
else()
message("Google test appears to already be downloaded")
endif()  

if (NOT EXISTS "${GTEST_ROOT}")
message("Extracting GTest")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz "${MY_DOWNLOAD_PATH}")
endif()

add_library(gtest OBJECT "${GTEST_ROOT}/src/gtest-all.cc")

set_property(TARGET gtest PROPERTY INCLUDE_DIRECTORIES
    "${GTEST_ROOT}"
    "${GTEST_ROOT}/include")

set(GTEST_INCLUDE_DIRS "${GTEST_ROOT}/include")
