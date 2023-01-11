/** @file
 * @author Jacob SP <jacob@moca.st>
 */

#include <string>
#include "SDPMsg.h"
#include <nlohmann/json.hpp>

namespace mocast {
	void to_json(nlohmann::json& j, const SDPMsg& msg) {
		j = nlohmann::json{
			{"id", msg.id},
			{"type", msg.type}
		};

		if (msg.IsCandidate()) {
			j["candidate"] = msg.candidate;
			j["mid"] = msg.mid;
		} else {
			j["description"] = msg.description;
		}
	}

	void from_json(const nlohmann::json& j, SDPMsg& msg) {
		j.at("id").get_to(msg.id);
		j.at("type").get_to(msg.type);

		if (msg.IsCandidate()) {
			j.at("candidate").get_to(msg.candidate);
			j.at("mid").get_to(msg.mid);
		} else {
			j.at("description").get_to(msg.description);
		}
	}
}
