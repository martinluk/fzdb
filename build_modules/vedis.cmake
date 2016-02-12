
  message("Attempting to download Vedis")
  set(MY_URL "https://github.com/symisc/vedis/archive/master.zip")
  set(MY_DOWNLOAD_PATH "./external/vedis.zip")
  set(VEDIS_ROOT "${CMAKE_BINARY_DIR}/vedis-master")

  if (NOT EXISTS "${MY_DOWNLOAD_PATH}")
      file(DOWNLOAD "${MY_URL}" "${MY_DOWNLOAD_PATH}" SHOW_PROGRESS)      
  else()
    message("Vedis appears to already be downloaded")
  endif()  

  if (NOT EXISTS "${VEDIS_ROOT}")
    message("Extracting Vedis")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz "./external/vedis.zip")
  endif()

  add_library(vedis-lib OBJECT "${VEDIS_ROOT}/vedis.c" "${VEDIS_ROOT}/vedis.h")
  set(VEDIS_INCLUDE_DIRS "${VEDIS_ROOT}")