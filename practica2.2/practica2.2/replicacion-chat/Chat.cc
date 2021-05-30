#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

using msgType = ChatMessage::MessageType;

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
    if (data == nullptr)
        return -1;

    alloc_data(MESSAGE_SIZE);
    if (strlen(bobj) >= _size)
    {
        std::cout << "Invalid data buffer in from_bin\n";
        return -1;
    }

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    memcpy(&type, _data, sizeof(uint8_t));
    memcpy(&nick, _data + sizeof(uint8_t), sizeof(char) * SIZENICK);
    memcpy(&message, _data + sizeof(uint8_t) + sizeof(char) * SIZENICK, sizeof(char) * SIZEMSG);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        ChatMessage msg;
        Socket* sck = nullptr; // lo inicializa recv
        socket.recv(msg, sck);
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
        case msgType::LOGIN:
            clients.push_back(std::move(client));
            std::cout << "LOGIN " << msg.nick << "\n";
            break;

        case msgType::LOGOUT:
            auto it = clients.begin();
            while(it != clients.end())
            {
                if(*it->get() == *sck) {       // hemos encontrado el que queríamos
                    std::cout << "LOGOUT " << msg.nick << "\n";
                    clients.erase(it);
                    return;                    // jaime me mataría por estas cosas pero por suerte no me lo corrige él
                }
                ++it;
            }
            std::cout << "ERROR: No se pudo hacer logout\n"; // no hemos encontrado el que queríamos (???)
            break;

        case msgType::MESSAGE:            
            std::cout << "MESSAGE " << msg.nick << "\n";
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{    
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    std::string msg;
    do
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::getline(std::cin, msg);

        ChatMessage chatMsg(nick, msg);
        chatMsg.type = (msg == "LOGOUT") ? ChatMessage::LOGOUT : ChatMessage::MESSAGE;
        
        socket.send(chatMsg, socket);
    
    } while(msg != "LOGOUT");

}

void ChatClient::net_thread()
{
    ChatMessage msg;
    while (true)
    {
        //Recibir Mensajes de red
        socket.recv(msg);

        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << msg.nick << ": " << msg.message << "\n";
    }
}
