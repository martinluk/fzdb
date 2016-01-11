ExternalProject_Add(
    rapidjson
    URL https://github.com/miloyip/rapidjson/archive/v1.0.2.zip
    DOWNLOAD_DIR external
    DOWNLOAD_NAME rapidjson.zip
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

ExternalProject_Get_Property(rapidjson install_dir)
set(RAPIDJSON_INCLUDE_DIRS "${install_dir}/src/rapidjson/include")
