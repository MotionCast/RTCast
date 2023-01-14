#include "DataStream.h"

#include <memory>
#include <string_view>

#include "rtc/rtc.hpp"
#include <nlohmann/json.hpp>

#include "SDPMsg.h"
#include "Signaler.h"
#include "ConnectionMode.h"

using namespace mocast;
using nlohmann::json;

DataStream::DataStream(ConnectionMode mode,
                       const std::string_view serverURL,
                       const std::string_view remoteID,
                       const std::vector<std::string>& iceServers) :
	// Set stream values and create Signaler object
	mode_{mode},
	remoteID_{remoteID},
	server(std::make_unique<Signaler<>>(mode, serverURL, remoteID)) {

	rtc::Configuration config;

	// Set ice servers from configuration
	for (const auto& iceServer : iceServers) {
		config.iceServers.emplace_back(iceServer);
	}

	// Set PeerConnection parameters
	peer = std::make_unique<rtc::PeerConnection>(config);

	peer->onStateChange(
		[](rtc::PeerConnection::State state) {
			std::cout << "State: " << state << std::endl;
		}
	);

	peer->onGatheringStateChange(
		[](rtc::PeerConnection::GatheringState state) {
			std::cout << "Gathering State: " << state << std::endl;
		}
	);

	peer->onLocalDescription(
		[this](rtc::Description description) {
			json message = SDPMsg(description, remoteID_);

			// Send description through Signaler
			std::cout << std::endl << "Sending description through Signaler." << std::endl;
			std::cout << message << std::endl << std::endl;
			server->Send(message.dump());
	});

	peer->onLocalCandidate(
		[this](rtc::Candidate candidate) {
			json message = SDPMsg(candidate, remoteID_);

			// Send candidate through Signaler
			std::cout << std::endl << "Sending candidate through Signaler." << std::endl;
			std::cout << message << std::endl << std::endl;
			server->Send(message.dump());
	});

	peer->onDataChannel(
		[this](std::shared_ptr<rtc::DataChannel> dc) {
			std::cout << "DataChannel from " << remoteID_;
			std::cout << " received with label \"" << dc->label() << "\"" << std::endl;

			channel = std::move(dc);
			SetupDataChannel();
	});

	server->OnReply(
		[this](SDPMsg sdp) {
			// Run callback
			if (sdp.IsCandidate()) {
				peer->addRemoteCandidate(rtc::Candidate(sdp.candidate, sdp.mid));
			} else {
				peer->setRemoteDescription(rtc::Description(sdp.description, sdp.type));
			}
	});
}

void DataStream::Open() {
	auto socketOpen = server->SocketFuture();

	// Open WebSocket
	server->Open();
	// Wait until socket has been opened
	socketOpen.get();

	// Create DataChannel
	if (mode_ == ConnectionMode::CONTROL) {
		channel = std::move(peer->createDataChannel(channelLabel_));
		SetupDataChannel();
	}
}

void DataStream::SetupDataChannel() {
	channel->onOpen([this]() {
		// Release channel open promise
		openPromise.set_value();

		std::cout << "DataChannel from " << remoteID_ << " open" << std::endl;
		channel->send("Hello, you must be " + remoteID_);
	});

	channel->onClosed([this]() {
		// Release channel closed promise
		closedPromise.set_value();

		std::cout << "DataChannel from " << remoteID_ << " closed" << std::endl;
	});

	channel->onMessage([this](auto data) {
		if (messageCallback) {
			messageCallback(data);
		}

		if (std::holds_alternative<std::string>(data)) {
			if (strMessageCallback) { strMessageCallback(std::get<std::string>(data)); }

			std::cout << "> " << remoteID_ << ": ";
			std::cout << std::get<std::string>(data) << std::endl;
		} else {
			if (binMessageCallback) { binMessageCallback(std::get<rtc::binary>(data)); }

			std::cout << "> " << remoteID_ << ": (binary) size=";
			std::cout << std::get<rtc::binary>(data).size() << std::endl;
		}
	});
}
