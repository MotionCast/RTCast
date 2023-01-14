/** @file
 * @author Jacob SP <jacob@moca.st>
 */

#ifndef DATASTREAM_H
#define DATASTREAM_H

#include <memory>
#include <vector>
#include <string_view>

#include "rtc/rtc.hpp"

#include "Signaler.h"
#include "ConnectionMode.h"

namespace mocast {
	/**
	 * @brief A connection to the client that will receive the data through WebRTC.
	 */
	class DataStream {
	public:
		/**
		 * @brief Setup a new connection with these parameters for the signaling server.
		 * @param mode Whether the connection should be offered or received.
		 * @param serverURL The complete URL of the signaling server.
		 * @param remoteID A String ID of the remote device to connect to.
		 * @param iceServers A list of servers to be used in the RTC configuration.
		 */
		DataStream(
			ConnectionMode mode,
			const std::string_view serverURL,
			const std::string_view remoteID,
			const std::vector<std::string>& iceServers);

		/// Open the Signaler WebSocket and wait for a reply or an incoming connection.
		void Open();
		/// Send a message through `rtc::DataChannel->send()`.
		inline bool Send(std::variant<rtc::binary, std::string> data) {
			return channel ? channel->send(data) : false;
		}
		/// Get future for channel open promise.
		inline std::future<void> ChannelOpen() { return openPromise.get_future(); }
		/// Get future for channel closed promise.
		inline std::future<void> ChannelClosed() { return closedPromise.get_future(); }
		/// Sets the callback to be called when the DataChannel receives a message.
		inline void OnMessage(std::function<void(std::variant<rtc::binary, std::string>)> callback) {
			messageCallback = callback;
		}
		/// Sets the callback to be called when the DataChannel receives a binary message.
		inline void OnBinaryMessage(std::function<void(rtc::binary)> callback) {
			binMessageCallback = callback;
		}
		/// Sets the callback to be called when the DataChannel receives a string message.
		inline void OnStringMessage(std::function<void(std::string)> callback) {
			strMessageCallback = callback;
		}
	protected:
		/// Sets the callbacks for the `channel` member variable.
		void SetupDataChannel();
		const ConnectionMode mode_;
		const std::string remoteID_;
		const std::string channelLabel_ = "mimic";
		/// Promise to signal when DataChannel is stablished.
		std::promise<void> openPromise;
		std::promise<void> closedPromise;
		std::unique_ptr<Signaler<rtc::WebSocket>> server;
		std::unique_ptr<rtc::PeerConnection> peer;
		std::shared_ptr<rtc::DataChannel> channel;

		// Callbacks
		std::function<void(std::variant<rtc::binary, std::string>)> messageCallback;
		std::function<void(rtc::binary)> binMessageCallback;
		std::function<void(std::string)> strMessageCallback;
	};
}

#endif
