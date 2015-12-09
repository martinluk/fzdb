ExternalProject_Add(
    nowide
    URL http://cppcms.com/files/nowide/nowide_standalone.zip
    DOWNLOAD_DIR external
    DOWNLOAD_NAME nowide.zip
    # TIMEOUT 10
    # # Force separate output paths for debug and release builds to allow easy
    # # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
    # CMAKE_ARGS -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
    #            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
    #            -Dgtest_force_shared_crt=ON
    # Disable install step
    INSTALL_COMMAND ""
    # Wrap download, configure and build steps in a script to log output
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

ExternalProject_Get_Property(nowide install_dir)
set(Boost_Nowide_INCLUDES "${install_dir}/src/nowide")

if(MSVC)
	set(Boost_Nowide_LIBRARIES "${install_dir}/src/nowide-build/Debug/libnowide.lib")
elseif(GCC)
	set(Boost_Nowide_LIBRARIES "${install_dir}/src/nowide-build/libnowide.a")
endif()
