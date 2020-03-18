#include <system_error>


#define SOCKET_FILENAME     "game.sock"
#define LISTEN_BACKLOG      5


class system_error : public std::system_error
{
public:
    system_error() :std::system_error() { }
};

