/**
 * @file client_interface.hpp
 * ----------
 * @author Bartolone, Kai
 * @date June 2021
 */

#ifndef _CLIENT_INTERFACE_HPP
#define _CLIENT_INTERFACE_HPP

#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include <functional>
#include <mutex>
#include <thread>
#include <asio.hpp>
#include "session.hpp"
#include "tsqueue.h"
#include "message.h"

namespace kbnet
{
/**
 * @class client_interface
 * ----------
 * @description: Defines client entity that can synchronously make connections and maintain a
 * session with such connection.
 */
class client_interface
{
  public:
    client_interface();
    ~client_interface();

    /**
     * @fn connect
     * ----------
     * @returns: True on connection false on failed to connect.
     */
    bool connect(const std::string& host, const uint16_t port);

    /**
     * @fn close
     * ----------
     * @brief: Deletes current session and and joins context thread.
     */
    void close();

    /**
     * @fn has_connectin
     * ----------
     * @brief: Checks session connectivity.
     */
    bool inline has_connection() { return m_session != nullptr && m_session->valid(); }

    /**
     * @fn push
     * ----------
     * @returns: True on message sent and false on sending error.
     */
    bool send(const message& msg);

    /**
     * @fn push
     * ----------
     * @brief: Adds message to session queue for sending. No guarantee that message reaches
     * destination.
     */
    void push(const message& msg);

    // virtual void on_disconnect() {std::cout << "tramp" << std::endl;}
    // virtual void on_connect() {}
    // virtual void process_message() {}
  protected:
    /**
     * @fn error_handler
     * ----------
     * @brief: Called when session reads an error.
     */
    void error_handler(std::error_code ec);

    /**
     * @fn error_handler
     * ----------
     * @brief: Connect but uses endpoints.
     */
    bool connect(const asio::ip::tcp::resolver::results_type& endpoints);

    /**
     * @fn error_handler
     * ----------
     * @brief: Deletes session.
     */
    void release();

  protected:
    tsqueue<message> m_incoming;

    asio::io_context m_context;
    std::thread m_context_thread;

    session* m_session = nullptr;

    std::mutex handler_mutex;
};
} // namespace kbnet
#endif