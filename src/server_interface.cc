/**
 * @file server_interface.cc
 * ----------
 * @author Bartolone, Kai
 * @date June 2021
 */

#include "server_interface.hpp"

kbnet::server_interface::server_interface(uint16_t port)
    : m_acceptor(m_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
    accepting_connections = false;
}

kbnet::server_interface::~server_interface() { stop(); }

void kbnet::server_interface::start()
{
    std::cout << "[SERVER] starting" << std::endl;
    accepting_connections = true;
    accept();
    m_context_thread = std::thread([this]() { m_context.run(); });
}

void kbnet::server_interface::stop()
{
    std::cout << "[SERVER] stopping" << std::endl;
    accepting_connections = false;
    // Delete previous connection
    release();

    // Join context thread which should have no jobs
    if (m_context_thread.joinable()) m_context_thread.join();
}

bool kbnet::server_interface::has_connection()
{
    std::unique_lock<std::recursive_mutex> ul(session_mutex);
    return m_session != nullptr && m_session->valid();
}

bool kbnet::server_interface::send(const message& msg)
{
    std::unique_lock<std::recursive_mutex> ul(session_mutex);
    if (has_connection())
    {
        return m_session->send(msg);
    }
    return false;
}

void kbnet::server_interface::push(const message& msg)
{
    std::unique_lock<std::recursive_mutex> ul(session_mutex);
    if (has_connection())
    {
        m_session->push(msg);
    }
}

void kbnet::server_interface::accept()
{
    m_acceptor.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket)
        {
            std::cout << "[SERVER] accepting connection" << std::endl;

            // If we already have a connection, do nothing
            if (ec || has_connection()) return;

            // Release previous session and construct new session
            std::unique_lock<std::recursive_mutex> ul(session_mutex);
            release();
            std::function<void(std::error_code ec)> handler =
                std::bind(&server_interface::error_handler, this, std::placeholders::_1);
            m_session = new session(std::move(socket), m_incoming, handler);
        });
}

void kbnet::server_interface::release()
{
    std::unique_lock<std::recursive_mutex> ul(session_mutex);
    if (m_session == nullptr) return;
    delete m_session;
    m_session = nullptr;
}

void kbnet::server_interface::error_handler(std::error_code ec)
{
    std::unique_lock<std::mutex> handler_mutex;
    std::cout << "[CLIENT] error: " << ec.message() << std::endl;
}