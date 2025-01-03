#include "Client.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <filesystem>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
namespace fs = std::filesystem;

void Client::send_request(const std::string& address, const std::string& request, int port) {
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

    result = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &addr);
    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << result << std::endl;
        WSACleanup();
        return;
    }

    SOCKET connect_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (connect_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(addr);
        WSACleanup();
        return;
    }

    result = connect(connect_socket, addr->ai_addr, (int)addr->ai_addrlen);
    if (result == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(connect_socket);
        freeaddrinfo(addr);
        WSACleanup();
        return;
    }

    freeaddrinfo(addr);

    send(connect_socket, request.c_str(), request.size(), 0);

    char buffer[1024] = { 0 };
    result = recv(connect_socket, buffer, sizeof(buffer), 0);
    if (result > 0) {
        std::cout << "Server response: " << std::string(buffer, result) << std::endl;
    }

    closesocket(connect_socket);
    WSACleanup();
}

void Client::add_files_to_server(const std::string& directory, const std::string& address, int port) {
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path());
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            send_request(address, "ADD_FILE " + content, port);
        }
    }
}
