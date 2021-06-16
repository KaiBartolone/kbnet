/**
 * @file session.hpp
 * ----------
 * @author Bartolone, Kai
 * @date June 2021
 */

#ifndef _SESSION_HPP
#define _SESSION_HPP

#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include "message.h"
#include <asio.hpp>
#include <functional>
#include <semaphore.h>
#include <tsqueue.h>

// using namespace boost;
namespace kbnet
{

// DISCLAIMER: I assume socket is thread safe. if not this is problematic code

/**
 * @class session
 * ----------
 * @description: Defines a wrapper for a socket that can read and write message structs with thread
 * safety.
 */
class session
{
  public:
    session(asio::ip::tcp::socket socket, tsqueue<message>& incoming,
            std::function<void()> on_disconnect);
    ~session();

    /**
     * @fn push
     * ----------
     * @brief: Pushes a message to session's internal queue that will be written.
     */
    void push(const message& msg);

    /**
     * @fn send
     * ----------
     * @brief: Sends a message and blocks until the message is send or an error is received.
     */
    bool send(const message& msg);

    /**
     * @fn valid
     * ----------
     * @brief: Checks if socket is still open. Once invalid a session cannot become valid.
     */
    bool inline valid() { return m_socket.is_open(); }

  protected:
    /**
     * @fn write
     * ----------
     * @brief: Writes to the socket until stop is called and blocks when no messages are in the
     * outgoing queue.
     */
    void write_async();

    /**
     * @fn read_header
     * ----------
     * @brief: Asynchronously reads header or error and calls read_body on success.
     */
    void read_header();

    /**
     * @fn read_body
     * ----------
     * @brief: Asynchronously reads body or error and calls read_header on success. Writes message
     * to incoming.
     */
    void read_body();

    /**
     * @fn stop
     * ----------
     * @brief: Stops reading and writing, closes socket, invockes error_handler, and waits on
     * writing thread.
     */
    void stop();
    void stop(std::error_code ec);

    /**
     * @fn error_handler
     * ----------
     * @brief: Called when session reads an error.
     */
    void error_handler(std::error_code ec);

  protected:
    std::atomic_bool m_stop_writing;
    kbthread::semaphore m_write_sm;
    std::thread m_writer;
    std::mutex m_write_mutex;

    tsqueue<message>& m_incoming;
    tsqueue<message> m_outgoing;
    message m_temp_msg;

    asio::ip::tcp::socket m_socket;
    // std::function<void(std::error_code ec)> m_error_handler;
    std::function<void()> m_on_disconnect;
};
} // namespace kbnet
#endif