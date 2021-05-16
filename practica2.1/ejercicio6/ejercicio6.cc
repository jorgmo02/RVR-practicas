#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include "SocketThread.h"

#define NUMCLIENTS 3

int main(int argc, char** argv)
{
    addrinfo hints;      // opciones de filtrado
    addrinfo* res;       // variable para almacenar el resultado de getaddrinfo

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;

    int resGetAddr = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(resGetAddr != 0) // ha habido un error
    {
        std::cerr << "ERROR: getaddrinfo failed with errorcode: " << resGetAddr << "\n";
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if(sd == -1) {
        std::cerr << "ERROR: socket creation failed\n";
        return -1;
    }

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    SocketThread* scktthreads[NUMCLIENTS];
    
    for(int i = 0; i < NUMCLIENTS; i++)
    {
        SocketThread* aux = new SocketThread(sd);
        std::thread t ([&aux]() {
            aux->controlMessage();
        });
        t.detach();
        scktthreads[i] = aux;
    }

    char input[BUFFLEN];
    do {
        std::cin >> input;
    } while (!(strlen(input) == 1 && input[0] == 'Q'));

    for(int i = 0; i < NUMCLIENTS; i++) delete scktthreads[i];
    close(sd);

    return 0;
}