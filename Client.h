#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client {
public:
    void send_request(const std::string& address, const std::string& request, int port);
    void add_files_to_server(const std::string& directory, const std::string& address, int port);
};

#endif
