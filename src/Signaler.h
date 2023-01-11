/** @file
 * @author Jacob SP <jacob@moca.st>
 */

#ifndef SIGNALER_H
#define SIGNALER_H

#include <string_view>
#include <functional>
#include <memory>
#include <cstddef>
#include <future>
#include <variant>
#include <string>

#include "rtc/rtc.hpp"

#include "ConnectionMode.h"
#include "SDPMsg.h"

namespace mocast {
	/**
	 * @brief Connects with the signaling server through a WebSocket.
	 */
	class Signaler {
	public:
		/**
		 * @brief Creates a connection with the signaling server.
		 * @param mode Whether the signaler will accept incoming SDP or drop it.
		 * @param host A WebSocket URL to the signaling server endpoint.
		 * @param remoteID The ID of the remote device to be paired with.
		 */
		Signaler(ConnectionMode mode,
						 const std::string_view host,
						 const std::string_view remoteID);

		/// See `rtc::WebSocket.open()`
		void Open();
		/// See `rtc::WebSocket.send()`
		bool Send(const std::variant<rtc::binary, std::string> data);
		/// See `rtc::WebSocket.isOpen()`
		bool IsOpen() const;
		/// See `rtc::WebSocket.isClosed()`
		bool IsClosed() const;
		/// Wait for the WebSocket to connect with the client.
		std::future<void> SocketFuture();
		/// Sets the callback to be called when the Signaler receives a reply.
		void OnReply(std::function<void(SDPMsg)> callback);

	protected:
		const std::string host_;
		const std::string remoteID_;
		const ConnectionMode mode_;

		/// Promise to signal when WebSocket is opened.
		std::promise<void> socketPromise;
		/// WebSocket to signaling server.
		std::unique_ptr<rtc::WebSocket> socket;
		std::function<void(SDPMsg)> messageCallback;
	};
}

#endif
