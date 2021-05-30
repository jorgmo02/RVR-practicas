#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

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

int ChatMessage::from_bin(char * bobj)
{
    if(data == nullptr) return -1;

    alloc_data(MESSAGE_SIZE);
    if(strlen(bobj) >= _size)
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
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
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
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

