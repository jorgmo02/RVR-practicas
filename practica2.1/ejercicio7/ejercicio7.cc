#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include "SocketThread.h"

#define BUFFLEN 80
#define NUMCLIENTS 5

int main(int argc, char** argv)
{
    addrinfo hints;      // opciones de filtrado
    addrinfo* res;       // variable para almacenar el resultado de getaddrinfo

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_socktype = SOCK_STREAM;        // tcp
    hints.ai_family = AF_INET;

    int rgetaddr = getaddrinfo(argv[1], argv[2], &hints, &res);
    if(rgetaddr != 0) {
        std::cerr << "ERROR: getaddrinfo failed with errorcode: " << rgetaddr << "\n";
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if(sd == -1) {
        std::cerr << "ERROR: socket creation failed\n";
        return -1;
    }

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    if (listen(sd, 10) != 0)    // el tam maximo de la cola de conexiones pendientes es 10 (man 2 listen)
    {
        std::cout << "ERROR: listen failed with errno: " << errno << "\n";
        return -1;
    }


    // threads

    while (true)
    {
        sockaddr client;
        socklen_t clientlen = sizeof(sockaddr);
        
        // se cierra en el thread
        int socketClient = accept(sd, (sockaddr *) &client, &clientlen);
        if (socketClient == -1) {
            std::cout << "ERROR: accept failed with errno: " << errno << "\n";
            return -1;
        }

        SocketThread* aux = new SocketThread(socketClient);

        std::thread t ([&aux, &client, clientlen]()
        {
            char host[NI_MAXHOST];
            char service[NI_MAXSERV];
            getnameinfo(&client, clientlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            std::cout << "Conexión desde " << host << " " << service << "\n";
            
            aux->controlMessage();
            delete aux;
            aux = nullptr;
        });
        t.detach();
    }
    
    close(sd);

    return 0;
}