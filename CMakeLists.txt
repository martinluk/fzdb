include(biicode/boost/setup)

ADD_BII_TARGETS()

set(Boost_USE_STATIC_LIBS ON)
bii_find_boost(COMPONENTS system coroutine context thread REQUIRED)
target_include_directories(${BII_BLOCK_TARGET} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${BII_BLOCK_TARGET} INTERFACE ${Boost_LIBRARIES})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
else()
    target_compile_options(${BII_BLOCK_TARGET} INTERFACE -std=c++11)
endif()