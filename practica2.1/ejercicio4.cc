#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

#define BUFFLEN 80

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

    sockaddr client;
    socklen_t clientlen = sizeof(sockaddr);
    
    int socketClient = accept(sd, (sockaddr *) &client, &clientlen);
    if (socketClient == -1) {
        std::cout << "ERROR: accept failed with errno: " << errno << "\n";
        return -1;
    }

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    int err = getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    if (err != 0) {
        std::cerr << "ERROR: getnameinfo failed with errorcode: " << err << "\n";
        return -1;
    }
    std::cout << "Conexión desde " << host << " " << service << "\n";

    while(true)
    {
        char buffer[BUFFLEN];

        int bytes = recv(socketClient, (void*) buffer, BUFFLEN-1, 0); 
        if(bytes == -1) {
            std::cout << "ERROR: recvfrom failed\n";
            return -1;
        }
        if(bytes == 0) {
            std::cout << "Conexión terminada\n";
            break;
        }

        buffer[bytes] = 0;

        int sent = send(socketClient, buffer, bytes, 0);
        if(sent == -1) {
            std::cout << "ERROR: sendto failed\n";
            return -1;
        }
    }
    
    close(sd);
    close(socketClient);

    return 0;
}