cmake_minimum_required(VERSION 3.20)
project(RTCast VERSION 0.2.0 LANGUAGES CXX)

# Options
option(NO_TESTS "Disable tests build" OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS OFF) # to force usrsctp to be built static

message("-- Configuring RTCast")

# Dependencies
include(FetchContent)

# libdatachannel
FetchContent_Declare(
	libdatachannel
	GIT_REPOSITORY https://github.com/paullouisageneau/libdatachannel
	GIT_TAG        v0.18.0
)

message("-- Fetching libdatachannel")
FetchContent_MakeAvailable(libdatachannel)

add_library(rtcast src/DataStream.cpp src/Signaler.cpp src/SDPMsg.cpp)
target_link_libraries(
	rtcast PRIVATE
	$<BUILD_INTERFACE:LibDataChannel::LibDataChannelStatic>
	nlohmann_json::nlohmann_json
)

target_include_directories(
	rtcast
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:include/rtcast>
)

set(
	RTCAST_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/src/ConnectionMode.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/DataStream.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SDPMsg.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/Signaler.h
)

# Install Target
install(
	TARGETS rtcast nlohmann_json #nlohmann_json datachannel-static plog juice-static srtp2
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

install(
	EXPORT RTCastTargets
	FILE RTCastTargets.cmake
	NAMESPACE RTCast::
	DESTINATION lib/cmake/RTCast
	EXPORT_LINK_INTERFACE_LIBRARIES
)

#target_compile_options(datachannel-static PRIVATE -DNO_EXAMPLES -DNO_TESTS)

#add_subdirectory(src)
