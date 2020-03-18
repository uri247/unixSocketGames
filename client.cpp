#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include "socket_games.h"


void do_client() {
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1) {
        throw system_error();
    }

    std::cout << "trying to connect ..." << std::endl;

    struct sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strcpy(addr.sun_path, SOCKET_FILENAME);
    unsigned long size = sizeof(addr.sun_family) + std::strlen(addr.sun_path);
    int result = connect(s, reinterpret_cast<struct sockaddr*>(&addr), size);
    if( result == -1 ) {
        throw system_error();
    }

    std::cout << "connected" << std::endl;

    while( true ) {
        std::string str;
        char buff[120];

        std::cout << "> ";
        std::getline(std::cin, str);
        result = send(s, str.c_str(), str.length(), 0);
        if( result == -1 ) {
            throw system_error();
        }

        int t = recv(s, buff, 100, 0);
        if( t <= 0 ) {
            if( t < 0 ) {
                throw system_error();
            }
            std::cout << "Server closed connection" << std::endl;
            break;
        }
        buff[t] = 0;
        std::cout << buff << std::endl;
    }
}



int main() {
    try {
        do_client();
    }
    catch( system_error& x ) {
        std::cout <<
                  "Caught system_error exception: " << x.code() << std::endl <<
                  "meaning: " << x.what() << std::endl;
        return 1;
    }
    return 0;
}

