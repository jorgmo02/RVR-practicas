#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define BUFFLEN 80

class SocketThread 
{
private:
    int sd_;
    char buffer[BUFFLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    time_t myTime;

public:
    
    SocketThread(int sd) : sd_(sd) {
        myTime = time(NULL);
    }

    ~SocketThread() {}

    void controlMessage()
    {
        while(true)
        {
            memset(buffer, 0, BUFFLEN);

            sockaddr client;
            socklen_t clientlen = sizeof(sockaddr);

            int bytes = recvfrom(sd_, (void*) buffer, BUFFLEN-1, 0, &client, &clientlen); 
            if(bytes == -1) {
                std::cout << "ERROR: recvfrom failed\n";
                return;
            }

            sleep(3);       // "procesamiento"

            int err = getnameinfo(&client, clientlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            if (err != 0) {
                std::cerr << "ERROR: getnameinfo failed with errorcode: " << err << "\n";
                return;      // ha habido un error
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
                return;

            default:
                std::cout << "Comando no soportado " << buffer;
                memset(buffer, 0, sizeof(buffer));
                continue;
            }

            int sent = sendto(sd_, clientbuff, strlen(clientbuff), 0, &client, clientlen);
            if(sent == -1) {
                std::cout << "ERROR: sendto failed\n";
                return; // ha habido un error
            }
        }
    }
};