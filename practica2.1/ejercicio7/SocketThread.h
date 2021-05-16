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

public:
    
    SocketThread(int sd) : sd_(sd) {
    }

    ~SocketThread() {}

    void controlMessage()
    {        
        while(true)
        {
            char buffer[BUFFLEN];

            int bytes = recv(sd_, (void*) buffer, BUFFLEN-1, 0); 
            if(bytes == -1) {
                std::cout << "ERROR: recvfrom failed\n";
                break;
            }
            if(bytes == 0) {
                std::cout << "Conexión terminada\n";
                break;
            }

            buffer[bytes] = 0;      // colocar el \0

            int sent = send(sd_, buffer, bytes, 0);
            if(sent == -1) {
                std::cout << "ERROR: sendto failed\n";
                break;
            }
        }

        close(sd_);
    }
};