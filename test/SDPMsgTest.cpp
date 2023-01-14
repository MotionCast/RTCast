/** @file
 * @author Jacob SP <jacob@moca.st>
 */

#include <string>

#include <gtest/gtest.h>

#include "rtcast/SDPMsg.h"
#include "rtc/rtc.hpp"

/**
 * @brief Test the `IsCandidate()` function of the `SDPMsg` class.
 *
 * This test creates an instance of the `SDPMsg` class using a `rtc::Candidate`
 * object and a remote ID, and then checks that the `IsCandidate()` function
 * returns true.
 */
TEST(SDPMsgTest, IsCandidate) {
	rtc::Candidate c;
	mocast::SDPMsg msg(c, "remoteID");
	EXPECT_TRUE(msg.IsCandidate());
}

/**
 * @brief Test the `IsOffer()` function of the `SDPMsg` class.
 *
 * This test creates an instance of the `SDPMsg` class using a `rtc::Description`
 * object with type "offer" and a remote ID, and then checks that the `IsOffer()`
 * function returns true.
 */
TEST(SDPMsgTest, IsOffer) {
	std::string sdp;
	rtc::Description desc(sdp, "offer");
	mocast::SDPMsg msg(desc, "remoteID");
	EXPECT_TRUE(msg.IsOffer());
}

/**
 * @brief Test the `IsAnswer()` function of the `SDPMsg` class.
 *
 * This test creates an instance of the `SDPMsg` class using a `rtc::Description`
 * object with type "answer" and a remote ID, and then checks that the `IsAnswer()`
 * function returns true.
 */
TEST(SDPMsgTest, IsAnswer) {
	std::string sdp;
	rtc::Description desc(sdp, "answer");
	mocast::SDPMsg msg(desc, "remoteID");
	EXPECT_TRUE(msg.IsAnswer());
}

/**
 * @brief Test the `to_json()` and `from_json()` functions of the `SDPMsg` class.
 *
 * This test creates an instance of the `SDPMsg` class using a `rtc::Description`
 * object and a remote ID, and then checks that the member variables of the
 * original object and the deserialized object match after serializing and
 * deserializing using `nlohmann::json`.
 */
TEST(SDPMsgTest, DescriptionJsonSerialization) {
	std::string sdp;
	rtc::Description desc(sdp, "offer");
	mocast::SDPMsg msg(desc, "remoteID");
	nlohmann::json j = msg;

	mocast::SDPMsg msg2;
	msg2 = j.get<mocast::SDPMsg>();

	EXPECT_EQ(msg.id, msg2.id);
	EXPECT_EQ(msg.type, msg2.type);
	EXPECT_EQ(msg.description, msg2.description);
}

/**
* @brief Test the `to_json()` and `from_json()` functions of the `SDPMsg` class when created with `rtc::Candidate`.
*
* This test creates an instance of the `SDPMsg` class using a `rtc::Candidate`
* object and a remote ID, and then checks that the member variables of the
* original object and the deserialized object match after serializing and
* deserializing using `nlohmann::json`.
*/
TEST(SDPMsgTest, CandidateJsonSerialization) {
	// Need a valid SDP candidate or an exception is thrown
	rtc::Candidate c("candidate:1 1 UDP 2122252543 10.0.0.1 1234 typ host generation 0", "mid-test");
	mocast::SDPMsg msg(c, "remoteID");
	nlohmann::json j = msg;

	mocast::SDPMsg msg2;
	msg2 = j.get<mocast::SDPMsg>();

	EXPECT_EQ(msg.id, msg2.id);
	EXPECT_EQ(msg.type, msg2.type);
	EXPECT_EQ(msg.candidate, msg2.candidate);
	EXPECT_EQ(msg.mid, msg2.mid);
}
