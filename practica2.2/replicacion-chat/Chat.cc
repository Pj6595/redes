#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
	char *tmp = _data;
	memcpy(tmp, &type, sizeof(uint8_t));
	tmp += sizeof(uint8_t);
	memcpy(tmp, nick.c_str(), 8 * sizeof(char));
	tmp += 8 * sizeof(char);
	memcpy(tmp, message.c_str(), 80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
	char *tmp = bobj;
	memcpy(&type, tmp, sizeof(uint8_t));
	tmp += sizeof(uint8_t);
	nick = tmp;
	tmp += 8 * sizeof(char);
	message = tmp;

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

		Socket* client;
		ChatMessage msg;
		socket.recv(msg, client);

		std::unique_ptr<Socket> clientPtr(client);

		switch(msg.type){
			case ChatMessage::LOGIN:
				clients.push_back(std::move(clientPtr));
				std::cout << msg.nick << " se ha conectado.";
				break;
			case ChatMessage::LOGOUT:
				for (auto it = clients.begin(); it != clients.end(); ++it) {
					if(**it == *client) clients.erase(it);
				}
				std::cout << msg.nick << " se ha desconectado.";
				break;
			case ChatMessage::MESSAGE:
				for (auto it = clients.begin(); it != clients.end(); ++it){
					if(!(**it == *client)) socket.send(msg, **it);
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

