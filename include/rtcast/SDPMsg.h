/** @file
 * @author Jacob SP <jacob@moca.st>
 */

#ifndef SDP_MSG_H
#define SDP_MSG_H

#include <string>
#include "rtc/rtc.hpp"
#include <nlohmann/json.hpp>


namespace mocast {
	/**
	 * @brief Wraps `rtc::Description` and `rtc::Candidate` for JSON conversion.
	 * @see rtc::Description
	 * @see rtc::Candidate
	 */
	struct SDPMsg {
		SDPMsg() : id{""}, type{""} {}
		SDPMsg(rtc::Description& desc, std::string remoteID) :
				id{remoteID},
				type{desc.typeString()},
				description{std::string(desc)} {}

		SDPMsg(rtc::Candidate& c, std::string remoteID) :
				id{remoteID},
				type{"candidate"},
				candidate{std::string(c)},
				mid{c.mid()} {}

		inline bool IsCandidate() const {
			return type == "candidate";
		}

		inline bool IsOffer() const {
			return type == "offer";
		}

		inline bool IsAnswer() const {
			return type == "answer";
		}

		std::string id;
		std::string type;
		std::string description = "";
		std::string candidate = "";
		std::string mid = "";
	};

	void to_json(nlohmann::json& j, const SDPMsg& msg);
	void from_json(const nlohmann::json& j, SDPMsg& msg);
}

#endif
