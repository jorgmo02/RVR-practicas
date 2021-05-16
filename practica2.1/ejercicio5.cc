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


    if (connect(sd, (sockaddr*) res->ai_addr, res->ai_addrlen) != 0)    // iniciar conexion con el socket
    {
        std::cout << "ERROR: connect failed with errno: " << errno << "\n";
        return -1;
    }

    freeaddrinfo(res);

    while(true)
    {
        char buffer[BUFFLEN];

        std::cin >> buffer;

        int sendbytes = strlen(buffer);
        if(strlen(buffer) == 1 && buffer[0] == 'Q') break;

        int sent = send(sd, buffer, sendbytes, 0);
        if(sent == -1) {
            std::cout << "ERROR: sendto failed\n";
            return -1;
        }

        int bytes = recv(sd, (void*) buffer, BUFFLEN-1, 0); 
        if(bytes == -1) {
            std::cout << "ERROR: recvfrom failed\n";
            return -1;
        }

        std::cout << buffer;
    }
    
    close(sd);

    return 0;
}