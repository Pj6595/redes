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
	char *tmp = _data;
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
				for (auto it = clients.begin(); it != clients.end(); ++it) socket.send(msg, **it);
				clients.push_back(std::move(clientPtr));
				std::cout << msg.nick << " se ha conectado.\n";
				break;
			case ChatMessage::MESSAGE:
				for (auto it = clients.begin(); it != clients.end(); ++it)
				{
					if (!(**it == *client))
						socket.send(msg, **it);
				}
				std::cout << "Mensaje de " << msg.nick << " reenviado.\n";
				break;
			case ChatMessage::LOGOUT:
				for (auto it = clients.begin(); it != clients.end(); ++it)
				{
					if(**it == *client){
						clients.erase(it);
						break;
					}
				}
				for (auto it = clients.begin(); it != clients.end(); ++it) socket.send(msg, **it);
				std::cout << msg.nick << " se ha desconectado.\n";
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

	ChatMessage logoutMsg(nick, msg);
	logoutMsg.type = ChatMessage::LOGOUT;

	socket.send(logoutMsg, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
		std::string msg;
		std::getline(std::cin, msg);

		//Si el mensaje es LOGOUT nos desconectamos, si es LOGIN volvemos a entrar al chat
		ChatMessage message(nick, msg);
		if(msg=="LOGOUT"){
			logout();
			continue;
		}
		else if(msg=="LOGIN"){
			login();
			continue;
		}
		else message.type = ChatMessage::MESSAGE;

		// Enviar al servidor usando socket
		socket.send(message, socket);
	}
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
		ChatMessage msg;
		socket.recv(msg);

		//Si el mensaje es login o logout mostramos un mensaje informativo
		if(msg.type == ChatMessage::LOGOUT)
			std::cout << msg.nick << " se ha ido del chat.\n";
		else if(msg.type == ChatMessage::LOGIN)
			std::cout << msg.nick << " se ha unido al chat.\n";
		else
			//Mostrar en pantalla el mensaje de la forma "nick: mensaje"
			std::cout << msg.nick << ": " << msg.message << '\n';
	}
}

