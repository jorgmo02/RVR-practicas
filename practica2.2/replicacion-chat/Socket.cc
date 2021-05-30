#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo.
    addrinfo hints;      // opciones de filtrado
    addrinfo* res;       // variable para almacenar el resultado de getaddrinfo

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = SOCK_DGRAM;
    hints.ai_flags = AF_INET;

    int resGetAddr = getaddrinfo(address, port, &hints, &res);
    if(resGetAddr != 0) // ha habido un error
    {
        std::cerr << "ERROR: getaddrinfo failed with errorcode: " << resGetAddr << "\n";
        return;
    }

    
    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase
    sd = socket(res->ai_family, res->ai_socktype, 0);
    if(sd == -1) {
        std::cerr << "ERROR: socket creation failed\n";
        return;
    }
    sa = *(res->ai_addr);
    sa_len = res->ai_addrlen;

    freeaddrinfo(res);
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    return obj.from_bin(buffer);
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    //Serializar el objeto
    obj.to_bin();
    
    //Enviar el objeto binario a sock usando el socket sd
    return sendto(sock.sd, obj.data(), obj.size(), 0, &sock.sa, sock.sa_len);
}

bool operator== (const Socket &s1, const Socket &s2)
{
    // cast de sockaddr a sockaddr_in
    const sockaddr_in* sock1 = (const sockaddr_in*)&s1; 
    const sockaddr_in* sock2 = (const sockaddr_in*)&s2; 

    return  sock1->sin_addr.s_addr == sock2->sin_addr.s_addr &&
            sock1->sin_family == sock2->sin_family &&
            sock1->sin_port == sock2->sin_port;
}

bool operator!= (const Socket &s1, const Socket &s2) {
    return !(s1 == s2);
}

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};

