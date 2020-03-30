#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include "socket_games.h"


void doit()
{
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1) {
        throw system_error();
    }

    struct sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strcpy(addr.sun_path, SOCKET_FILENAME);
    std::remove(addr.sun_path);
    unsigned long size = sizeof(addr.sun_family) + std::strlen(addr.sun_path);
    int result = bind(s, reinterpret_cast<struct sockaddr*>(&addr), size);
    if( result == -1 ) {
        throw system_error();
    }

    result = listen(s, LISTEN_BACKLOG);
    if( result == -1 ) {
        throw system_error();
    }

    while( true ) {
        struct sockaddr remote_addr{};
        socklen_t size2 = sizeof(remote_addr);
        int s2 = accept(s, reinterpret_cast<struct sockaddr*>(&remote_addr), &size2);
        if( s2 == -1 ) {
            throw system_error();
        }
        std::cout << "Connected ..." << std::endl;

        while( true ) {
            char buff[120];
            int n = recv(s2, buff, sizeof(buff) - 20, 0);
            if( n <= 0 ) {
                if( n < 0 ) {
                    throw system_error();
                }
                std::cout << "client disconnected" << std::endl;
                break;
            }
            buff[n] = 0x00;
            std::cout << "received: " << buff << std::endl;

            n = send(s2, buff, n, 0);
            if( n < 0 ) {
                throw system_error();
            }
        }
    }
}


int main() {
    try {
        doit();
    }
    catch( system_error& x ) {
        std::cout <<
            "Caught system_error exception: " << x.code() << std::endl <<
            "meaning: " << x.what() << std::endl;
    }
    return 0;
}
