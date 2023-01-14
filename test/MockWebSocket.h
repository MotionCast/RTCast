#include <string>
#include <variant>
#include <functional>
#include "rtc/rtc.hpp"

#include "gmock/gmock.h"
#include <gtest/gtest.h>

class MockWebSocket {
public:
	MOCK_METHOD(void, open, (std::string host), ());
	MOCK_METHOD(bool, send, ((const std::variant<rtc::binary, std::string> data)), ());
	MOCK_METHOD(bool, isOpen, (), (const));
	MOCK_METHOD(bool, isClosed, (), (const));

	// Callbacks
	MOCK_METHOD(void, onOpen, (std::function<void()> callback), ());
	MOCK_METHOD(void, onClosed, (std::function<void()> callback), ());
	MOCK_METHOD(void, onError, (std::function<void(const std::string&)> callback), ());
	MOCK_METHOD(void, onMessage, ((std::function<void(const std::variant<rtc::binary, std::string>)> callback)), ());
};
