set(QT_USE_QTTEST TRUE)

include_directories(../../src/cpplib ${CMAKE_CURRENT_BINARY_DIR})

set(TEST_NAME pdu_test)

message(status "Building ${TEST_NAME}")
add_executable(${TEST_NAME} ${TEST_NAME}.cpp)

target_link_libraries(${TEST_NAME}
	${QT_LIBRARIES}
	${TEST_LIBRARIES}
		iot
)

add_test(${TEST_NAME} ${TEST_NAME})
