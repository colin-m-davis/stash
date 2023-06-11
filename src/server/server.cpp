#include <iostream>
#include <boost/asio.hpp>
#include <experimental/coroutine>
#include <utility>
#include <unordered_set>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include "server.h"

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::redirect_error;
using boost::asio::use_awaitable;


void Stash::join(session_ptr session)
{
    sessions_.insert(session);
}

void Stash::leave(session_ptr session)
{
    sessions_.erase(session);
}

void Stash::deliver(const std::string &msg) {
    std::cout << msg << '\n';
}


Session::Session(tcp::socket socket, Stash &stash)
    :   socket_(std::move(socket)),
        timer_(socket_.get_executor()),
        stash_(stash)
{
    timer_.expires_at(std::chrono::steady_clock::time_point::max());
}

void Session::start()
{
    stash_.join(shared_from_this());

    co_spawn(
        socket_.get_executor(),
        [self = shared_from_this()]{ return self->reader(); },
        detached
    );

    co_spawn(
        socket_.get_executor(),
        [self = shared_from_this()]{ return self->writer(); },
        detached
    );
}

awaitable<void> Session::reader()
{
    try
    {
        for (std::string read_msg;;) {
            std::size_t n = co_await boost::asio::async_read_until(
                socket_,
                boost::asio::dynamic_buffer(read_msg, 1024), "\n", use_awaitable
            );
            stash_.deliver(read_msg.substr(0, n));
            read_msg.erase(0, n);
        }
    }
    catch(const std::exception& e)
    {
        stop();
    }
}

awaitable<void> Session::writer()
{
    try
    {
        while (socket_.is_open())
        {
            if (write_msgs_.empty())
            {
                boost::system::error_code ec;
                co_await timer_.async_wait(redirect_error(use_awaitable, ec));
            }
            else
            {
                co_await boost::asio::async_write(
                    socket_,
                    boost::asio::buffer(write_msgs_.front()),
                    use_awaitable
                );
                write_msgs_.pop();
            }
        }
    }
    catch(const std::exception& e)
    {
        stop();
    }
    
}

void Session::stop()
{
    stash_.leave(shared_from_this());
    socket_.close();
    timer_.cancel();
}

awaitable<void> listener(tcp::acceptor acceptor)
{
    Stash stash;

    for (;;)
    {
        std::make_shared<Session>(
            co_await acceptor.async_accept(use_awaitable),
            stash
        )->start();
    }
}

int main(int argc, char* argv[]){
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: stash_server <port> [<port> ...]\n";
            return 1;
        }
        boost::asio::io_context io_context(1);

        for (int i = 1; i < argc; ++i) {
            unsigned short port = std::atoi(argv[i]);
            co_spawn(io_context,
            listener(tcp::acceptor(io_context, {tcp::v4(), port})),
            detached);
        }

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto){io_context.stop(); });

        io_context.run();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << '\n';
    }

    return 0;
}
