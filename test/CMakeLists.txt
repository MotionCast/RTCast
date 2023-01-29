### LIST TESTS
set(
	RTCAST_TESTS
	SDPMsgTest.cpp
	SignalerTest.cpp
)

### FETCH DEPENDENCIES
# googletest
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

### TARGET
add_executable(RTCastTestSuite ${RTCAST_TESTS})

target_link_libraries(
	RTCastTestSuite
	GTest::gtest_main
	GTest::gmock_main
	rtcast-static
)

include(GoogleTest)
gtest_discover_tests(RTCastTestSuite)
