/**
 * @file server_interface.hpp
 * ----------
 * @author Bartolone, Kai
 * @date June 2021
 */

#ifndef _SERVER_INTERFACE
#define _SERVER_INTERFACE

#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include "message.h"
#include "session.hpp"
#include "tsqueue.h"
#include <asio.hpp>
#include <atomic>
#include <mutex>
#include <thread>

namespace kbnet
{
/**
 * @class server_interface
 * ----------
 * @brief: Continously listens on a port and makes connections
 */
class server_interface
{
  public:
    server_interface(uint16_t port);
    ~server_interface();

    /**
     * @fn start
     * ----------
     * @brief: Starts listening and accepting connections on specified port.
     */
    void start();

    /**
     * @fn stop
     * ----------
     * @brief: Stops listening on port and accepting connections.
     */
    void stop();

    /**
     * @fn has_connection
     * ----------
     * @brief: Checks if session is valid.
     */
    bool has_connection();

    /**
     * @fn send
     * ----------
     * @brief: Sends message blocks until sent.
     */
    bool send(const message& msg);

    /**
     * @fn push
     * ----------
     * @brief: Pushes message to session's internal queue.
     */
    void push(const message& msg);

    void on_disconnect() {}
    void on_connect() {}

  protected:
    /**
     * @fn accept
     * ----------
     * @brief: Continuously makes connections to any thing that connects to it.
     */
    void accept();

    /**
     * @fn release
     * ----------
     * @brief: Destroys current session.
     */
    void release();

  public:
    tsqueue<message> m_incoming;

  protected:
    std::atomic_bool accepting_connections;

    asio::io_context m_context;
    std::thread m_context_thread;

    session* m_session = nullptr;
    std::recursive_mutex session_mutex;

    asio::ip::tcp::acceptor m_acceptor; // Handles new incoming connection attempts...
};
} // namespace kbnet
#endif