#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <experimental/coroutine>
#include <utility>
#include <queue>
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

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::redirect_error;
using boost::asio::use_awaitable;


class Stash;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, Stash &stash);
    
    void start();
    ~Session();

private:
    awaitable<void> reader();

    awaitable<void> writer();

    void stop();

    tcp::socket socket_;
    boost::asio::steady_timer timer_;
    Stash &stash_;
    std::queue<std::string> write_msgs_;
};


class Stash {
    typedef std::shared_ptr<Session> session_ptr;
public:
    void join(session_ptr session);

    void leave(session_ptr session);

    void deliver(const std::string &msg);

private:
    std::unordered_set<session_ptr> sessions_;
};