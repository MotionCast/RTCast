cmake_minimum_required(VERSION 3.13)

### PROJECT
project(rtcast VERSION 0.3.0 LANGUAGES CXX)

### OPTIONS
option(BUILD_TESTS "Build tests alongside project." OFF)
option(BUILD_DOCS "Build documentation with Doxygen." OFF)
option(USE_LIBDATACHANNEL "Use the libdatachannel library." ON)

# DOWNLOAD_TIMESTAMP
cmake_policy(SET CMP0135 NEW)
# OVERRIDE CHILD PROJECT OPTIONS
cmake_policy(SET CMP0077 NEW)

### INCLUDES
include(FetchContent)

### TARGETS
message("-- Configuring rtcast")
add_subdirectory(src)

### TESTING
if(BUILD_TESTS)
	message("-- Configuring tests with googletest")
	enable_testing()
	add_subdirectory(test)
endif()

### DOCUMENTATION
if(BUILD_DOCS)
	message("-- Configuring documentation")

	# Doxygen
	find_package(Doxygen
	             REQUIRED dot dia
	             OPTIONAL_COMPONENTS mscgen)

	if (DOXYGEN_FOUND)
		set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs")
		set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${PROJECT_SOURCE_DIR}/README.md")
		set(DOXYGEN_HTML_EXTRA_FILES "${PROJECT_SOURCE_DIR}/LICENSE")
		doxygen_add_docs(docs
			"${PROJECT_SOURCE_DIR}/README.md"
			"${PROJECT_SOURCE_DIR}/LICENSE"
			"${PROJECT_SOURCE_DIR}/src"
			"${PROJECT_SOURCE_DIR}/include/rtcast"
			"${PROJECT_SOURCE_DIR}/test"
			ALL
			COMMENT "Generate HTML documentation")
	else()
		message("-- Doxygen not found")
	endif()
endif()

### PACKAGING
message("-- Setting packaging variables")
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${rtcast_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${rtcast_VERSION_MINOR}")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
