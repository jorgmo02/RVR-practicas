#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

//using msgType = ChatMessage::MessageType;

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data

    char* aux = _data;

    // tipo
    memcpy(aux, &type, sizeof(uint8_t));
    aux += sizeof(uint8_t);

    memcpy(aux, nick.c_str(), sizeof(char) * SIZENICK);
    aux += sizeof(char) * SIZENICK;

    memcpy(aux, message.c_str(), sizeof(char) * SIZEMSG);
}

int ChatMessage::from_bin(char *bobj)
{
    // if (bobj == nullptr){
    //     std::cout << "bobj was nullptr in ChatMessage::from_bin\n";
    //     return -1;
    // }

    alloc_data(MESSAGE_SIZE);
    // if (strlen(bobj) >= _size)
    // {
    //     std::cout << "Invalid data buffer in from_bin\n";
    //     return -1;
    // }

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char* aux = _data;
    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    nick = aux;
    aux += sizeof(char) * SIZENICK;
    message = aux;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        ChatMessage msg;
        msg.type = (ChatMessage::MessageType)3;
        Socket* sck = &socket;
        if(socket.recv(msg, sck) == -1)
        {
            //std::cout << "Error in socket recv; abort connection.\n";
            continue;
        }
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        std::unique_ptr<Socket> client(sck);

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        switch (msg.type)
        {
        case ChatMessage::MessageType::LOGIN:
        {
            clients.push_back(std::move(client));
            std::cout << "LOGIN " << msg.nick << "\n";
            break;
        }

        case ChatMessage::MessageType::LOGOUT:
        {
            auto it = clients.begin();
            while(it != clients.end())
            {
                if(*it->get() == *sck) {       // hemos encontrado el que queríamos
                    std::cout << "LOGOUT " << msg.nick << "\n";
                    clients.erase(it);
                    it = clients.end();
                }
                else ++it;
            }
            //std::cout << "ERROR: No se pudo hacer logout\n"; // no hemos encontrado el que queríamos (???)
            break;
        }

        case ChatMessage::MessageType::MESSAGE: {
            std::cout << "MESSAGE " << msg.nick << "\n";
            std::cout << msg.message << "\n";
            for(auto it = clients.begin(); it != clients.end(); ++it)
            {
                if(*it->get() != *sck) {                    // enviar msg si != el que lo ha mandado
                    it->get()->send(msg, *it->get());
                }
            }
            break;
        }

        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    if(socket.send(em, socket) == -1)
        std::cout << "no se pudo enviar\n";
}

void ChatClient::logout()
{    
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    if(socket.send(em, socket) == -1)
        std::cout << "ERROR: no se puso send\n";
    //std::cout << "LOGOUT\n";
}

void ChatClient::input_thread()
{
    std::string msg = "";
    do
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::getline(std::cin, msg);
        
        ChatMessage chatMsg(nick, msg);
        chatMsg.type = (msg != "LOGOUT") ? ChatMessage::MESSAGE : ChatMessage::LOGOUT;

        if(socket.send(chatMsg, socket) == -1)
            std::cout << "ERROR: no se pudo send\n";
    
    } while(msg != "LOGOUT");

    std::cout << "se pudo terminar\n";
    logout();
}

void ChatClient::net_thread()
{
    while (true)
    {
        ChatMessage msg;
        //Recibir Mensajes de red
        if(socket.recv(msg) == -1)
            std::cout << "ERROR: no se pudo recv\n";

        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        if(msg.nick != nick)
            std::cout << msg.nick << ": " << msg.message << "\n";
    }
}
