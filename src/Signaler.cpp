#include "Signaler.h"

#include <string>
#include <memory>
#include <cstddef>
#include <iostream>
#include <variant>
#include <stdexcept>
#include <string_view>

#include "rtc/rtc.hpp"
#include "nlohmann/json.hpp"

#include "ConnectionMode.h"
#include "SDPMsg.h"

using namespace std;
using namespace mocast;

Signaler::Signaler(ConnectionMode mode, const string_view host, const string_view remoteID) :
	host_{host}, remoteID_{remoteID}, mode_{mode}, socket{make_unique<rtc::WebSocket>()} {

	// Define behaviour for WebSocket
	socket->onOpen([this]() {
		cout << "Connected to server." << endl;
		socketPromise.set_value();
	});

	socket->onClosed([]() {
		cout << "Connection to server closed." << endl;
	});

	socket->onError([this](const string& error) {
		cerr << "Error connecting to server: " << error << endl;
		cerr << "Is the signaling server up and running?" << endl << endl;
		socketPromise.set_exception(make_exception_ptr(runtime_error(error)));
	});

	socket->onMessage([this](auto data) {
		// Receive incoming client descriptions if connection mode is SURRENDER
		// Ignore non-string messages
		if (!holds_alternative<string>(data)) { return; }

		// Parse string -> json -> SDPMsg
		nlohmann::json jsonMsg = nlohmann::json::parse(get<string>(data));
		auto sdpMsg = jsonMsg.get<SDPMsg>();

		// Ignore messages from other remotes
		if(sdpMsg.id != remoteID_) { return; }

		if (messageCallback) {
			messageCallback(sdpMsg);
		}
	});
}

void Signaler::Open() {
	cout << "Opening WebSockets connection to " << host_ << endl;
	socket->open(host_);
}

bool Signaler::Send(const variant<rtc::binary, string> data) {
	return socket->send(data);
}

bool Signaler::IsOpen() const {
	return socket->isOpen();
}

bool Signaler::IsClosed() const {
	return socket->isClosed();
}

future<void> Signaler::SocketFuture() {
	return socketPromise.get_future();
}

void Signaler::OnReply(function<void(SDPMsg)> callback) {
	messageCallback = callback;
}
