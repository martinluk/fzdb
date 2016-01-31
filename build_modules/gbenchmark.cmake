ExternalProject_Add(
    benchmark
    URL https://github.com/google/benchmark/archive/v1.0.0.zip
    DOWNLOAD_DIR external
    DOWNLOAD_NAME benchmark.zip
    LOG_DOWNLOAD ON
    INSTALL_COMMAND ""
    LOG_CONFIGURE ON
    LOG_BUILD ON)

ExternalProject_Get_Property(benchmark install_dir)
set(BENCHMARK_INCLUDE_DIRS "${install_dir}/src/benchmark/include")

if(WIN32)
find_library(SHLWAPI Shlwapi.lib) 
else()
set(SHLWAPI)
endif()