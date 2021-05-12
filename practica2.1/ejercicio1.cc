#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv)
{
    addrinfo hints;      // opciones de filtrado
    addrinfo* res;       // variable para almacenar el resultado de getaddrinfo

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AF_UNSPEC;

    int resGetAddr = getaddrinfo(argv[1], 0, &hints, &res);

    if(resGetAddr != 0) // ha habido un error
    {
        std::cerr << "ERROR: getaddrinfo failed with errorcode: " << resGetAddr << "\n";
        return -1;
    }

    for(addrinfo* i = res; i != nullptr; i = i->ai_next)
    {
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        int err = getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST,
                            service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        if (err != 0)    //gestion de errores
        {
            std::cerr << "ERROR: getnameinfo failed with errorcode: " << err << "\n";
            return -1;
        }

        std::cout << host << "      " << i->ai_family << "     " << i->ai_socktype << "\n";
    }

    freeaddrinfo(res);

    return 0;
}