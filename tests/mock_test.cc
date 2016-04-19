#include <limits.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include "isession.h"
#include "jobs/ping.h"
#include "jobs/echo.h"

#include "query_result.h"

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
  std::shared_ptr<ISession> mockSession = std::make_shared<MockSession>();
  PingJob* ping = new PingJob(mockSession);

  QueryResult result = ping->execute();

  EXPECT_EQ(result.resultDataText(), "PONG");
}

TEST_F(MockTest, echo_command) {

	std::shared_ptr<ISession> mockSession = std::make_shared<MockSession>();
  EchoJob* echo = new EchoJob(mockSession, "Hello World");
  
  QueryResult result = echo->execute();

  EXPECT_EQ(result.resultDataText(), "Hello World");
}

TEST_F(MockTest, echo_command_unicode) {
	std::shared_ptr<ISession> mockSession = std::make_shared<MockSession>();

  const std::string testString = "Testing «ταБЬℓσ»: 1<2 & 4+1>3, now 20% off!";
  EchoJob* echo = new EchoJob(mockSession, testString);

	QueryResult result = echo->execute();

  EXPECT_EQ(result.resultDataText(), "Testing «ταБЬℓσ»: 1<2 & 4+1>3, now 20% off!");
}
