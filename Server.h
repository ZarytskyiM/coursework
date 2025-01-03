#ifndef SERVER_H
#define SERVER_H

#include "InvertedIndex.h"
#include <string>

class Server {
public:
    void start_server(const std::string& address, int port, InvertedIndex& index);
};

#endif
