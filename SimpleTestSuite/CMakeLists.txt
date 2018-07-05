project (SimpleTestSuite)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

if(UNIX)
    set(CMAKE_INSTALL_PREFIX "/usr")
endif(UNIX)


find_package(Qt4 REQUIRED)

if(QT4_FOUND)
    include(${QT_USE_FILE})
else(QT4_FOUND)
    message(FATAL_ERROR "Could not find Qt4")
endif(QT4_FOUND)

find_package(lfDevices REQUIRED)
find_package(QextSerialPort REQUIRED)
find_package(Qwt REQUIRED)
find_package(Inkscape REQUIRED)

INCLUDE_DIRECTORIES(${QWT_INCLUDE_DIR})

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(include ${CMAKE_BINARY_DIR} ${QEXTSERIALPORT_INCLUDE_DIR} Qwt ${QWT_INCLUDE_DIR})

link_libraries(
    ${QT_LIBRARIES}
    ${QLFDEVICES_LIBRARY}
    ${QEXTSERIALPORT_LIBRARY}
    ${QWT_LIBRARY}
)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

if(WIN32)
  add_definitions(-D_TTY_WIN_)
else(WIN32)
  add_definitions(-D_TTY_POSIX_)
endif(WIN32)

aux_source_directory (src SOURCES)

set(QTHEADERS)

file(GLOB HEADERS src/*.h)
file(GLOB QTUI ui/*.ui)
file(GLOB QTRESOURCES rc/*.qrc)

qt4_wrap_cpp(mocSOURCES ${HEADERS})
qt4_wrap_ui(UI ${QTUI})
qt4_add_resources(RESOURCES ${QTRESOURCES})

add_executable(SimpleTestSuite ${SOURCES} ${mocSOURCES} ${RESOURCES} ${UI})


#Subversion

#find_package(Subversion)
#if(Subversion_FOUND)
#    subversion_wc_info(${PROJECT_SOURCE_DIR} Project)
#endif(Subversion_FOUND)
#End Subversion

#GENERATE UNIX ICONS
if(UNIX AND INKSCAPE_FOUND)

add_custom_target(LinuxIcons ALL)

set(ICONS_PATH ${PROJECT_SOURCE_DIR}/resources/linux/mime/icons/hicolor)
set(ICON_SIZES 16 22 24 32 48 64)

#foreach(SIZE ${ICON_SIZES})
#  file(MAKE_DIRECTORY ${ICONS_PATH}/${SIZE}x${SIZE}/apps)
#
#  add_custom_command(
#    SOURCE    ${ICONS_PATH}/scalable/apps/SimpleTestSuite.svg
#    COMMAND   ${INKSCAPE_EXECUTABLE}
#    ARGS      -f ${ICONS_PATH}/scalable/apps/SimpleTestSuite.svg -D -w ${SIZE} -h ${SIZE} -e ${ICONS_PATH}/${SIZE}x${SIZE}/apps/#SimpleTestSuite.png
#    TARGET    LinuxIcons
#    OUTPUTS   ${ICONS_PATH}/${SIZE}x${SIZE}/apps/SimpleTestSuite.png
#    DEPENDS   ${ICONS_PATH}/${SIZE}x${SIZE}/apps/
#    )
#
#  add_custom_command(
#    SOURCE    LinuxIcons
#    TARGET    LinuxIcons
#    DEPENDS   
#    ${ICONS_PATH}/${SIZE}x${SIZE}/apps/SimpleTestSuite.png
#    )
#
#endforeach(SIZE)

#unset(ICON_SIZES)

#unset(ICONS_PATH)

endif()
#end GENERATE UNIX ICONS

#PACKAGING

install(TARGETS SimpleTestSuite
    RUNTIME DESTINATION bin
    PERMISSIONS OWNER_EXECUTE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_EXECUTE WORLD_READ)

set(SIMPLE_TEST_SUITE_BIN bin
    CACHE PATH "Location of SimpleTestSuite binaries.")

set(SIMPLE_TEST_SUITE_CONF local/etc/lab5
    CACHE PATH "Location of SimpleTestSuite configuration files.")

mark_as_advanced(SIMPLE_TEST_SUITE_BIN
                 SIMPLE_TEST_SUITE_CONF)

set(STSCONFIG_FILENAME "${CMAKE_INSTALL_PREFIX}/${SIMPLE_TEST_SUITE_CONF}/SimpleTestSuite.conf")
  #configure version info file
  configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/STSConfig.h.cmake
    ${CMAKE_BINARY_DIR}/src/STSConfig.h)

install(FILES SimpleTestSuite.conf
        DESTINATION ${SIMPLE_TEST_SUITE_CONF}
        COMPONENT resources)


# FreeDesktop .desktop and MIME resources
#if(UNIX)
    # Set paths
#    set(UNIX_MIME_DIR resources/linux/mime)
#    set(UNIX_MIMELNK_FILES ${UNIX_MIME_DIR}/mimelnk)
#    set(UNIX_MIME_FILES ${UNIX_MIME_DIR}/mime)
#    set(UNIX_ICONS_FILES ${UNIX_MIME_DIR}/icons)
#    set(UNIX_APPLICATIONS_FILES ${UNIX_MIME_DIR}/applications)

    # Install Mimelnk directory
#    install(DIRECTORY ${UNIX_MIMELNK_FILES}
#            DESTINATION share
#            COMPONENT resources
#            PATTERN ".svn" EXCLUDE)

    # Install Mime directory
#    install(DIRECTORY ${UNIX_ICONS_FILES}
#            DESTINATION share
#            COMPONENT resources
#            PATTERN ".svn" EXCLUDE)

    # Install Icons
#    install(DIRECTORY ${UNIX_MIME_FILES}
#            DESTINATION share
#            COMPONENT resources
#            PATTERN ".svn" EXCLUDE)

    # Install Applications directory (.desktop files)
#    install(DIRECTORY ${UNIX_APPLICATIONS_FILES}
#            DESTINATION share
#            COMPONENT resources
#            PATTERN ".svn" EXCLUDE)
#endif(UNIX)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SimpleTestSuite")
set(CPACK_PACKAGE_VENDOR "lab5")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "754")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SimpleTestSuite ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtgui4 (>= 4.8.7), libqwt5-qt4 (>= 5.2.3-1), zip")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "lexaficus@yandex.ru")
include(CPack)

#END PACKAGING
