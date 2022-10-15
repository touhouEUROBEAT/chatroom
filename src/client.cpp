#include "include/chatroom.h"
#include "include/client.h"

using namespace std;

bool exit_flag=false;
thread t_send, t_recv;
int client_socket;

int main()
{
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = htons(10000);
	client.sin_addr.s_addr = INADDR_ANY;
	bzero(&client.sin_zero, 0);

	if (connect(client_socket, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) == -1) {
		perror("connect: ");
		exit(-1);
	}

	signal(SIGINT, catch_ctrl_c);
	
	/*
	  Take in user's name.
	  
	  TODO: Implement user registration system.
	*/
	char name[MAX_LEN];
	cout << "Enter your name : ";
	cin.getline(name, MAX_LEN);

	/* Notify server of this new user. */
	send(client_socket, name, sizeof(name), 0);
	
	/* Print welcome message. */
	cout << colors[NUM_COLORS-1] << "\n\t  ====== Welcome to the chat-room ======   "<< endl << def_col;

	/* Spawn out two threads to handle receivng and sending message from and to the server. */
	thread t1(send_message, client_socket);
	thread t2(recv_message, client_socket);

	t_send = move(t1);
	t_recv = move(t2);

	if(t_send.joinable())
		t_send.join();
	if(t_recv.joinable())
		t_recv.join();
			
	return 0;
}


/*
  Terminate the program properly in the event of Ctrl-C.

  Notify the server, and clean up before exiting.
*/
void catch_ctrl_c(int signal) 
{
	char exit_signal[MAX_LEN] = "#exit";
	send(client_socket, exit_signal, sizeof(exit_signal), 0);
	
	exit_flag = true;
	t_send.detach();
	t_recv.detach();

	cout << endl;
	
	close(client_socket);
	exit(signal);
}


// Erase text from terminal
void eraseText(int cnt)
{
	char back_space=8;
	for(int i=0; i<cnt; i++)
	{
		cout<<back_space;
	}	
}


/*
  Send message to the serve to be broadcasted.
*/
void send_message(int client_socket)
{

	/* TODO: Use real keys and ivs. */
	unsigned char *key = (unsigned char *)"01234567890123456789012345678901";
	unsigned char *iv = (unsigned char *)"0123456789012345";
	
	/*
	  Keep handling user's input until user enters
	  the command for exit.
	*/
	while(1) {
		cout <<colors[1] << "You : " << def_col;
		
		char cur_msg[128];
		cin.getline(cur_msg, MAX_LEN);

		//////////////////////////////////////
		/* Encrypt */
		unsigned char ciphertext[MAX_LEN];
		int ciphertext_len;

		ciphertext_len = encrypt ((unsigned char *)cur_msg, strlen ((char *)cur_msg), key, iv,
						ciphertext);

		/* Build hex encoding. */
		std::stringstream ss;
		ss << std::hex;
		for (int i = 0; i < ciphertext_len; i += 1) {
			ss << std::setw(2) << std::setfill('0') << (int)ciphertext[i];
		}

	        char to_send[128];
		strcpy(to_send, ss.str().c_str());
		
		send(client_socket, to_send, sizeof(to_send),0);
		send(client_socket, &ciphertext_len, sizeof(ciphertext_len), 0);

		cout << "SENT: " << to_send << " " << ciphertext_len << endl;
		

		/* Exit command found. */
		/*
		if (strcmp(cur_msg, "#exit") == 0) {
     			exit_flag = true;


			t_recv.detach();	
			close(client_socket);

			return;
		}
		*/
	}		
}


/*
 */
void recv_message(int client_socket)
{
	/* TODO: Use real keys and ivs. */
	unsigned char *key = (unsigned char *)"01234567890123456789012345678901";
	unsigned char *iv = (unsigned char *)"0123456789012345";

	while(1) {
		if(exit_flag)
			return;
		
		char name[MAX_LEN], str[MAX_LEN];
		
		int color_code;
		int ciphertext_len;

		/*
		  Keep listening for broadcast.
		*/
		int bytes_received = recv(client_socket, name, sizeof(name), 0);
		if(bytes_received <= 0)
			continue;

		/*
		  Now we know server is broadcasting us a message. Capture it.
		*/
		recv(client_socket, &color_code, sizeof(color_code), 0);
		recv(client_socket, str, sizeof(str), 0);
		recv(client_socket, &ciphertext_len, sizeof(ciphertext_len), 0);
		
		eraseText(6);

		/////////
		/*
		  Decript.
		*/

		unsigned char decryptedtext[128];
		int decryptedtext_len;

		string temp(str);
		unsigned char foo[128];
		for (int i = 0; i < ciphertext_len * 2 - 1; i += 2) {
			foo[i / 2] = std::stoi(temp.substr(i, 2), nullptr, 16);
		}

	        decryptedtext_len = decrypt(foo, ciphertext_len, key, iv,
                                decryptedtext);


		decryptedtext[decryptedtext_len] = '\0';
		
		
		if (strcmp(name,"#NULL") != 0) {
			cout << color(color_code)<< name <<" : "<< def_col<< decryptedtext << endl;
		} else {
			cout << color(color_code) << decryptedtext << endl;
		}
		
		cout << colors[1] << "You : " << def_col;
		fflush(stdout);
	}	
}
