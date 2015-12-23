ExternalProject_Add(
    spdlog
    URL https://github.com/gabime/spdlog/archive/master.zip
    DOWNLOAD_DIR external
    DOWNLOAD_NAME spdlog.zip
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

ExternalProject_Get_Property(spdlog install_dir)
set(SPDLOG_INCLUDE_DIRS "${install_dir}/src/spdlog/include")
