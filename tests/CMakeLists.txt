find_package(Qt5Test REQUIRED)
include_directories(${Qt5Test_INCLUDE_DIRS})
add_definitions(${Qt5Test_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Test_EXECUTABLE_COMPILE_FLAGS}")
set(TEST_LIBRARIES ${Qt5Test_LIBRARIES})

set(TEST_SUBDIRS pdu)

foreach(subdir ${TEST_SUBDIRS})
	add_subdirectory(${subdir})
endforeach()