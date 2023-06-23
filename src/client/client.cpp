#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <query.hpp>
#include <response.hpp>
#include <string>

using boost::asio::ip::tcp;


int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "usage: stashclient <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        
        tcp::socket sock(io_context);
        tcp::resolver resolver(io_context);
        char *host = argv[1];
        char *port = argv[2];
        boost::asio::connect(sock, resolver.resolve(host, port));
        
        std::cout << "connected.\n";

        while (true) {
            std::cout << "enter query:\n> ";
            char query_buff[Query::MAX_LENGTH];
            std::cin.getline(query_buff, Query::MAX_LENGTH);
            std::size_t query_length = std::strlen(query_buff);

            if (query_length == 0) {
                continue;
            }
            if (query_length == 4 && std::strcmp(query_buff, "exit") == 0) {
                std::cout << "bye :)\n";
                return 0;
            }

            boost::asio::write(
                sock,
                boost::asio::buffer(query_buff, query_length
            ));

            for (std::size_t i = 0; i <= query_length; ++i) {
                query_buff[i] = '\0';
            }

            char response_buff[Response::MAX_LENGTH];
            boost::system::error_code error;
            std::size_t response_length = sock.read_some(boost::asio::buffer(response_buff), error);

            if (error == boost::asio::error::eof) {
                std::cerr << "unexpected EOF, exiting\n";
                return 1;
            } else if (error) {
                throw boost::system::system_error(error);
            }

            response_buff[response_length] = '\0';
            std::cout << response_buff << '\n';
            
        }
    }
    catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << '\n';
    }
}
