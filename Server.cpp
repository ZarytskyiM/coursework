#include "Server.h"
#include "ThreadPool.h" 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <functional> 

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 1024

void handle_client(SOCKET client_socket, InvertedIndex& index) {
    char buffer[BUFFER_SIZE] = { 0 };
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        std::string request(buffer, bytes_received);

        std::istringstream iss(request);
        std::string command;
        iss >> command;

        if (command == "SEARCH") {
            std::string word;
            iss >> word;
            auto results = index.search(word);

            std::ostringstream oss;
            for (int doc_id : results) {
                oss << doc_id << " ";
            }
            std::string response = oss.str() + "\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
        else if (command == "ADD_FILE") {
            std::string content;
            std::getline(iss, content);
            static int doc_id = 1;
            index.add_document(doc_id++, content);
            std::string response = "File added to index.\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
    }

    closesocket(client_socket);
}

void Server::start_server(const std::string& address, int port, InvertedIndex& index) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return;
    }

    struct addrinfo* addr = nullptr, hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &addr);
    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << result << std::endl;
        WSACleanup();
        return;
    }

    SOCKET listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(addr);
        WSACleanup();
        return;
    }

    result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);
    if (result == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(addr);
        closesocket(listen_socket);
        WSACleanup();
        return;
    }

    freeaddrinfo(addr);

    result = listen(listen_socket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return;
    }

    std::cout << "Server listening on " << address << ":" << port << "...\n";

 
    ThreadPool pool(4); 

    while (true) {
        SOCKET client_socket = accept(listen_socket, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            closesocket(listen_socket);
            WSACleanup();
            return;
        }


        pool.enqueue_task([client_socket, &index]() {
            handle_client(client_socket, index);
            });
    }

    closesocket(listen_socket);
    WSACleanup();
}
