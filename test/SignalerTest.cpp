/** @file
 * @author Jacob SP <jacob@moca.st>
 */

#include <string>

#include "MockWebSocket.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rtcast/ConnectionMode.h"
#include "rtcast/Signaler.h"
#include "rtc/rtc.hpp"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Invoke;

/**
* @brief Test that the Signaler class sets callbacks when created.
*/
TEST(SignalerTest, TestCallbacksAreSet) {
	std::string host = "ws://localhost:8080";
	std::string remote = "remote1";

	// Create mock WebSocket
	auto mockSocket = std::make_unique<MockWebSocket>();

	EXPECT_CALL(*mockSocket, onOpen(_)).Times(1);
	EXPECT_CALL(*mockSocket, onClosed(_)).Times(1);
	EXPECT_CALL(*mockSocket, onError(_)).Times(1);
	EXPECT_CALL(*mockSocket, onMessage(_)).Times(1);

	// Create Signaler with mock WebSocket
	mocast::Signaler<MockWebSocket> signaler(
		mocast::ConnectionMode::CONTROL, host, remote, std::move(mockSocket)
	);
}

/**
* @brief Test that the Signaler class calls the open method of the WebSocket.
*/
TEST(SignalerTest, TestOpenMethodIsCalled) {
	std::string host = "ws://localhost:8080";
	std::string remote = "remote1";

	// Create mock WebSocket
	auto mockSocket = std::make_unique<MockWebSocket>();

	// Expect open() to be called
	EXPECT_CALL(*mockSocket, open(host)).Times(1);

	// Create Signaler with mock WebSocket
	mocast::Signaler<MockWebSocket> signaler(
		mocast::ConnectionMode::CONTROL, host, remote, std::move(mockSocket)
	);

	// Call open() on Signaler
	signaler.Open();
}

/**
* @brief Test that the Signaler class calls the isOpen method of the WebSocket.
*/
TEST(SignalerTest, TestIsOpenMethodIsCalled) {
	std::string host = "ws://localhost:8080";
	std::string remote = "remote1";

	// Create mock WebSocket
	auto mockSocket = std::make_unique<MockWebSocket>();

	// Expect open() to be called
	EXPECT_CALL(*mockSocket, isOpen()).Times(1);

	// Create Signaler with mock WebSocket
	mocast::Signaler<MockWebSocket> signaler(
		mocast::ConnectionMode::CONTROL, host, remote, std::move(mockSocket)
	);

	// Call open() on Signaler
	signaler.IsOpen();
}

/**
* @brief Test that the Signaler class calls the isClosed method of the WebSocket.
*/
TEST(SignalerTest, TestIsClosedMethodIsCalled) {
	std::string host = "ws://localhost:8080";
	std::string remote = "remote1";

	// Create mock WebSocket
	auto mockSocket = std::make_unique<MockWebSocket>();

	// Expect open() to be called
	EXPECT_CALL(*mockSocket, isClosed()).Times(1);

	// Create Signaler with mock WebSocket
	mocast::Signaler<MockWebSocket> signaler(
		mocast::ConnectionMode::CONTROL, host, remote, std::move(mockSocket)
	);

	// Call open() on Signaler
	signaler.IsClosed();
}

/**
* @brief Test that the Signaler class calls the send method of the WebSocket.
*/
TEST(SignalerTest, TestSendMethodIsCalled) {
	std::string host = "ws://localhost:8080";
	std::string remote = "remote1";

	// Create mock WebSocket
	auto mockSocket = std::make_unique<MockWebSocket>();

	// Expect open() to be called
	EXPECT_CALL(*mockSocket, send(_)).WillOnce(Return(true));

	// Create Signaler with mock WebSocket
	mocast::Signaler<MockWebSocket> signaler(
		mocast::ConnectionMode::CONTROL, host, remote, std::move(mockSocket)
	);

	// Call open() on Signaler
	signaler.Send("Hello, World!");
}
