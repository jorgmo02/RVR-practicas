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

    time_t myTime = time(NULL);
    
    while(true)
    {
        #define BUFFLEN 80
        char buffer[BUFFLEN];
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        sockaddr client;
        socklen_t clientlen = sizeof(sockaddr);

        int bytes = recvfrom(sd, (void*) buffer, BUFFLEN-1, 0, &client, &clientlen); 
        if(bytes == -1) {
            std::cout << "ERROR: recvfrom failed\n";
            return -1;
        }
        int err = getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        if (err != 0) {
            std::cerr << "ERROR: getnameinfo failed with errorcode: " << err << "\n";
            return -1;
        }
        std::cout << std::to_string(bytes) << " bytes de " << host << ":" << service << "\n";

        tm* hora = localtime(&myTime);
        char clientbuff[BUFFLEN] = {};
        
        switch (buffer[0])
        {
        case 't':
            strftime(clientbuff, sizeof(clientbuff), "%I:%M:%S %p", hora);            
            break;

        case 'd':
            strftime(clientbuff, sizeof(clientbuff), "%F", hora);
            break;
        
        case 'q':
            std::cout << "Saliendo...\n";
            return 1;
            break;      // no hace falta pero no está de más ponerlo

        default:
            std::cout << "Comando no soportado " << buffer;
            memset(buffer, 0, sizeof(buffer));
            continue;
            break;      // no hace falta pero no está de más ponerlo
        }

        int sent = sendto(sd, clientbuff, strlen(clientbuff), 0, &client, clientlen);
        if(sent == -1) {
            std::cout << "ERROR: sendto failed\n";
            return -1;
        }
    }

    return 0;
}