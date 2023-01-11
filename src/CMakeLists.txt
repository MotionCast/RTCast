# Declare `rtcast` library
add_library(rtcast DataStream.cpp Signaler.cpp SDPMsg.cpp)
target_compile_features(rtcast INTERFACE cxx_std_17)

set(
	RTCAST_TARGETS
	datachannel-static
	usrsctp
	plog
	srtp2
	juice-static
	nlohmann_json
)

set(
	RTCAST_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/ConnectionMode.h
	${CMAKE_CURRENT_SOURCE_DIR}/DataStream.h
	${CMAKE_CURRENT_SOURCE_DIR}/SDPMsg.h
	${CMAKE_CURRENT_SOURCE_DIR}/Signaler.h
)

# Link the libdatachannel library
target_link_libraries(
	rtcast PRIVATE
	LibDataChannel::LibDataChannelStatic
	nlohmann_json::nlohmann_json
)

# Header file directory
target_include_directories(
	rtcast
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include/rtcast>
)

# Install Target
install(
	TARGETS rtcast nlohmann_json datachannel-static plog juice-static srtp2
	EXPORT RTCastTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
)

# Install Header Files
install(
	FILES ${RTCAST_HEADERS}
	DESTINATION include/rtcast
)

# Install .cmake File
install(
	EXPORT RTCastTargets
	FILE RTCastTargets.cmake
	NAMESPACE RTCast::
	DESTINATION lib/cmake/RTCast
	EXPORT_LINK_INTERFACE_LIBRARIES
)
