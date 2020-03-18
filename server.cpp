#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdio>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_FILENAME     "dump.sock"
#define LISTEN_BACKLOG      5

class system_error : public std::system_error
{
public:
    system_error() :std::system_error() { }
};


class quick_fmt : protected std::ostringstream
{
public:
    quick_fmt() : std::ostringstream{}
    { }

    template <typename ty>
    quick_fmt& operator<<(ty o) {
        std::ostringstream::operator<<(o);
        return *this;
    }
    operator std::string() { return this->str(); } // NOLINT(google-explicit-constructor)
};


int doit()
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
            char* buff[100];
            int n = recv(s2, buff, sizeof(buff), 0);
            if( n <= 0 ) {
                if( n < 0 ) {
                    throw system_error();
                }
                break;
            }

            n = send(s2, buff, n, 0);
            if( n < 0 ) {
                throw system_error();
            }
        }

    }


    return 0;
}


void echo(std::string s)
{
    std::cout << s << std::endl;
}


int main() {
    try {
        return doit();
    }
    catch( system_error& x ) {
        std::cout <<
            "Caught system_error exception: " << x.code() << std::endl <<
            "meaning: " << x.what() << std::endl;
    }
}

