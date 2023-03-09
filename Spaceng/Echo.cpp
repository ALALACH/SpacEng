#include "PCH.h"
#include <asio.hpp>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

class session : public std::enable_shared_from_this<session> {
    using tcp = asio::ip::tcp;

public:
    explicit session(asio::io_context& io_context);
    void start();
    tcp::socket& socket();
    void do_read();
    std::size_t do_handle(std::size_t read_len);
    void do_write(std::size_t write_len);
    void simulate_work();

    static const int max_len = 1024;
    static const int max_time = 5;

private:
    tcp::socket socket_;
    char read_data_[max_len] = {};
    char write_data_[max_len] = {};
};

class server {
    using tcp = asio::ip::tcp;

public:
    server(asio::io_context& io_context, short port);
    void do_accept(asio::io_context& io_context);

private:
    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: async_tcp_echo_server <port>" << std::endl;
        return 1;
    }

    try {
        asio::io_context io_context;
        server s(io_context, std::atoi(argv[1]));

        int concurrency_num = std::thread::hardware_concurrency() > 1
            ? static_cast<int>(std::thread::hardware_concurrency() - 1)
            : 2;
        std::vector<std::thread> v;
        v.reserve(concurrency_num);

        using AsioRun = asio::io_context::count_type(asio::io_context::*)();
        for (int i : std::ranges::iota_view{ 0, concurrency_num })
            v.emplace_back(static_cast<AsioRun>(&asio::io_context::run),
                std::ref(io_context));
        std::for_each(v.begin(), v.end(), std::mem_fn(&std::thread::join));
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

session::session(asio::io_context& io_context)
    : socket_(io_context)
{
}

void session::start() { do_read(); }

auto session::socket() -> asio::ip::tcp::socket& { return socket_; }

void session::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(
        asio::buffer(read_data_, max_len),
        [this, self](std::error_code ec, std::size_t read_len) {
            if (!ec) {
                auto write_len = do_handle(read_len);
                simulate_work();
                do_write(write_len);
            }
        });
}

size_t session::do_handle(std::size_t read_len)
{
    std::cout << "Read: " << read_len << std::endl;
    strncpy(write_data_, read_data_, read_len);
    return read_len;
}

void session::do_write(std::size_t write_len)
{
    auto self(shared_from_this());
    socket_.async_write_some(
        asio::buffer(write_data_, write_len),
        [this, self](std::error_code ec, std::size_t write_len) {
            if (!ec) {
                std::cout << "Write: " << write_len << std::endl;
                do_read();
            }
        });
}

void session::simulate_work()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> dist(0, max_time);
    std::this_thread::sleep_for(std::chrono::seconds(dist(mt)));
}

server::server(asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
    do_accept(io_context);
}

void server::do_accept(asio::io_context& io_context)
{
    auto conn = std::make_shared<session>(io_context);
    acceptor_.async_accept(conn->socket(),
        [this, conn, &io_context](std::error_code ec) {
            if (!ec) {
                conn->start();
            }

            do_accept(io_context);
        });
}