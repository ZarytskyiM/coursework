#include "Server.h"
#include "Client.h"
#include "InvertedIndex.h"
#include <thread>
#include <iostream>

void run_cpp_client(const std::string& directory, const std::string& address, int port) {
    Client client;
    std::cout << "Starting C++ client to upload files from: " << directory << std::endl;
    client.add_files_to_server(directory, address, port);
    std::cout << "C++ client: All files uploaded successfully." << std::endl;
}

int main() {
    InvertedIndex index;

    std::thread server_thread([&]() {
        Server server;
        server.start_server("127.0.0.1", 8080, index);
        });

    // Затримка для старту сервера
    std::this_thread::sleep_for(std::chrono::seconds(1));

    run_cpp_client("neg", "127.0.0.1", 8080);

    Client client;
    client.send_request("127.0.0.1", "SEARCH character", 8080);

    std::cout << "Python client: Please execute the Python script to upload files and test search." << std::endl;

    server_thread.join();

    return 0;
}
