set(GTEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src)

ExternalProject_Add(googletest
    URL https://github.com/google/googletest/archive/master.zip
# Disable install step
    INSTALL_COMMAND ""
    CONFIGURE_COMMAND cmake "${GTEST_DIR}/googletest/googletest"
    BUILD_COMMAND cmake --build "${GTEST_DIR}/googletest-build"
)

set(GTEST_INCLUDE_DIRS "${GTEST_DIR}/googletest/googletest/include")

if(WIN32)
    if(CMAKE_BUILD_TYPE MATCHES "DEBUG")
        set(GTEST_LIBRARIES "${GTEST_DIR}/googletest-build/Debug/gtest.lib" "${GTEST_DIR}/googletest-build/Debug/gtest_main.lib")
    elseif(CMAKE_BUILD_TYPE MATCHES "RELEASE")
        set(GTEST_LIBRARIES "${GTEST_DIR}/googletest-build/Release/gtest.lib" "${GTEST_DIR}/googletest-build/Release/gtest_main.lib")
    endif()
elseif(UNIX)
    set(GTEST_LIBRARIES "${GTEST_DIR}/googletest-build/gtest.a" "${GTEST_DIR}/googletest-build/gtest_main.a")
endif()