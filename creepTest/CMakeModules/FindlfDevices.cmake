find_path(QLFDEVICES_INCLUDE_DIR QlfThread.h
../QlfDevices/src
${QLFDEVICES_INCLUDE_DIR}
${QLFDEVICES_DIR}/src
)

if(NOT QLFDEVICES_INCLUDE_DIR)
  message(FATAL_ERROR "QlfDevices not found. Use cmake -DQLFDEVICES_DIR=lfDevicesDir")
endif(NOT QLFDEVICES_INCLUDE_DIR)

find_library(QLFDEVICES_LIBRARY qlfdevices
../QlfDevices
${QLFDEVICES_LIB_DIR}
${QLFDEVICES_DIR}
${QLFDEVICES_DIR}
)

if(NOT QLFDEVICES_LIBRARY)
  message(FATAL_ERROR "QlfDevices library not found. Use cmake -DQLFDEVICES_DIR=lfDevicesDir")
endif(NOT QLFDEVICES_LIBRARY)

include_directories(${QLFDEVICES_INCLUDE_DIR})

