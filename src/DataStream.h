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
		DataStream(ConnectionMode mode,
							 const std::string_view serverURL,
							 const std::string_view remoteID,
							 const std::vector<std::string>& iceServers);
		/// Open the Signaler WebSocket and wait for a reply or an incoming connection.
		void Open();
		/// Send a message through `rtc::DataChannel->send()`.
		bool Send(std::variant<rtc::binary, std::string> data);
		/// Get future for channel open promise.
		std::future<void> ChannelOpen();
		/// Get future for channel closed promise.
		std::future<void> ChannelClosed();
		/// Sets the callback to be called when the DataChannel receives a message.
		void OnMessage(std::function<void(std::variant<rtc::binary, std::string>)> callback);
		/// Sets the callback to be called when the DataChannel receives a binary message.
		void OnBinaryMessage(std::function<void(rtc::binary)> data);
		/// Sets the callback to be called when the DataChannel receives a string message.
		void OnStringMessage(std::function<void(std::string)> data);
	protected:
		/// Sets the callbacks for the `channel` member variable.
		void SetupDataChannel();
		const ConnectionMode mode_;
		const std::string remoteID_;
		const std::string channelLabel_ = "mimic";
		/// Promise to signal when DataChannel is stablished.
		std::promise<void> openPromise;
		std::promise<void> closedPromise;
		std::unique_ptr<mocast::Signaler> server;
		std::unique_ptr<rtc::PeerConnection> peer;
		std::shared_ptr<rtc::DataChannel> channel;

		// Callbacks
		std::function<void(std::variant<rtc::binary, std::string>)> messageCallback;
		std::function<void(rtc::binary)> binMessageCallback;
		std::function<void(std::string)> strMessageCallback;
	};
}

#endif
