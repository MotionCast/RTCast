### LIST FILES
set(
	RTCAST_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/DataStream.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Signaler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/SDPMsg.cpp
)

set(
	RTCAST_HEADERS
	${PROJECT_SOURCE_DIR}/include/rtcast/ConnectionMode.h
	${PROJECT_SOURCE_DIR}/include/rtcast/DataStream.h
	${PROJECT_SOURCE_DIR}/include/rtcast/SDPMsg.h
	${PROJECT_SOURCE_DIR}/include/rtcast/Signaler.h
)

### TARGET
# Provide STATIC and SHARED targets
add_library(rtcast-static STATIC ${RTCAST_SOURCES})
add_library(rtcast-shared SHARED ${RTCAST_SOURCES})

### HEADERS

## STATIC
# Including headers in other projects (PUBLIC) require `rtcast` prefix
target_include_directories(
	rtcast-static
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

# Include headers in library (PRIVATE) without prefixing with `rtcast`
target_include_directories(
	rtcast-static
	PRIVATE
	${PROJECT_SOURCE_DIR}/include/rtcast
)

target_compile_features(rtcast-static PUBLIC cxx_std_17)

## SHARED
# Including headers in other projects (PUBLIC) require `rtcast` prefix
target_include_directories(
	rtcast-shared
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

# Include headers in library (PRIVATE) without prefixing with `rtcast`
target_include_directories(
	rtcast-shared
	PRIVATE
	${PROJECT_SOURCE_DIR}/include/rtcast
)

target_compile_features(rtcast-shared PUBLIC cxx_std_17)

### DEPENDENCIES
if(USE_LIBDATACHANNEL)
	message("-- Fetching libdatachannel from repository")

	# libdatachannel
	FetchContent_Declare(
  	libdatachannel
  	GIT_REPOSITORY https://github.com/paullouisageneau/libdatachannel
  	GIT_TAG        v0.18.0
	)

	FetchContent_GetProperties(libdatachannel)
	if(NOT libdatachannel_POPULATED)
		# Disable JSON Tests
		set(JSON_BuildTests OFF CACHE INTERNAL "Disable JSON tests.")

		# Fetch the content using previously declared details
	  FetchContent_Populate(libdatachannel)

	  # Bring the populated content into the build
	  add_subdirectory(${libdatachannel_SOURCE_DIR} ${libdatachannel_BINARY_DIR} EXCLUDE_FROM_ALL)

		target_compile_definitions(datachannel-static PUBLIC NO_TESTS=${BUILD_TESTS} NO_EXAMPLES=1)
		target_compile_definitions(datachannel PUBLIC NO_TESTS=${BUILD_TESTS} NO_EXAMPLES=1)
	endif()


	# Save time by not compiling tests and examples
	target_link_libraries(rtcast-static PUBLIC datachannel-static nlohmann_json)
	target_link_libraries(rtcast-shared PUBLIC datachannel nlohmann_json)
endif()

### INSTALL
install(
	TARGETS rtcast-shared datachannel nlohmann_json
	EXPORT RTCastTargets
	DESTINATION lib
)

install(
	FILES ${RTCAST_HEADERS}
	DESTINATION include/rtcast
)

install(
	EXPORT RTCastTargets
	FILE RTCastTargets.cmake
	DESTINATION lib/cmake/RTCast
)

include(CMakePackageConfigHelpers)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/RTCastConfig.cmake"
	INSTALL_DESTINATION "lib/cmake/RTCast"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/RTCastConfigVersion.cmake"
	VERSION "${rtcast_VERSION_MAJOR}.${rtcast_VERSION_MINOR}"
	COMPATIBILITY AnyNewerVersion
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/RTCastConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/RTCastConfigVersion.cmake
	DESTINATION lib/cmake/RTCast
)

export(EXPORT RTCastTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/RTCastTargets.cmake"
)
