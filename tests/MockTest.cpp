#include <limits.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include "ISession.h"
#include "jobs/Ping.h"
#include "jobs/Echo.h"

using ::testing::AtLeast; 

class MockSession : public ISession {
 public:
  MOCK_METHOD0(socket, boost::asio::ip::tcp::socket&());
  MOCK_METHOD0(start, void());
  MOCK_METHOD1(respond, void(const std::string response));
  MOCK_METHOD0(uuid, boost::uuids::uuid());
  MOCK_METHOD0(terminate, void());
 protected:
  MOCK_METHOD2(handle_read, void(const boost::system::error_code& error, size_t bytes_transferred));
  MOCK_METHOD1(handle_write, void(const boost::system::error_code& error));  
};

class MockTest : public ::testing::Test {
protected:
	virtual void setUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(MockTest, ping_command) {
	MockSession mockSession;
	PingJob* ping = new PingJob(&mockSession);

	EXPECT_CALL(mockSession, respond("Pong\n"))
           .Times(AtLeast(1));
	
	ping->execute();
}

TEST_F(MockTest, echo_command) {
	MockSession mockSession;
	EchoJob* echo = new EchoJob(&mockSession, "Hello World");

	EXPECT_CALL(mockSession, respond("Hello World\n"))
           .Times(AtLeast(1));
	
	echo->execute();
}

TEST_F(MockTest, echo_command_unicode) {
	MockSession mockSession;

	const std::string testString = "Testing «ταБЬℓσ»: 1<2 & 4+1>3, now 20% off!";
	EchoJob* echo = new EchoJob(&mockSession, testString);

	EXPECT_CALL(mockSession, respond(testString + "\n"))
           .Times(AtLeast(1));
	
	echo->execute();
}
