set(Qt5Core_DIR "/opt/Qt/5.5/gcc_64/lib/cmake/Qt5Core")
set(Qt5Network_DIR "/opt/Qt/5.5/gcc_64/lib/cmake/Qt5Network")
set(Qt5Test_DIR "/opt/Qt/5.5/gcc_64/lib/cmake/Qt5Test")
set(Qt5_DIR "/opt/Qt/5.5/gcc_64/lib/cmake/Qt5Core")
set(QT_QMAKE_EXECUTABLE "/opt/Qt/5.5/gcc_64/bin/qmake")

cmake_minimum_required(VERSION 2.8.11)

project(iotlib)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Network)


#add extra search paths for libraries and includes
set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Directory where lib will install")
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "The directory the headers are installed in")
set(CMAKECONFIG_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/${CMAKE_PROJECT_NAME}" CACHE PATH "Directory where to install qmsgpack.cmake")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

set(IOTLIB_MAJOR	"0")
set(IOTLIB_MINOR	"1")
set(IOTLIB_VERSION	"0")

set(IOTLIB_VERSION_STRING "${IOTLIB_MAJOR}.${IOTLIB_MINOR}.${IOTLIB_VERSION}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

add_subdirectory(src)
if (BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif ()

install(EXPORT iotlib-export DESTINATION ${CMAKECONFIG_INSTALL_DIR} FILE IoTLibQtTargets.cmake)

file(RELATIVE_PATH relInstallDir ${CMAKE_INSTALL_PREFIX}/$CMAKECONFIG_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX})

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")