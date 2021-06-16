/**
 * @file session.cc
 * ----------
 * @author Bartolone, Kai
 * @date June 2021
 */

#include "session.hpp"

kbnet::session::session(asio::ip::tcp::socket socket, tsqueue<message>& incoming,
                        std::function<void()> on_disconnect)
    : m_socket(std::move(socket)), m_incoming(std::ref(incoming)), m_on_disconnect(on_disconnect)
{
    // Begin asynchronous read and write of socket
    read_header();
    m_stop_writing = false;
    m_writer = std::thread(&session::write_async, this);
}

kbnet::session::~session() { stop(); }

void kbnet::session::write_async()
{
    for (;;)
    {
        // Wait for outgoing message to be added
        m_write_sm.wait();
        if (m_stop_writing) break;
        send(m_outgoing.front());
        m_outgoing.pop_front();
    }
}

void kbnet::session::push(const message& msg)
{
    // Add message and signal that message is added
    m_outgoing.push_back(msg);
    m_write_sm.signal();
}

bool kbnet::session::send(const message& msg)
{
    std::unique_lock<std::mutex> lg(m_write_mutex);

    // Write header and message to socket
    asio::error_code ec;
    asio::write(m_socket, asio::buffer(&msg.header, HEADER_SIZE), ec);
    if (!ec && msg.size() > 0)
    {
        std::cout << "[SESSION] writes header" << std::endl;
        asio::write(m_socket, asio::buffer(msg.data(), msg.size()), ec);
        if (!ec)
        {
            std::cout << "[SESSION] writes " << msg.size() << " body bytes" << std::endl;
            return true;
        }
    }
    return false;
}

void kbnet::session::read_header()
{
    asio::async_read(m_socket, asio::buffer(&m_temp_msg.header, HEADER_SIZE),
                     [this](std::error_code ec, std::size_t length)
                     {
                         if (!ec)
                         {
                             std::cout << "[SESSION] read header" << std::endl;
                             if (m_temp_msg.sizeh() > 0)
                             {
                                 m_temp_msg.resize();
                                 read_body();
                             }
                         }
                         else
                         {
                             stop(ec);
                         }
                     });
}

void kbnet::session::read_body()
{
    asio::async_read(m_socket, asio::buffer(m_temp_msg.body.data(), m_temp_msg.size()),
                     [this](std::error_code ec, std::size_t length)
                     {
                         if (!ec)
                         {
                             std::cout << "[SESSION] read " << length << " body bytes" << std::endl;
                             m_incoming.push_back(m_temp_msg);
                             read_header();
                         }
                         else
                         {
                             stop(ec);
                         }
                     });
}

void kbnet::session::stop(std::error_code ec)
{
    // Stop writing
    m_stop_writing = true;
    m_write_sm.signal();

    // Close socket
    if (valid())
    {
error_handler(ec);
        m_socket.close();
        m_on_disconnect();
    }

    // Join writing thread
    if (m_writer.joinable()) m_writer.join();
}

void kbnet::session::stop()
{
    // Stop writing
    m_stop_writing = true;
    m_write_sm.signal();

    // Close socket
    if (valid())
    {
        m_socket.close();
        m_on_disconnect();
    }

    // Join writing thread
    if (m_writer.joinable()) m_writer.join();
}

void kbnet::session::error_handler(std::error_code ec)
{
    std::cout << "[CLIENT] error: " << ec.message() << std::endl;
}