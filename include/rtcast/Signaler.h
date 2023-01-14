/** @file
 * @author Jacob SP <jacob@moca.st>
 */

#ifndef SIGNALER_H
#define SIGNALER_H

#include <string_view>
#include <functional>
#include <iostream>
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
	template <class T = rtc::WebSocket>
	class Signaler {
	public:
		/**
		 * @brief Creates a connection with the signaling server.
		 * @param mode Whether the signaler will accept incoming SDP or drop it.
		 * @param host A WebSocket URL to the signaling server endpoint.
		 * @param remoteID The ID of the remote device to be paired with.
		 * @param socketPtr A `unique_ptr<T>` to call instead of creating one.
		 */
		Signaler(
			ConnectionMode mode,
			const std::string_view host,
			const std::string_view remoteID,
			std::unique_ptr<T> socketPtr = std::make_unique<T>()) :
				host_{host}, remoteID_{remoteID}, mode_{mode}, socket{std::move(socketPtr)} {
				SetSocketCallbacks();
			}

		/// @brief See `rtc::WebSocket.open()`.
		/// @see rtc::WebSocket.open()
		virtual inline void Open() { socket->open(host_); }
		/// @brief See `rtc::WebSocket.send()`.
		/// @see rtc::WebSocket.send()
		virtual inline bool Send(const std::variant<rtc::binary, std::string> data) { return socket->send(data); }
		/// @brief See `rtc::WebSocket.isOpen()`.
		/// @see rtc::WebSocket.isOpen()
		virtual inline bool IsOpen() const { return socket->isOpen(); }
		/// @brief See `rtc::WebSocket.isClosed()`.
		/// @see rtc::WebSocket.isClosed()
		virtual inline bool IsClosed() const { return socket->isClosed(); }
		/// Wait for the WebSocket to connect with the client.
		virtual inline std::future<void> SocketFuture() {return socketPromise.get_future(); }
		/// Sets the callback to be called when the Signaler receives a reply.
		virtual inline void OnReply(std::function<void(SDPMsg)> callback) { messageCallback = callback; }

	protected:
		void SetSocketCallbacks() {
			// Define behaviour for WebSocket
			socket->onOpen([this]() {
				std::cout << "Connected to server." << std::endl;
				socketPromise.set_value();
			});

			socket->onClosed([]() {
				std::cout << "Connection to server closed." << std::endl;
			});

			socket->onError([this](const std::string& error) {
				std::cerr << "Error connecting to server: " << error << std::endl;
				std::cerr << "Is the signaling server up and running?" << std::endl << std::endl;
				socketPromise.set_exception(std::make_exception_ptr(std::runtime_error(error)));
			});

			socket->onMessage([this](auto data) {
				// Ignore non-string messages
				if (!std::holds_alternative<std::string>(data)) { return; }

				// Parse string -> json -> SDPMsg
				nlohmann::json jsonMsg = nlohmann::json::parse(std::get<std::string>(data));
				auto sdpMsg = jsonMsg.get<SDPMsg>();

				// Ignore messages from other remotes
				if(sdpMsg.id != remoteID_) { return; }

				if (messageCallback) {
					messageCallback(sdpMsg);
				}
			});
		}

		const std::string host_;
		const std::string remoteID_;
		const ConnectionMode mode_;

		/// Promise to signal when WebSocket is opened.
		std::promise<void> socketPromise;
		/// WebSocket to signaling server.
		std::unique_ptr<T> socket;
		std::function<void(SDPMsg)> messageCallback;
	};
}

#endif
