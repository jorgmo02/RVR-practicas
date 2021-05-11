#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv)
{
    struct addrinfo hints;
    struct addrinfo* res;

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;

    int resGetAddr = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(resGetAddr != 0) // ha habido un error
    {
        std::cerr << "ERROR: getaddrinfo failed with errorcode: " << resGetAddr << "\n";
        return -1;
    }

    for(addrinfo* i = res; i != nullptr; i = i->ai_next) 
    {
        char host[NI_MAXHOST];
        char server[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST,
                    server, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Host: " << host << "      Port: " << server << "\n";
    }

    freeaddrinfo(res);

    return 0;
}