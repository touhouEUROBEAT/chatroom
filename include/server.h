#include <string>

void set_name(int id, char name[]);
void server_out(std::string str, bool endLine);
int broadcast(std::string name, int sender_id, std::string message);
void end_connection(int id);
void handle_client(int client_socket, int id);
