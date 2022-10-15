#ifndef INCLUDE_CHATROOM_H
#define INCLUDE_CHATROOM_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <errno.h>
#include <string.h>
#include <signal.h>

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <iterator>


const int MAX_LEN = 128;
const int NUM_COLORS = 6;

const std::string def_col = "\033[0m";
const std::string colors[] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m","\033[36m"};

std::string color(int code);

void handleErrors(void);

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);

#endif
