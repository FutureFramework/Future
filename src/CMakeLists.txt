set(iotlib_srcs cpplib/coap/coap.cpp)
set(iotlib_headers cpplib/iotlib_global.h cpplib/coap/coap.hpp)
#set(qmsgpack_stream_headers stream/location.h stream/time.h stream/geometry.h)

add_library(iot SHARED ${iotlib_srcs} ${iotlib_headers})

qt5_use_modules(iot Core Network)

#configure_file(
#  "${CMAKE_CURRENT_SOURCE_DIR}/msgpackcommon.h.in"
#  "${CMAKE_CURRENT_SOURCE_DIR}/msgpackcommon.h"
#  IMMEDIATE @ONLY)

if (NOT android)
	set_target_properties(iot PROPERTIES
									VERSION ${IOTLIB_MAJOR}.${IOTLIB_MINOR}.${IOTLIB_VERSION}
									SOVERSION ${IOTLIB_MAJOR})
endif ()
set_target_properties(iot PROPERTIES
								DEFINE_SYMBOL	MAKE_IOTLIB
								PUBLIC_HEADER	"${iotlib_headers}")

install(TARGETS iot EXPORT iotlib-export
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
	PUBLIC_HEADER DESTINATION ${INCLUDE_INSTALL_DIR}/iotlib)


#install(FILES  ${qmsgpack_stream_headers} DESTINATION ${INCLUDE_INSTALL_DIR}/qmsgpack/stream)