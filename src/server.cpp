#include "include/server.h"
#include "include/chatroom.h"


using namespace std;

struct client_struct
{
	int id;
	string name;
	int socket;
	thread th;
};

/* TODO: Upgrade to map or unordered map. */
vector<client_struct> clients;

int seed = 0;
mutex cout_mtx, clients_mtx, broadcast_mtx;


/*
  Sets the server for connection, and then keep listening for connection from
  ne client.
  
  Spawns out one seperate thread to handle that client.

  TODO: Account/registraton system.
*/
int main()
{
	int server_socket;
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(10000);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero, 0);

	if ((bind(server_socket, (struct sockaddr *)&server, sizeof(struct sockaddr_in))) == -1) {
		perror("bind error: ");
		exit(-1);
	}

	if ((listen(server_socket, 8)) == -1) {
		perror("listen error: ");
		exit(-1);
	}

   	struct sockaddr_in client;
	int client_socket;
	unsigned int len = sizeof(sockaddr_in);

	cout << colors[NUM_COLORS - 1] << "\n\t === Welcome === " << endl << def_col;

	while (1) {
		if ((client_socket = accept(server_socket, (struct sockaddr *)&client, &len)) == -1) {
			perror("accept error: ");
			exit(-1);
		}

		seed += 1;
		thread t(handle_client, client_socket, seed);
		
		lock_guard<mutex> guard(clients_mtx);
		clients.push_back({seed, string("No Name"), client_socket, (move(t))});
	}
      
	for (long unsigned int i = 0; i < clients.size(); i += 1) {
		if (clients[i].th.joinable())
			clients[i].th.join();
	}

	close(server_socket);
	return 0;
}


/*
  Given id, set the name of user associated with the id.
*/
void set_name(int id, char name[])
{
	for (long unsigned int i = 0; i < clients.size(); i += 1) {
		if (clients[i].id == id)
			clients[i].name = string(name);
	}
}


/*
  Handles the server-side chat history.

  TODO: Persist chat history to file or database.
*/
void server_out(string str, bool endLine = true)
{	
	lock_guard<mutex> guard(cout_mtx);
	cout << str;
	if (endLine)
		cout << endl;
}


/*
  Broadcast user message to all other users except the sender.

  TODO: Error handling.
*/
int broadcast(string name, int sender_id, string message)
{
	lock_guard<mutex> guard(broadcast_mtx);
	for (long unsigned int i = 0; i < clients.size(); i += 1) {
		/*
		  Format: name, id, message.
		*/
		if (clients[i].id != sender_id) {
			char foo[MAX_LEN];
			strcpy(foo, name.c_str());
			
			send(clients[i].socket, foo, sizeof(foo), 0);
			send(clients[i].socket, &sender_id, sizeof(sender_id), 0);

			strcpy(foo, message.c_str());
			send(clients[i].socket, foo, sizeof(foo), 0);
		}
	}

	return 0;
}


/*
  Clean-up and terminate connection to a client.
*/
void end_connection(int id)
{
	for (long unsigned int i = 0; i < clients.size(); i += 1) {
		if (clients[i].id == id) {
			lock_guard<mutex> guard(clients_mtx);
			clients[i].th.detach();
			clients.erase(clients.begin() + i);
			close(clients[i].socket);

			break;
		}
	}
}


/*
  Thread for handling the connection to a single
  client.

  TODO: Support more commands.
*/
void handle_client(int client_socket, int id)
{
	
	char name[MAX_LEN], cur_message[MAX_LEN];
	recv(client_socket, name, sizeof(name), 0);
	set_name(id, name);

	/*
	string welcome_message = string(name) + string(" has entered the chat");
	
	broadcast(string("#NULL"), id, welcome_message);	
	server_out(color(id) + welcome_message + def_col);
	*/
	
	while (1) {
		int bytes_received = recv(client_socket, cur_message, sizeof(cur_message), 0);
		cout << bytes_received << endl;
		if (bytes_received <= 0)
			return;

		int ciphertext_len;
		recv(client_socket, &ciphertext_len, sizeof(ciphertext_len), 0);

		cout << "RECEIVED: " << cur_message << " " << ntohl(ciphertext_len) << endl;

		/*
		  Exit command detected. Broadcast exit message, and terminate
		  connection to that user.
		*/
		/*
		if (strcmp(cur_message, "#exit") == 0) {
			string exit_message = string(name) + string(" has left");
			broadcast(string("#NULL"), id, exit_message);
			server_out(color(id) + exit_message + def_col);

			end_connection(id);
			return;
		}
		*/

		//broadcast(string(name), id, string(cur_message));
		//server_out(color(id) + name + " : " + def_col + cur_message);

		
		lock_guard<mutex> guard(broadcast_mtx);
		for (long unsigned int i = 0; i < clients.size(); i += 1) {
			/*
			  Format: name, id, message.
			*/
			if (clients[i].id != id) {
				char foo[MAX_LEN];
				strcpy(foo, name);
			
				send(clients[i].socket, foo, sizeof(foo), 0);
				send(clients[i].socket, &id, sizeof(id), 0);

				strcpy(foo, cur_message);
				send(clients[i].socket, foo, sizeof(foo), 0);

				send(clients[i].socket, &ciphertext_len, sizeof(ciphertext_len), 0);
			}
		}

	}
}
