#ifndef INCLUDE_CLIENT_H
#define INCLUDE_CLIENT_H

void catch_ctrl_c(int signal);
void eraseText(int code);
void send_message(int client_socket);
void recv_message(int client_socket);

#endif
