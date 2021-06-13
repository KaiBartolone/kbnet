/**
 * @file client_interface.cc
 * ----------
 * @author Bartolone, Kai
 * @date June 2021
 */

#include "client_interface.hpp"

kbnet::client_interface::client_interface()
{
    // Start context thread
    m_context_thread = std::thread([this]() { m_context.run(); });
}

kbnet::client_interface::~client_interface() { close(); }

bool kbnet::client_interface::connect(const std::string& host, const uint16_t port)
{
    try
    {
        // Resolve hostname/ip-address into tangiable physical address
        asio::ip::tcp::resolver resolver(m_context);
        asio::ip::tcp::resolver::results_type endpoints =
            resolver.resolve(host, std::to_string(port));
        connect(endpoints);
    }
    catch (std::exception& e)
    {
        std::cerr << "[CLIENT] Exception: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void kbnet::client_interface::close()
{
    std::cout << "[CLIENT] closing" << std::endl;
    // Release previous session
    release();

    // Join context thread which should have no jobs
    if (m_context_thread.joinable()) m_context_thread.join();
}

bool kbnet::client_interface::send(const message& msg)
{
    std::cout << "[CLIENT] sending" << std::endl;
    if (has_connection())
    {
        return m_session->send(msg);
    }
    return false;
}

void kbnet::client_interface::push(const message& msg)
{
    std::cout << "[CLIENT] pushing" << std::endl;
    if (has_connection())
    {
        m_session->push(msg);
    }
}

void kbnet::client_interface::error_handler(std::error_code ec)
{
    std::unique_lock<std::mutex> handler_mutex;
    std::cout << "[CLIENT] error: " << ec.message() << std::endl;
}

bool kbnet::client_interface::connect(const asio::ip::tcp::resolver::results_type& endpoints)
{
    std::cout << "[Client] connecting " << std::endl;
    release();

    asio::ip::tcp::socket socket(m_context);
    asio::error_code ec;

    // Request asio attempts to connect to an endpoint
    asio::connect(socket, endpoints, ec);
    if (!ec)
    {
        std::function<void(std::error_code ec)> handler =
            std::bind(&client_interface::error_handler, this, std::placeholders::_1);
        m_session = new session(std::move(socket), m_incoming, handler);
    }
    else
    {
        std::cout << "error" << std::endl;
        return false;
    }
    return true;
}

void kbnet::client_interface::release()
{
    if (m_session == nullptr) return;
    delete m_session;
    m_session = nullptr;
}