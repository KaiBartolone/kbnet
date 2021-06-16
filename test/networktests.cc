#include "client_interface.hpp"
#include "server_interface.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

using namespace kbnet;

constexpr uint16_t DEFAULT_PORT = 80;

struct net_info_t
{
    bool has_connection;
};

class mock_client : public client_interface
{
  public:
    mock_client() : client_interface() {}
    // void on_connect() override { std::cout << "[Client] on_connect called" << std::endl; }
    // void on_disconnect() override { std::cout << "[Client] on_disconnect called" << std::endl; }

  private:
    net_info_t data;
};

class mock_server : public server_interface
{
  public:
    mock_server(uint16_t port) : server_interface(port) {}
    // void on_disconnect() override { std::cout << "[Server] on_disconnect called" << std::endl; }

  private:
    net_info_t data;
};

class network_test : public ::testing::Test
{
  protected:
    network_test() : ms(DEFAULT_PORT) {}

    void SetUp() override
    {
        stdmsg.header.id = MESSAGE_TYPE::COMMAND;
        stdmsg << 5 << 10 << 15;
        // ms(1234);
    }

    void sleep(int sec) { std::this_thread::sleep_for(std::chrono::milliseconds(sec)); }

    void sleep() { sleep(20); }

    // void TearDown() override {}
    message stdmsg;
    mock_server ms;
    mock_client mcl;
};

// has no connection on startup
TEST_F(network_test, no_connection_on_startup)
{
    ASSERT_EQ(ms.has_connection(), false);
    ASSERT_EQ(mcl.has_connection(), false);
}

// Accept connection
TEST_F(network_test, client_connect_disconnect)
{
    // mock_server s(80);
    ms.start();

    mcl.connect("localhost", DEFAULT_PORT);
    sleep();
    ASSERT_EQ(ms.has_connection(), true);

    mcl.close();
    sleep();
    ASSERT_EQ(ms.has_connection(), false);
    ms.stop();
}

// Receive message, send message
TEST_F(network_test, basic_send_receive1)
{
    ms.start();
    mcl.connect("localhost", DEFAULT_PORT);
    sleep(500);
    mcl.send(stdmsg);
    sleep(1000);
    ASSERT_EQ(ms.m_incoming.size(), 1);
}

TEST_F(network_test, basic_send_receive2)
{
    ms.start();
    mcl.connect("localhost", DEFAULT_PORT);
    mcl.send(stdmsg);
    mcl.send(stdmsg);
    mcl.send(stdmsg);
    mcl.send(stdmsg);
    sleep(1000);
    ASSERT_EQ(ms.m_incoming.size(), 4);
}
// Do add messages to queue when there is no connection
// Accept multiple connections
// Disconnect handler tests
// close connection while writing