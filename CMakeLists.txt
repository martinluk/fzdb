include(biicode/boost/setup)

if(UNIX)
	FIND_PACKAGE ( Threads REQUIRED )
endif()

ADD_BII_TARGETS()

if(UNIX)
	TARGET_LINK_LIBRARIES ( ${BII_BLOCK_TARGET} INTERFACE ${CMAKE_THREAD_LIBS_INIT} )
endif()

set(Boost_USE_STATIC_LIBS ON)

bii_find_boost(COMPONENTS thread date_time filesystem system REQUIRED)
target_include_directories(${BII_BLOCK_TARGET} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${BII_BLOCK_TARGET} INTERFACE ${Boost_LIBRARIES})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
else()
    target_compile_options(${BII_BLOCK_TARGET} INTERFACE -std=c++11)
endif()