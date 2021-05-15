#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#define BUFFLEN 80

int main(int argc, char** argv)
{
    if(argc < 4) {
        std::cout << "ERROR: not enough parameters.\nUsage: <name> <service> <command>\n";
        return -1;
    }

    addrinfo hints;      // opciones de filtrado
    addrinfo* res;       // variable para almacenar el resultado de getaddrinfo

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;

    int resGetAddr = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(resGetAddr != 0) {   // ha habido un error 
        std::cerr << "ERROR: getaddrinfo failed with errorcode: " << resGetAddr << "\n";
        return -1;
    }
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if(sd == -1) {
        std::cerr << "ERROR: socket creation failed\n";
        return -1;
    }

//    bind(sd, res->ai_addr, res->ai_addrlen);          esto no se hace aquí

    freeaddrinfo(res);

    char buffer[BUFFLEN] = { *argv[3] };    // stores input that is going to be sent

    while (true)
    {
        char recvbuffer[BUFFLEN] = {};      // stores data received from server
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        sockaddr server;
        socklen_t serverlen = sizeof(sockaddr);

        int sent = sendto(sd, (void*) buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
        if(sent == -1) {
            std::cout << "ERROR: sendto failed\n";
            return -1;
        }
        if (buffer[0] == 'q') return 1;

        int bytes = recvfrom(sd, (void*) recvbuffer, BUFFLEN-1, 0, &server, &serverlen); 
        if(bytes == -1) {
            std::cout << "ERROR: recvfrom failed\n";
            return -1;
        }
        std::cout << recvbuffer;

        std::cin >> buffer;
    }

    close(sd);

    return 0;
}