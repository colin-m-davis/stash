#include <iostream>
#include <thread>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


// class Client {
// public:
//     Client(
//         boost::asio::io_context& io_context,
//         const tcp::resolver::results_type& endpoints)
//     :   io_context_(io_context),
//         socket_(io_context)
//     {
//         boost::asio::async_connect(socket_, endpoints,
//             boost::bind(&Client::handle_connect, this,
//             boost::asio::placeholders::error)
//         );
//     }

// private:
//     void handle_connect(const boost::system::error_code& error) {
//         if (!error) {
//         }
//     }
//     boost::asio::io_context& io_context_;
//     tcp::socket socket_;
// };

int main(int argc, char* argv[]) {
    try
    {
        if (argc != 3) {
            std::cerr << "Usage: client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(argv[1], argv[2]);

        std::thread t([&io_context]{ io_context.run(); });

        t.join();
    }
    catch(std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
    }
    return 0;
}