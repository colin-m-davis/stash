#include <cstdlib>
#include <iostream>
#include <thread>
#include <mutex>
#include <utility>
#include <boost/asio.hpp>
#include <unordered_map>
#include <string>
#include <sstream>
#include <optional>
#include <query.hpp>

using boost::asio::ip::tcp;


class Stash {
public:
    [[nodiscard]] std::optional<std::string> get(const std::string &key) {
        std::lock_guard<std::mutex> guard(storage_mutex);
        auto found = storage.find(key);
        if (found == storage.cend()) {
            return {};
        }
        return found->second;
    }

    void put(const std::string &key, const std::string &val) {
        std::lock_guard<std::mutex> guard(storage_mutex);
        storage[key] = val;
    }

private:
    std::unordered_map<std::string, std::string> storage;
    std::mutex storage_mutex;
};


class Server {
public:
    Server(boost::asio::io_context &io_context, unsigned short port) {
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
        while (true) {
            std::thread(&Server::session, this, acceptor.accept()).detach();
        }
    }

private:
    Stash stash{};
    void session(tcp::socket sock) {
        try {
            while (true) {
                char query_buff[Query::MAX_LENGTH];

                boost::system::error_code error;
                sock.read_some(boost::asio::buffer(query_buff), error);

                if (error == boost::asio::error::eof) {
                    break;
                } else if (error) {
                    throw boost::system::system_error(error);
                }
                
                // TODO: data validation and scalability in command types
                std::istringstream query_iss(query_buff);
                std::string operation;
                query_iss >> operation;

                std::string key;
                query_iss >> key;

                std::string result_str = "stash: ";
                if (operation == "get") {
                    result_str += stash.get(key).value_or("error: key not in stash");
                } else if (operation == "put") {
                    std::string val;
                    query_iss >> val;
                    stash.put(key, val);
                    result_str += "put success";
                }

                boost::asio::write(sock, boost::asio::buffer(result_str.data(), result_str.length()));
            }
        } catch (std::exception &e) {
            std::cerr << "exception in thread: " << e.what() << '\n';
        }
    }
};


int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "usage: stashserver <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        Server(io_context, std::atoi(argv[1]));
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}
