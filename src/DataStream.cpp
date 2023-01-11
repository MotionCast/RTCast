#include "DataStream.h"

#include <memory>
#include <string_view>

#include "rtc/rtc.hpp"
#include <nlohmann/json.hpp>

#include "SDPMsg.h"
#include "Signaler.h"
#include "ConnectionMode.h"

using namespace std;
using namespace mocast;

using nlohmann::json;

DataStream::DataStream(ConnectionMode mode,
                       const string_view serverURL,
                       const string_view remoteID,
                       const vector<string>& iceServers) :
	// Set stream values and create Signaler object
	mode_{mode},
	remoteID_{remoteID},
	server(make_unique<Signaler>(mode, serverURL, remoteID)) {

	rtc::Configuration config;

	// Set ice servers from configuration
	for (const auto& iceServer : iceServers) {
		config.iceServers.emplace_back(iceServer);
	}

	// Set PeerConnection parameters
	peer = make_unique<rtc::PeerConnection>(config);

	peer->onStateChange(
		[](rtc::PeerConnection::State state) {
			cout << "State: " << state << endl;
		}
	);

	peer->onGatheringStateChange(
		[](rtc::PeerConnection::GatheringState state) {
			cout << "Gathering State: " << state << endl;
		}
	);

	peer->onLocalDescription(
		[this](rtc::Description description) {
			json message = SDPMsg(description, remoteID_);

			// Send description through Signaler
			cout << endl << "Sending description through Signaler." << endl;
			cout << message << endl << endl;
			server->Send(message.dump());
	});

	peer->onLocalCandidate(
		[this](rtc::Candidate candidate) {
			json message = SDPMsg(candidate, remoteID_);

			// Send candidate through Signaler
			cout << endl << "Sending candidate through Signaler." << endl;
			cout << message << endl << endl;
			server->Send(message.dump());
	});

	peer->onDataChannel(
		[this](shared_ptr<rtc::DataChannel> dc) {
			cout << "DataChannel from " << remoteID_;
			cout << " received with label \"" << dc->label() << "\"" << endl;

			channel = move(dc);
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
		channel = move(peer->createDataChannel(channelLabel_));
		SetupDataChannel();
	}
}

bool DataStream::Send(variant<rtc::binary, std::string> data) {
	if (channel) {
		return channel->send(data);
	}

	return false;
}

void DataStream::SetupDataChannel() {
	channel->onOpen([this]() {
		// Release channel open promise
		openPromise.set_value();

		cout << "DataChannel from " << remoteID_ << " open" << endl;
		channel->send("Hello, you must be " + remoteID_);
	});

	channel->onClosed([this]() {
		// Release channel closed promise
		closedPromise.set_value();

		cout << "DataChannel from " << remoteID_ << " closed" << endl;
	});

	channel->onMessage([this](auto data) {
		if (messageCallback) {
			messageCallback(data);
		}

		if (holds_alternative<std::string>(data)) {
			if (strMessageCallback) { strMessageCallback(get<string>(data)); }

			cout << "> " << remoteID_ << ": ";
			cout << get<string>(data) << endl;
		} else {
			if (binMessageCallback) { binMessageCallback(get<rtc::binary>(data)); }

			cout << "> " << remoteID_ << ": (binary) size=";
			cout << get<rtc::binary>(data).size() << endl;
		}
	});
}

future<void> DataStream::ChannelOpen() {
	return openPromise.get_future();
}

future<void> DataStream::ChannelClosed() {
	return closedPromise.get_future();
}

void DataStream::OnMessage(function<void(variant<rtc::binary, string>)> callback) {
	messageCallback = callback;
}

void DataStream::OnBinaryMessage(function<void(rtc::binary)> callback) {
	binMessageCallback = callback;
}

void DataStream::OnStringMessage(function<void(string)> callback) {
	strMessageCallback = callback;
}
